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
    BeadCollection* local_beads;  // Store beads locally for now
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
    // Initialize local collection if needed
    if (!surreal_state.local_beads) {
        printf("Creating new local bead collection\n");
        surreal_state.local_beads = create_bead_collection();
        if (!surreal_state.local_beads) {
            printf("Error: Failed to create bead collection\n");
            return false;
        }
    }
    
    // Generate ID if needed
    if (!bead->id) {
        static int next_id = 1;
        char id_buffer[32];
        snprintf(id_buffer, sizeof(id_buffer), "bead_%d", next_id++);
        bead->id = strdup(id_buffer);
        printf("Generated new bead ID: %s\n", bead->id);
    }
    
    // Create deep copy of bead
    BeadDefinition bead_copy;
    memset(&bead_copy, 0, sizeof(BeadDefinition));  // Initialize to zero
    
    bead_copy.id = strdup(bead->id);
    bead_copy.name = strdup(bead->name);
    bead_copy.description = bead->description ? strdup(bead->description) : NULL;
    bead_copy.category = bead->category ? strdup(bead->category) : strdup("Default");
    bead_copy.material = bead->material;
    bead_copy.shape = bead->shape;
    bead_copy.finish = bead->finish;
    bead_copy.color = bead->color;
    bead_copy.image_id = bead->image_id;
    bead_copy.size_mm = bead->size_mm;
    bead_copy.is_premium = bead->is_premium;
    
    printf("Saving bead to local collection:\n");
    printf("  - ID: %s\n", bead_copy.id);
    printf("  - Name: %s\n", bead_copy.name);
    printf("  - Image ID: %d\n", bead_copy.image_id);
    printf("  - Category: %s\n", bead_copy.category);
    
    if (!add_bead_definition(surreal_state.local_beads, bead_copy)) {
        // Clean up on failure
        free((void*)bead_copy.id);
        free((void*)bead_copy.name);
        free((void*)bead_copy.description);
        free((void*)bead_copy.category);
        printf("Error: Failed to add bead to collection!\n");
        return false;
    }
    
    printf("Successfully added bead to collection. Total beads: %d\n", 
           surreal_state.local_beads->count);
    return true;
}

BeadDefinition* surreal_get_bead(const char* bead_id) {
    if (!surreal_state.curl) return NULL;
    return NULL;  // Temporary mock implementation
}

BeadCollection* surreal_get_all_beads(void) {
    // Remove the curl check here too
    // if (!surreal_state.curl) return NULL;
    if (!surreal_state.local_beads) {
        surreal_state.local_beads = create_bead_collection();
    }
    return surreal_state.local_beads;
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