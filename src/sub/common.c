/*
    Common elements used by most files in this project.
*/




#include "common.h"




int isWhitespace(char c) {
    return (!(c)) || (c == ' ') || (c == '\n') || (c == '\t');
}


char *readWord(char **str) {
    if (!(str) || !(*str)) return NULL;
    // Get to the first word, ignore all whitespaces before it
    while ((*str)[0] && isWhitespace((*str)[0])) (*str)++;
    char *start = (*str);
    // Find the end of the word
    while (!(isWhitespace((*str)[0]))) (*str)++;
    // If the word is empty, return the NULL value
    if (start == (*str)) return NULL;
    // Copy the word into a new variable
    char *word = malloc(((*str) - start + 1) * sizeof(char));
    for (int i = 0; start+i < (*str); i++) word[i] = start[i];
    word[(*str) - start] = '\0';
    return word;
}
