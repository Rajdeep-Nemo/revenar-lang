//
//Copyright (c) 2026 Rajdeep Nemo and Sujay Paul
//
#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <stdbool.h>

//Function to initialize our scanner
void initScanner(const char* source);
//Function that checks if we read the complete file or not
bool isAtEnd(void);
//Function that moves the pointer 'current' forward
char advance(void);
//Function to check the next character
char peek(void);
//Function to check the second next character
char peekNext(void);
//Function to skip whitespace characters
void skipWhitespace(void);
//Function to create a token
Token createToken(TokenType token_type);
//Function for error reporting
Token errorToken(const char* message);
//Function to read input file into a buffer
char *readFile(const char *path);
//Helper function to evaluate conditional advances - '!=' , '=='
bool match(char expected);
//Helper function to check if it is a number literal
bool isDigit(char c);
//Helper function to check if it is a number literal
bool isAlpha(char c);
//Function to evaluate tokens
Token scanToken(void);
//Function to manage the process
void runFile(const char* path);

#endif
