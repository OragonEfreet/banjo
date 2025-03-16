#include <banjo/error.h>
#include <banjo/system.h>
#include <banjo/log.h>

#include "config.h"
#include "system_t.h"

extern bj_system_backend_create_info fake_backend_create_info;
#if BJ_HAS_FEATURE(X11)
extern bj_system_backend_create_info x11_backend_create_info;
#endif
#if BJ_HAS_FEATURE(WIN32)
extern bj_system_backend_create_info win32_backend_create_info;
#endif

static const bj_system_backend_create_info* backend_create_infos[] = {
#if BJ_HAS_FEATURE(WIN32)
    &win32_backend_create_info,
#endif
#if BJ_HAS_FEATURE(X11)
    &x11_backend_create_info,
#endif
    &fake_backend_create_info,
};

bj_system_backend* s_backend = 0;

#define FORCED_BACKEND 0

bool bj_system_init(
    bj_error** p_error
) {
    const size_t n_backends = sizeof(backend_create_infos) / sizeof(bj_system_backend_create_info*);

    for(size_t b = 0 ; b < n_backends ; ++b) {

        bj_error* sub_err = 0;

        const bj_system_backend_create_info* p_create_info = backend_create_infos[b];
        bj_trace("Will try to initialize %s system backend", p_create_info->name);
        bj_system_backend* p_backend = p_create_info->create(&sub_err);

        if(sub_err) {
            bj_message(p_backend == 0 ? 0 : 1, 0, 0,
                "Error while initializing %s: %s (code 0x%08X)",
                p_create_info->name, sub_err->message, sub_err->code
            );
            bj_clear_error(&sub_err);
        }

        if(p_backend != 0) {
            bj_info("Initialized %s system backend", p_create_info->name);
            s_backend = p_backend;
            return true;
        }
    }

    bj_set_error(p_error, BJ_ERROR_INITIALIZE, "No suitable system backend found");
    return false;
}

void bj_system_dispose(
    bj_error** p_error
) {
    void (*dispose)(struct bj_system_backend_t*, bj_error** p_error) = s_backend->dispose;
    dispose(s_backend, p_error);
    bj_info("Disposed system backend");
}

