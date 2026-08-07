#include "defs.h"
#include "data.h"
#include "decl.h"

void parseStatements(int currentWhile){
    ASTNode *n;
    int reg;
    char *globSymb;

    match(T_LCBRACKET,"{");

    while(1){

        switch(Token.token){
            case T_PRINT:
                match(T_PRINT,"print");
                match(T_LBRACKET,"(");
        
                n = parseExpression();
                reg = genAST(n);
                printASM(reg);
                freeAllReg();

                match(T_RBRACKET,")");
                match(T_SEMI,";");
                break;
            case T_INTTYPE:
                match(T_INTTYPE,"int");
                globSymb = strdup(Text);

                if(searchSym(globSymb) != -1){
                    printf("Error, symbol %s already exists\n",globSymb);
                    exit(1);
                }
                if(searchFunc(globSymb) != -1){
                    printf("Error, function %s already exists\n",globSymb);
                    exit(1);
                }
                match(T_IDENTIFIER,"identifier");

                switch(Token.token){
                    case T_SEMI:
                        addSym(globSymb);
                        symbolGlobalASM(globSymb);
                        match(T_SEMI,";");
                        
                        break;
                    
                    case T_EQUALS:
                        match(T_EQUALS,"="); 
                        char *varName = strdup(globSymb);

                        globSymb = strdup(Text);
                        int find = searchFunc(globSymb); 

                        if(find != -1){
                            scan(&Token);
                            int nbArgs = lstFunc[find].u.nbArgs;
                            pushParameters(nbArgs);

                            callFuncASM(globSymb,nbArgs);
                            
                            reg = loadReturnFunc();
                            
                        }
                        else{
                            n = parseExpression();
                            reg = genAST(n);
                        }

                        addSym(varName);
                        symbolGlobalASM(varName);
                        storeGlobalASM(reg,varName);
                        
                        match(T_SEMI,";");
                        break;
                    case T_LBRACKET:
                        if(!funcDef){
                            printf("Error at line %d, can't initialize functions in main\n",line);
                            exit(1);
                        }
                        if(funcDef == 2){
                            printf("Error at line %d, can't initialize functions inside a function\n",line);
                            exit(1);
                        }
                        // int tokentype = keyword(globSymb);
                        // if(tokentype){
                        //     printf("Error at line %d, %s already exists\n",line,globSymb);
                        //     exit(1);
                        // }

                        definefuncASM(globSymb);
                        
                        int nbArgs = findParameters();
                        
                        addFunc(globSymb,nbArgs);
                        
                        funcDef = 2;
                        parseStatements(currentWhile);

                        funcDef =1;

                        scan(&Token);
                        break;
                    default:
                        printf("Error at line %d, need to declare a function or a variable\n",line);
                        exit(1);       
                }
                break;
            
            case T_IDENTIFIER:
                globSymb = strdup(Text);
                match(T_IDENTIFIER,"identifier");

                if(searchSym(globSymb) == -1 && searchFunc(globSymb) == -1){
                    printf("Error, symbol or function %s doesn't exists at line %d\n",globSymb,line);
                    exit(1);
                }

                switch (Token.token){
                    case T_EQUALS:
                        if(searchSym(globSymb) == -1){
                            printf("Error, symbol %s doesn't exists at line %d\n",globSymb,line);
                            exit(1);
                        }
                        match(T_EQUALS,"="); 

                        n = parseExpression();
                        reg = genAST(n);
                        
                        storeGlobalASM(reg,globSymb);
                        match(T_SEMI,";");

                        break;
                    
                    case T_INC:
                        if(searchSym(globSymb) == -1){
                            printf("Error, symbol %s doesn't exists at line %d\n",globSymb,line);
                            exit(1);
                        }
                        match(T_INC,"++"); 
                        incASM(globSymb);
                        match(T_SEMI,";");
                        
                        break;
                    
                    case T_DEC:
                        if(searchSym(globSymb) == -1){
                            printf("Error, symbol %s doesn't exists at line %d\n",globSymb,line);
                            exit(1);
                        }
                        match(T_DEC,"--"); 
                        decASM(globSymb);
                        match(T_SEMI,";");

                        break;  
                    case T_LBRACKET:
                        if(searchFunc(globSymb) == -1){
                            printf("Error, function %s doesn't exists at line %d\n",globSymb,line);
                            exit(1);
                        }
                        int nbArgs = lstFunc[searchFunc(globSymb)].u.nbArgs;
                        pushParameters(nbArgs);

                        callFuncASM(globSymb,nbArgs);

                        match(T_SEMI,";");
                        break;
                    default:
                        printf("Error, unrecognized token at line %d\n",line);
                        exit(1);
                }
                
                break;
            
            case T_IF:
                ifStatementNb++;
                int currentIf = ifStatementNb;
                char *jumpIf;
                
                match(T_IF,"if");

                match(T_LBRACKET,"(");
                n = parseExpression();
                compareForJumpASM(n,&jumpIf);
                freeAllReg();
                match(T_RBRACKET,")");

                ifASM(jumpIf,currentIf);

                parseStatements(currentWhile);

                scan(&Token);
                
                elseASM(currentIf);
                if(Token.token == T_ELSE){
                    scan(&Token);
                    parseStatements(currentWhile);
                    scan(&Token);
                }

                endifASM(currentIf);

                //jsp encore si c'est bien de le mettre ou pas
                freeAllReg();

                break;
            case T_WHILE:
                whileStatementNb++;
                
                char *jumpW;
                
                match(T_WHILE,"while");

                startwhileASM(currentWhile);

                match(T_LBRACKET,"(");
                n = parseExpression();
                compareForJumpASM(n,&jumpW);
                freeAllReg();
                match(T_RBRACKET,")");
                
                whileASM(jumpW,currentWhile);

                parseStatements(whileStatementNb);

                endwhileASM(currentWhile);

                //jsp encore si c'est bien de le mettre ou pas
                freeAllReg();
                
                scan(&Token);
                break;
            case T_BREAK:
                match(T_BREAK,"break");
                breakwhileASM(currentWhile -1);
                match(T_SEMI,";");
                
                break;
            case T_RETURN:
                match(T_RETURN,"return");
                n = parseExpression();
                reg = genAST(n);
                returnASM(reg);
                
                match(T_SEMI,";");

                endfuncASM();
                break;
            case T_RCBRACKET:
                return;
            case T_EOF:
                return;
            default:
                printf("Error at line %d token at wrong place\n",line);
                exit(1);
        }
    }
}

