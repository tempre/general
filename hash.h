#ifndef HASH_H_
#define HASH_H_

#include "common.h"

#define MAX_ELEMS_TABLE 100u
typedef struct {
   void* table[MAX_ELEMS_TABLE];
   uint8_t size;
  /* can include other info here if we want */
} HashTable_T;

void HT_Insert(HashTable_T* ht, const char* key, void* data);
void* HT_Delete(HashTable_T* ht, const char* key);
void* HT_Get(HashTable_T* ht, const char* key);
void HT_Set(HashTable_T* ht, const char* key, void* data);

#endif // HASH_H_
