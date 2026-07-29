#include "defs.h"
#include "data.h"
#include "decl.h"

void parseStatements(){
    ASTNode *n;
    int res,reg;
    char *globSymb;

    match(T_LCBRACKET,"{");

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

                int id = searchSym(globSymb);
                if(id == -1){
                    printf("Error, symbol %s doesn't exists at line %d\n",globSymb,line);
                    exit(1);
                }

                switch (Token.token){
                    case T_EQUALS:
                        match(T_EQUALS,"="); 

                        n = parseExpr();
                        reg = genAST(n);
                        
                        storeGlobalASM(reg,globSymb);
                        match(T_SEMI,";");

                        break;
                    
                    case T_INC:
                        match(T_INC,"++"); 
                        incASM(globSymb);
                        match(T_SEMI,";");
                        
                        break;
                    
                    case T_DEC:
                        match(T_DEC,"--"); 
                        decASM(globSymb);
                        match(T_SEMI,";");

                        break;  
                }
                
                break;
            
            case T_IF:
                ifStatementNb++;
                int currentIf = ifStatementNb;
                char *jumpIf;
                
                match(T_IF,"if");

                match(T_LBRACKET,"(");
                n = parseExpr();
                compareForJumpASM(n,&jumpIf);
                freeAllReg();
                match(T_RBRACKET,")");

                ifASM(jumpIf,currentIf);

                parseStatements();

                scan(&Token);
                
                elseASM(currentIf);
                if(Token.token == T_ELSE){
                    scan(&Token);
                    parseStatements();
                }

                endifASM(currentIf);

                //jsp encore si c'est bien de le mettre ou pas
                freeAllReg();

                scan(&Token);
                break;
            case T_WHILE:
                whileStatementNb++;
                int currentWhile = whileStatementNb;
                char *jumpW;
                
                match(T_WHILE,"while");

                startwhileASM(currentWhile);

                match(T_LBRACKET,"(");
                n = parseExpr();
                compareForJumpASM(n,&jumpW);
                freeAllReg();
                match(T_RBRACKET,")");
                
                whileASM(jumpW,currentWhile);

                parseStatements();

                endwhileASM(currentWhile);

                //jsp encore si c'est bien de le mettre ou pas
                freeAllReg();
                
                scan(&Token);
                break;
            case T_RCBRACKET:
                return;
            case T_EOF:
                return ;
            default:
                printf("Error at line %d\n",line);
                exit(1);
        }
    }
}

