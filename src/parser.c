/*
 * state machine:
 *
 * CODE          (EXPR-BLOCK|FUNCTION)*
 * EXPR-BLOCK    EXPRESSION -> BLOCK
 * FUNCTION      "function" -> IDENTIFIER -> FUNCARGS -> BLOCK
 * IDENTIFIER    "[a-zA-Z_][a-zA-Z_0-9]*"
 * FUNCARGS      "(" -> (IDENTIFIER -> ",")* -> ")"
 * ...
 *
 */

#include "lib/common.h"

#define NDEBUG


#define dynarray(type) \
    typedef struct { \
        type *data; \
        size_t size, alloc; \
    }

#define push(array, elem) \
    do { \
        if ((array)->size == (array)->alloc) { \
            (array)->alloc = ((array)->alloc + 10) * 2; \
            (array)->data = realloc((array)->data, sizeof *((array)->data) * (array)->alloc); \
        } \
        (array)->data[(array)->size++] = elem; \
    } while (0)



#pragma GCC diagnostic ignored "-Wmultichar"
enum {
    CURO   = '{' , CURC   = '}' ,
    BRKO   = '[' , BRKC   = ']' ,
    PARO   = '(' , PARC   = ')' ,

    LT     = '<' , LTE    = '<=',
    GT     = '>' , GTE    = '>=',
    ADD    = '+' , ADDADD = '++', ADDE   = '+=',
    SUB    = '-' , SUBSUB = '--', SUBE   = '-=',
    MUL    = '*' ,                MULE   = '*=',
    DIV    = '/' ,                DIVE   = '/=',
    HAT    = '^' ,                HATE   = '^=',
    AND    = '&' , ANDAND = '&&', ANDE   = '&=',
    OR     = '|' , OROR   = '||', ORE    = '|=',
    TILDE  = '~' ,
    EXCLAM = '!' ,
    QUEST  = '?' , COLON  = ':' ,

    DOLLAR = '$' ,
    COMMA  = ',' ,

    IDENTIFIER = 'ID',
    NUMBER = 'NU',
    STRING = 'ST',

    BEGIN    = 'BE',
    BREAK    = 'BR',
    CONTINUE = 'CO',
    DELETE   = 'DE',
    ELSE     = 'EL',
    END      = 'EN',
    FOR      = 'FO',
    FUNCTION = 'FU',
    IF       = 'IF',
    IN       = 'IN',
    NEXT     = 'NE',
    RETURN   = 'RE',
    WHILE    = 'WH',

};
// debug purposes only
static char *syntax[] = {
    [CURO]   = "{" , [CURC]   = "}" ,
    [BRKO]   = "[" , [BRKC]   = "]" ,
    [PARO]   = "(" , [PARC]   = ")" ,

    [LT]     = "<" , [LTE ]   = "<=",
    [GT]     = ">" , [GTE ]   = ">=",
    [ADD]    = "+" , [ADDADD] = "++", [ADDE]  = "+=",
    [SUB]    = "-" , [SUBSUB] = "--", [SUBE]  = "-=",
    [MUL]    = "*" ,                  [MULE]  = "*=",
    [DIV]    = "/" ,                  [DIVE]  = "/=",
    [HAT]    = "^" ,                  [HATE]  = "^=",
    [AND]    = "&" , [ANDAND] = "&&", [ANDE]  = "&=",
    [OR]     = "|" , [OROR ]  = "||", [ORE]   = "|=",
    [TILDE]  = "~" ,
    [EXCLAM] = "!" ,
    [QUEST]  = "?" , [COLON]  = ":" ,

    [DOLLAR] = "$" ,
    [COMMA]  = "," ,

    [IDENTIFIER] = "IDENTIFIER",
    [NUMBER] = "NUMBER",
    [STRING] = "STRING",

    [BEGIN   ] = "BE",
    [BREAK   ] = "BR",
    [CONTINUE] = "CO",
    [DELETE  ] = "DE",
    [ELSE    ] = "EL",
    [END     ] = "EN",
    [FOR     ] = "FOR",
    [FUNCTION] = "FUNCTION",
    [IF      ] = "IF",
    [IN      ] = "IN",
    [NEXT    ] = "NE",
    [RETURN  ] = "RETURN",
    [WHILE   ] = "WHILE",
};

dynarray(char) str;

struct token {
    int type;
    str data;
};





// only tokenizer calls this
static struct token *token(FILE *file) {
    static struct token t;
    int c;

    memset(&t, 0, sizeof t);

    // strip line comments and whitespace
    while (1) {
        c = getc(file);
        if (c == EOF) return 0;
        if (!isspace(c) && c != '#') break;
        if (c == '#') {
            while (c != EOF && c != '\n') c = getc(file);
            if (c != EOF) ungetc(c, file);
        }
    }


