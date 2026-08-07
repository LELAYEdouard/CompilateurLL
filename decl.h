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

int searchFuncSym(char *str);
int createFuncSym();
int addFuncSym(char *str,int stackPos);


int searchFunc(char *str);
int createFunc();
int addFunc(char *str,int nbArgs);

int searchFuncLocal(int idFunc,char *str);
int createFuncLocal(int idFunc);
int addFuncLocal(int idFunc,char *str);

//scan
void scanident(int c,char *str);
int keyword(char *str);
int scan(struct token *t);

//misc
FILE *getCurrentFile(int isHead);
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

//function
int findParameters();
void pushParameters(int nbArgs);

//genASM
void freeReg(int reg);
int allocateReg();
void freeAllReg();

void preambleASM();
void postambleASM();
void mainASM();
void variableASM();

int loadASM(FILE* file,int val);
int addASM(FILE* file,int reg1,int reg2);
int subASM(FILE* file,int reg1,int reg2);
int multASM(FILE* file,int reg1,int reg2);
int divASM(FILE* file,int reg1,int reg2);
void incASM(FILE* file,char *identifier);
void decASM(FILE* file,char *identifier);

void printASM(FILE* file,int reg);
void symbolGlobalASM(char *str);
void storeGlobalASM(FILE* file,int reg,char *identifier);

int loadReturnFunc(FILE* file);

int compareForJumpASM(FILE* file,struct ASTNode *n,char **jump);

void startwhileASM(FILE* file,int currentWhile);
void whileASM(FILE* file,char *jump,int currentWhile);
void endwhileASM(FILE* file,int currentWhile);
void breakwhileASM(FILE* file,int currentWhile);

void ifASM(FILE* file,char *jump,int currentIf);
void elseASM(FILE* file,int currentIf);
void endifASM(FILE* file,int currentIf);

void definefuncASM(FILE* file,char *func);
void endfuncASM(FILE* file);
void returnASM(FILE* file,int reg);
void callFuncASM(FILE* file,char *str,int nbArgs);
void pushASM(FILE* file,int reg);
void initFuncLocalASM(FILE* file,int stackPos);
void setFuncLocalASM(FILE* file,int stackPos,int reg);
void localSpaceASM(FILE* file,int localSpace);

int genAST(FILE* file,struct ASTNode *n);

