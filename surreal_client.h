#ifndef SURREAL_CLIENT_H
#define SURREAL_CLIENT_H

#include "bead.h"
#include <stdbool.h>

// Connection config
typedef struct {
    const char* host;
    const char* port;
    const char* namespace_;
    const char* database;
    const char* username;
    const char* password;
} SurrealConfig;

// Initialize connection
bool surreal_init(SurrealConfig config);

// Bead operations
bool surreal_save_bead(BeadDefinition* bead);
bool surreal_delete_bead(const char* bead_id);
BeadDefinition* surreal_get_bead(const char* bead_id);
BeadCollection* surreal_get_all_beads(void);

// Cleanup
void surreal_cleanup(void);

#endif 