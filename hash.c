#include "hash.h"

#include <string.h>

#if 1
#define DEBUG_HASH_TABLE
#endif

static uint32_t jenkins_hash(const char* key)
{
    uint32_t hash = 0u;
    if(key == NULL)
    {
#ifdef DEBUG_HASH_TABLE
        AX_DEBUG("key is null, returning zero. This hash should be discarded");
#endif
        return hash;
    }

    for(uint8_t i = 0u; key[i] != 0u; ++i)
    {
        hash += (uchar)key[i];
        hash += hash << 10u;
        hash ^= hash >> 6u;
    }
    hash += hash << 3u;
    hash ^= hash >> 11u;
    hash += hash << 15u;
    return hash;
}

void HT_Insert(HashTable_T* ht, const char* key, void* data)
{
    uint32_t jenk_hash = jenkins_hash(key);
    uint8_t jenk_hash_idx = jenk_hash % MAX_ELEMS_TABLE;

    if(jenk_hash == 0)
    {
        AX_DEBUG("COLLISION same hash key in bucket (%s)", key);
    }
    else
    {
        ht->table[jenk_hash_idx] = data;
        ++ht->size;
    }
}

void* HT_Get(HashTable_T* ht, const char* key)
{
    uint32_t jenk_hash = jenkins_hash(key);
    uint8_t jenk_hash_idx = jenk_hash % MAX_ELEMS_TABLE;
    return ht->table[jenk_hash_idx];
}

void HT_Set(HashTable_T* ht, const char* key, void* data)
{
    uint32_t jenk_hash = jenkins_hash(key);
    uint8_t jenk_hash_idx = jenk_hash % MAX_ELEMS_TABLE;
    ht->table[jenk_hash_idx] = data;
}

void* HT_Delete(HashTable_T* ht, const char* key)
{
    uint32_t jenk_hash = jenkins_hash(key);
    uint8_t jenk_hash_idx = jenk_hash % MAX_ELEMS_TABLE;
    void* data = ht->table[jenk_hash_idx];
    ht->table[jenk_hash_idx] = 0;
    ht->size--;
    return data;
}
