#include "expressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)


#define MAX_STACK 512
typedef struct {
    float data[MAX_STACK];
    int top;
} FloatStack;

void initFloatStack(FloatStack *s) { s->top = -1; }
void pushFloat(FloatStack *s, float v) { if (s->top < MAX_STACK - 1) s->data[++(s->top)] = v; }
float popFloat(FloatStack *s) { return (s->top >= 0) ? s->data[(s->top)--] : 0.0f; }


typedef struct {
    char texto[512];
    int prioridade; 
} NodeExpr;


typedef struct {
    NodeExpr data[MAX_STACK];
    int top;
} ExprStack;

void initExprStack(ExprStack *s) { s->top = -1; }
void pushExpr(ExprStack *s, NodeExpr val) { if (s->top < MAX_STACK - 1) s->data[++(s->top)] = val; }
NodeExpr popExpr(ExprStack *s) { 
    NodeExpr vazio = {"", 0};
    return (s->top >= 0) ? s->data[(s->top)--] : vazio; 
}



int ehNumero(char *token) {
 
    return isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]));
}

int obterPrioridadeOp(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '^') return 3;
    return 0;
}



float getValorPosFixa(char *StrPosFixa) {
    if (!StrPosFixa) return 0.0;

    FloatStack pilha;
    initFloatStack(&pilha);


    char copia[512];
    strncpy(copia, StrPosFixa, 511);
    copia[511] = '\0';

    char *token = strtok(copia, " ");
    
    while (token != NULL) {
        if (ehNumero(token)) {
            pushFloat(&pilha, strtof(token, NULL));
        } else {
            
            if (strcmp(token, "sen") == 0 || strcmp(token, "cos") == 0 || 
                strcmp(token, "tg") == 0 || strcmp(token, "log") == 0 || 
                strcmp(token, "raiz") == 0) {
                
                float val = popFloat(&pilha);
                float res = 0.0;

                if (strcmp(token, "sen") == 0) res = sin(DEG_TO_RAD(val));
                else if (strcmp(token, "cos") == 0) res = cos(DEG_TO_RAD(val));
                else if (strcmp(token, "tg") == 0) res = tan(DEG_TO_RAD(val));
                else if (strcmp(token, "log") == 0) res = log10(val);
                else if (strcmp(token, "raiz") == 0) res = sqrt(val);
                
                pushFloat(&pilha, res);
            }
            
            else {
                float b = popFloat(&pilha);
                float a = popFloat(&pilha);
                float res = 0.0;

                switch (token[0]) {
                    case '+': res = a + b; break;
                    case '-': res = a - b; break;
                    case '*': res = a * b; break;
                    case '/': res = a / b; break;
                    case '%': res = fmod(a, b); break;
                    case '^': res = pow(a, b); break;
                    default: break;
                }
                pushFloat(&pilha, res);
            }
        }
        token = strtok(NULL, " ");
    }

    return popFloat(&pilha);
}

char * getFormaInFixa(char *Str) {
    if (!Str) return NULL;

    ExprStack pilha;
    initExprStack(&pilha);

    char copia[512];
    strncpy(copia, Str, 511);
    copia[511] = '\0';

    char *token = strtok(copia, " ");

    while (token != NULL) {
        if (ehNumero(token)) {
            NodeExpr novo;
            strcpy(novo.texto, token);
            novo.prioridade = 10; 
            pushExpr(&pilha, novo);
        } else {
            
            if (strcmp(token, "sen") == 0 || strcmp(token, "cos") == 0 || 
                strcmp(token, "tg") == 0 || strcmp(token, "log") == 0 || 
                strcmp(token, "raiz") == 0) {
                
                NodeExpr a = popExpr(&pilha);
                NodeExpr novo;
                
                sprintf(novo.texto, "%s(%s)", token, a.texto);
                novo.prioridade = 10; 
                pushExpr(&pilha, novo);
            } 
            
            else {
                NodeExpr b = popExpr(&pilha); 
                NodeExpr a = popExpr(&pilha); 
                NodeExpr novo;
                
                int pOp = obterPrioridadeOp(token[0]);
                
                
                char ladoEsq[256], ladoDir[256];
                
                if (a.prioridade < pOp) sprintf(ladoEsq, "(%s)", a.texto);
                else strcpy(ladoEsq, a.texto);


                if (b.prioridade < pOp || (b.prioridade == pOp && (token[0] == '-' || token[0] == '/' || token[0] == '^'))) {
                     sprintf(ladoDir, "(%s)", b.texto);
                } else {
                     strcpy(ladoDir, b.texto);
                }

                
                sprintf(novo.texto, "%s%s%s", ladoEsq, token, ladoDir);
                novo.prioridade = pOp;
                pushExpr(&pilha, novo);
            }
        }
        token = strtok(NULL, " ");
    }

    
    NodeExpr resultado = popExpr(&pilha);
    

    char *retorno = (char*) malloc(sizeof(char) * 512);
    if (retorno) {
        strcpy(retorno, resultado.texto);
    }
    return retorno;
}