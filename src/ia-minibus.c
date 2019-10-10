#include <stdio.h>
#include <stdlib.h>



int main(void){

    int NB_STATIONS = 3;

    int NJ = 0;
    int J = 0;
    int ID = 0;
    int X = 0;
    int Y = 0;
    int K = 0;

    scanf("%d", &NJ);
    scanf("%d", &J);
    fprintf(stderr, "%d", NJ);
    fprintf(stderr, "%d\n", J);

    for(int i = 0; i < NB_STATIONS; i++){
        scanf("%d", &ID);
        scanf("%d", &X);
        scanf("%d", &Y);
        scanf("%d", &K);

        fprintf(stderr, "%d", ID);
        fprintf(stderr, "%d", X);
        fprintf(stderr, "%d", Y);
        fprintf(stderr, "%d\n", K);
    }

    printf("PASS\n");
    
    while(1){

    }
}