#ifndef extern_ 
#define extern_ extern
#endif

extern_ FILE *Outfile;
extern_ int line;
extern_ int Putback;
extern_ FILE *file;
extern_ struct token Token;
extern_ struct ASTNode *n;
extern_ int lastRead;
extern_ char Text[TEXTLEN + 1];
extern_ struct identifier lstIdent[MAXIDENT];
extern_ int lastLstIdent;
