/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: David An
 * Date: Jun 3 2018
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key) {
	int r = 0;
	for (int i = 0; key[i] != '\0'; i++) {
		r += key[i];
	}
	return r;
}

int hashFunction2(const char* key) {
	int r = 0;
	for (int i = 0; key[i] != '\0'; i++) {
		r += (i + 1) * key[i];
	}
	return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next) {
	HashLink* link = malloc(sizeof(HashLink));
	link->key = malloc(sizeof(char) * (strlen(key) + 1));
	strcpy(link->key, key);
	link->value = value;
	link->next = next;
	return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link) {
	free(link->key);
	free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity) {
	map->capacity = capacity;
	map->size = 0;
	map->table = (HashLink**)malloc(sizeof(HashLink*) * capacity);
	
	for (int i = 0; i < capacity; i++) {
		map->table[i] = NULL;
	}
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
	HashLink* current;
	HashLink* next;
	
	for (int i = 0; i < hashMapCapacity(map); i++) {
		current = map -> table[i];		// set current to beginning of bucket
		while (current != NULL) {
			next = current ->next;		// iterate through each link in the bucket
			hashLinkDelete(current);	// delete link
			current = next;
		}
	}
	free(map -> table);
	map -> capacity = 0;
	map -> size = 0;
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
	HashMap* map = malloc(sizeof(HashMap));
	hashMapInit(map, capacity);
	return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
	hashMapCleanUp(map);
	free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
	assert(map != NULL);
	assert(key != NULL);
	
	int idx;				// find index for the given key
	int mapCapacity = hashMapCapacity(map);
	
	if (HASH_FUNCTION(key) == 1) {
		idx = hashFunction1(key) % mapCapacity;
	} else {
		idx = hashFunction2(key) % mapCapacity;
	}
	
	if (idx < 0) {
		idx += mapCapacity;
	}
	
	// set pointer to bucket containing the desired key
	struct HashLink * current = map -> table[idx];
	
	while (current != NULL) {
		if (strcmp(current -> key, key) == 0) {
			return &(current -> value);		// return associated value
		}
		current = current -> next;	// iterate through each link in bucket
	}
	
	return NULL;							// key was not found
}

/**
 * Resizes the hash table to have a number of buckets equal to the given
 * capacity. After allocating the new table, all of the links need to be
 * rehashed into it because the capacity has changed.
 *
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 *
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
	HashMap * newMap = hashMapNew(capacity);
	HashLink * current;
	
	for (int i = 0; i < hashMapCapacity(map); i++) {
		current = map -> table[i];		// set pointer to beginning of bucket
		while (current != NULL) {
			hashMapPut(newMap, current -> key, current -> value);
			current = current -> next;	// iterate through each link
		}
	}
	
	hashMapCleanUp(map);						// clear old map
	map -> capacity = newMap -> capacity;		// copy everything to new map
	map -> size = newMap -> size;
	map -> table = newMap -> table;
	
	free(newMap);								// free allocated data
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
	int idx;			// find index for the given key
	int mapCapacity = hashMapCapacity(map);
	
	if (HASH_FUNCTION(key) == 1) {
		idx = hashFunction1(key) % mapCapacity;
	} else {
		idx = hashFunction2(key) % mapCapacity;
	}
	
	if (idx < 0) {
		idx += mapCapacity;
	}
	
	if (hashMapContainsKey(map, key)) {	// key already exists in map
		int *trueValue = hashMapGet(map, key);
		(*trueValue) = value;			// set pointer to value to new value
	} else {
		
		// create new link containing given information
		HashLink * newLink = hashLinkNew(key, value, NULL);
		
		if (map -> table[idx] == NULL) {	// bucket is empty
			map -> table[idx] = newLink;	// set newLink to beginning of bucket
		} else {
			HashLink * current = map -> table[idx];
			while (current -> next != NULL) {
				current = current -> next;
			}
			current -> next = newLink;			// set newLink to end of the chain
		}
		map -> size++;
	}
	
	float loadFactor = hashMapTableLoad(map);
	
	if (loadFactor >= MAX_TABLE_LOAD) {		// resize hashtable if necessary
		int newSize = 2 * mapCapacity;
		resizeTable(map, newSize);
	}
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
	assert(map != NULL);
	assert(key != NULL);
	
	int idx;				// find index for given key
	int mapCapacity = hashMapCapacity(map);
	
	if (HASH_FUNCTION(key) == 1) {
		idx = hashFunction1(key) % mapCapacity;
	} else {
		idx = hashFunction2(key) % mapCapacity;
	}
	
	if (idx < 0) {
		idx += mapCapacity;
	}
	
	HashLink * current = map -> table[idx];
	HashLink * previous = NULL;
	
	if (current != NULL) {
		if (strcmp(current -> key, key) == 0) {		// key has been found
			map -> table[idx] = current -> next;	// remove link
		} else {
			while (strcmp(current -> key, key) != 0) {
				previous = current;				// iterate through chain
				current = current -> next;
			}
			if (previous != NULL) {
				previous -> next = current -> next;
			}
		}
		hashLinkDelete(current);
		map -> size--;
	}
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
	int idx;			// find index for given key
	int mapCapacity = hashMapCapacity(map);
	
	if (HASH_FUNCTION(key) == 1) {
		idx = hashFunction1(key) % mapCapacity;
	} else {
		idx = hashFunction2(key) % mapCapacity;
	}
	
	if (idx < 0) {
		idx += mapCapacity;
	}
	
	struct HashLink * current = map -> table[idx];
	while (current != NULL) {
		if (strcmp(current -> key, key) == 0) {
			return 1;			// link with given key has been found
		}
		current = current -> next;	// iterate through rest of the chain
	}
	return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
	return map -> size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
	return map -> capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
	int count = 0;
	
	for (int i = 0; i < hashMapCapacity(map); i++) {
		if (map -> table[i] == NULL) {
			count++;
		}
	}
	
	return count;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
	float ratio = (float)hashMapSize(map) / (float)hashMapCapacity(map);
	return ratio;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
	for (int i = 0; i < map->capacity; i++) {
		HashLink* link = map->table[i];
		if (link != NULL) {
			printf("\nBucket %i ->", i);
			while (link != NULL) {
				printf(" (%s, %d) ->", link->key, link->value);
				link = link->next;
			}
		}
	}
	printf("\n");
}

