#include "ibp.h"

/*
         ------ Token Type Map ------
   Specials      Stack Ops   Math Ops   Bool Ops
  EOF     = 1     DUP = 6     + = 11     =  = 16
  NEWLINE = 2     POP = 7     - = 12     !  = 17
  NUMBER  = 3     SWP = 8     * = 13     >  = 18
  VAR     = 4     OVR = 9     / = 14     <  = 19
  STRING  = 5     ROT = 10    % = 15

  Bitwise Ops     I/O Ops     New Op Creator
   AND = 20       DGT = 23      :   = 27
   ORR = 21       LTR = 24      BYE = 28
   INV = 22       STK = 25
                  RTN = 26
*/

struct Token *lex(char *input) {
    // iterate over the input stream and tokenize it.
    // catch any invalid tokens and throw the appropriate error
    // if no errors in the tokens are detected, return the list of tokens

    struct Token *tokens = NULL;
    char *substr;
    int curr_pos, key;

    for(int i = 0; i < strlen(input); i++) {
    
        switch(input[i]) {
            case ' ':
                break;
            case '\n':
                break;
            case '+':
                substr = malloc(2 * sizeof(char));
                substr[0] = '+';
                substr[1] = '\0';
                create_token(&tokens, substr, 11);
                free(substr);
                break;
            case '-':
                substr = malloc(2 * sizeof(char));
                substr[0] = '-';
                substr[1] = '\0';
                create_token(&tokens, substr, 12);
                free(substr);
                break;
            case '*':
                substr = malloc(2 * sizeof(char));
                substr[0] = '*';
                substr[1] = '\0';
                create_token(&tokens, substr, 13);
                free(substr);
                break;
            case '/':
                substr = malloc(2 * sizeof(char));
                substr[0] = '/';
                substr[1] = '\0';
                create_token(&tokens, substr, 14);
                free(substr);
                break;
            case '%':
                substr = malloc(2 * sizeof(char));
                substr[0] = '%';
                substr[1] = '\0';
                create_token(&tokens, substr, 15);
                free(substr);
                break;
            case '=':
                substr = malloc(2 * sizeof(char));
                substr[0] = '=';
                substr[1] = '\0';
                create_token(&tokens, substr, 16);
                free(substr);
                break;
            case '!':
                substr = malloc(2 * sizeof(char));
                substr[0] = '!';
                substr[1] = '\0';
                create_token(&tokens, substr, 17);
                free(substr);
                break;
            case '>':
                substr = malloc(2 * sizeof(char));
                substr[0] = '>';
                substr[1] = '\0';
                create_token(&tokens, substr, 18);
                free(substr);
                break;
            case '<':
                substr = malloc(2 * sizeof(char));
                substr[0] = '<';
                substr[1] = '\0';
                create_token(&tokens, substr, 19);
                free(substr);
                break;
            case ':':
                substr = malloc(2 * sizeof(char));
                substr[0] = ':';
                substr[1] = '\0';
                create_token(&tokens, substr, 27);
                free(substr);
                break;
            case '\"':
                i++; // get our current position passed the first qutoation
                curr_pos = i; // save the position of the first character of the string
                while(input[i] != '\"') {
                    // iterate over the string checking for illegal characters
                    if(input[i] == '\n' || input[i] == '\t' || input[i] == '\r' || input[i] == '\\' || input[i] == '%') {
                        printf("illegal char in string...\n");
                        exit(1);
                    }
                    i++; // increase the iterator so we keep moving through the file
                }
                // allocate memory for and copy the contents of the string 
                substr = malloc((i - curr_pos + 1) * sizeof(char));
                memcpy(substr, &input[curr_pos], i - curr_pos + 1);
                substr[i - curr_pos] = '\0'; // set the null-terminator
                create_token(&tokens, substr, 5);
                free(substr);
                break;
            default:
                // If we're dealing with an instruction word
                if(isalpha(input[i])) {
                    curr_pos = i;

                    while(isalnum(input[i]))
                        i++;
                    
                    substr = malloc((i - curr_pos + 1) * sizeof(char));
                    memcpy(substr, &input[curr_pos], i - curr_pos + 1);
                    substr[i - curr_pos] = '\0';

                    strupper(substr); // make sure the letters are uppercase
                    key = iskeyword(substr);
                    if(key)
                        create_token(&tokens, substr, key);
                    else
                        create_token(&tokens, substr, 30);

                    free(substr);
                }
                // if we're dealing with an integer literal
                else if(isdigit(input[i])) {
                    // save the current position
                    curr_pos = i;
                    while(isdigit(input[i]))
                        i++;

                    if(input[i] == '.') {
                        printf("no floats allowed...\n");
                        exit(1);
                    }

                    substr = malloc((i - curr_pos + 1) * sizeof(char));
                    memcpy(substr, &input[curr_pos], i - curr_pos + 1);
                    substr[i - curr_pos] = '\0'; // set the null-terminator
                    create_token(&tokens, substr, 3);
                    free(substr);
                    i--;
                }
                // an unrecognized token
                else {
                    printf("what?\n");
                }
                break;
        }
    }

    return tokens;
}

/*
Takes a double pointer to a linked list of tokens to which we will append a new node with the 
text of source(the second arg) and a type of type(the third arg).
*/
void create_token(struct Token **head, char *source, int type) {
    struct Token **tmp = head, *add;
    int source_len = strlen(source);
    
    add = malloc(sizeof(struct Token));
    add->src = malloc(source_len + 1 * sizeof(char));
    add->type = type;
    strcpy(add->src, source);
    add->next = NULL;
    
    while(*tmp)
        tmp = &(*tmp)->next;
    
    add->next = *tmp;
    *tmp = add;
}

int iskeyword(char *name) {
    char keys[28][7] = {
        "EOF", "NEWLINE", "NUMBER",
        "VAR", "STRING", "DUP",
        "POP", "SWP", "OVR", "ROT",
        "+", "-", "*", "/", "%%",
        "=", "!", ">", "<", "AND",
        "ORR", "INV", "DGT", "LTR",
        "STK", "RTN", ":", "BYE"};
    
    for(int i = 0; i < 28; i++) {
        if(strcmp(name, keys[i]) == 0)
            return i + 1;
    }

    return 0;
}

void makeshorttoken(char letter, int type, struct Token *tokens) {
    char *substr = malloc(2 * sizeof(char));
    substr[0] = letter;
    substr[1] = '\0';
    create_token(&tokens, substr, type);
    free(substr);
}

void strupper(char *str) {
    int len = strlen(str);

    for(int i = 0; i < len; i++)
        str[i] = toupper(str[i]);
}

char *append_line(char *curr_code, char *new_code) {
    int curr_len = strlen(curr_code), added_len = strlen(new_code);

    char *full_code = malloc((curr_len + added_len + 1) * sizeof(char));
    memcpy(full_code, curr_code, curr_len);
    memcpy(full_code + curr_len, new_code, added_len);
    full_code[(curr_len + added_len)] = '\0';
    
    free(curr_code);
    return full_code;
}

/*
Prints all the tokens in the given list
*/
void print_tokens(struct Token *tokens) {
    struct Token *tmp = tokens;

    while(tmp != NULL) {
        printf("[%s] -- [%d]\n", tmp->src, tmp->type);
        tmp = tmp->next;
    }
}

/*
Frees all the tokens in the given list
*/

void free_tokens(struct Token *tokens) {
    struct Token *tmp = tokens, *tmp2;

    while(tmp != NULL) {
        tmp2 = tmp;
        tmp = tmp->next;
        free(tmp2->src);
        free(tmp2);
    }
}