//
//Copyright (c) 2026 RAJDEEP NEMO and SUJAY PAUL
//
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//struct to iterate through the source code
typedef struct
{
    const char* start;
    const char* current;
    int line;
} Scanner;
//Instance created
Scanner scanner;
//Function to initialize our scanner
void initScanner(const char* source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}
//Function that checks if we read the complete file or not
bool isAtEnd(void)
{
    if (*scanner.current == '\0')
    {
        return true;
    }
    return false;
}
//Function that moves the pointer 'current' forward
char advance(void)
{
    if (isAtEnd()) return '\0';
    return *scanner.current++;
}
//Function to check the next character
char peek(void)
{
    return *scanner.current;
}
//Function to check the second next character
char peekNext(void)
{
    if (*scanner.current == '\0' && scanner.current[1] == '\0')
    {
        return '\0';
    }
    return scanner.current[1];
}
//Function to skip whitespace characters
void skipWhitespace(void)
{
    while (true)
    {
        const char current_char = peek();

        switch (current_char)
        {
        case ' ':
        case '\t':
        case '\r':
            advance();
            break;
        case '\n':
            scanner.line++;
            advance();
            break;
        case '/':
            if (peekNext() == '/')
            {
                while (!isAtEnd() && *scanner.current != '\n')
                {
                    advance();
                }
            } else
            {
                return;
            }
            break;
        default:
            return;
        }
    }
}
//Function to create a token
Token createToken(const TokenType token_type)
{
    Token token;
    token.token = token_type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}
//Function for error reporting
Token errorToken(const char* message)
{
    Token token;
    token.token = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}
//Function to read input file into a buffer
char *readFile(const char *path)
{
    //Opens the file
    FILE* file = fopen(path , "rb");
    //Checks if the file exists
    if (file == NULL)
    {
        fprintf(stderr , "Could not open file \"%s\"\n",path);
        exit(74);
    }
    //Check the end of the file to get the size and check for errors
    if (fseek(file, 0L, SEEK_END) != 0) {
        fprintf(stderr, "Could not seek to end of file \"%s\"\n", path);
        exit(74);
    }
    //fileSize contains the size of the file in long, as error can return -1
    const long fileSize = ftell(file);
    //Checks if fileSize is non-negative, means size is found without any error
    if (fileSize < 0)
    {
        fprintf(stderr, "Could not determine size of file \"%s\"\n", path);
        exit(74);
    }
    //fileSize(long) is being casted to size_t(unsigned int) for larger buffer
    const size_t uFileSize = (size_t) fileSize;
    rewind(file);
    //Allocate memory for the buffer (+1 for the NULL terminator)
    char* buffer = malloc(uFileSize + 1);
    //Checks if allocation is successful
    if (buffer == NULL)
    {
        fprintf(stderr , "Not enough memory to read \"%s\"\n",path);
        exit(74);
    }
    //Read bytes into the buffer
    const size_t bytesRead = fread(buffer , sizeof(char) , uFileSize , file);
    //Checks if reading was successful
    if (bytesRead < uFileSize)
    {
        fprintf(stderr , "Could not read file \"%s\"\n",path);
        exit(74);
    }
    //Null terminate the string
    buffer[bytesRead] = '\0';
    //Cleanup and return
    fclose(file);
    return buffer;

}
//Helper function to evaluate conditional advances - '!=' , '=='
bool match(const char expected)
{
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;
    scanner.current++;
    return true;
}
//Function to evaluate tokens
Token scanToken(void)
{
    skipWhitespace();
    scanner.start = scanner.current;
    //If the pointer hits '\0' the program stops
    if (isAtEnd()) return createToken(TOKEN_EOF);

    const char c = advance();
    switch (c) {
        //Single character symbols
    case ';':
        return createToken(TOKEN_SEMICOLON);
    case ',':
        return createToken(TOKEN_COMMA);
        //One or Two character operators
    case '!':
        return createToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
        return createToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);\
    case '-':
            return createToken(match('>') ? TOKEN_ARROW : TOKEN_MINUS);
        //Returns when an unexpected character is found
        //Operator

    default:
        return errorToken("Unexpected character.");
    }
}
//Function to manage the process
void runFile(const char* path)
{
    //Read the file into source
    const char* source = readFile(path);
    //Initialize the scanner with the source
    initScanner(source);
    //The lexing loop
    while (true)
    {
        const Token token = scanToken();
        //debug test print statement
        printf("%4d | Type: %2d | '%.*s'\n",
               token.line, token.token, token.length, token.start);

        if (token.token == TOKEN_EOF)
        {
            break;
        }
    }
    free((void*)source);
}

int main(const int argc , char *argv[])
{
    if (argc == 1)
    {
        printf("No input file provided.\n");
        printf("Usage: %s <file.rv>\n" , argv[0]);
        printf("Program terminated.\n");
    }
    else if (argc == 2)
    {
        runFile(argv[1]);
    }
    else if (argc > 2)
    {
        printf("Too many arguments.\n");
        printf("Usage: %s <file.rv>\n" , argv[0]);
        printf("Program terminated.\n");
    }

    return 0;
}

