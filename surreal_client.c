#include "surreal_client.h"
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

static struct {
    char* host;
    char* port;
    char* ns;
    char* db;
    char* username;
    char* password;
    CURL* curl;
} surreal_state = {0};

// Helper struct for curl response
struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;
    
    char* ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) return 0;
    
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

bool surreal_init(SurrealConfig config) {
    curl_global_init(CURL_GLOBAL_ALL);
    surreal_state.curl = curl_easy_init();
    if (!surreal_state.curl) return false;
    
    surreal_state.host = strdup(config.host);
    surreal_state.port = strdup(config.port);
    surreal_state.ns = strdup(config.namespace_);
    surreal_state.db = strdup(config.database);
    surreal_state.username = strdup(config.username);
    surreal_state.password = strdup(config.password);
    
    return true;
}

bool surreal_save_bead(BeadDefinition* bead) {
    if (!surreal_state.curl) return false;
    
    // For now, just print the bead info
    printf("Saving bead: %s (%s)\n", bead->name, bead->category);
    return true;  // Temporary mock implementation
}

BeadDefinition* surreal_get_bead(const char* bead_id) {
    if (!surreal_state.curl) return NULL;
    return NULL;  // Temporary mock implementation
}

BeadCollection* surreal_get_all_beads(void) {
    if (!surreal_state.curl) return NULL;
    
    // For now, return an empty collection
    BeadCollection* collection = malloc(sizeof(BeadCollection));
    collection->count = 0;
    collection->capacity = 10;
    collection->definitions = malloc(collection->capacity * sizeof(BeadDefinition));
    return collection;  // Temporary mock implementation
}

bool surreal_delete_bead(const char* bead_id) {
    if (!surreal_state.curl) return false;
    return true;  // Temporary mock implementation
}

void surreal_cleanup(void) {
    if (surreal_state.curl) {
        curl_easy_cleanup(surreal_state.curl);
        curl_global_cleanup();
    }
    
    free(surreal_state.host);
    free(surreal_state.port);
    free(surreal_state.ns);
    free(surreal_state.db);
    free(surreal_state.username);
    free(surreal_state.password);
    
    memset(&surreal_state, 0, sizeof(surreal_state));
} 