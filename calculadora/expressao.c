#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



typedef struct {
    char expr[512];
    int precedence;
} Node;


static int get_precedence(const char *op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    return 4; 
}

static int is_operator(const char *tok) {
    return (strcmp(tok, "+") == 0 ||
            strcmp(tok, "-") == 0 ||
            strcmp(tok, "*") == 0 ||
            strcmp(tok, "/") == 0 ||
            strcmp(tok, "%") == 0 ||
            strcmp(tok, "^") == 0);
}

static int is_function(const char *tok) {
    return (strcmp(tok, "sen") == 0 ||
            strcmp(tok, "cos") == 0 ||
            strcmp(tok, "tg")  == 0 ||
            strcmp(tok, "raiz") == 0 ||
            strcmp(tok, "log") == 0);
}

static int is_number(const char *tok) {
    int i = 0;
    int has_digit = 0;
    if (tok[0] == '-' && tok[1] != '\0')
        i = 1;
    for (; tok[i] != '\0'; i++) {
        if (isdigit((unsigned char)tok[i]) || tok[i] == '.') {
            has_digit = 1;
        } else {
            return 0;
        }
    }
    return has_digit;
}


static int get_next_token(const char **p, char *token, int max_len) {
    const char *s = *p;
    char *t = token;

  
    while (*s && isspace((unsigned char)*s)) s++;
    if (!*s) {
        *p = s;
        token[0] = '\0';
        return 0;
    }

    if (isdigit((unsigned char)*s) || *s == '.' ||
       ((*s == '-') && (isdigit((unsigned char) *(s+1)) || *(s+1) == '.'))) {
        
        *t++ = *s++;
        while (*s && (isdigit((unsigned char)*s) || *s == '.')) {
            if (t - token < max_len - 1) {
                *t++ = *s;
            }
            s++;
        }
    } else if (isalpha((unsigned char)*s)) {
        while (*s && isalpha((unsigned char)*s)) {
            if (t - token < max_len - 1) {
                *t++ = *s;
            }
            s++;
        }
    } else {
        *t++ = *s++;
    }

    *t = '\0';
    *p = s;
    return 1;
}


static float aplica_func_unaria(const char *f, float x) {
    if (strcmp(f, "sen") == 0) {
        return sinf((float)(x * (float)M_PI / 180.0f));
    } else if (strcmp(f, "cos") == 0) {
        return cosf((float)(x * (float)M_PI / 180.0f));
    } else if (strcmp(f, "tg") == 0) {
        return tanf((float)(x * (float)M_PI / 180.0f));
    } else if (strcmp(f, "raiz") == 0) {
        if (x < 0) {
            return NAN;
        }
        return sqrtf(x);
    } else if (strcmp(f, "log") == 0) {
        if (x <= 0) {
            return NAN;
        }
        return log10f(x);
    }
    return NAN;
}


float getValorPosFixa(char *StrPosFixa) {
    float stack[512];
    int top = -1;

    const char *p = StrPosFixa;
    char tok[64];

    while (get_next_token(&p, tok, sizeof(tok))) {
        if (tok[0] == '\0') break;

        if (is_number(tok)) {
            if (top >= 511) return NAN;
            stack[++top] = strtof(tok, NULL);
        } else if (is_function(tok)) {
            if (top < 0) return NAN;
            float a = stack[top--];
            float r = aplica_func_unaria(tok, a);
            if (isnan(r)) return NAN;
            stack[++top] = r;
        } else if (is_operator(tok)) {
            if (top < 1) return NAN;
            float b = stack[top--];
            float a = stack[top--];
            float r;

            if (strcmp(tok, "+") == 0) {
                r = a + b;
            } else if (strcmp(tok, "-") == 0) {
                r = a - b;
            } else if (strcmp(tok, "*") == 0) {
                r = a * b;
            } else if (strcmp(tok, "/") == 0) {
                if (b == 0.0f) return NAN;
                r = a / b;
            } else if (strcmp(tok, "%") == 0) {
                if ((int)b == 0) return NAN;
                r = (float)((int)a % (int)b);
            } else if (strcmp(tok, "^") == 0) {
                r = powf(a, b);
            } else {
                return NAN;
            }

            stack[++top] = r;
        } else {
            return NAN;
        }
    }

    if (top != 0) {
        return NAN;
    }

    return stack[0];
}



char * getFormaInFixa(char *Str) {
    static char result[512];

    Node stack[256];
    int top = -1;

    const char *p = Str;
    char tok[64];

    while (get_next_token(&p, tok, sizeof(tok))) {
        if (tok[0] == '\0') break;

        if (is_number(tok)) {
            if (top >= 255) return NULL;
            Node n;
            strncpy(n.expr, tok, sizeof(n.expr));
            n.expr[sizeof(n.expr)-1] = '\0';
            n.precedence = 4;
            stack[++top] = n;
        } else if (is_function(tok)) {
            if (top < 0) return NULL;
            Node a = stack[top--];
            Node n;
            n.precedence = 4;

            char buf[512];
            snprintf(buf, sizeof(buf), "%s(%s)", tok, a.expr);
            strncpy(n.expr, buf, sizeof(n.expr));
            n.expr[sizeof(n.expr)-1] = '\0';

            stack[++top] = n;
        } else if (is_operator(tok)) {
            if (top < 1) return NULL;
            Node b = stack[top--];
            Node a = stack[top--];

            int prec = get_precedence(tok);

            char left[512];
            char right[512];

            if (a.precedence < prec) {
                snprintf(left, sizeof(left), "(%s)", a.expr);
            } else {
                strncpy(left, a.expr, sizeof(left));
                left[sizeof(left)-1] = '\0';
            }

            if (b.precedence < prec ||
               ((strcmp(tok, "-") == 0 || strcmp(tok, "/") == 0 || strcmp(tok, "%") == 0) && b.precedence == prec) ||
               ((strcmp(tok, "^") == 0) && b.precedence == prec)) {
                snprintf(right, sizeof(right), "(%s)", b.expr);
            } else {
                strncpy(right, b.expr, sizeof(right));
                right[sizeof(right)-1] = '\0';
            }

            char buf[512];
            snprintf(buf, sizeof(buf), "%s%s%s", left, tok, right);

            Node n;
            strncpy(n.expr, buf, sizeof(n.expr));
            n.expr[sizeof(n.expr)-1] = '\0';
            n.precedence = prec;

            stack[++top] = n;
        } else {
            return NULL;
        }
    }

    if (top != 0) {
        return NULL;
    }
    strncpy(result, stack[0].expr, sizeof(result));
    result[sizeof(result)-1] = '\0';

    return result;
}
