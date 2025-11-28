#include "config.h"

#if BJ_HAS_FEATURE(COCOA)

#include <banjo/assert.h>
#include <banjo/error.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/video.h>
#include <banjo/window.h>

#include "check.h"
#include "window_t.h"

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

typedef struct bj_video_layer_data_t {
  NSApplication *app;
} cocoa;

typedef struct cocoa_window_t {
  struct bj_window_t common;
  NSWindow *handle;
  void *view;
  void *buffer;
  int buffer_width;
  int buffer_height;
} cocoa_window;

@interface BanjoView : NSView <NSWindowDelegate>
@end

@implementation BanjoView
- (BOOL)acceptsFirstResponder {
  return YES;
}

- (void)drawRect:(NSRect)dirtyRect {
  cocoa_window *window =
      (cocoa_window *)objc_getAssociatedObject(self, "banjo_window");
  bj_check(window);
  bj_check(window->buffer);
  bj_check(window->buffer_width > 0);
  bj_check(window->buffer_height > 0);

  CGContextRef cg_context = [[NSGraphicsContext currentContext] CGContext];
  CGDataProviderRef provider = CGDataProviderCreateWithData(
      NULL, window->buffer, window->buffer_width * window->buffer_height * 4,
      NULL);
  CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();

  CGImageRef image =
      CGImageCreate(window->buffer_width, window->buffer_height, 8, 32,
                    window->buffer_width * 4, colorspace,
                    kCGImageAlphaNoneSkipFirst | kCGImageByteOrder32Little,
                    provider, NULL, false, kCGRenderingIntentDefault);

  CGContextDrawImage(cg_context, [self bounds], image);

  CGImageRelease(image);
  CGColorSpaceRelease(colorspace);
  CGDataProviderRelease(provider);
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
  cocoa_window *window =
      (cocoa_window *)objc_getAssociatedObject(self, "banjo_window");
  if (window) {
    bj_set_window_should_close(&window->common);
  }
  return NO;
}
@end

static bj_key cocoa_keycode_to_bj_key(unsigned short keyCode) {
    static const bj_key keymap[128] = {
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

static void cocoa_dispatch_event(cocoa_window *window, NSEvent *event) {
    bj_check(window);
    bj_check(event);

    NSEventType type = [event type];

    switch (type) {
        case NSEventTypeKeyDown: {
            bj_key key = cocoa_keycode_to_bj_key([event keyCode]);
            bj_push_key_event(&window->common, BJ_PRESS, key, [event keyCode]);
            break;
        }
        case NSEventTypeKeyUp: {
            bj_key key = cocoa_keycode_to_bj_key([event keyCode]);
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

static void cocoa_poll_events(bj_video_layer *p_layer) {
    (void)p_layer;
    @autoreleasepool {
        NSEvent *event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
            untilDate:nil
            inMode:NSDefaultRunLoopMode
            dequeue:YES])) {

            cocoa_window *window = 0;
            if ([event window]) {
                BanjoView *view = (BanjoView *)[[event window] contentView];
                if ([view isKindOfClass:[BanjoView class]]) {
                    window = (cocoa_window *)objc_getAssociatedObject(view, "banjo_window");
                }
            }

            if (window != 0) {
                cocoa_dispatch_event(window, event);
            }

            [NSApp sendEvent:event];
        }
    }
}

static int cocoa_get_window_size(bj_video_layer *p_video,
                                 const bj_window *p_abstract_window, int *width,
                                 int *height) {
  (void)p_video;
  bj_check_or_0(p_abstract_window);
  bj_check_or_0(width);
  bj_check_or_0(height);
  cocoa_window *p_window = (cocoa_window *)p_abstract_window;
  NSRect bounds = [(NSView *)p_window->view bounds];

  *width = bounds.size.width;
  *height = bounds.size.height;

  return 1;
}

static void cocoa_delete_window(bj_video_layer *p_video,
                                bj_window *p_abstract_window) {
  (void)p_video;
  bj_check(p_abstract_window);
  @autoreleasepool {
    cocoa_window *p_window = (cocoa_window *)p_abstract_window;
    [p_window->handle close];
    if (p_window->buffer) {
      bj_free(p_window->buffer);
    }
    bj_free(p_window);
    bj_info("Cocoa window deleted");
  }
}

static bj_window *cocoa_create_window(bj_video_layer *p_video,
                                      const char *p_title, uint16_t x,
                                      uint16_t y, uint16_t width,
                                      uint16_t height, uint8_t flags) {
  (void)p_video;
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

    cocoa_window *window = bj_malloc(sizeof(cocoa_window));
    window->common.flags = flags;
    window->handle = nsWindow;
    window->view = view;

    objc_setAssociatedObject(view, "banjo_window", (id)(void *)window,
                             OBJC_ASSOCIATION_ASSIGN);

    [nsWindow makeKeyAndOrderFront:nil];

    bj_info("Cocoa window created: %dx%d", width, height);
    return (bj_window *)window;
  }
}

static bj_bitmap *
cocoa_create_window_framebuffer(bj_video_layer *p_video,
                                const bj_window *p_abstract_window,
                                bj_error **p_error) {
  (void)p_error;
  @autoreleasepool {
    cocoa_window *window = (cocoa_window *)p_abstract_window;

    int width, height;
    cocoa_get_window_size(p_video, p_abstract_window, &width, &height);

    if (window->buffer) {
      bj_free(window->buffer);
    }

    bj_pixel_mode mode = BJ_PIXEL_MODE_XRGB8888;
    size_t stride = bj_compute_bitmap_stride(width, mode);
    window->buffer = bj_malloc(stride * height);
    window->buffer_width = width;
    window->buffer_height = height;

    bj_info("Framebuffer created: %dx%d", width, height);

    return bj_create_bitmap_from_pixels(window->buffer, width, height, mode,
                                        stride);
  }
}

static void cocoa_flush_window_framebuffer(bj_video_layer *p_video,
                                           const bj_window *p_abstract_window) {
  (void)p_video;
  cocoa_window *p_window = (cocoa_window *)p_abstract_window;
  NSView *view = (NSView *)p_window->view;
  [view setNeedsDisplay:YES];
  [view displayIfNeeded];
}

static void cocoa_end_video(bj_video_layer *p_video, bj_error **p_error) {
  (void)p_error;

  cocoa *p_cocoa = (cocoa *)p_video->data;
  bj_free(p_cocoa);
  bj_free(p_video);
  bj_info("Cocoa backend terminated");
}

static bj_video_layer *cocoa_init_video(bj_error **p_error) {
  (void)p_error;
  @autoreleasepool {
    NSApplication *app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];
    [app activateIgnoringOtherApps:YES];

    cocoa *p_cocoa = bj_malloc(sizeof(cocoa));
    p_cocoa->app = app;

    bj_video_layer *p_layer = bj_malloc(sizeof(bj_video_layer));
    p_layer->data = p_cocoa;
    p_layer->end = cocoa_end_video;
    p_layer->create_window = cocoa_create_window;
    p_layer->delete_window = cocoa_delete_window;
    p_layer->poll_events = cocoa_poll_events;
    p_layer->get_window_size = cocoa_get_window_size;
    p_layer->create_window_framebuffer = cocoa_create_window_framebuffer;
    p_layer->flush_window_framebuffer = cocoa_flush_window_framebuffer;

    bj_info("Cocoa backend initialized");
    return p_layer;
  }
}

bj_video_layer_create_info cocoa_video_layer_info = {
    .name = "cocoa",
    .create = cocoa_init_video,
};

#endif
