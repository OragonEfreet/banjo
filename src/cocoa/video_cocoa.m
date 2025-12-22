

#ifdef BJ_CONFIG_COCOA_BACKEND

#include <banjo/assert.h>
#include <banjo/error.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#include <bitmap.h>
#include <check.h>
#include <renderer.h>
#include <video_layer.h>
#include <window.h>

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

struct bj_renderer_data {
    struct bj_bitmap  framebuffer;
    void*             buffer;
    int               buffer_width;
    int               buffer_height;
    void*             configured_view;
};

struct {
    NSApplication *app;
} cocoa;

struct cocoa_window {
    struct bj_window common;
    NSWindow*        handle;
    void*            view;
};

@interface BanjoView : NSView <NSWindowDelegate>
@end

@implementation BanjoView
- (BOOL)acceptsFirstResponder {
  return YES;
}

- (void)drawRect:(NSRect)dirtyRect {


    struct bj_renderer* renderer = 
        (struct bj_renderer*)objc_getAssociatedObject(self, "bj_sw_render");

    bj_check(renderer);
    bj_assert(renderer->data);

    const struct bj_renderer_data* data = renderer->data;
    void* buffer = data->buffer;
    int buffer_width = data->buffer_width;
    int buffer_height = data->buffer_height;
  
  
    CGContextRef cg_context = [[NSGraphicsContext currentContext] CGContext];

    CGDataProviderRef provider = CGDataProviderCreateWithData(
        NULL, buffer, buffer_width * buffer_height * 4,
        NULL);
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
  
    CGImageRef image =
        CGImageCreate(buffer_width, buffer_height, 8, 32,
                      buffer_width * 4, colorspace,
                      kCGImageAlphaNoneSkipFirst | kCGImageByteOrder32Little,
                      provider, NULL, false, kCGRenderingIntentDefault);
  
    CGContextDrawImage(cg_context, [self bounds], image);
  
    CGImageRelease(image);
    CGColorSpaceRelease(colorspace);
    CGDataProviderRelease(provider);
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
  struct cocoa_window *window =
      (struct cocoa_window *)objc_getAssociatedObject(self, "bj_window");
  if (window) {
    bj_set_window_should_close(&window->common);
  }
  return NO;
}
@end

static enum bj_key cocoa_keycode_to_bj_key(unsigned short keyCode) {
    static const enum bj_key keymap[128] = {
        [0x00] = BJ_KEY_A,        [0x01] = BJ_KEY_S,
        [0x02] = BJ_KEY_D,        [0x03] = BJ_KEY_F,
        [0x04] = BJ_KEY_H,        [0x05] = BJ_KEY_G,
        [0x06] = BJ_KEY_Z,        [0x07] = BJ_KEY_X,
        [0x08] = BJ_KEY_C,        [0x09] = BJ_KEY_V,
        [0x0B] = BJ_KEY_B,        [0x0C] = BJ_KEY_Q,
        [0x0D] = BJ_KEY_W,        [0x0E] = BJ_KEY_E,
        [0x0F] = BJ_KEY_R,        [0x10] = BJ_KEY_Y,
        [0x11] = BJ_KEY_T,        [0x12] = BJ_KEY_1,
        [0x13] = BJ_KEY_2,        [0x14] = BJ_KEY_3,
        [0x15] = BJ_KEY_4,        [0x16] = BJ_KEY_6,
        [0x17] = BJ_KEY_5,        [0x19] = BJ_KEY_9,
        [0x1A] = BJ_KEY_7,        [0x1C] = BJ_KEY_8,
        [0x1D] = BJ_KEY_0,        [0x1F] = BJ_KEY_O,
        [0x20] = BJ_KEY_U,        [0x22] = BJ_KEY_I,
        [0x23] = BJ_KEY_P,        [0x24] = BJ_KEY_RETURN,
        [0x25] = BJ_KEY_L,        [0x26] = BJ_KEY_J,
        [0x28] = BJ_KEY_K,        [0x2D] = BJ_KEY_N,
        [0x2E] = BJ_KEY_M,        [0x30] = BJ_KEY_TAB,
        [0x31] = BJ_KEY_SPACE,    [0x33] = BJ_KEY_BACKSPACE,
        [0x35] = BJ_KEY_ESCAPE,   [0x37] = BJ_KEY_LWIN,
        [0x38] = BJ_KEY_LSHIFT,   [0x3A] = BJ_KEY_LMENU,
        [0x3B] = BJ_KEY_LCONTROL, [0x3C] = BJ_KEY_RSHIFT,
        [0x3D] = BJ_KEY_RMENU,    [0x3E] = BJ_KEY_RCONTROL,
        [0x7B] = BJ_KEY_LEFT,     [0x7C] = BJ_KEY_RIGHT,
        [0x7D] = BJ_KEY_DOWN,     [0x7E] = BJ_KEY_UP,
    };

