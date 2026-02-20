//
//Copyright (c) 2026 Rajdeep Nemo and Sujay Paul
//
#include "lexer.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
// The Scanner struct holds the state of the lexer as it iterates through the source code.
typedef struct
{
    const char* start;      // Pointer to the start of the current token being scanned.
    const char* current;    // Pointer to the current character being looked at.
    int line;               // The current line number, for error reporting.
} Scanner;
// A single global instance of the scanner.
Scanner scanner;
// Initializes the scanner with the source code to be tokenized.
void initScanner(const char* source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}
// Checks if the scanner has reached the end of the source code.
bool isAtEnd(void)
{
    return *scanner.current == '\0';
}
// Consumes and returns the current character, and advances the scanner.
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
// Returns the character after the current one, without consuming it.
// Useful for lookahead of two characters.
char peekNext(void)
{
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}
// Skips over whitespace characters and comments.
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
        // Also handles single-line comments.
        case '/':
            if (peekNext() == '/')
            {
                // A comment goes until the end of the line.
                // Consume the two '/' characters.
                advance();
                advance();
                while (!isAtEnd() && peek() != '\n')
                {
                    advance();
                }
            } else
            {
                // Not a comment, so we're done skipping.
                return;
            }
            break;
        default:
            // If it's not whitespace or a comment, we're done.
            return;
        }
    }
}
// Creates a new token of the given type.
Token createToken(const TokenType token_type)
{
    Token token;
    token.token = token_type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}
// Creates an error token with a specific message.
Token errorToken(const char* message)
{
    Token token;
    token.token = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}
