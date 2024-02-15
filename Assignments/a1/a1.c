#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>


#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_SIZE_PATH 1000000


struct  __attribute__((packed)) section_header {
    char name[16];
    short int stype;
    int offset;
    int size;
} section_header;


DIR *opendir(const char *name);
int closedir(DIR *dirp);
struct dirent *readdir(DIR *dirp);
int stat(const char *pathname, struct stat *statbuf);
int lstat(const char *pathname, struct stat *statbuf);




void do_variant(){
    printf("38391\n");
}

char* find_parameter(int argc, char** argv, char* type){
    for(int i = 1; i < argc; i++){
        char operation[20];
        int index=0;
        operation[0]='\0';
        //numele operatiilor nu ar trebui sa depaseasca 20 de caractere
        while(argv[i][index] && argv[i][index]!='='&&index<20){
            operation[index]=argv[i][index];
            index++;
        }
        operation[index]='\0';
        if(argv[i][index]=='='&&!strcmp(operation,type)){
            index++;
            char* parameter=(char*)malloc(sizeof(char)*(strlen(argv[i])-strlen(operation)+2));
            int new_index=0;
            while(argv[i][index]){
                parameter[new_index++]=argv[i][index++];
            }
            parameter[new_index]='\0';
            //printf("pppp--->%s\n",parameter);
            return parameter;
        }
    }
    return NULL;
}

int is_recursive(int argc, char** argv){
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"recursive\0")){
            return true;
        }
    }
    return false;
}

/*
source insp to convert st_mode to ls format  : 
https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
*/
char* get_string_permission(mode_t mode){
    char*s = (char*)malloc(9); // todo nu inteleg de ce nu merge sa pun pur si simplu array...
    int index=0;
    //owner permission
    s[index++]= ((mode & S_IRUSR) ? 'r' : '-');
    s[index++]= ((mode & S_IWUSR) ? 'w' : '-');
    s[index++]= ((mode & S_IXUSR) ? 'x' : '-');
    s[index++]= ((mode & S_IRGRP) ? 'r' : '-');
    s[index++]= ((mode & S_IWGRP) ? 'w' : '-');
    s[index++]= ((mode & S_IXGRP) ? 'x' : '-');
    s[index++]=((mode & S_IROTH) ? 'r' : '-');
    s[index++]= ((mode & S_IWOTH) ? 'w' : '-');
    s[index++]= ((mode & S_IXOTH) ? 'x' : '-');
    s[index]='\0';
    return s;
}

int do_list_func(char* path,char* perm_string, off_t size_smaller, int recursive){
    DIR* dir = NULL;
    struct dirent* entry = NULL;
    dir = opendir(path);
    struct stat statbuf;

    if(dir == NULL){
        printf("ERROR\ninvalid directory path\n");
        return 1;
    }
    
    while((entry = readdir(dir))!=NULL){

        if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name, "..") != 0){
            char* file_path = (char*)malloc(sizeof(char*)*MAX_SIZE_PATH);

            // path-ul poate avea sau nu '/' pe ultima pozitie
            if(path[strlen(path)-1]!='/'){
               snprintf(file_path,MAX_SIZE_PATH,"%s/%s",path,entry->d_name);
            }
            else{
               snprintf(file_path,MAX_SIZE_PATH,"%s%s",path,entry->d_name);
            }

            if(lstat(file_path,&statbuf)==0){
                int size_verif = true;
                int perm_verif = true;
                //size filter
                if(size_smaller!=-1){
                    if(S_ISDIR(statbuf.st_mode)){
                        //printf("E DIRECTOR ---> %s\n",file_path); // todo --- ceva e neinregula aici eg. size_smaller_1
                        size_verif=false;
                    }
                    else{
                        off_t size_file = statbuf.st_size;
                        if(size_file >= size_smaller){
                            size_verif=false;
                       }
                    }
                }
                if(perm_string!=NULL){    
                    char* s = get_string_permission(statbuf.st_mode);
                    if(strcmp(s,perm_string)!=0){
                        perm_verif=false;
                    }
                    free(s);
                }
                if(size_verif==true && perm_verif==true){
                    printf("%s\n",file_path);
                }
                if(S_ISDIR(statbuf.st_mode)&& recursive==true){
                    do_list_func(file_path,perm_string,size_smaller,recursive);
                }
            }
            free(file_path);
        }

    }
    closedir(dir);
    return 0;
}

