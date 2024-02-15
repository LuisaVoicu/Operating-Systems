#include <stdio.h>
#include <stdlib.h>

int insert(int* v1, int n1, int c1, int*v2, int n2, int pos){
    if(n1+n2 > c1){
        return -1; // capacitate insuficienta
    }

    // pornesc de la sfarsitul lui v1 si mut fiecare element pana la pozitia pos cu n2 elemente mai la dreapta
    for(int i = 0 ; i < n2; i++){
        for(int j = n1+i;j>=pos+i;j--)
        v1[j+1] = v1[j];
        v1[pos+i] = v2[i];
    }
    printf("\n");
    return 0;
}

int main(){
    int c1 = 20;
    int n1;
    int n2;

    do{
    printf("Introduceti valorile pentru n1:");
    scanf("%d",&n1);
    }while(n1<=0);


    do{
    printf("Introduceti valorile pentru n2:");
    scanf("%d",&n2);
    }while(n2<=0);

    do{
    printf("Introduceti valorile pentru c1:");
    scanf("%d",&c1);
    }while(c1<=0 || c1<n1);
    
    int*v1 = (int*) malloc(c1*sizeof(int));
    int*v2 = (int*) malloc((n2+1)*sizeof(int));
    


    if(v1==NULL||v2==NULL){
        exit(123456);
    }

    printf("Introduceti valorile pt v1:");
    for(int i = 0 ; i < n1; i++){
        scanf("%d",&v1[i]);
    }

    printf("Introduceti valorile pentru v2:");
    for(int i = 0; i < n2; i++){
        scanf("%d",&v2[i]);
    }

    int pos;
    do{
    printf("Introduceti valoarea pentru pos:");
    scanf("%d",&pos);
    }while(pos>n1 || pos<0);

    int result = insert(v1,n1,c1,v2,n2,pos);
    if(!result){
    for(int i = 0 ; i < n1+n2; i++){
        printf("%d " ,v1[i]);
    }
    printf("\n");
    }
    else{
        printf("capacitate insuficienta!");
    }
    free(v1);
    free(v2);

}