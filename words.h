#ifndef WORDS_H
#define WORDS_H 

#define MAX_WORDS 1000
#define MAX_LINE_LENGTH 100

int load_words(const char *filename, char words[][MAX_LINE_LENGTH]);
void choose_random_word(char words[][MAX_LINE_LENGTH], int count, char secret_word[]);
int is_valid_word(const char *word, char words[][MAX_LINE_LENGTH], int count);

#endif 