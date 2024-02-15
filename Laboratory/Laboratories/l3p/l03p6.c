#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

    if(argc != 2){
        printf("prea multe/putine argumente pe lina de comanda!" );
        exit(123);
    }

    unsigned int nr;
    // parsare de la char la int
    sscanf(argv[1],"%u",&nr);

    unsigned int mask1=0;    
    unsigned int mask2=1;
    unsigned int cpy=nr;

printf("number:%d\n",nr);
    // folosesc 2 masti de biti
    // pentru numarul reprezentat: b1b2b3...b(n-1)bn
    // mask1 = bnb(n-1)00..0b2b1
    // mask2 = 001111..1100
    // rezultat = mask1 | (nr & mask2)
    // obs--> do-while-ul va inversa toti bitii - inclusiv cei din mijloc

    while(cpy>0){
        
        if(cpy<=3 || mask2 <=3){
            mask1 |= cpy&1; 
        }
        else{
            mask1|=0;
        }
        cpy=cpy/2;
        mask1*=2;
        mask2*=2;
        mask2|=1;

    }
    mask1/=2;
    mask2/=2;

    mask2/=4; // setez primii 2 biti cu 00 ( era 11 inainte )
    mask2-=3; //setez ultimii 2 biti 00
    printf("m1:%d\n",mask1);
    printf("m2:%d\n",mask2);
    nr = mask1|(nr&mask2); // nr&mask extrage bitii b3b4..b(n-2) IN ORDINE 
                            // iar | pune primii si ultimii 2 biti in ordinea ceruta in enunt in rezultat
    printf("result: %u\n",nr);          
}