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
        "\n"
        "main:\n"
        "\tpush\trbp\n"
        "\tmov	rbp, rsp\n",
    Outfile);
}

// Print out the assembly postamble
void postambleASM(){
    fputs(
        "\tmov	eax, 0\n"
        "\tpop	rbp\n"
        "\tret\n",
    Outfile);

    fputs(
        "\nsection .note.GNU-stack noalloc noexec nowrite progbits\n",
        Outfile
    );
}


int loadASM(int val){
    int reg = allocateReg();
    fprintf(Outfile,"\tmov\t%s, %d\n",reglist[reg],val);
    return reg;
}

int addASM(int reg1,int reg2){
    fprintf(Outfile,"\tadd\t%s, %s\n",reglist[reg1],reglist[reg2]);
    freeReg(reg2);
    return reg1;
}

int subASM(int reg1,int reg2){
    fprintf(Outfile,"\tsub\t%s, %s\n",reglist[reg1],reglist[reg2]);
    freeReg(reg2);
    return reg1;
}

int multASM(int reg1,int reg2){
    fprintf(Outfile,"\timul\t%s, %s\n",reglist[reg1],reglist[reg2]);
    freeReg(reg2);
    return reg1;
}

int divASM(int reg1,int reg2){
    fprintf(Outfile, "\tmov\trax, %s\n", reglist[reg1]);
    fprintf(Outfile, "\tcqo\n");
    fprintf(Outfile, "\tidiv\t%s\n", reglist[reg2]);
    fprintf(Outfile, "\tmov\t%s, rax\n", reglist[reg1]);
    freeReg(reg2);
    return reg1;
}

void printASM(int reg){
    fprintf(Outfile, "\tmov\trdi, %s\n", reglist[reg]);
    fprintf(Outfile, "\tcall\tprintint\n");
    freeAllReg();    
}

void symbolGlobalASM(char *str){
    fprintf(Outfile, "\tcommon\t%s 8:8\n",str);
}

int storeGlobalASM(int reg,char *identifier){
    fprintf(Outfile,"\tmov\t[%s], %s\n",identifier, reglist[reg]);
    return reg;
}

int loadGlobalASM(char *symbol){
    int reg = allocateReg();
    fprintf(Outfile,"\tmov\t%s, [%s]\n",reglist[reg],symbol);
    return reg;
}
int compareASM(int reg1, int reg2,char *compare){
    fprintf(Outfile, "\tcmp\t%s, %s\n", reglist[reg1], reglist[reg2]);
    fprintf(Outfile, "\t%s\t%s\n", compare, breglist[reg2]);
    fprintf(Outfile, "\tand\t%s,255\n", reglist[reg2]);
    freeReg(reg1);
    return reg2;
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

