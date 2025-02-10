#include <banjo/error.h>
#include <banjo/system.h>
#include <banjo/log.h>

#include "system_t.h"

extern bj_system_backend_create_info fake_backend_create_info;
#ifdef BANJO_FEATURE_X11
extern bj_system_backend_create_info x11_backend_create_info;
#endif

static const bj_system_backend_create_info* backend_create_infos[] = {
    &fake_backend_create_info,
#ifdef BANJO_FEATURE_X11
    &x11_backend_create_info,
#endif
};

bj_system_backend* s_backend = 0;

#define FORCED_BACKEND 0

bool bj_system_init(
    bj_error** p_error
) {
    // TODO Smarter Backend selection
    // For now, the backend is forced by taking the first available.
    // What I want is the possibility to test each backend initialization in
    // order of favorited, and return on the first that worked.
    const bj_system_backend_create_info* p_create_info = backend_create_infos[FORCED_BACKEND];
    s_backend = p_create_info->create(p_error);
    bj_info("Initialized %s system backend", p_create_info->name);
    return true;
}

void bj_system_dispose(
    bj_error** p_error
) {
    void (*dispose)(struct bj_system_backend_t*, bj_error** p_error) = s_backend->dispose;
    dispose(s_backend, p_error);
    bj_info("Disposed system backend");
}

