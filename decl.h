//expr
int getop(int tokenOperator);
struct ASTNode* parseExpression();
struct ASTNode* parseExpr();
struct ASTNode* parseBool();
struct ASTNode* parseAdd();
struct ASTNode* parseMult();
struct ASTNode* parseInt();

//symbol
int searchSym(char *str);
int createSym();
int addSym(char *str);

//scan
void scanident(int c,char *str);
int keyword(char *str);
int scan(struct token *t);

//misc
void match(int tok,char *str);

void debugINT(int val);
void debugSTR(char *val);
void debugCHAR(char val);
void debug();

//tree
struct ASTNode* mkleft();
struct ASTNode* mkleaf(int op,int val);
struct ASTNode* mknode(int op,struct ASTNode *left,struct ASTNode *right,int value);

//stmt
void parseStatements(int currentWhile);

//genASM
void freeReg(int reg);
int allocateReg();
void freeAllReg();

void preambleASM();
void postambleASM();
void variableASM();

int loadASM(int val);
int addASM(int reg1,int reg2);
int subASM(int reg1,int reg2);
int multASM(int reg1,int reg2);
int divASM(int reg1,int reg2);
void incASM(char *identifier);
void decASM(char *identifier);

void printASM(int reg);
void symbolGlobalASM(char *str);
void storeGlobalASM(int reg,char *identifier);

int compareForJumpASM(struct ASTNode *n,char **jump);

void startwhileASM(int currentWhile);
void whileASM(char *jump,int currentWhile);
void endwhileASM(int currentWhile);
void breakwhileASM(int currentWhile);

void ifASM(char *jump,int currentIf);
void elseASM(int currentIf);
void endifASM(int currentIf);

int genAST(struct ASTNode *n);

