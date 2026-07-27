#include "defs.h"
#include "data.h"
#include "decl.h"

struct ASTNode *mkleft(){
    struct ASTNode *n;

    switch(Token.token){
        case T_INTLT:
            n = mkleaf(A_INTLT,Token.intval);
            scan(&Token);
            return n;
        default:
            printf("error on line %d, unknown character: %c\n",line,lastRead);
            exit(1);
    }
    
    
}

struct ASTNode* mkleaf(int op,int val){
    return mknode(op,NULL,NULL,val);
}

struct ASTNode* mknode(int op,struct ASTNode *left,struct ASTNode *right,int value){
    struct ASTNode *n;

    n = (struct ASTNode*) malloc(sizeof(struct ASTNode));

    n->op = op;
    n->right = right;
    n->left = left;
    n->u.intvalue = value;

    return n;

}