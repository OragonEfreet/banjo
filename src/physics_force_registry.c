#include <banjo/memory.h>
#include <banjo/physics.h>

#include "check.h"

typedef struct {
    bj_real*           force;
    bj_force_generator generator;
    void*              data;
} bj_force_register;

struct bj_force_registry_t {
    size_t        len;
    size_t        capacity;
    bj_force_register* registers;
};

bj_force_registry* bj_create_force_registry(
    void
) {
    return bj_calloc(sizeof(bj_force_registry));
}

void bj_delete_force_registry(
    bj_force_registry* registry
) {
    bj_clear_force_registry(registry);
    bj_free(registry);
}

void bj_register_force(
    bj_force_registry* registry,
    bj_real*           force,
    bj_force_generator generator,
    void*              data
) {
    if(registry->len == registry->capacity) {
        registry->capacity = (registry->len + 1) * 2;
        registry->registers = bj_realloc(registry->registers, sizeof(bj_force_register) * registry->capacity * 2);
    }
    registry->registers[registry->len].force     = force;
    registry->registers[registry->len].generator = generator;
    registry->registers[registry->len].data      = data;
    ++registry->len;
}

void bj_clear_force_registry(
    bj_force_registry* registry
) {
    bj_check(registry);
    bj_free(registry->registers);
    registry->registers = 0;
    registry->len = 0;
    registry->capacity = 0;
}

void bj_apply_force_registry(
    bj_force_registry* registry,
    double dt
) {
    for(size_t i = 0 ; i < registry->len ; ++i) {
        bj_force_register* r = registry->registers+i;
        r->generator(r->force, dt, r->data);
    }
}



