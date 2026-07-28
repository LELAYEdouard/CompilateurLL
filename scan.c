#include "defs.h"
#include "data.h"
#include "decl.h"

//recupere le prochaine charactere du fichier
static int next(){
    int c;

    if(Putback){
        c = Putback;
        Putback = 0;
        return c;
    }

    c = fgetc(file); //recupere le charactere
    lastRead = c;

    if('\n' == c){ //compte la ligne actuelle
        line++;
    }

    return c;
}

//remet la charactere dans le input stream
static void putback(int c){
    Putback = c;
}


//revoie la position du charatere dans la chaine ou -1
static int chrpos(char *str , int c){
    char *p;

    p = strchr(str, c);

    return (p ? p - str : -1);
}

//recupere l'integer scanné
static int scanint(int c){
    int k , val = 0;

    while ((k = chrpos("0123456789",c)) >= 0){
        val = val *10 +k;
        c = next();
    }

    putback(c);
    return val;
}

//skip jusqu'au prochain charactere
static int skip(){
    int c;

    c = next();
    while('\n'==c || '\t'==c || ' '==c || '\f'==c || '\r'==c){
        c = next();
    }
    return c;
}

void scanident(int c,char *str){
    int i = 0 ;

    while(isalpha(c) || isdigit(c) || '_' == c){
        if(i == TEXTLEN - 1){
            printf("identifier too long on line %d\n",line);
            exit(1);
        }
        str[i] = c;
        i++;
        c=next();
    }
    putback(c);
    str[i] = '\0';
}

int keyword(char *str){
    switch(*str){
        case 'm':
            if(!strcmp(str,"main")){
                return T_MAIN;
            }
            break;
        case 'p':
            if(!strcmp(str,"print")){
                return T_PRINT;
            }
            break;
        case 'i':
            if(!strcmp(str,"int")){
                return T_INTTYPE;
            }
            if(!strcmp(str,"if")){
                return T_IF;
            }
            break;
        case 'e':
            if(!strcmp(str,"else")){
                return T_ELSE;
            }
            break;
    }
    return 0;
}

//scan et renvoie le token lu, retourne 1 si le token est valide, 0 si plus de token
int scan(struct token *t){
    int c,tokentype;
    
    c = skip();

    switch(c){
        case EOF:
            t->token = T_EOF;
            return 0;

        case '+':
            t->token = T_PLUS;
            break;
        case '-':
            t->token = T_MIN;
            break;
        case '*':
            t->token = T_MULT;
            break;
        case '/':
            t->token = T_DIV;
            break;
        case ';':
            t->token = T_SEMI;
            break;
        case '(':
            t->token = T_LBRACKET;
            break;
        case ')':
            t->token = T_RBRACKET;
            break;
        case '{':
            t->token = T_LCBRACKET;
            break;
        case '}':
            t->token = T_RCBRACKET;
            break;
        case '=':
            if('=' == (c = next())){
                t->token = T_BOOLEQ;
                break;
            }
            else{
                putback(c);
                t->token = T_EQUALS;
            }
            break;
        case '!':
            if('=' == (c = next())){
                t->token = T_BOOLDIFF;
                break;
            }
            else{
                // putback(c);
                // t->token = ;
                printf("Error at line %d, character : %c is not recognised\n",line,c);
                exit(1);
            }
            break;
        case '<':
            if('=' == (c = next())){
                t->token = T_BOOLLE;
                break;
            }
            else{
                putback(c);
                t->token = T_BOOLLT;
            }
            break;
        case '>':
            if('=' == (c = next())){
                t->token = T_BOOLGE;
                break;
            }
            else{
                putback(c);
                t->token = T_BOOLGT;
            }
            break;
        default:
            if(isdigit(c)){
                t->token = T_INTLT;
                t->intval = scanint(c);
                break;
            }
            else if(isalpha(c) || '_' == c){
                scanident(c,Text);
                tokentype = keyword(Text);
                if(tokentype){
                    t->token = tokentype;
                    break;
                }
                else{
                    t->token = T_IDENTIFIER;
                    
                    break;
                }
            }
            else{
                printf("Error at line %d, character : %c is not recognised\n",line,c);
                exit(1);
            }
    }
    return 1;
}

