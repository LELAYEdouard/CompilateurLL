#include "defs.h"
#include "data.h"
#include "decl.h"

int getop(int tokenOperator){
    switch(tokenOperator){
        case T_PLUS:
            return A_PLUS;
        case T_MIN:
            return A_MIN;
        case T_MULT:
            return A_MULT;
        case T_DIV:
            return A_DIV;
        case T_BOOLDIFF:
            return A_BOOLDIFF;
        case T_BOOLEQ:
            return A_BOOLEQ;
        case T_BOOLGE:
            return A_BOOLGE;
        case T_BOOLGT:
            return A_BOOLGT;
        case T_BOOLLE:
            return A_BOOLLE;
        case T_BOOLLT:
            return A_BOOLLT;
        default:
            printf("Error, unknown operator: %c at line %d\n",lastRead,line);
            exit(1);
    }
}
//scan expression
struct ASTNode* parseExpression(){
    struct ASTNode *left;
    
    left = parseExpr();
    maxBool = 0;

    return left;
}
//scan expression
struct ASTNode* parseExpr(){
    struct ASTNode *left;

    if(maxBool == 0){
        left = parseBool();
    }
    if(maxBool > 1){
        printf("Error, more than one boolean statement at line %d\n",line);
        exit(1);
    }

    return left;
}

//scan bool
struct ASTNode* parseBool(){
    struct ASTNode *left,*right;
    int tokentype;

    left = parseAdd();

    tokentype = Token.token;
    if(tokentype == T_SEMI){
        return left;
    }

    while(tokentype == T_BOOLEQ || tokentype == T_BOOLDIFF || tokentype == T_BOOLLE || tokentype == T_BOOLGE || tokentype == T_BOOLLT || tokentype == T_BOOLGT){
        maxBool++;
        scan(&Token);

        right = parseAdd();

        left = mknode(getop(tokentype),left,right,0);

        tokentype = Token.token;
    }

    return left;
}

//scan additions
struct ASTNode* parseAdd(){
    struct ASTNode *left,*right;
    int tokentype;

    left = parseMult();

    tokentype = Token.token;
    if(tokentype == T_SEMI){
        return left;
    }

    while(tokentype == T_PLUS || tokentype == T_MIN){

        scan(&Token);
        right = parseMult();

        left = mknode(getop(tokentype),left,right,0);

        tokentype = Token.token;
    }
    
    return left;
}
//scan multiplications
struct ASTNode* parseMult(){
    struct ASTNode *left,*right;
    int tokentype;

    left = parseInt();

    tokentype = Token.token;
    if(tokentype == T_SEMI){
        return left;
    }

    while(tokentype == T_MULT || tokentype == T_DIV){

        scan(&Token);
        right = parseInt();

        left = mknode(getop(tokentype),left,right,0);

        tokentype = Token.token;
    }

    return left;
}
//scan an INTEGER
struct ASTNode *parseInt(){
    struct ASTNode *n,*left,*right;

    switch(Token.token){
        case T_INTLT:
            n = mkleaf(A_INTLT,Token.intval);
            break;

        case T_IDENTIFIER:
            int id = searchSym(Text);

            int stackPos = searchFuncSym(Text);


            if(id == -1 && stackPos == -1){
                printf("Error, symbol %s doesn't exists at line %d\n",Text,line);
                exit(1);
            }

            if(id != -1){
                n = mkleaf(A_IDENTIFIER,id);
            }
            else if(stackPos != -1){
                n = mkleaf(A_FUNCIDENTIFIER,stackPos);
            }
            break;
        
        case T_LBRACKET:
            match(T_LBRACKET,"(");
            n = parseExpr();
            match(T_RBRACKET,")");
            return n;

        case T_MIN:
            left = mkleaf(A_INTLT,0);
            scan(&Token);
            right = parseInt();
            n = mknode(A_OPPOSITE,left,right,0);
            return n;

        case T_EOF:
            printf("Error, no value after operator on line %d\n",line);
            exit(1);
        default:
            printf("Error, unknown value: %c on line %d\n",lastRead,line);
            exit(1);
    }

    scan(&Token);
    return n;
}
