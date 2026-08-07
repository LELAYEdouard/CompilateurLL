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


int loadASM(FILE* file, int val){
    int reg = allocateReg();
    fprintf(file,"\tmov\t%s, %d\n",reglist[reg],val);
    return reg;
}

int addASM(FILE* file, int reg1,int reg2){
    fprintf(file,"\tadd\t%s, %s\n",reglist[reg1],reglist[reg2]);
    freeReg(reg2);
    return reg1;
}

int subASM(FILE* file, int reg1,int reg2){
    fprintf(file,"\tsub\t%s, %s\n",reglist[reg1],reglist[reg2]);
    freeReg(reg2);
    return reg1;
}

int multASM(FILE* file, int reg1,int reg2){
    fprintf(file,"\timul\t%s, %s\n",reglist[reg1],reglist[reg2]);
    freeReg(reg2);
    return reg1;
}

int divASM(FILE* file, int reg1,int reg2){
    fprintf(file, "\tmov\trax, %s\n", reglist[reg1]);
    fprintf(file, "\tcqo\n");
    fprintf(file, "\tidiv\t%s\n", reglist[reg2]);
    fprintf(file, "\tmov\t%s, rax\n", reglist[reg1]);
    freeReg(reg2);
    return reg1;
}

void incASM(FILE* file, char *identifier){
    fprintf(file,"\tinc\tbyte\t[%s]\n",identifier);
}

void decASM(FILE* file, char *identifier){
    fprintf(file,"\tdec\tbyte [%s]\n",identifier);
}


void printASM(FILE* file, int reg){
    fprintf(file, "\tmov\trdi, %s\n", reglist[reg]);
    fprintf(file, "\tcall\tprintint\n");
    freeAllReg();    
}

void symbolGlobalASM(char *str){
    fprintf(Varfile, "\t%s: resq 1\n",str);
}

void storeGlobalASM(FILE* file, int reg,char *identifier){
    fprintf(file,"\tmov\t[%s], %s\n",identifier, reglist[reg]);
    freeReg(reg);
}

int loadGlobalASM(FILE* file, char *symbol){
    int reg = allocateReg();
    fprintf(file,"\tmov\t%s, [%s]\n",reglist[reg],symbol);
    return reg;
}

int loadFuncGlobalASM(FILE* file, int stackPos){
    int reg = allocateReg();
    fprintf(file,"\tmov\t%s, [rbp+%d]\n",reglist[reg],stackPos);
    return reg;
}

int loadFuncLocalASM(FILE* file, int stackPos){
    int reg = allocateReg();
    fprintf(file,"\tmov\t%s, [rbp-%d]\n",reglist[reg],stackPos);
    return reg;
}

int loadReturnFunc(FILE* file){
    int reg = allocateReg();
    fprintf(file,"\tmov\t%s, rax\n",reglist[reg]);
    return reg;
}

int compareASM(FILE* file, int reg1, int reg2,char *compare){
    fprintf(file, "\tcmp\t%s, %s\n", reglist[reg1], reglist[reg2]);
    fprintf(file, "\t%s\t%s\n", compare, breglist[reg2]);
    fprintf(file, "\tand\t%s,255\n", reglist[reg2]);
    freeReg(reg1);
    return reg2;
}