    switch (c) {
        // 1-char tokens
        case '(': case ')':
        case '{': case '}':
        case '[': case ']':
        case ';':
        case '?': case ':':
        case '~':
        case '!':
        case '$':
        case ',':
            t.type = c;
            break;

        // 1/2-char tokens
        case '+': case '-':
        case '&': case '|':
        case '=':
            t.type = c;
            if ((c = getc(file)) == EOF) goto out;

            if (c == '=' || c == t.type) t.type = t.type << 8 | c;
            else ungetc(c, file);
            break;

        case '*': case '/':
        case '^': case '%':
            t.type = c;
            if ((c = getc(file)) == EOF) goto out;

            if (c == '=') t.type = t.type << 8 | c;
            else ungetc(c, file);
            break;

        // numbers
        case casedec:
            t.type = NUMBER;
            do {
                if (isdigit(c)) push(&t.data, c);
                else {
                    ungetc(c, file);
                    break;
                }
            } while ((c = getc(file)) != EOF);
            push(&t.data, 0);
            break;

        // strings
        // "(\\.|[^\\"])*"
        case '"':
            t.type = STRING;
            while ((c = getc(file)) != EOF && c != '"') {
                if (c != '\\') push(&t.data, c);
                else if ((c = getc(file)) != EOF) {
                    switch (c) {
                        case 'n':  push(&t.data, '\n'); break;
                        case 'f':  push(&t.data, '\f'); break;
                        case 'v':  push(&t.data, '\v'); break;
                        case 'b':  push(&t.data, '\b'); break;
                        case 'r':  push(&t.data, '\r'); break;
                        case 't':  push(&t.data, '\t'); break;
                        case '"':  push(&t.data, '"' ); break;
                        case '\\': push(&t.data, '\\'); break;
                        // todo hex oct
                    }
                }
                else return 0;
            }
            push(&t.data, 0);
            break;

        // identifiers
        case casealpha: case '_':
            t.type = IDENTIFIER;
            do {
                if (isalnum(c) || c == '_') push(&t.data, c);
                else {
                    ungetc(c, file);
                    break;
                }
            } while ((c = getc(file)) != EOF);
            push(&t.data, 0);

            if (!strcmp(t.data.data, "BEGIN"   )) t.type = BEGIN;
            if (!strcmp(t.data.data, "break"   )) t.type = BREAK;
            if (!strcmp(t.data.data, "continue")) t.type = CONTINUE;
            if (!strcmp(t.data.data, "delete"  )) t.type = DELETE;
            if (!strcmp(t.data.data, "else"    )) t.type = ELSE;
            if (!strcmp(t.data.data, "END"     )) t.type = END;
            if (!strcmp(t.data.data, "for"     )) t.type = FOR;
            if (!strcmp(t.data.data, "function")) t.type = FUNCTION;
            if (!strcmp(t.data.data, "if"      )) t.type = IF;
            if (!strcmp(t.data.data, "in"      )) t.type = IN;
            if (!strcmp(t.data.data, "next"    )) t.type = NEXT;
            if (!strcmp(t.data.data, "return"  )) t.type = RETURN;
            if (!strcmp(t.data.data, "while"   )) t.type = WHILE;
            break;
    }

out:
    return &t;
}


dynarray(struct token) tokenarray;
static tokenarray tokens;
static ssize_t tokpos;


static struct token *nexttoken() {
    return tokpos <= tokens.size ? tokens.data + tokpos++ : 0;
}

static struct token *prevtoken() {
    return tokpos >= 0           ? tokens.data + tokpos-- : 0;
}

static void tokenizer(const char *code) {
    FILE *file = fmemopen((void *)code, strlen(code), "r");

    for (struct token *t; (t = token(file)); )
        push(&tokens, *t);

    fclose(file);
}

dynarray(struct token *) funcargs;

typedef struct {
    struct token *name;
    funcargs args;
    //block body;
} funcdef;

dynarray(funcdef) func;
func funcs;




static void function() {
    // function f(arg1, arg2, arg3) { body }
    assert(nexttoken()->type == FUNCTION);

    funcdef f = { .name = nexttoken() };
    assert(f.name->type == IDENTIFIER);

    push(&funcs, f);
    assert(nexttoken()->type == PARO);

    for (struct token *t; t = nexttoken(), t->type != PARC; ) {
        if (t->type == IDENTIFIER) push(&f.args, t);
        if (nexttoken()->type == COMMA) { /* skip it */ }
        else prevtoken();

    }
//        if (t->type == IDENTIFIER) {
//            if (nexttoken()->type == 
 //       }
}
#if 0





void code() {
    for (char *t; t = token(); ) {
        if (!strcmp(t, "function")) function();
        else exprblock();
    }
}
#endif

int main() {
    char *text = "function meow(moo, bark) { return woof(69 + 420asdf1234\"asdf   tab<\t>qqq\") }";
    puts(text);

    tokenizer(text);

    for (size_t i = 0; i < tokens.size; i++) {
        printf("type: %-20s ", syntax[tokens.data[i].type]);
        if (tokens.data[i].type == STRING || tokens.data[i].type == IDENTIFIER || tokens.data[i].type == NUMBER)
            printf("%s", tokens.data[i].data.data);
        printf("\n");
    }

    return 0;
}
