

#ifdef BJ_CONFIG_EMSCRIPTEN_BACKEND

#include <banjo/event.h>
#include <banjo/string.h>
#include <banjo/video.h>

#include <check.h>
#include <window_t.h>

#include <emscripten/dom_pk_codes.h> 
#include <emscripten/html5.h>

#include <ctype.h>

#define BJ_EM_CANVAS_SELECTOR "#canvas"

struct {
    bj_bool window_exist;
} emscripten;

typedef struct {
    struct bj_window common;
    const char* selector;
    int         width;
    int         height;
} emscripten_window;


static enum bj_key em_translate_keycode(const EM_UTF8 key[32], unsigned int location) {
    // Common named keys
    if (bj_strcmp(key, "Enter") == 0) return BJ_KEY_RETURN;
    if (bj_strcmp(key, "Tab") == 0) return BJ_KEY_TAB;
    if (bj_strcmp(key, "Escape") == 0) return BJ_KEY_ESCAPE;
    if (bj_strcmp(key, "Backspace") == 0) return BJ_KEY_BACK;
    if (bj_strcmp(key, " ") == 0 || bj_strcmp(key, "Spacebar") == 0) return BJ_KEY_SPACE;
    if (bj_strcmp(key, "ArrowLeft") == 0) return BJ_KEY_LEFT;
    if (bj_strcmp(key, "ArrowRight") == 0) return BJ_KEY_RIGHT;
    if (bj_strcmp(key, "ArrowUp") == 0) return BJ_KEY_UP;
    if (bj_strcmp(key, "ArrowDown") == 0) return BJ_KEY_DOWN;
    if (bj_strcmp(key, "Home") == 0) return BJ_KEY_HOME;
    if (bj_strcmp(key, "End") == 0) return BJ_KEY_END;
    if (bj_strcmp(key, "PageUp") == 0) return BJ_KEY_PRIOR;
    if (bj_strcmp(key, "PageDown") == 0) return BJ_KEY_NEXT;
    if (bj_strcmp(key, "Insert") == 0) return BJ_KEY_INSERT;
    if (bj_strcmp(key, "Delete") == 0) return BJ_KEY_DELETE;
    if (bj_strcmp(key, "Pause") == 0) return BJ_KEY_PAUSE;
    if (bj_strcmp(key, "CapsLock") == 0) return BJ_KEY_CAPITAL;

    // Function keys F1-F24
    if (key[0] == 'F' && isdigit((unsigned char)key[1])) {
        int fnum = 0;
        for (int i = 1; key[i] && isdigit((unsigned char)key[i]); ++i) {
            fnum = fnum * 10 + (key[i] - '0');
        }
        if (fnum >= 1 && fnum <= 24) {
            return (enum bj_key)(BJ_KEY_F1 + (fnum - 1));
        }
    }

    // Modifier keys
    if (bj_strcmp(key, "Shift") == 0) return location == 1 ? BJ_KEY_LSHIFT : BJ_KEY_RSHIFT;
    if (bj_strcmp(key, "Control") == 0) return location == 1 ? BJ_KEY_LCONTROL : BJ_KEY_RCONTROL;
    if (bj_strcmp(key, "Alt") == 0) return location == 1 ? BJ_KEY_LMENU : BJ_KEY_RMENU;
    if (bj_strcmp(key, "Meta") == 0) return location == 1 ? BJ_KEY_LWIN : BJ_KEY_RWIN;

    // Single-character keys
    if (strlen(key) == 1) {
        char c = key[0];
        if (c >= '0' && c <= '9') return (enum bj_key)(BJ_KEY_0 + (c - '0'));
        if (c >= 'A' && c <= 'Z') return (enum bj_key)(BJ_KEY_A + (c - 'A'));
        if (c >= 'a' && c <= 'z') return (enum bj_key)(BJ_KEY_A + (c - 'a'));
        // Handle symbols (US layout)
        switch (c) {
            case ';': return BJ_KEY_OEM_1;
            case '+': return BJ_KEY_OEM_PLUS;
            case ',': return BJ_KEY_OEM_COMMA;
            case '-': return BJ_KEY_OEM_MINUS;
            case '.': return BJ_KEY_OEM_PERIOD;
            case '/': return BJ_KEY_OEM_2;
            case '`': return BJ_KEY_OEM_3;
            case '[': return BJ_KEY_OEM_4;
            case '\\': return BJ_KEY_OEM_5;
            case ']': return BJ_KEY_OEM_6;
            case '\'': return BJ_KEY_OEM_7;
        }
    }

    return BJ_KEY_UNKNOWN;
}

static bool em_key_callback(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData) {
    bj_push_key_event(
        userData, 
        eventType == EMSCRIPTEN_EVENT_KEYDOWN ? BJ_PRESS : BJ_RELEASE, 
        em_translate_keycode(keyEvent->key, keyEvent->location), 
        emscripten_compute_dom_pk_code(keyEvent->code)
    );
    return 1;
}

static bool em_mouse_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    const int x = mouseEvent->targetX;
    const int y = mouseEvent->targetY;

    switch (eventType) {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
        case EMSCRIPTEN_EVENT_MOUSEUP: {
            const int action = (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN) ? BJ_PRESS : BJ_RELEASE;
            // +1 because EMSCRITEN buttons are the same as ours -1
            bj_push_button_event(userData, mouseEvent->button + 1, action, x, y);
            break;
        }
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            bj_push_cursor_event(userData, x, y);
            break;
        case EMSCRIPTEN_EVENT_MOUSEENTER:
        case EMSCRIPTEN_EVENT_MOUSELEAVE: {
            bj_push_enter_event(userData, eventType == EMSCRIPTEN_EVENT_MOUSEENTER, x, y);
            break;
        }
        case EMSCRIPTEN_EVENT_CLICK:
        case EMSCRIPTEN_EVENT_DBLCLICK:
        default:
            return 0;
    }

    return 1;
}

