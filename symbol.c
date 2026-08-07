#include "defs.h"
#include "data.h"
#include "decl.h"

int searchSym(char *str){
    for(int id=0;id<lastLstIdent;id++){
        if(!strcmp(str,lstIdent[id].identName)){
            return id;
        }
    }
    return -1;
}

int createSym(){
    if(lastLstIdent == MAXIDENT){
        printf("Error, too many global symbols at line %d\n",line);
        exit(1);
    }
    lastLstIdent++;
    
    return lastLstIdent - 1;
}

int addSym(char *str){
    
    int sym;
    
    sym = searchSym(str);
    
    if(sym != -1){
        return sym;
    }
    
    sym = createSym();
    lstIdent[sym].identName = strdup(str);
    return sym;
    
}

int searchFuncSym(char *str){
    for(int id=0;id<lastLstFuncParam;id++){
        
        if(!strcmp(str,lstFuncParam[id].identName)){
            return id;
        }
    }
    return -1;
}


int createFuncSym(){
    if(lastLstFuncParam == MAXIDENT){
        printf("Error, too many symbols for the function at line %d\n",line);
        exit(1);
    }
    lastLstFuncParam++;

    return lastLstFuncParam - 1;
}

int addFuncSym(char *str,int stackPos){

    int sym;

    sym = searchFuncSym(str);

    if(sym != -1){
        return sym;
    }

    sym = createFuncSym();
    lstFuncParam[sym].identName = strdup(str);
    lstFuncParam[sym].u.stackPos = stackPos;
    return sym;
    
}



int searchFunc(char *str){
    for(int id=0;id<lastLstFunc;id++){
        if(!strcmp(str,lstFunc[id].identName)){
            return id;
        }
    }
    return -1;
}


int createFunc(){
    if(lastLstFunc == MAXIDENT){
        printf("Error, too many symbols for the function at line %d\n",line);
        exit(1);
    }
    lastLstFunc++;

    return lastLstFunc - 1;
}

int addFunc(char *str,int nbArgs){

    int sym;

    sym = searchFunc(str);

    if(sym != -1){
        return sym;
    }

    sym = createFunc();  
    lstFunc[sym].identName = strdup(str);
    lstFunc[sym].u.nbArgs = nbArgs;

    return sym;
    
}