    return (keyCode < 128) ? keymap[keyCode] : BJ_KEY_UNKNOWN;
}

static void cocoa_dispatch_event(struct cocoa_window *window, NSEvent *event) {
    bj_check(window);
    bj_check(event);

    NSEventType type = [event type];

    switch (type) {
        case NSEventTypeKeyDown: {
            enum bj_key key = cocoa_keycode_to_bj_key([event keyCode]);
            bj_push_key_event(&window->common, BJ_PRESS, key, [event keyCode]);
            break;
        }
        case NSEventTypeKeyUp: {
            enum bj_key key = cocoa_keycode_to_bj_key([event keyCode]);
            bj_push_key_event(&window->common, BJ_RELEASE, key, [event keyCode]);
            break;
        }
        case NSEventTypeLeftMouseDown:
        case NSEventTypeLeftMouseUp: {
            NSPoint loc = [event locationInWindow];
            int action = (type == NSEventTypeLeftMouseDown) ? BJ_PRESS : BJ_RELEASE;
            bj_push_button_event(&window->common, BJ_BUTTON_LEFT, action, (int)loc.x,
            (int)loc.y);
            break;
        }
        case NSEventTypeRightMouseDown:
        case NSEventTypeRightMouseUp: {
            NSPoint loc = [event locationInWindow];
            int action = (type == NSEventTypeRightMouseDown) ? BJ_PRESS : BJ_RELEASE;
            bj_push_button_event(&window->common, BJ_BUTTON_RIGHT, action, (int)loc.x,
            (int)loc.y);
            break;
        }
        case NSEventTypeMouseMoved:
        case NSEventTypeLeftMouseDragged:
        case NSEventTypeRightMouseDragged: {
            NSPoint loc = [event locationInWindow];
            bj_push_cursor_event(&window->common, (int)loc.x, (int)loc.y);
            break;
        }
        default:
            break;
    }
}

static void cocoa_poll_events(
    void
) {
    @autoreleasepool {
        NSEvent *event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
            untilDate:nil
            inMode:NSDefaultRunLoopMode
            dequeue:YES])) {

            struct cocoa_window *window = 0;
            if ([event window]) {
                BanjoView *view = (BanjoView *)[[event window] contentView];
                if ([view isKindOfClass:[BanjoView class]]) {
                    window = (struct cocoa_window *)objc_getAssociatedObject(view, "bj_window");
                }
            }

            if (window != 0) {
                cocoa_dispatch_event(window, event);
            }

            [NSApp sendEvent:event];
        }
    }
}

static int cocoa_get_window_size(
    const struct bj_window* p_abstract_window,
    int*                    width,
    int*                    height
) {
    bj_check_or_0(p_abstract_window);
    bj_check_or_0(width);
    bj_check_or_0(height);
    struct cocoa_window *p_window = (struct cocoa_window *)p_abstract_window;
    NSRect bounds = [(NSView *)p_window->view bounds];

    *width = bounds.size.width;
    *height = bounds.size.height;

    return 1;
}

static void cocoa_delete_window(
    struct bj_window *p_abstract_window
) {
    bj_check(p_abstract_window);
    @autoreleasepool {
        struct cocoa_window *p_window = (struct cocoa_window *)p_abstract_window;
        objc_setAssociatedObject(p_window->view, "bj_window", nil, OBJC_ASSOCIATION_ASSIGN);
        [p_window->handle close];
        bj_free(p_window);
    }
}

static struct bj_window *cocoa_create_window(
    const char*            p_title,
    uint16_t               x,
    uint16_t               y,
    uint16_t               width,
    uint16_t               height,
    uint8_t                flags
) {
  @autoreleasepool {

    // Flip Y coordinate (Cocoa uses bottom-left origin)
    NSScreen *screen = [NSScreen mainScreen];
    NSRect screenFrame = [screen frame];
    CGFloat flippedY = screenFrame.size.height - y - height;

    // Create window
    NSRect contentRect = NSMakeRect(x, flippedY, width, height);
    NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                           NSWindowStyleMaskMiniaturizable;

    NSWindow *nsWindow =
        [[NSWindow alloc] initWithContentRect:contentRect
                                    styleMask:styleMask
                                      backing:NSBackingStoreBuffered
                                        defer:NO];

    [nsWindow setTitle:[NSString stringWithUTF8String:p_title]];

    BanjoView *view = [[BanjoView alloc] initWithFrame:contentRect];
    [nsWindow setContentView:view];
    [nsWindow setDelegate:view];

    struct cocoa_window *window = bj_calloc(sizeof(struct cocoa_window));
    window->common.flags = flags;
    window->handle = nsWindow;
    window->view = view;

    objc_setAssociatedObject(view, "bj_window", (id)(void *)window,
                             OBJC_ASSOCIATION_ASSIGN);

    [nsWindow makeKeyAndOrderFront:nil];

    return (struct bj_window *)window;
  }
}

