#include "defs.h"
#include "data.h"
#include "decl.h"

void match(int tok,char *str){
    if(Token.token == tok){
        scan(&Token);
    }
    else{
        if(!strcmp(str,";")){
            printf("Error, %s expected at line %d\n",str,line-1);
            exit(1);
        }
        printf("Error, %s expected at line %d\n",str,line);
        exit(1);
    }
}

void debugINT(int val){
    printf("%d\n",val);
}

void debugSTR(char *val){
    printf("%s\n",val);
}

void debugCHAR(char val){
    printf("%c\n",val);
}

void debug(){
    printf("test\n");
    exit(1);
}