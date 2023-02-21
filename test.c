#include <stdio.h>


int test[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

int main(){
    int i = 0;
    while(!(test[i] == 3 || test[i] == '7')){
        // printf("search");
        i++;
    }
    printf("%d", test[i]);
}