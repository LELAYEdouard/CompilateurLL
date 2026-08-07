#include "defs.h"
#include "data.h"
#include "decl.h"

int findParameters(){
    match(T_LBRACKET,"(");
    int stackPos = 0;
    int nbArgs = 0;
    char* globSymb;

    while(1){
        switch (Token.token){
            case T_INTTYPE:
                match(T_INTTYPE,"int");

                globSymb = strdup(Text);
                match(T_IDENTIFIER,"identifier");

                int id = searchFuncSym(globSymb);
                if(id != -1){
                    printf("Error, identifier already exists\n");
                    exit(1);
                }
                
                addFuncSym(globSymb,stackPos);

                nbArgs++;

                stackPos++;
                break;
            case T_RBRACKET:
                match(T_RBRACKET,")");
                return nbArgs;
            case T_COMMA:
                match(T_COMMA,",");
                break;
            default:
                printf("Error, unrecognized identifier at line %d\n",line);
                exit(1);
        }
    }
    return 0;
}

void pushParameters(int nbArgs){
    match(T_LBRACKET,"(");

    struct ASTNode *n;
    int reg;
    int i = 0;

    struct ASTNode *lstArgs[nbArgs];

    while(i < nbArgs){

        n = parseExpression();
        lstArgs[i] = n;
        i++;

        switch (Token.token){
            case T_RBRACKET:
                match(T_RBRACKET,")");
                for(int j = nbArgs-1; j >= 0; j--){
                    reg = genAST(lstArgs[j]);
                    pushASM(reg);
                }
                return;
            case T_COMMA:
                match(T_COMMA,",");
                break;
            default:
                printf("Error, unrecognized identifier at line %d\n",line);
                exit(1);
        }
    }
}