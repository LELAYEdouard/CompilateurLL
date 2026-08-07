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
                reg = genAST(currentFile,n);
                printASM(currentFile,reg);
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
                if(searchFuncLocal(currentFuncId,globSymb) != -1){
                    printf("Error, local variable %s already exists in function %s\n",globSymb,lstFunc[currentFuncId].identName);
                    exit(1);
                }
                match(T_IDENTIFIER,"identifier");

                switch(Token.token){
                    case T_SEMI:
                        if(currentFuncId != -1){ // in a function
                            int idFuncLocal = addFuncLocal(currentFuncId,globSymb);
                            initFuncLocalASM(currentFile,lstFunc[currentFuncId].u.function.lstLocFunc[idFuncLocal].u.variable.stackPos);
                        }
                        else{ // in main
                            addSym(globSymb);
                            symbolGlobalASM(globSymb);
                        }
                        match(T_SEMI,";");
                        
                        break;
                    
                    case T_EQUALS:
                        match(T_EQUALS,"="); 
                        char *varName = strdup(globSymb);

                        globSymb = strdup(Text);
                        int findFunc = searchFunc(globSymb); 
                        
                        //if it's a function, put in reg the result of the function
                        if(findFunc != -1){ 
                            scan(&Token);
                            int nbArgs = lstFunc[findFunc].u.function.nbArgs;
                            pushParameters(nbArgs);

                            callFuncASM(currentFile,globSymb,nbArgs);
                            
                            reg = loadReturnFunc(currentFile);
                            
                        }
                        else{
                            n = parseExpression();
                            reg = genAST(currentFile,n);
                        }
                        // if we are in a function
                        if(currentFuncId != -1){
                        
                            int idFuncLocal = addFuncLocal(currentFuncId,varName);
                            setFuncLocalASM(currentFile,lstFunc[currentFuncId].u.function.lstLocFunc[idFuncLocal].u.variable.stackPos,reg);
                            
                        }//in main
                        else{
                            addSym(varName);
                            symbolGlobalASM(varName);
                            storeGlobalASM(currentFile,reg,varName);
                        }

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
                        
                        

                        int nbArgs = findParameters();
                        
                        addFunc(globSymb,nbArgs);
                        currentFile = getCurrentFile(0);
                        
                        //defition of the function
                        FILE *head = getCurrentFile(1);
                        definefuncASM(head,globSymb);
                        
                        
                        funcDef = 2;

                        parseStatements(currentWhile);

                        if(!hasReturn){
                            printf("Error, no return at line %d\n",line);
                            exit(1);
                        }
                        else{
                            hasReturn =0;
                        }

                        //reserve space for local variable;
                        localSpaceASM(head,lstFunc[currentFuncId].u.function.lastLocFunc);

                        //concat head and body file into functions file
                        int c;
                        rewind(head);
                        rewind(currentFile);

                        Functions = fopen("functions.s","w");

                        while((c = fgetc(head)) != EOF){
                            fputc(c,Functions);
                        }
                    
                        while((c = fgetc(currentFile)) != EOF){
                            fputc(c,Functions);
                        }
                        
                        //allows to redefine a new function
                        funcDef =1;

                        //set the current function to the next one (allows definiton of local variable)
                        currentFuncId++;

                        //close the "temporary" files
                        fclose(head);
                        fclose(currentFile);
                        fclose(Functions);

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
                        char *varName= strdup(globSymb);
                        match(T_EQUALS,"="); 

                        globSymb = strdup(Text);
                        int findFunc = searchFunc(globSymb);
                        //if it's a function, put in reg the result of the function
                        if(findFunc != -1){ 
                            scan(&Token);

                            int nbArgs = lstFunc[findFunc].u.function.nbArgs;

                            pushParameters(nbArgs);

                            callFuncASM(currentFile,globSymb,nbArgs);
                            
                            reg = loadReturnFunc(currentFile);
                            // scan(&Token);
                            
                        }
                        else{
                            n = parseExpression();
                            reg = genAST(currentFile,n);
                        }

                        
                        storeGlobalASM(currentFile,reg,varName);
                        match(T_SEMI,";");

                        break;
                    
                    case T_INC:
                        if(searchSym(globSymb) == -1){
                            printf("Error, symbol %s doesn't exists at line %d\n",globSymb,line);
                            exit(1);
                        }
                        match(T_INC,"++"); 
                        incASM(currentFile,globSymb);
                        match(T_SEMI,";");
                        
                        break;
                    
                    case T_DEC:
                        if(searchSym(globSymb) == -1){
                            printf("Error, symbol %s doesn't exists at line %d\n",globSymb,line);
                            exit(1);
                        }
                        match(T_DEC,"--"); 
                        decASM(currentFile,globSymb);
                        match(T_SEMI,";");

                        break;  
                    case T_LBRACKET:
                        if(searchFunc(globSymb) == -1){
                            printf("Error, function %s doesn't exists at line %d\n",globSymb,line);
                            exit(1);
                        }
                        int nbArgs = lstFunc[searchFunc(globSymb)].u.function.nbArgs;
                        pushParameters(nbArgs);
                        

                        callFuncASM(currentFile,globSymb,nbArgs);
                        
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
                compareForJumpASM(currentFile,n,&jumpIf);
                
                freeAllReg();
                match(T_RBRACKET,")");

                ifASM(currentFile,jumpIf,currentIf);
                parseStatements(currentWhile);
                

                scan(&Token);
                
                elseASM(currentFile,currentIf);
                
                if(Token.token == T_ELSE){
                    scan(&Token);
                    parseStatements(currentWhile);
                    scan(&Token);
                }

                endifASM(currentFile,currentIf);

                //jsp encore si c'est bien de le mettre ou pas
                freeAllReg();

                break;
            case T_WHILE:
                whileStatementNb++;
                
                char *jumpW;
                
                match(T_WHILE,"while");

                startwhileASM(currentFile,currentWhile);

                match(T_LBRACKET,"(");
                n = parseExpression();
                compareForJumpASM(currentFile,n,&jumpW);
                freeAllReg();
                match(T_RBRACKET,")");
                
                whileASM(currentFile,jumpW,currentWhile);

                parseStatements(whileStatementNb);

                endwhileASM(currentFile,currentWhile);

                //jsp encore si c'est bien de le mettre ou pas
                freeAllReg();
                
                scan(&Token);
                break;
            case T_BREAK:
                match(T_BREAK,"break");
                breakwhileASM(currentFile,currentWhile -1);
                match(T_SEMI,";");
                
                break;
            case T_RETURN:
                match(T_RETURN,"return");
                n = parseExpression();
                reg = genAST(currentFile,n);
                returnASM(currentFile,reg);
                
                match(T_SEMI,";");
                hasReturn = 1;

                endfuncASM(currentFile);
                break;
            case T_RCBRACKET:
                return;
            case T_EOF:
                return;
            case T_DEBUG:
                debug();
            default:
                printf("Error at line %d token at wrong place\n",line);
                exit(1);
        }
    }
}

