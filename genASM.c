#include "defs.h"
#include "data.h"
#include "decl.h"

#define NBREG 6

int freeregister[NBREG] = {1,1,1,1,1,1};
char *reglist[NBREG] = {"r8","r9","r10","r11","r12","r13"};
char *breglist[NBREG] = {"r8b","r9b","r10b","r11b","r12b","r13b"};

void freeReg(int reg){
    if(freeregister[reg]){
        printf("Error, trying to free register %s\n",reglist[reg]);
        exit(1);
    }
    freeregister[reg] = 1;
}

int allocateReg(){
    for(int i = 0;i<NBREG;i++){
        if(freeregister[i]){
            freeregister[i]=0;
            return i;
        }
    }
    printf("Error, no register left!\n");
    exit(1);
}

void freeAllReg(){
    freeregister[0] = freeregister[1] = freeregister[2] = freeregister[3] = 1;
}

//pre and postamble taken from the github

// Print out the assembly preamble
void preambleASM(){
    freeAllReg();
    fputs(
        "\tglobal\tmain\n"
        "\textern\tprintf\n"
        "\tsection\t.text\n"
        "LC0:\tdb\t\"%d\",10,0\n"
        "printint:\n"
        "\tpush\trbp\n"
        "\tmov\trbp, rsp\n"
        "\tsub\trsp, 16\n"
        "\tmov\t[rbp-4], edi\n"
        "\tmov\teax, [rbp-4]\n"
        "\tmov\tesi, eax\n"
        "\tlea	rdi, [rel LC0]\n"
        "\tmov	eax, 0\n"
        "\tcall	printf\n"
        "\tnop\n"
        "\tleave\n"
        "\tret\n"
        "\n",
    Preamble);
}

void mainASM(){
    fputs(
        "main:\n"
        "\tpush\trbp\n"
        "\tmov	rbp, rsp\n",
    Main);
}
// Print out the assembly postamble
void postambleASM(){
    fputs(
        "\tmov	eax, 0\n"
        "\tpop	rbp\n"
        "\tret\n",
    Main);

    fputs(
        "\nsection .note.GNU-stack noalloc noexec nowrite progbits\n",
        Main
    );
}

void variableASM(){
    fputs(
        "section\t.bss\n",
    Varfile);
}


int loadASM(int val){
    int reg = allocateReg();
    fprintf(Main,"\tmov\t%s, %d\n",reglist[reg],val);
    return reg;
}

int addASM(int reg1,int reg2){
    fprintf(Main,"\tadd\t%s, %s\n",reglist[reg1],reglist[reg2]);
    freeReg(reg2);
    return reg1;
}

int subASM(int reg1,int reg2){
    fprintf(Main,"\tsub\t%s, %s\n",reglist[reg1],reglist[reg2]);
    freeReg(reg2);
    return reg1;
}

int multASM(int reg1,int reg2){
    fprintf(Main,"\timul\t%s, %s\n",reglist[reg1],reglist[reg2]);
    freeReg(reg2);
    return reg1;
}

int divASM(int reg1,int reg2){
    fprintf(Main, "\tmov\trax, %s\n", reglist[reg1]);
    fprintf(Main, "\tcqo\n");
    fprintf(Main, "\tidiv\t%s\n", reglist[reg2]);
    fprintf(Main, "\tmov\t%s, rax\n", reglist[reg1]);
    freeReg(reg2);
    return reg1;
}

void incASM(char *identifier){
    fprintf(Main,"\tinc\tbyte\t[%s]\n",identifier);
}

void decASM(char *identifier){
    fprintf(Main,"\tdec\tbyte [%s]\n",identifier);
}


void printASM(int reg){
    fprintf(Main, "\tmov\trdi, %s\n", reglist[reg]);
    fprintf(Main, "\tcall\tprintint\n");
    freeAllReg();    
}

void symbolGlobalASM(char *str){
    fprintf(Varfile, "\t%s: resq 1\n",str);
}

void storeGlobalASM(int reg,char *identifier){
    fprintf(Main,"\tmov\t[%s], %s\n",identifier, reglist[reg]);
    freeReg(reg);
}

int loadGlobalASM(char *symbol){
    int reg = allocateReg();
    fprintf(Main,"\tmov\t%s, [%s]\n",reglist[reg],symbol);
    return reg;
}

int loadFuncGlobalASM(int stackPos){
    int reg = allocateReg();
    fprintf(Main,"\tmov\t%s, [rbp+%d]\n",reglist[reg],stackPos);
    return reg;
}

int loadFuncLocalASM(int stackPos){
    int reg = allocateReg();
    fprintf(Main,"\tmov\t%s, [rbp-%d]\n",reglist[reg],stackPos);
    return reg;
}

int loadReturnFunc(){
    int reg = allocateReg();
    fprintf(Main,"\tmov\t%s, rax\n",reglist[reg]);
    return reg;
}

int compareASM(int reg1, int reg2,char *compare){
    fprintf(Main, "\tcmp\t%s, %s\n", reglist[reg1], reglist[reg2]);
    fprintf(Main, "\t%s\t%s\n", compare, breglist[reg2]);
    fprintf(Main, "\tand\t%s,255\n", reglist[reg2]);
    freeReg(reg1);
    return reg2;
}

