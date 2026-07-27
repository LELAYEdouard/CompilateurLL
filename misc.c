#include "defs.h"
#include "data.h"
#include "decl.h"

void match(int tok,char *str){
    if(Token.token == tok){
        scan(&Token);
    }
    else{
        printf("Error, %s expected at line %d\n",str,line);
        exit(1);
    }
}

void debugINT(int val){
    printf("%d\n",val);
    exit(1);
}

void debugSTR(char *val){
    printf("%s\n",val);
    exit(1);
}

void debugCHAR(char val){
    printf("%c\n",val);
    exit(1);
}

void debug(){
    printf("test\n");
    exit(1);
}