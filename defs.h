#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define TEXTLEN 200
#define MAXIDENT 1024

//structure du token
struct token {
    int token;
    int intval;
};

enum {
    T_EOF, // EOF
    T_PLUS, // +
    T_MIN, // -
    T_MULT,// *
    T_DIV, // /
    T_EQUALS, // =
    
    T_PRINT, // print
    T_LBRACKET, // (
    T_RBRACKET, // )
    T_SEMI, // ;

    T_INTTYPE, // int
    
    T_IDENTIFIER, //identifier
    T_INTLT, // 0123456789
        
    T_BOOLEQ, // ==
    T_BOOLDIFF, // !=
    T_BOOLLT, // <
    T_BOOLGT, // >
    T_BOOLLE, // <=
    T_BOOLGE // >=
};

typedef struct ASTNode {
    int op;
    struct ASTNode *right;
    struct ASTNode *left;
    union{
        int intvalue;
        int idIdent;
    }u;
} ASTNode ;

enum {
    A_PLUS,
    A_MIN,
    A_MULT,
    A_DIV,
    A_OPPOSITE,

    A_INTLT,
    A_IDENTIFIER,

    A_BOOLEQ, 
    A_BOOLDIFF,
    A_BOOLLT, 
    A_BOOLGT, 
    A_BOOLLE, 
    A_BOOLGE
};

struct identifier {
    char *identName;
};