int compareForJumpASM(FILE* file, struct ASTNode *n,char **jump){
    int left,right=0;
    if(n->left){
        left = compareForJumpASM(file, n->left,jump);
    }
    if(n->right){
        right = compareForJumpASM(file, n->right,jump);
    }

    if(left == -1){
        return 0;
    }

    switch(n->op){
        case A_PLUS:
            return addASM(file, left,right);
        case A_MIN:
            return subASM(file, left,right);
        case A_MULT:
            return multASM(file, left,right);
        case A_DIV:
            return divASM(file, left,right);
        case A_INTLT:
            return loadASM(file, n->u.intvalue);
        case A_IDENTIFIER:
            return loadGlobalASM(file, lstIdent[n->u.idIdent].identName);
        case A_FUNCIDENTIFIER:
            return loadFuncGlobalASM(file, lstFuncParam[n->u.idIdent].u.variable.stackPos);
        case A_FUNCLOCAL:
            return loadFuncLocalASM(file, lstFunc[currentFuncId].u.variable.stackPos);
        case A_OPPOSITE:
            return subASM(file, left,right);
        case A_BOOLDIFF:
            fprintf(file, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("je");
            return -1;
        case A_BOOLEQ:
            
            fprintf(file, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jne");
            return -1;
        case A_BOOLGE:
            fprintf(file, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jl");
            return -1;
        case A_BOOLGT:
            fprintf(file, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jle");
            return -1;
        case A_BOOLLE:
            fprintf(file, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jg");
            return -1;
        case A_BOOLLT:
            fprintf(file, "\tcmp\t%s, %s\n", reglist[left], reglist[right]);
            *jump = strdup("jge");
            return -1;
        default:
            exit(1);
    }
}

void startwhileASM(FILE* file, int currentWhile){
    fprintf(file, "\tW%d:\n",currentWhile);
}

void whileASM(FILE* file, char *jump,int currentWhile){
    fprintf(file, "\t%s Wend%d\n",jump,currentWhile);
}

void endwhileASM(FILE* file, int currentWhile){
    fprintf(file, "\tjmp W%d\n",currentWhile);
    fprintf(file, "\tWend%d:\n",currentWhile);
}

void breakwhileASM(FILE* file, int currentWhile){
    fprintf(file, "\tjmp Wend%d\n",currentWhile);
}

void ifASM(FILE* file, char *jump,int currentIf){
    fprintf(file, "\t%s ELSE%d\n",jump,currentIf);
}

void elseASM(FILE* file, int currentIf){
    fprintf(file, "\tjmp IFend%d\n",currentIf);
    fprintf(file, "\tELSE%d: \n",currentIf);
}

void endifASM(FILE* file, int currentIf){
    fprintf(file, "\tIFend%d:\n",currentIf);
}

void definefuncASM(FILE* file, char *func){
    fprintf(file,"%s:\n",func);
    fprintf(file,"\tpush\trbp\n");
    fprintf(file,"\tmov\trbp, rsp\n");
}

void endfuncASM(FILE* file){
    fprintf(file,"\tleave\n");
    fprintf(file,"\tret\n");
}

void returnASM(FILE* file, int reg){
    fprintf(file,"\tmov rax, %s\n",reglist[reg]);
}

void callFuncASM(FILE* file, char *str,int nbArgs){
    fprintf(file,"\tcall %s\n",str);
    fprintf(file,"\tadd rsp, %d\n",nbArgs*8);
}

void pushASM(FILE* file, int reg){
    fprintf(file,"\tpush %s\n",reglist[reg]);
    freeReg(reg);
}

void initFuncLocalASM(FILE* file, int stackPos){
    fprintf(file,"\tmov\tQWORD [rbp-%d], 0\n",stackPos);
}

void setFuncLocalASM(FILE* file, int stackPos,int reg){
    fprintf(file,"\tmov\tQWORD [rbp-%d], %s\n",stackPos,reglist[reg]);
    freeReg(reg);
}

void localSpaceASM(FILE* file,int localSpace){
    fprintf(file,"\tsub\trsp, %d\n",8*localSpace);
}

//gen code from an AST
int genAST(FILE* file, struct ASTNode *n){
    int left,right=0;
    if(n->left){
        left = genAST(file, n->left);
    }
    if(n->right){
        right = genAST(file, n->right);
    }

    switch(n->op){
        case A_PLUS:
            return addASM(file, left,right);
        case A_MIN:
            return subASM(file, left,right);
        case A_MULT:
            return multASM(file, left,right);
        case A_DIV:
            return divASM(file, left,right);
        case A_INTLT:
            return loadASM(file, n->u.intvalue);
        case A_IDENTIFIER:
            return loadGlobalASM(file, lstIdent[n->u.idIdent].identName);
        case A_FUNCIDENTIFIER:
            return loadFuncGlobalASM(file, lstFuncParam[n->u.idIdent].u.variable.stackPos);
        case A_FUNCLOCAL:
            return loadFuncLocalASM(file, lstFunc[currentFuncId].u.function.lstLocFunc[n->u.intvalue].u.variable.stackPos);
        case A_OPPOSITE:
            return subASM(file, left,right);
        case A_BOOLDIFF:
            return compareASM(file, left,right,"setne");
        case A_BOOLEQ:
            return compareASM(file, left,right,"sete");
        case A_BOOLGE:
            return compareASM(file, left,right,"setge");
        case A_BOOLGT:
            return compareASM(file, left,right,"setg");
        case A_BOOLLE:
            return compareASM(file, left,right,"setle");
        case A_BOOLLT:
            return compareASM(file, left,right,"setl");
        default:
            exit(1);
    }
}