int do_list(int argc, char** argv){ 

    if(argc < 3){
        printf("ERROR\ninvalid number of arguments\n");
        return 1;
    }
        
    int result=0;
    // extragerea optiunilor/ argumentelor
    bool recursive = is_recursive(argc,argv);
    char* path = find_parameter(argc,argv,"path");
    
    if(path == NULL){
        printf("ERROR\ninvalid directory path");
                
        result=1;
        goto free_path;
    }
    char* filter1 = find_parameter(argc, argv, "permissions");
    char* filter2 = find_parameter(argc, argv, "size_smaller");
    off_t size=-1;
    if(filter2!=NULL){
        sscanf(filter2,"%ld",&size);
    }

    printf("SUCCESS\n"); 
    result = do_list_func(path,filter1,size,recursive);

    free(filter1);
    free(filter2);
    free_path:    
    free(path);
    
    return result;
}

int valid_sf(int fd, int mute){

    lseek(fd,0,SEEK_SET);

    // verify MAGIC
    char magic[3];
    memset(magic,0,3);
    if(read(fd,magic,2)!=2){
        if(!mute)
        printf("ERROR\ncan't read MAGIC from file\n");
        return 1;
    }
    if(strcmp(magic,"sL")){
        if(!mute)
        printf("ERROR\nwrong magic\n");
        return 1;
    }

    // sar peste header size
    lseek(fd,4,SEEK_SET); 
    
    // verify VERSION
    int version=0;
    if(read(fd,&version,4)!=4){
        if(!mute)
        printf("ERROR\ncan't read VERSION from file\n");
        return 1;
    }
    if(version < 109 || version > 165){
        if(!mute)
        printf("ERROR\nwrong version\n");
        return 1;
    }

    // verify no_version
    int no_of_sections=0;
    if(read(fd,&no_of_sections,1)!=1){
        if(!mute)
        printf("ERROR\ncan't read NO_of_VERSION from file\n");
        return 1;
    }
    
    //verify no_of_sections
    if(no_of_sections<4||no_of_sections>18){
        if(!mute)
        printf("ERROR\nwrong sect_nr");
        return 1;
    }
        

    struct section_header* h = (struct section_header*)malloc((no_of_sections+1)*sizeof(struct section_header));
    
    for(int i =0;i<no_of_sections; i++){
        memset(h[i].name,0,16);
        h[i].stype=0;
        h[i].stype=0;
        h[i].size=0;
        read(fd,h[i].name,15);
        read(fd,&h[i].stype,2);
        read(fd,&h[i].offset,4);
        read(fd,&h[i].size,4);
        //printf("%d--->%s\n" ,i,h[i].name);
        unsigned short  sect_type = h[i].stype;
        if(sect_type!= 76 && sect_type!=95 && sect_type!=30 && sect_type!=71&& sect_type!=32 && sect_type!=31){
            if(!mute)
            printf("ERROR\nwrong sect_types\n");
            free(h);
            return 1;
        }
        
    }
    free(h);
    return 0;
}

int do_parse(int argc, char** argv){
    char* path = find_parameter(argc, argv, "path");
    int result = 0;
    if(path == NULL){
        printf("ERROR\nnull path\n");
        return 1;
    }
    int fd=open(path,O_RDONLY);
    if(fd==-1){
        printf("ERROR\nfile can't be open\n");
        free(path);
        return 1;
    }


    if(valid_sf(fd,false)){
        result=1;
        goto free_path;
    }


    lseek(fd,4,SEEK_SET);
    int version=0;
    if(read(fd,&version,4)!=4){
        printf("ERROR\ncan't read VERSION from file\n");
        result=1;
        goto free_path;
    }
    int no_of_sections=0;
    if(read(fd,&no_of_sections,1)!=1){
        printf("ERROR\ncan't read NO_of_VERSION from file\n");
        result=1;
        goto free_path;
    }

  
    printf("SUCCESS\n");
    printf("version=%d\n",version);
    printf("nr_sections=%d\n",no_of_sections);

    struct section_header* h = (struct section_header*)malloc((no_of_sections+1)*sizeof(struct section_header));
    for(int i =0;i<no_of_sections; i++){
        memset(h[i].name,0,16);
        h[i].stype=0;
        h[i].stype=0;
        h[i].size=0;
        read(fd,h[i].name,15);
        read(fd,&h[i].stype,2);
        read(fd,&h[i].offset,4);
        read(fd,&h[i].size,4);
        printf("section%d: %s %hu %d\n",i+1,h[i].name,h[i].stype,h[i].size);

        
    }

    free(h);
    
    free_path:
    free(path);
    close(fd);   
    return result;
}