// Reads the entire content of a file into a dynamically allocated string.
// The caller is responsible for freeing the returned buffer.
static char *readFile(const char *path)
{
    FILE* file = fopen(path , "rb");
    if (file == NULL)
    {
        fprintf(stderr , "Could not open file \"%s\"\n",path);
        return NULL;
    }

    // Seek to the end of the file to determine its size.
    if (fseek(file, 0L, SEEK_END) != 0) {
        fprintf(stderr, "Could not seek to end of file \"%s\"\n", path);
        fclose(file);
        return NULL;
    }

    const long fileSize = ftell(file);
    if (fileSize < 0)
    {
        fprintf(stderr, "Could not determine size of file \"%s\"\n", path);
        fclose(file);
        return NULL;
    }
    const size_t uFileSize = (size_t) fileSize;
    rewind(file); // Go back to the start of the file.

    // Allocate enough memory for the file content plus a null terminator.
    char* buffer = malloc(uFileSize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr , "Not enough memory to read \"%s\"\n",path);
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer.
    const size_t bytesRead = fread(buffer , sizeof(char) , uFileSize , file);
    if (bytesRead < uFileSize)
    {
        fprintf(stderr , "Could not read file \"%s\"\n",path);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[bytesRead] = '\0'; // Null-terminate the string.
    fclose(file);
    return buffer;
}
// Checks if the current character matches 'expected'. If so, consumes it and returns true.
// Otherwise, returns false without consuming the character.
bool match(const char expected)
{
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;
    scanner.current++;
    return true;
}
// Helper to check if a character is a digit '0'-'9'.
bool isDigit(const char c)
{
    return c >= '0' && c <= '9';
}
// Helper to check if a character is an alphabet ('a'-'z', 'A'-'Z') or an underscore.
bool isAlpha(const char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
               c == '_';
}
// Scans a character literal, which is enclosed in single quotes.
static Token isCharLiteral(void)
{
    // 1. Check for empty literal ''
    if (peek() == '\'') {
        return errorToken("Empty character literal.");
    }

    // 2. Handle Escape Sequence
    if (peek() == '\\') {
        advance(); // consume '\'

        const char escaped = peek();
        switch (escaped) {
        case '\'': case '"': case '\\': case 'n':
        case '{':  case '}': case 't':  case 'r': case '0':
            advance(); // consume the valid escaped char
            break;
        default:
            return errorToken("Invalid escape sequence in character literal.");
        }
    }
    // 3. Handle Regular Character
    else {
        advance();
    }

    // 4. Ensure it closes correctly immediately after the character
    if (peek() != '\'') {
        return errorToken("Character literal must contain exactly one character.");
    }

    advance(); // Consume the closing '
    return createToken(TOKEN_CHAR_LITERAL);
}
// Scans a string literal, which is enclosed in double quotes.
// Supports multi-line strings and escape sequences.
static Token isStringLiteral(void)
{
    while (peek() != '"' && !isAtEnd())
    {
        // If newline then line count is increased, thus allowing multiline string
        if (peek() == '\n')
        {
            scanner.line++;
        }
        // To check escape sequence
        if (peek() == '\\')
        {
            advance(); // Consume the backslash
            if (isAtEnd())
            {
                return errorToken("Unterminated string after escape.");
            }
            switch (peek())
            {
                // Specified escape character list
            case '\'':
            case '"':
            case '\\':
            case 'n':
            case '{':
            case '}':
            case 't':
            case 'r':
            case '0':
                advance(); // Valid escape sequence, consume the char
                break;
            default:
                return errorToken("Invalid escape sequence.");
            }
        }
        else
        {
            advance();
        }
    }
    if (isAtEnd())
    {
        return errorToken("Unterminated string");
    }
    //Consume the closing quote
    advance();
    return createToken(TOKEN_STRING_LITERAL);
}
// Scans a number literal, which can be an integer or a floating-point number.
static Token isNumberLiteral(void)
{
    //Flag to check if an integer or float
    bool isFloat = false;
    //Consume digits before the decimal point.
    while (isDigit(peek()))
    {
        advance();
    }
    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext()))
    {
        isFloat = true;
        // Consume the "."
        advance();
        // Consume digits after the decimal point.
        while (isDigit(peek()))
        {
            advance();
        }
    }
    //Create a token based on whether a decimal point was found.
    return createToken(isFloat ? TOKEN_FLOAT_LITERAL : TOKEN_INT_LITERAL);
}
// A helper for identifierType to check if the scanned identifier matches a specific keyword.
// This is part of a hand-rolled trie for keyword matching.
TokenType checkKeyword(const int start ,const int length , const char* rest ,const TokenType type)
{
    if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start , rest , length) == 0)
    {
        return type;
    }
    return TOKEN_IDENTIFIER;
}
// Determines if an identifier is a keyword. This uses a series of nested switches
// to create a trie-like structure for efficient keyword matching.
static TokenType identifierType(void)
{
    switch (scanner.start[0])
    {
    case 'b':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'o': return checkKeyword(2 , 2 , "ol" , TOKEN_BOOL);
            case 'r': return checkKeyword(2 , 3 , "eak" , TOKEN_BREAK);
            default: ;
            }
        }
        break;
    case 'c':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'h': return checkKeyword(2 , 2 , "ar" , TOKEN_CHAR);
            case 'o':
                if (scanner.current - scanner.start > 2 && scanner.start[2] == 'n')
                {
                    if (scanner.current - scanner.start > 3)
                    {
                        switch (scanner.start[3])
                        {
                        case 's': return checkKeyword(4 , 1 , "t" , TOKEN_CONST);
                        case 't': return checkKeyword(4 , 4 , "inue" , TOKEN_CONTINUE);
                        default: ;
                        }
                    }
                }
                break;
            default: ;
            }
        }
        break;
    case 'd': return checkKeyword(1 , 1 , "o" , TOKEN_DO);
    case 'e': return checkKeyword(1 , 3 , "lse" , TOKEN_ELSE);
    case 'f':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'a': return checkKeyword(2 , 3 , "lse" , TOKEN_FALSE);
            case 'n': return checkKeyword(2 , 0 , "" , TOKEN_FN);
            case 'o': return checkKeyword(2 , 1 , "r" , TOKEN_FOR);
            case '3': return checkKeyword(2 , 1 , "2" , TOKEN_F32);
            case '6': return checkKeyword(2 , 1 , "4" , TOKEN_F64);
            default: ;
            }
        }
        break;
    case 'i':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'f': return checkKeyword(2 , 0 , "" , TOKEN_IF);
            case 'n': return checkKeyword(2 , 0 , "" , TOKEN_IN);
            case '8': return checkKeyword(2 , 0 , "" , TOKEN_I8);
            case '1': return checkKeyword(2 , 1 , "6" , TOKEN_I16);
            case '3': return checkKeyword(2 , 1 , "2" , TOKEN_I32);
            case '6': return checkKeyword(2 , 1 , "4" , TOKEN_I64);
            default: ;
            }
        }
        break;
    case 'l': return checkKeyword(1 , 3 , "oop" , TOKEN_LOOP);
    case 'm':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'a': return checkKeyword(2 , 3 , "tch" , TOKEN_MATCH);
            case 'u': return checkKeyword(2 , 1 , "t" , TOKEN_MUT);
            default: ;
            }
        }
        break;
    case 'n': return checkKeyword(1 , 3 , "ull" , TOKEN_NULL);
    case 'r': return checkKeyword(1 , 5 , "eturn" , TOKEN_RETURN);
    case 's': return checkKeyword(1 , 5 , "tring" , TOKEN_STRING);
    case 't': return checkKeyword(1 , 3 , "rue" , TOKEN_TRUE);
    case 'u':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case '8': return checkKeyword(2 , 0 , "" , TOKEN_U8);
            case '1': return checkKeyword(2 , 1 , "6" , TOKEN_U16);
            case '3': return checkKeyword(2 , 1 , "2" , TOKEN_U32);
            case '6': return checkKeyword(2 , 1 , "4" , TOKEN_U64);
            default: ;
            }
        }
        break;
    case 'v': return checkKeyword(1 , 3 , "oid" , TOKEN_VOID);
    case 'w': return checkKeyword(1 , 4 , "hile" , TOKEN_WHILE);

    default: ;
    }
    // If it doesn't match any keyword, it's a user-defined identifier.
    return TOKEN_IDENTIFIER;
}
// Scans an identifier or a keyword.
static Token isIdentifier(void)
{
    // Consume all alphanumeric characters (and underscores).
    while (isAlpha(peek()) || isDigit(peek()))
    {
        advance();
    }
    // Check if the identifier is a reserved keyword.
    return createToken(identifierType());
}
// The main entry point for the scanner. It scans and returns the next token from the source.
Token scanToken(void)
{
    // First, skip any leading whitespace or comments.
    skipWhitespace();
    // Set the start of the new token to the current position.
    scanner.start = scanner.current;

    // At the end of the file, return an EOF token.
    if (isAtEnd()) return createToken(TOKEN_EOF);

    const char c = advance();
    switch (c) {
        // Single-character tokens.
    case '(': return createToken(TOKEN_LEFT_PAREN);
    case ')': return createToken(TOKEN_RIGHT_PAREN);
    case '{': return createToken(TOKEN_LEFT_BRACE);
    case '}': return createToken(TOKEN_RIGHT_BRACE);
    case '[': return createToken(TOKEN_LEFT_BRACKET);
    case ']': return createToken(TOKEN_RIGHT_BRACKET);
    case ',': return createToken(TOKEN_COMMA);
    case ':': return createToken(TOKEN_COLON);
    case ';': return createToken(TOKEN_SEMICOLON);
    case '?': return createToken(TOKEN_QUESTION);
    case '.': return createToken(match('.') ? TOKEN_DOT_DOT : TOKEN_DOT);
        // One or two-character tokens.
    case '+': return createToken(match('=') ? TOKEN_PLUS_EQUAL : TOKEN_PLUS);
    case '*': return createToken(match('=') ? TOKEN_STAR_EQUAL : TOKEN_STAR);
    case '/': return createToken(match('=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH);
    case '%': return createToken(match('=') ? TOKEN_PERCENT_EQUAL : TOKEN_PERCENT);
    case '-': return createToken(match('>') ? TOKEN_ARROW : match('=') ? TOKEN_MINUS_EQUAL : TOKEN_MINUS);
    case '=': return createToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '!': return createToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '<': return createToken(match('<') ? TOKEN_LEFT_SHIFT : (match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS));
    case '>': return createToken(match('>') ? TOKEN_RIGHT_SHIFT : (match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER));
    case '&': return createToken(match('&') ? TOKEN_AND : TOKEN_BIT_AND);
    case '|': return createToken(match('|') ? TOKEN_OR : TOKEN_BIT_OR);
    case '^': return createToken(TOKEN_BIT_XOR);
    case '~': return createToken(TOKEN_BIT_NOT);

    // Literals
    case '\'':
        return isCharLiteral();
    case '"':
        return isStringLiteral();
    default:
        // Number literals start with a digit.
        if (isDigit(c)) return isNumberLiteral();
        // Identifiers start with a letter or underscore.
        if (isAlpha(c)) return isIdentifier();
        // If none of the above, it's an unexpected character.
        return errorToken("Unexpected character.");
    }
}
// Reads a source file, initializes the scanner, and returns the source buffer.
// The caller is responsible for freeing the returned buffer.
static char* runFile(const char* path)
{
    char* source = readFile(path);
    if (source == NULL) {
        // readFile will have printed an error.
        return NULL;
    }
    initScanner(source);
    return source;
}

// Main entry point for the lexer executable.
int main(const int argc , char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file.lk>\n", argv[0]);
        return 1;
    }

    char* source = runFile(argv[1]);
    if (source == NULL) {
        return 74;
    }

    int line = -1;
    for (;;) {
        Token token = scanToken();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }

        if (token.token == TOKEN_ERROR) {
            printf("Error: %.*s\n", token.length, token.start);
        } else {
            printf("Token %3d '%.*s'\n", token.token, token.length, token.start);
        }

        if (token.token == TOKEN_EOF) {
            break;
        }
    }

    free(source);
    return 0;
}
