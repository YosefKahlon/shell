#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define MAX_SIZE 100

typedef struct HashNode {
    char *key;
    char *value;
    struct HashNode *next;
} HashNode;

typedef struct VarMap {
    HashNode **buckets;
    int size;
} VarMap;

int hash(char *key) {
    int hashval = 0;
    for (int i = 0; i < strlen(key); i++) {
        hashval = key[i] + (31 * hashval);
    }
    return hashval;
}

VarMap *createVarMap(int size) {
    VarMap *map = malloc(sizeof(VarMap));
    map->buckets = calloc(size, sizeof(HashNode *));
    map->size = size;
    return map;
}

void put(VarMap *map, char *key, char *value) {
    int hashval = hash(key) % map->size;
    HashNode *node = map->buckets[hashval];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            free(node->value);
            node->value = malloc(strlen(value) + 1);
            strcpy(node->value, value);
            return;
        }
        node = node->next;
    }
    HashNode *newNode = malloc(sizeof(HashNode));
    newNode->key = malloc(strlen(key) + 1);
    strcpy(newNode->key, key);
    newNode->value = malloc(strlen(value) + 1);
    strcpy(newNode->value, value);
    newNode->next = map->buckets[hashval];
    map->buckets[hashval] = newNode;
}

char *get(VarMap *map, char *key) {
    int hashval = hash(key) % map->size;
    HashNode *node = map->buckets[hashval];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

void destroyVarMap(VarMap *map) {
    for (int i = 0; i < map->size; i++) {
        HashNode *node = map->buckets[i];
        while (node != NULL) {
            HashNode *temp = node;
            node = node->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(map->buckets);
    free(map);
}

// int main() {
//     VarMap *map = createVarMap(10);
    
//     // Test adding key-value pairs
//     put(map, "name", "John");
//     put(map, "age", "25");
//     put(map, "gender", "Male");
//     printf("Added key-value pairs\n");
//     printf("name=%s\n", get(map, "name"));
//     printf("age=%s\n", get(map, "age"));
//     printf("gender=%s\n\n", get(map, "gender"));

//     // Test updating existing key-value pairs
//     put(map, "name", "Jane");
//     put(map, "age", "30");
//     printf("Updated existing key-value pairs\n");
//     printf("name=%s\n", get(map, "name"));
//     printf("age=%s\n", get(map, "age"));
//     printf("gender=%s\n\n", get(map, "gender"));

//     // Test adding new key-value pairs after updating existing ones
//     put(map, "city", "New York");
//     put(map, "country", "USA");
//     printf("Added new key-value pairs\n");
//     printf("name=%s\n", get(map, "name"));
//     printf("age=%s\n", get(map, "age"));
// }