int do_extract(int argc, char** argv){

    char* path = NULL;
    char* sect = NULL;
    char* lin = NULL;
    
    path = find_parameter(argc,argv,"path");
    sect = find_parameter(argc,argv,"section");
    lin = find_parameter(argc,argv,"line");
    int result=0;
    if(path==NULL){
        printf("ERROR\ninvalid path\n");
        // free(null) has no effect --> deci e ok daca sect sau lin e null
        result=0;
        goto free_param;
    }
    if(sect==NULL){
        printf("ERROR\ninvalid section\n");
        result=1;
        goto free_param;
    }
    if(lin==NULL){
        printf("ERROR\ninvalid line\n");
        result=1;
        goto free_param;
    }
    int section=0,line=0;
    sscanf(sect,"%d",&section);
    sscanf(lin,"%d",&line);

    int fd=open(path,O_RDONLY);
    if(fd==-1){
        printf("ERROR\ninvalid file\n");
        result=1;
        goto free_param;
    }

    // verify SF format
    if(valid_sf(fd,false)){
        result=1;
        goto close_file;
    }

    lseek(fd,8,SEEK_SET);
    int no_of_sections=0;
    if(read(fd,&no_of_sections,1)!=1){
        printf("ERROR\ninvalid no_section\n");        
        result=1;
        goto close_file;
    }

    off_t sect_offset_pos = 25*(section-1)+ 8+15+2+1; // inceputul unei sectiuni in HEADER
    off_t sect_offset=0;
    off_t sect_size=0;

    lseek(fd,sect_offset_pos,SEEK_SET);
    if(read(fd,&sect_offset,4)!=4){
        printf("ERROR\ncan't read offset");
        result=1;
        goto close_file;
    }
    if(read(fd,&sect_size,4)!=4){
        printf("ERROR\ncan't read size");
        result=1;
        goto close_file;
    }
    //printf("extr----------> %ld %ld\n",sect_offset,sect_size);
    //mutam cursorul la adresa offset
    lseek(fd,sect_offset,SEEK_SET);

    int cnt_line=1;
    off_t cnt_oct=0;
    char* extracted=(char*)malloc(sizeof(char)*(sect_size+10));
    memset(extracted,0,0);
    int ind=0;

    while(cnt_line<=line&&cnt_oct<=sect_size){
        char c;
        if(read(fd,&c,1)!=1){
            printf("ERROR\ncan't read character\n"); // todo schimba
            free(extracted);
            free(path);
            free(sect);
            free(lin);
            close(fd);
            return 1;
       }
        cnt_oct++;
        if(c==0x0D){
            if(read(fd,&c,1)!=1){ 
                printf("ERROR\ncan't read character");
                result=1;
                goto free_all;
            }
            cnt_oct++;
            if(c==0x0A){
                cnt_line++;
            }
            else if(cnt_line==line){
                extracted[ind++]=0x0D;
                extracted[ind++]=c;
            }
        }
        else{
        if(cnt_line==line){
            extracted[ind++]=c;
        }}

    }

    if(cnt_line<line){
        printf("ERROR\ninvalid line");
        result=1;
        goto free_all;
    }

    printf("SUCCESS\n%s\n",extracted);

    free_all:
    free(extracted);
    extracted=NULL;

    close_file:
    close(fd);

    free_param:
    free(path);
    free(sect);
    free(lin);



    return result;
}

