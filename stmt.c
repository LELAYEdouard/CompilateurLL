#include "defs.h"
#include "data.h"
#include "decl.h"

void parseStatements(){
    ASTNode *n;
    int res,reg;
    char *globSymb;

    while(1){
        switch(Token.token){
            case T_PRINT:
                match(T_PRINT,"print");
                match(T_LBRACKET,"(");
        
                n = parseExpr();
                reg = genAST(n);
                printASM(reg);
                freeAllReg();
                
                match(T_RBRACKET,")");
                match(T_SEMI,";");
                break;
            case T_INTTYPE:
                match(T_INTTYPE,"int");
                match(T_IDENTIFIER,"identifier");
                globSymb = strdup(Text);

                switch(Token.token){
                    case T_SEMI:
                        addSym(globSymb);
                        symbolGlobalASM(globSymb);
                        match(T_SEMI,";");
                        
                        break;
                    
                    case T_EQUALS:
                        match(T_EQUALS,"="); 
                        
                        n = parseExpr();
                        reg = genAST(n);
                        
                        addSym(globSymb);
                        symbolGlobalASM(globSymb);
                        storeGlobalASM(reg,globSymb);
                        match(T_SEMI,";");
                        break;

                    default:
                        printf("Error at line %d\n",line);
                        exit(1);       
                }

                break;
            
            case T_IDENTIFIER:
                globSymb = strdup(Text);
                match(T_IDENTIFIER,"identifier");
                match(T_EQUALS,"="); 

                int id = searchSym(globSymb);
                if(id == -1){
                    printf("Error, symbol %s doesn't exists at line %d\n",globSymb,line);
                    exit(1);
                }
                n = parseExpr();
                reg = genAST(n);
                
                storeGlobalASM(reg,globSymb);
                match(T_SEMI,";");
                break;

            case T_EOF:
                return ;
            default:
                printf("Error at line %d\n",line);
                exit(1);
        }
    }
}