static void cocoa_end_video(
    struct bj_error **error
) {
    (void)error;
  // TODO Remove NSApplication from global?
}

static void cocoa_renderer_configure(
    struct bj_renderer* renderer,
    struct bj_window* window
) {
    @autoreleasepool {

        struct bj_renderer_data *data = renderer->data;

        // Clear previous view association if reconfiguring
        if (data->configured_view) {
            objc_setAssociatedObject(data->configured_view, "bj_sw_render", nil, OBJC_ASSOCIATION_ASSIGN);
        }

        int width, height;
        bj_get_window_size(window, &width, &height);

        if(data->buffer) {
            bj_free(data->buffer);
        }

        enum bj_pixel_mode mode = BJ_PIXEL_MODE_XRGB8888;
        size_t stride = bj_compute_bitmap_stride(width, mode);
        data->buffer = bj_calloc(stride * height);
        data->buffer_width = width;
        data->buffer_height = height;

        bj_info("framebuffer created: %dx%d", width, height);

        bj_assign_bitmap(&data->framebuffer,
            data->buffer, width, height, mode, stride
        );

        data->configured_view = ((struct cocoa_window*)window)->view;
        objc_setAssociatedObject(
            data->configured_view,
            "bj_sw_render",
            (id)(void*)renderer,
            OBJC_ASSOCIATION_ASSIGN
        );
    }
}

static struct bj_bitmap* cocoa_renderer_get_framebuffer(
    struct bj_renderer* renderer
) {
    bj_assert(renderer);
    bj_assert(renderer->data);
    return &renderer->data->framebuffer;
}

static void cocoa_renderer_present(
    struct bj_renderer* renderer,
    struct bj_window* abstract_window
) {
    (void)renderer;
    struct cocoa_window *window = (struct cocoa_window *)abstract_window;
    NSView *view = (NSView *)window->view;
    [view setNeedsDisplay:YES];
    [view displayIfNeeded];
}

static struct bj_renderer* cocoa_create_renderer(
    enum bj_renderer_type  type
) {
    (void)type;

    struct bj_renderer* renderer = bj_calloc(sizeof(struct bj_renderer));

    // This part will later depend on the renderer type
    renderer->data = bj_calloc(sizeof(struct bj_renderer_data));

    // VTable
    // Fill the list of function pointers depending on the renderer type
    renderer->configure       = cocoa_renderer_configure;
    renderer->get_framebuffer = cocoa_renderer_get_framebuffer;
    renderer->present         = cocoa_renderer_present;
    
    // end of "This part will later depend on the renderer type"

    return renderer;
}

static void cocoa_destroy_renderer(
    struct bj_renderer* renderer
) {
    bj_check(renderer);

    // This part will later depend on the renderer type
    if(renderer->data) {
        if(renderer->data->configured_view) {
            objc_setAssociatedObject(renderer->data->configured_view, "bj_sw_render", nil, OBJC_ASSOCIATION_ASSIGN);
        }
        bj_reset_bitmap(&renderer->data->framebuffer);
        bj_free(renderer->data->buffer);
    }
    bj_free(renderer->data);

    // end of "This part will later depend on the renderer type"
    bj_free(renderer);
}

static bj_bool cocoa_init_video(
    struct bj_video_layer* layer,
    struct bj_error **p_error
) {
  (void)p_error;
  bj_assert(cocoa.app == 0);

  @autoreleasepool {
    NSApplication *app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];
    [app activateIgnoringOtherApps:YES];

    layer->create_renderer           = cocoa_create_renderer;
    layer->create_window             = cocoa_create_window;
    layer->delete_window             = cocoa_delete_window;
    layer->destroy_renderer          = cocoa_destroy_renderer;
    layer->end                       = cocoa_end_video;
    layer->get_window_size           = cocoa_get_window_size;
    layer->poll_events               = cocoa_poll_events;

    return BJ_TRUE;
  }
}

struct bj_video_layer_create_info cocoa_video_layer_info = {
    .name = "cocoa",
    .create = cocoa_init_video,
};

#endif