int compareForJumpASM(struct ASTNode *n,char **jump){
    int left,right=0;
    if(n->left){
        left = compareForJumpASM(n->left,jump);
    }
    if(n->right){
        right = compareForJumpASM(n->right,jump);
    }

    if(left == -1){
        return 0;
    }

    switch(n->op){
        case A_PLUS:
            return addASM(left,right);
        case A_MIN:
            return subASM(left,right);
        case A_MULT:
            return multASM(left,right);
        case A_DIV:
            return divASM(left,right);
        case A_INTLT:
            return loadASM(n->u.intvalue);
        case A_IDENTIFIER:
            return loadGlobalASM(lstIdent[n->u.idIdent].identName);
        case A_FUNCIDENTIFIER:
            return loadFuncGlobalASM(lstFuncParam[n->u.idIdent].u.variable.stackPos);
        case A_FUNCLOCAL:
            return loadFuncLocalASM(lstFunc[currentFuncId].u.variable.stackPos);
        case A_OPPOSITE:
            return subASM(left,right);
        case A_BOOLDIFF:
            fprintf(Main, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("je");
            return -1;
        case A_BOOLEQ:
            
            fprintf(Main, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jne");
            return -1;
        case A_BOOLGE:
            fprintf(Main, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jl");
            return -1;
        case A_BOOLGT:
            fprintf(Main, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jle");
            return -1;
        case A_BOOLLE:
            fprintf(Main, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jg");
            return -1;
        case A_BOOLLT:
            fprintf(Main, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jge");
            return -1;
        default:
            exit(1);
    }
}

void startwhileASM(int currentWhile){
    fprintf(Main, "\tW%d:\n",currentWhile);
}

void whileASM(char *jump,int currentWhile){
    fprintf(Main, "\t%s Wend%d\n",jump,currentWhile);
}

void endwhileASM(int currentWhile){
    fprintf(Main, "\tjmp W%d\n",currentWhile);
    fprintf(Main, "\tWend%d:\n",currentWhile);
}

void breakwhileASM(int currentWhile){
    fprintf(Main, "\tjmp Wend%d\n",currentWhile);
}

void ifASM(char *jump,int currentIf){
    fprintf(Main, "\t%s ELSE%d\n",jump,currentIf);
}

void elseASM(int currentIf){
    fprintf(Main, "\tjmp IFend%d\n",currentIf);
    fprintf(Main, "\tELSE%d: \n",currentIf);
}

void endifASM(int currentIf){
    fprintf(Main, "\tIFend%d:\n",currentIf);
}

void definefuncASM(char *func){
    fprintf(Main,"%s:\n",func);
    fprintf(Main,"\tpush\trbp\n");
    fprintf(Main,"\tmov\trbp, rsp\n");
}

void endfuncASM(){
    fprintf(Main,"\tleave\n");
    fprintf(Main,"\tret\n");
}

void returnASM(int reg){
    fprintf(Main,"\tmov rax, %s\n",reglist[reg]);
}

void callFuncASM(char *str,int nbArgs){
    fprintf(Main,"\tcall %s\n",str);
    fprintf(Main,"\tadd rsp, %d\n",nbArgs*8);
}

void pushASM(int reg){
    fprintf(Main,"\tpush %s\n",reglist[reg]);
    freeReg(reg);
}

void initFuncLocalASM(int stackPos){
    fprintf(Main,"\tmov\tQWORD [rbp-%d], 0\n",stackPos);
}

void setFuncLocalASM(int stackPos,int reg){
    fprintf(Main,"\tmov\tQWORD [rbp-%d], %s\n",stackPos,reglist[reg]);
    freeReg(reg);
}

//gen code from an AST
int genAST(struct ASTNode *n){
    int left,right=0;
    if(n->left){
        left = genAST(n->left);
    }
    if(n->right){
        right = genAST(n->right);
    }

    switch(n->op){
        case A_PLUS:
            return addASM(left,right);
        case A_MIN:
            return subASM(left,right);
        case A_MULT:
            return multASM(left,right);
        case A_DIV:
            return divASM(left,right);
        case A_INTLT:
            return loadASM(n->u.intvalue);
        case A_IDENTIFIER:
            return loadGlobalASM(lstIdent[n->u.idIdent].identName);
        case A_FUNCIDENTIFIER:
            return loadFuncGlobalASM(lstFuncParam[n->u.idIdent].u.variable.stackPos);
        case A_FUNCLOCAL:
            return loadFuncLocalASM(lstFunc[currentFuncId].u.function.lstLocFunc[n->u.intvalue].u.variable.stackPos);
        case A_OPPOSITE:
            return subASM(left,right);
        case A_BOOLDIFF:
            return compareASM(left,right,"setne");
        case A_BOOLEQ:
            return compareASM(left,right,"sete");
        case A_BOOLGE:
            return compareASM(left,right,"setge");
        case A_BOOLGT:
            return compareASM(left,right,"setg");
        case A_BOOLLE:
            return compareASM(left,right,"setle");
        case A_BOOLLT:
            return compareASM(left,right,"setl");
        default:
            exit(1);
    }
}