//functie care verifica daca intr-un fisier dat prin path ca argument are cel putin o sectiune cu exact 15 linii
int findall_valid_lines(int fd,int sect_offset, int sect_size){
    int cnt_line=0;

    lseek(fd,sect_offset,SEEK_SET); // plasez cursorul la inceputul fisierului
    char* file_content = (char*)malloc(sizeof(char)*(sect_size+1));
    file_content[sect_size]=0;

    if(read(fd,file_content,sect_size)!=sect_size){
        free(file_content);
        return 1;
    }

    char s[2];
    memset(s,0,2);
    s[0]=0x0A;//d;
    char* token=strtok(file_content,s);
    while(token!=NULL){
        if(token[strlen(token)-1]==0x0D){
            cnt_line++;
        }
    if(cnt_line>14){
        break;
    }
    token=strtok(NULL,s);
    }

    if(cnt_line!=14){
        free(file_content); //ultima nu e numarata pentru ca ajunge la sfarsitul sirului
        return 1;
    }
    free(file_content);
    return 0;

}

int at_least_one_section(char* path){
    int fd=-1;
    fd=open(path,O_RDONLY);
    if(fd==-1){
        return 1;
    }

    if(valid_sf(fd,true)){
        close(fd);
        return 1;
    }

    lseek(fd,8,SEEK_SET);
    int no_of_sections=0;
    if(read(fd,&no_of_sections,1)!=1){
        close(fd);
        return 1;
    }
    
    int sect_offset_pos=9;
    for(int i = 0 ; i < no_of_sections; i++){

        int sect_offset=0;
        int sect_size=0;
       sect_offset_pos+=17;
        lseek(fd,sect_offset_pos,SEEK_SET);
        if(read(fd,&sect_offset,4)!=4){
            close(fd);
            return 1;
        }
        if(read(fd,&sect_size,4)!=4){ //todo
            close(fd);
            return 1;
        }

        int found = findall_valid_lines(fd,sect_offset,sect_size);
        if(!found){
            close(fd);
            return 0;
        }
        sect_offset_pos += 8; // inceputul unei sectiuni in HEADER
    } 

    close(fd);
    return 1; 
}

void do_findall_func(char* path, int* succeed){
    DIR* dir = NULL;
    struct dirent* entry = NULL;
    dir = opendir(path);
    struct stat statbuf;


    if(dir == NULL){
        printf("ERROR\ninvalid directory path\n");
        return;
    }
   
    // path-ul poate avea sau nu '/' pe ultima pozitie
    while((entry=readdir(dir))!=NULL){
        char* file_path = (char*)malloc(sizeof(char*)*MAX_SIZE_PATH);
        if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name, "..") != 0){

            if(path[strlen(path)-1]!='/'){
               snprintf(file_path,MAX_SIZE_PATH,"%s/%s",path,entry->d_name);
            }
            else{
               snprintf(file_path,MAX_SIZE_PATH,"%s%s",path,entry->d_name);
            }

            if(lstat(file_path,&statbuf)==0){
            
                if(S_ISDIR(statbuf.st_mode)){
                    do_findall_func(file_path,succeed);
                }
                else{
                  if(!at_least_one_section(file_path))
                    {
                    
                        if(*succeed == false){
                            printf("SUCCESS\n");
                            *succeed = true;
                        }
                        printf("%s\n",file_path);
                    }
                }
            }
        }
        free(file_path);
        file_path=NULL;
    }
    closedir(dir);
}

int do_findall(int argc, char** argv){


    char* path=NULL;
    path=find_parameter(argc,argv,"path");
    if(path == NULL){
        printf("ERROR\ninvalid directory path\n");
        return 1;
    }    
    int succeed = false;
    do_findall_func(path,&succeed);
    free(path);
    return 0;
}

void do_operation(int argc, char** argv){
// operatiile se pot afla oriunde in lista de optiuni 
        // => e necesar sa cautam ce operatie trebuie efectuata din cele 5 printre argumente

    for(int i = 1; i < argc; i++){
        if(!strcmp(argv[i],"variant\0")){
            do_variant();
            return;
        }
        if(!strcmp(argv[i],"list\0")){
            do_list(argc,argv);
            return;
        }
        if(!strcmp(argv[i],"parse\0")){
            do_parse(argc,argv);
            return;
        }
        if(!strcmp(argv[i],"extract\0")){
            do_extract(argc,argv);
            return;
        }
        if(!strcmp(argv[i],"findall\0")){
            do_findall(argc,argv);
            return;
        }
    }
}


int main(int argc, char** argv){
    do_operation(argc,argv);
//test_citire(argc,argv);

     return 0;
}
