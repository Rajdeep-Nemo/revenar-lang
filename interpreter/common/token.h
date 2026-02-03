#ifndef TOKEN_H
#define TOKEN_H
//Enum to hold all the token types
typedef enum
{
    //Punctuations
    TOKEN_SEMICOLON,
    //Operators
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL, TOKEN_BANG,
    //Literals
    TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_STRING,
    //Keywords
    TOKEN_IF, TOKEN_ELSE,
    //Meta
    TOKEN_ERROR, TOKEN_EOF
} TokenType;
//Struct to handle the token
typedef struct
{
    TokenType token;
    const char* start;
    int length;
    int line;
} Token;

#endif //TOKEN_H