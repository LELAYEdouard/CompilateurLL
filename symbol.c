#include "defs.h"
#include "data.h"
#include "decl.h"

int searchSym(char *str){
    int id;
    for(int id=0;id<lastLstIdent;id++){
        if(!strcmp(str,lstIdent[id].identName)){
            return id;
        }
    }
    return -1;
}

int createSym(){
    if(lastLstIdent == MAXIDENT){
        printf("Error, too many global symbols\n");
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