static struct bj_window* emscripten_window_new(
    struct bj_video_layer* p_layer,
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t  flags
) {
    (void)p_layer;
    (void)p_title;
    (void)x;
    (void)y;

    if(emscripten.window_exist == BJ_TRUE) {
        return 0;
    }

    emscripten_window window = { 
        .common = {
            .flags = flags,
        },
        .selector = BJ_EM_CANVAS_SELECTOR,
        .width    = width,
        .height   = height,
    };

    EMSCRIPTEN_RESULT res = emscripten_set_canvas_element_size(window.selector, width, height);
    if(res != EMSCRIPTEN_RESULT_SUCCESS) {
        return 0;
    }

    emscripten_window* p_window = bj_malloc(sizeof(emscripten_window));

    emscripten_set_mousedown_callback(window.selector, p_window, 0, em_mouse_callback);
    emscripten_set_mouseup_callback(window.selector, p_window, 0, em_mouse_callback);
    emscripten_set_mousemove_callback(window.selector, p_window, 0, em_mouse_callback);
    emscripten_set_mouseenter_callback(window.selector, p_window, 0, em_mouse_callback);
    emscripten_set_mouseleave_callback(window.selector, p_window, 0, em_mouse_callback);
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, p_window, 0, em_key_callback);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, p_window, 0, em_key_callback);

    bj_memcpy(p_window, &window, sizeof(emscripten_window));
    return (struct bj_window*)p_window;
}

static void emscripten_window_del(
    struct bj_video_layer* p_layer,
    struct bj_window* p_abstract_window
) {
    (void)p_layer;
    emscripten_window* p_window = (emscripten_window*)p_abstract_window;
    emscripten_set_canvas_element_size(p_window->selector, 0, 0);
    emscripten.window_exist = BJ_FALSE;

    bj_free(p_window);
}

static void emscripten_end_layer(
    struct bj_video_layer* p_layer,
    struct bj_error** p_error
) {
    (void)p_error;
    bj_free(p_layer);
}

static void emscripten_window_poll(
    struct bj_video_layer* p_layer
) {
    (void)p_layer;
}

static int emscripten_get_window_size(
    struct bj_video_layer* p_layer,
    const struct bj_window* p_abstract_window,
    int* width,
    int* height
) {
    (void)p_layer;
    bj_check_or_0(p_abstract_window);
    bj_check_or_0(width || height);
    const emscripten_window* p_window = (const emscripten_window*)p_abstract_window;
    if(width) {
        *width = p_window->width;
    }
    if(height) {
        *height = p_window->height;
    }
    return 1;
}

static struct bj_bitmap* emscripten_create_window_framebuffer(
    struct bj_video_layer* p_layer,
    const struct bj_window* p_abstract_window,
    struct bj_error** p_error
) {
    (void)p_layer;
    (void)p_error;
    emscripten_window* p_window = (emscripten_window*)p_abstract_window;
    return bj_create_bitmap(
        p_window->width, p_window->height,
        BJ_PIXEL_MODE_XRGB8888, 0
    );
}

static void emscripten_flush_window_framebuffer(
    struct bj_video_layer* p_layer,
    const struct bj_window*   p_abstract_window
) {
    const emscripten_window* p_window = (emscripten_window*)p_abstract_window;
    (void)p_layer;
    (void)p_abstract_window;

    MAIN_THREAD_EM_ASM({
        var w = $0;
        var h = $1;
        var pixels = $2;
        var canvasId = UTF8ToString($3);
        var canvas = document.querySelector(canvasId);
        var ctx = canvas.getContext('2d');

        var imageData = ctx.createImageData(w, h);
        var data = imageData.data;

        var src32 = HEAP32.subarray(pixels >> 2, (pixels >> 2) + w * h);
        var dst32 = new Uint32Array(data.buffer);

        for (var i = 0; i < src32.length; ++i) {
            var xrgb = src32[i];
            var r = (xrgb >> 16) & 0xFF;
            var g = (xrgb >> 8) & 0xFF;
            var b = xrgb & 0xFF;
            dst32[i] = (0xFF << 24) | (b << 16) | (g << 8) | r; // RGBA
        }

        ctx.putImageData(imageData, 0, 0);
    }, p_window->width, p_window->height, bj_bitmap_pixels(p_abstract_window->p_framebuffer), p_window->selector);
}

static struct bj_video_layer* emscripten_init_layer(
    struct bj_error** p_error
) {
    (void)p_error;

    struct bj_video_layer* p_layer = bj_malloc(sizeof(struct bj_video_layer));
    p_layer->end                       = emscripten_end_layer;
    p_layer->create_window             = emscripten_window_new;
    p_layer->delete_window             = emscripten_window_del;
    p_layer->poll_events               = emscripten_window_poll;
    p_layer->get_window_size           = emscripten_get_window_size;
    p_layer->create_window_framebuffer = emscripten_create_window_framebuffer;
    p_layer->flush_window_framebuffer  = emscripten_flush_window_framebuffer;
    p_layer->data = 0;
    return p_layer;
}

struct bj_video_layer_create_info emscripten_video_layer_info = {
    .name   = "emscripten",
    .create = emscripten_init_layer,
};

#endif
