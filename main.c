#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"

char *tokstr[] = {"+","-","*","/","intlt"};
char *aritstr[] = {"+","-","*","/"};

void init();

int main(int argc, char *argv[]){

    if(argc != 2){
        printf("%s needs 1 ll file \n",argv[0]);
        exit(1);
    }

    if((Preamble = fopen("preamble.s", "w")) == NULL){
        printf("Unable to create preamble file\n");
    }

    if((Functions = fopen("functions.s", "w")) == NULL){
        printf("Unable to create function file\n");
    }

    if((Main = fopen("main.s", "w")) == NULL){
        printf("Unable to create main file\n");
    }

    if((Varfile = fopen("var.s", "w")) == NULL){
        printf("Unable to create variable file\n");
    }
    
    init();

    file = fopen(argv[1],"r");
    
    variableASM();
    preambleASM();

    scan(&Token);
    if(Token.token == T_FUNCTION){
        match(T_FUNCTION,"function");
        parseStatements(whileStatementNb);
        scan(&Token);
    }

    //avoid func create
    funcDef = 0;
    //avoid local var
    currentFuncId = -1;
    
    currentFile = getCurrentFile(0);
    mainASM();
    match(T_MAIN,"main");
    parseStatements(whileStatementNb);

    postambleASM();
    fclose(currentFile);
    fclose(file);
    exit(0);
}

void init(){
    line = 1;
    Putback = '\n';
    lastLstIdent = 0;
    lastLstFuncParam = 0;
    lastLstFunc = 0;
    ifStatementNb = 0;
    whileStatementNb = 0;
    funcDef = 1;
    currentFuncId = 0;
    hasReturn = 0;
    
}