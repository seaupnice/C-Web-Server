#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "cache.h"

/**
 * Allocate a cache entry
 */
struct cache_entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //

    struct cache_entry *newentry = malloc(sizeof(struct cache_entry));

    newentry->path = malloc(strlen(path));
    memcpy(newentry->path, path, strlen(path));

    newentry->content_type = malloc(strlen(content_type));
    memcpy(newentry->content_type, content_type, strlen(content_type));

    newentry->content = malloc(content_length);
    memcpy(newentry->content, content, content_length);

    newentry->content_length = content_length;

    newentry->prev = NULL;
    newentry->next = NULL;

    return newentry;
    ///////////////////
}

/**
 * Deallocate a cache entry
 */
void free_entry(struct cache_entry *entry)
{
    ///////////////////
    // IMPLEMENT ME! //
    if (entry != NULL) {
        if (entry->path != NULL) {
            free(entry->path);
        }
        if (entry->content_type != NULL) {
            free(entry->content_type);
        }
        if (entry->content != NULL) {
            free(entry->content);
        }
        entry->prev = entry->next = NULL;
        free(entry);
        entry = NULL;
    }
    ///////////////////
}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(struct cache *cache, struct cache_entry *ce)
{
    // Insert at the head of the list
    if (cache->head == NULL) {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    } else {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(struct cache *cache, struct cache_entry *ce)
{
    if (ce != cache->head) {
        if (ce == cache->tail) {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;

        } else {
            // We're neither the head nor the tail
            ce->prev->next = ce->next;
            ce->next->prev = ce->prev;
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}


/**
 * Removes the tail from the list and returns it
 * 
 * NOTE: does not deallocate the tail
 */
struct cache_entry *dllist_remove_tail(struct cache *cache)
{
    struct cache_entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 * 
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
struct cache *cache_create(int max_size, int hashsize)
{
    ///////////////////
    // IMPLEMENT ME! //
    struct cache *cache = malloc(sizeof(struct cache));
    cache->index = hashtable_create(hashsize, NULL);
    cache->max_size = max_size;
    cache->cur_size = 0;
    cache->head = cache->tail = NULL;
    return cache;
    ///////////////////
}

void cache_free(struct cache *cache)
{
    struct cache_entry *cur_entry = cache->head;

    hashtable_destroy(cache->index);

    while (cur_entry != NULL) {
        struct cache_entry *next_entry = cur_entry->next;

        free_entry(cur_entry);

        cur_entry = next_entry;
    }

    free(cache);
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 * 
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(struct cache *cache, char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    struct cache_entry *newcacheentry = alloc_entry(
        path, content_type, content, content_length);
    dllist_insert_head(cache, newcacheentry);
    hashtable_put(cache->index, path, newcacheentry);
    cache->cur_size++;
    while (cache->cur_size > cache->max_size) {
        struct cache_entry *removeentry = dllist_remove_tail(cache);
        hashtable_delete(cache->index, removeentry->path);
        free_entry(removeentry);
    }
    ///////////////////
}

/**
 * Retrieve an entry from the cache
 */
struct cache_entry *cache_get(struct cache *cache, char *path)
{
    ///////////////////
    // IMPLEMENT ME! //
    struct cache_entry *findcache = hashtable_get(cache->index, path);
    if (findcache == NULL) {
        return NULL;
    }
    dllist_move_to_head(cache, findcache);
    return findcache;
    ///////////////////
}
