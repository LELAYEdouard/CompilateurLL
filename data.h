#ifndef extern_ 
#define extern_ extern
#endif

extern_ FILE *Preamble;
extern_ FILE *Varfile;
extern_ FILE *Functions;
extern_ FILE *Main;
extern_ FILE *Head;
extern_ FILE *Body;
extern_ FILE *currentFile;
extern_ int line;
extern_ int Putback;
extern_ FILE *file;
extern_ struct token Token;
extern_ struct ASTNode *n;
extern_ int lastRead;
extern_ char Text[TEXTLEN + 1];
extern_ struct identifier lstIdent[MAXIDENT];
extern_ int lastLstIdent;
extern_ int ifStatementNb;
extern_ int whileStatementNb;
extern_ int maxBool;
extern_ int funcDef;
extern_ struct identifier lstFuncParam[MAXIDENT];
extern_ int lastLstFuncParam;
extern_ struct identifier lstFunc[MAXIDENT];
extern_ int lastLstFunc;
extern_ int currentFuncId;
extern_ int hasReturn;