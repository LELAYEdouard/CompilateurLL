#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"

char *tokstr[] = {"+","-","*","/","intlt"};
char *aritstr[] = {"+","-","*","/"};

void init();
static void scanfile();

int main(int argc, char *argv[]){
    struct ASTNode* n;

    if(argc != 2){
        printf("%s needs 1 ll file \n",argv[0]);
        exit(1);
    }

    if((Outfile = fopen("out.s", "w")) == NULL){
        printf("Unable to create output file\n");
    }
    
    init();

    file = fopen(argv[1],"r");

    preambleASM();

    scan(&Token);
    match(T_MAIN,"main");
    parseStatements();

    postambleASM();

    fclose(file);
    exit(0);
}

void init(){
    line = 1;
    Putback = '\n';
    lastLstIdent = 0;
    ifStatementNb = 0;
}

static void scanfile(){
    struct token T;

    while(scan(&T)){
        printf("Token %s",tokstr[T.token]);
        if(T.token == T_INTLT){
            printf(",value %d",T.intval);
        }
        printf("\n");
    }
}
