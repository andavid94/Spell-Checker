#include "hashMap.h"
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file){
	int maxLength = 16;
	int length = 0;
	char* word = malloc(sizeof(char) * maxLength);
	
	while (1) {
		char c = fgetc(file);
		if ((c >= '0' && c <= '9') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			c == '\'') {
			if (length + 1 >= maxLength) {
				maxLength *= 2;
				word = realloc(word, maxLength);
			}
			word[length] = c;
			length++;
		}
		else if (length > 0 || c == EOF) {
			break;
		}
	}
	if (length == 0) {
		free(word);
		return NULL;
	}
	word[length] = '\0';
	return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map) {
	char * word = nextWord(file);
	
	while (word != NULL) {
		hashMapPut(map, word, 0);
		free(word);
		word = nextWord(file);
	}
	
	fclose(file);
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv) {
	HashMap* map = hashMapNew(1000);
	
	FILE* file = fopen("dictionary.txt", "r");
	clock_t timer = clock();
	loadDictionary(file, map);
	timer = clock() - timer;
	printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
	fclose(file);
	
	char inputBuffer[256];
	int quit = 0;
	while (!quit) {
		printf("Enter a word or \"quit\" to quit: ");
		scanf("%s", inputBuffer);
		
		if (strcmp(inputBuffer, "quit") == 0) {
			quit = 1;
			break;
		}
		
		if (hashMapContainsKey(map, inputBuffer)) {
			printf("The inputted word %s is spelled correctly\n", inputBuffer);
		} else {
			printf("The inputted word %s is not spelled correctly\n", inputBuffer);
			
			for (int i = 0; i < hashMapCapacity(map); i++) {
				HashLink * current = map -> table[i];
				if (current != NULL) {
					while (current != NULL) {
						int len_s = sizeof(hashMapGet(map, current -> key));
						int len_t = sizeof(inputBuffer);
						int difference = distance(hashMapGet(map, current -> key), len_s, inputBuffer, len_t);
						hashMapPut(map, current -> key, difference);
						current = current -> next;
					}
				}
			}
		}
	}
	
	hashMapDelete(map);
	return 0;
}

/*
 ** Description: Calculates and returns the Levenshtein distance between two words
 */
int distance(const char* s, int len_s, const char* t, int len_t) {
	
	int matrix[len_s+1][len_t+1];		// initialize 2d array
	int i;
	
	for (i = 0; i <= len_s; i++) {
		matrix[i][0] = i;				// populate array with characters of first word
	}
	
	for (i = 0; i <= len_t; i++) {
		matrix[0][i] = i;				// populate array with characters of second word
	}
	
	for (i = 1; i <= len_s; i++) {
		int j;
		char c1;
		
		c1 = s[i-1];
		for (j = 1; j <= len_t; j++) {
			char c2;
			c2 = t[j-1];
			if (c1 == c2) {
				matrix[i][j] = matrix[i-1][j-1];
			} else {
				int delete;
				int insert;
				int substitute;
				int minimum;
				
				delete = matrix[i-1][j] + 1;
				insert = matrix[i][j-1] + 1;
				substitute = matrix[i-1][j-1] + 1;
				minimum = delete;
				if (insert < minimum) {
					minimum = insert;
				}
				if (substitute < minimum) {
					minimum = substitute;
				}
				matrix[i][j] = minimum;
			}
		}
	}
	
	return matrix[len_s][len_t];
}

