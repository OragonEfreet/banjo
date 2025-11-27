# macOS/Cocoa Video Backend Implementation Course

## Overview

Add native macOS window support to Banjo using Cocoa/AppKit. This will enable the framework to create windows, handle events, and render 2D graphics on macOS without X11 dependencies.

**Learning approach**: Incremental implementation with testing at each milestone.

**Implementation order**:
1. Build system setup → Test compilation
2. Minimal skeleton → Test initialization
3. Window creation → Test window appears
4. Event handling → Test keyboard/mouse
5. Framebuffer rendering → Test drawing
6. Audio (future work, not covered here)

---

## Files Overview

### New Files
- `src/video_cocoa.m` (~700 lines) - Cocoa backend implementation

### Modified Files
- `src/video.c` - Register cocoa backend in layer array
- `CMakeLists.txt` - Add Cocoa option and framework linking

---

## Phase 0: macOS Development Fundamentals

### Required Reading

**Official Apple Documentation**:
- [About App Development with Cocoa](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/CocoaFundamentals/WhatIsCocoa/WhatIsCocoa.html)
- [NSApplication](https://developer.apple.com/documentation/appkit/nsapplication)
- [NSWindow](https://developer.apple.com/documentation/appkit/nswindow)
- [NSView](https://developer.apple.com/documentation/appkit/nsview)
- [NSEvent](https://developer.apple.com/documentation/appkit/nsevent)

**Objective-C Basics**:
- [Programming with Objective-C](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ProgrammingWithObjectiveC/Introduction/Introduction.html)
- [Memory Management](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/MemoryMgmt.html)

### Key Concepts

**Cocoa/AppKit**: macOS's native UI framework, built on Objective-C.

**Application Architecture**:
```
NSApplication (singleton)
    └── NSWindow (your window)
            └── NSView (content view - drawing surface)
```

**Objective-C Message Syntax**:
```objc
[object method:parameter]        // vs C: function(object, parameter)
@"string"                         // NSString literal
@autoreleasepool { code }         // Memory management scope
@interface/@implementation/@end  // Class definition
- (void)instanceMethod;           // Instance method
+ (void)classMethod;              // Class/static method
```

**Coordinate Systems** (CRITICAL):
- **Win32/X11**: Origin (0,0) at TOP-LEFT, Y+ goes DOWN
- **Cocoa**: Origin (0,0) at BOTTOM-LEFT, Y+ goes UP
- You'll flip Y coordinates for mouse events and window positioning

**Memory Management**:
- Modern Objective-C uses ARC (Automatic Reference Counting)
- Always wrap Cocoa calls in `@autoreleasepool { }` to prevent leaks
- Objects created with `alloc/init/new/copy` are retained automatically

---

## Phase 1: Build System Setup (30 minutes)

**Goal**: Configure CMake so `video_cocoa.m` compiles and links against Cocoa framework.

**Why first**: You need a working build before you can test anything.

### Theory: CMake and macOS Frameworks

**Frameworks on macOS**: System libraries packaged as `.framework` bundles (e.g., `Cocoa.framework`).

**Linking**: Use `-framework FrameworkName` flag to link.

**File extensions**:
- `.c` = C source (compiled with C compiler)
- `.m` = Objective-C source (compiled with Objective-C compiler)
- `.mm` = Objective-C++ source

CMake automatically recognizes `.m` files and uses the Objective-C compiler.

**References**:
- [CMake target_link_libraries](https://cmake.org/cmake/help/latest/command/target_link_libraries.html)
- [Apple Framework Programming](https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/BPFrameworks/Frameworks.html)

### Step 1.1: Modify CMakeLists.txt

Add the Cocoa feature option (line 17, after `BJ_FEATURE_EMSCRIPTEN`):

```cmake
option(BJ_FEATURE_COCOA "Add Cocoa/macOS support" OFF)
```

Add framework linking (after line 64):

```cmake
if(BJ_FEATURE_COCOA)
    if(APPLE)
        target_link_libraries(banjo PUBLIC "-framework Cocoa")
    endif()
endif()
```

### Step 1.2: Create Minimal File Skeleton

Create `src/video_cocoa.m`:

```objc
#include "config.h"

#if BJ_HAS_FEATURE(COCOA)

#include <banjo/video.h>
#include <banjo/error.h>
#include <banjo/log.h>

#import <Cocoa/Cocoa.h>

// Minimal stub - just returns NULL for now
static bj_video_layer* cocoa_init_video(bj_error** p_error) {
    return NULL;
}

bj_video_layer_create_info cocoa_video_layer_info = {
    .name = "Cocoa",
    .create = cocoa_init_video,
};

#endif
```

###Step 1.3: Register Backend Stub

Modify `src/video.c` (line 9, add extern):
```c
extern bj_video_layer_create_info cocoa_video_layer_info;
```

Modify `src/video.c` (layer_infos array, after emscripten):
```c
static const bj_video_layer_create_info* layer_infos[] = {
#if BJ_HAS_FEATURE(EMSCRIPTEN)
    &emscripten_video_layer_info,
#endif
#if BJ_HAS_FEATURE(COCOA)
    &cocoa_video_layer_info,
#endif
    // ... rest
};
```

### Step 1.4: Test Build

```bash
cd /Users/kevin.dorange/ws/banjo
cmake -B build -DBJ_FEATURE_COCOA=ON
cmake --build build
```

**Expected result**: Build succeeds. The backend won't work yet (returns NULL), but compilation proves your setup is correct.

**Troubleshooting**:
- "Framework not found": Ensure you're on macOS
- ".m file not compiled": Check CMake recognizes Objective-C
- Missing symbols: Verify `#if BJ_HAS_FEATURE(COCOA)` guards

---

## Phase 2: Minimal Backend Skeleton (1 hour)

**Goal**: Implement initialization and cleanup so the Cocoa backend actually loads.

**Why now**: Verify the backend registration works before adding complexity.

### Theory: NSApplication Lifecycle

**NSApplication** is a singleton that manages your app's lifecycle.

**Getting the singleton**:
```objc
NSApplication* app = [NSApplication sharedApplication];
```

Creates or returns the existing instance. Thread-safe. Call this once at startup.

**Activation Policy**:
```objc
[app setActivationPolicy:NSApplicationActivationPolicyRegular];
```

Three policies:
- `NSApplicationActivationPolicyRegular`: Normal app (appears in Dock, has menu bar)
- `NSApplicationActivationPolicyAccessory`: Background app (no Dock icon)
- `NSApplicationActivationPolicyProhibited`: No UI activation

We use **Regular** because Banjo creates visible windows.

**References**:
- [NSApplication Class](https://developer.apple.com/documentation/appkit/nsapplication)
- [NSApplicationActivationPolicy](https://developer.apple.com/documentation/appkit/nsapplicationactivationpolicy)

### Implementation

### Step 2.1: Define Backend Data Structure

Add to `video_cocoa.m` (before init function):

```objc
typedef struct {
    NSApplication* app;
} cocoa_backend;
```

### Step 2.2: Implement Initialization

Replace the stub `cocoa_init_video`:

```objc
static bj_video_layer* cocoa_init_video(bj_error** p_error) {
    @autoreleasepool {
        // Get or create NSApplication singleton
        NSApplication* app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];

        // Allocate backend state
        cocoa_backend* backend = bj_malloc(sizeof(cocoa_backend));
        backend->app = app;

        // Allocate video layer
        bj_video_layer* layer = bj_malloc(sizeof(bj_video_layer));
        layer->data = backend;

        // Assign function pointers (stubs for now)
        layer->end = cocoa_end_video;
        layer->create_window = NULL;  // TODO
        layer->delete_window = NULL;  // TODO
        layer->poll_events = NULL;    // TODO
        layer->get_window_size = NULL; // TODO
        layer->create_window_framebuffer = NULL; // TODO
        layer->flush_window_framebuffer = NULL;  // TODO

        bj_info("Cocoa backend initialized");
        return layer;
    }
}
```

### Step 2.3: Implement Cleanup

Add before `cocoa_init_video`:

```objc
static void cocoa_end_video(bj_video_layer* p_layer, bj_error** p_error) {
    cocoa_backend* backend = p_layer->data;
    bj_free(backend);
    bj_free(p_layer);
    bj_info("Cocoa backend terminated");
}
```

### Step 2.4: Test Initialization

Build and run an example:

```bash
cmake --build build
./build/examples/template
```

**Expected output**:
```
[INFO] video: Cocoa
[INFO] Cocoa backend initialized
```

Then crash (no window functions yet). That's fine - the backend loaded successfully!

**What this proves**: Your backend is registered and the init function runs.

---

## Phase 3: Window Creation (2 hours)

**Goal**: Create a window that appears on screen.

**Why now**: Visual confirmation that Cocoa APIs work.

### Theory: NSWindow and NSView

**NSWindow**: The window object containing the frame, title bar, and content area.

**Creating a window**:
```objc
NSWindow* window = [[NSWindow alloc]
    initWithContentRect:frame
              styleMask:styles
                backing:bufferingType
                  defer:deferCreation];
```

**Parameters**:
- `contentRect`: Frame in screen coordinates (x, y, width, height)
- `styleMask`: Window decorations (title bar, close button, etc.)
- `backing`: Buffering strategy (`NSBackingStoreBuffered` = double-buffered)
- `defer`: Create window immediately (NO) or lazily (YES)

**Style masks** (combine with `|`):
- `NSWindowStyleMaskTitled`: Has title bar
- `NSWindowStyleMaskClosable`: Has close button
- `NSWindowStyleMaskMiniaturizable`: Has minimize button
- `NSWindowStyleMaskResizable`: User can resize

**NSView**: The drawing surface inside the window.

We'll create a custom `NSView` subclass to handle our framebuffer later.

**References**:
- [NSWindow Class](https://developer.apple.com/documentation/appkit/nswindow)
- [NSView Class](https://developer.apple.com/documentation/appkit/nsview)
- [Window Programming Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/WinPanel/Introduction.html)

### Implementation

### Step 3.1: Define Window Structure

Add after `cocoa_backend` definition:

```objc
// Custom view class (minimal for now)
@interface BanjoView : NSView
@end

@implementation BanjoView
- (BOOL)acceptsFirstResponder {
    return YES;  // Allow keyboard input
}
@end

// Window structure
typedef struct {
    struct bj_window_t common;  // MUST be first!
    NSWindow*          window;
    BanjoView*         view;
} cocoa_window;
```

**Why `common` first**: Allows casting `cocoa_window*` to `bj_window*`.

### Step 3.2: Implement Window Creation

Add before `cocoa_init_video`:

```objc
static bj_window* cocoa_create_window(
    bj_video_layer* p_layer,
    const char* p_title,
    uint16_t x, uint16_t y,
    uint16_t width, uint16_t height,
    uint8_t flags
) {
    @autoreleasepool {
        // Flip Y coordinate (Cocoa uses bottom-left origin)
        NSScreen* screen = [NSScreen mainScreen];
        NSRect screenFrame = [screen frame];
        CGFloat flippedY = screenFrame.size.height - y - height;

        // Create window
        NSRect contentRect = NSMakeRect(x, flippedY, width, height);
        NSUInteger styleMask = NSWindowStyleMaskTitled
                             | NSWindowStyleMaskClosable
                             | NSWindowStyleMaskMiniaturizable;

        NSWindow* nsWindow = [[NSWindow alloc]
            initWithContentRect:contentRect
                      styleMask:styleMask
                        backing:NSBackingStoreBuffered
                          defer:NO];

        [nsWindow setTitle:[NSString stringWithUTF8String:p_title]];

        // Create content view
        BanjoView* view = [[BanjoView alloc] initWithFrame:contentRect];
        [nsWindow setContentView:view];

        // Allocate window structure
        cocoa_window* window = bj_malloc(sizeof(cocoa_window));
        window->common.flags = flags;
        window->window = nsWindow;
        window->view = view;

        // Show window
        [nsWindow makeKeyAndOrderFront:nil];

        bj_info("Cocoa window created: %dx%d", width, height);
        return (bj_window*)window;
    }
}
```

### Step 3.3: Implement Window Deletion

Add before `cocoa_create_window`:

```objc
static void cocoa_delete_window(
    bj_video_layer* p_layer,
    bj_window* p_abstract_window
) {
    @autoreleasepool {
        cocoa_window* window = (cocoa_window*)p_abstract_window;
        [window->window close];
        bj_free(window);
        bj_info("Cocoa window deleted");
    }
}
```

### Step 3.4: Implement get_window_size

Add before `cocoa_delete_window`:

```objc
static int cocoa_get_window_size(
    bj_video_layer* p_layer,
    const bj_window* p_abstract_window,
    int* width, int* height
) {
    cocoa_window* window = (cocoa_window*)p_abstract_window;
    NSRect frame = [window->view bounds];

    if (width) *width = (int)frame.size.width;
    if (height) *height = (int)frame.size.height;

    return 1;
}
```

### Step 3.5: Update init function

In `cocoa_init_video`, replace NULLs:

```objc
layer->create_window = cocoa_create_window;
layer->delete_window = cocoa_delete_window;
layer->get_window_size = cocoa_get_window_size;
```

### Step 3.6: Add Stub for poll_events

Add before `cocoa_create_window`:

```objc
static void cocoa_poll_events(bj_video_layer* p_layer) {
    // TODO: implement event polling
}
```

Update init:
```objc
layer->poll_events = cocoa_poll_events;
```

### Step 3.7: Test Window Creation

```bash
cmake --build build
./build/examples/window
```

**Expected result**: A window appears! It won't respond to events yet (comes next).

**Success criteria**:
- Window appears with correct title
- Window has title bar, close button, minimize button
- Window is at approximately the right position
- Window doesn't crash when closed

---

## Phase 4: Event Handling (2-3 hours)

**Goal**: Handle keyboard and mouse input.

**Why now**: Makes the window interactive before adding rendering.

### Theory: NSEvent and the Event Loop

**Event Types**: `NSEvent` represents user input (keyboard, mouse, scroll, etc.)

**Event Loop Pattern**:
1. Application polls for next event
2. Event is dispatched to appropriate window/view
3. View handles event (calls delegate or override methods)
4. Repeat

**Polling vs. Running**: Two approaches:
- **Run loop** (`[NSApp run]`): Blocks until app quits (traditional Cocoa apps)
- **Polling** (`nextEventMatchingMask:untilDate:nil`): Non-blocking, check once (what we use)

Banjo uses polling because the game controls the main loop.

**Getting events**:
```objc
NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                    untilDate:nil  // don't wait
                                       inMode:NSDefaultRunLoopMode
                                      dequeue:YES]; // remove from queue
```

**Event types**:
```objc
NSEventTypeKeyDown, NSEventTypeKeyUp          // Keyboard
NSEventTypeLeftMouseDown, NSEventTypeLeftMouseUp   // Mouse buttons
NSEventTypeMouseMoved, NSEventTypeLeftMouseDragged // Mouse motion
```

**References**:
- [NSEvent Class](https://developer.apple.com/documentation/appkit/nsevent)
- [Event Handling Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/EventOverview/Introduction/Introduction.html)
- [NSEventType](https://developer.apple.com/documentation/appkit/nseventtype)

### Theory: Key Code Mapping

**macOS key codes**: Hardware-based, consistent across keyboards.
**Banjo key codes**: Windows VK codes (Virtual Key codes).

You need a translation table mapping macOS codes → VK codes.

**Example**:
- macOS `0x00` = A key → VK `0x41` = BJ_KEY_A
- macOS `0x7B` = Left arrow → VK `0x25` = BJ_KEY_LEFT

**Reference**: [macOS Key Code List](https://eastmanreference.com/complete-list-of-applescript-key-codes)

### Implementation

### Step 4.1: Implement Event Polling

Replace the stub `cocoa_poll_events`:

```objc
static void cocoa_poll_events(bj_video_layer* p_layer) {
    @autoreleasepool {
        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:nil
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {

            // Get window from event
            cocoa_window* window = NULL;
            if ([event window]) {
                BanjoView* view = (BanjoView*)[[event window] contentView];
                if ([view isKindOfClass:[BanjoView class]]) {
                    // Store window pointer in view (we'll add this)
                    window = (cocoa_window*)objc_getAssociatedObject(
                        view, "banjo_window"
                    );
                }
            }

            // Dispatch event if we have a window
            if (window) {
                cocoa_dispatch_event(window, event);
            }

            // Let Cocoa handle event (updates, focus, etc.)
            [NSApp sendEvent:event];
        }
    }
}
```

### Step 4.2: Store Window Pointer in View

Update `cocoa_create_window` (after creating view, before showing window):

```objc
// Associate window with view for event dispatch
objc_setAssociatedObject(view, "banjo_window",
    (id)(void*)window, OBJC_ASSOCIATION_ASSIGN);
```

Add import at top of file:
```objc
#import <objc/runtime.h>
```

### Step 4.3: Implement Event Dispatcher

Add before `cocoa_poll_events`:

```objc
static void cocoa_dispatch_event(cocoa_window* window, NSEvent* event) {
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
            // Note: Y coordinate flip happens in framebuffer phase
            bj_push_button_event(&window->common, BJ_BUTTON_LEFT, action,
                                 (int)loc.x, (int)loc.y);
            break;
        }
        case NSEventTypeRightMouseDown:
        case NSEventTypeRightMouseUp: {
            NSPoint loc = [event locationInWindow];
            int action = (type == NSEventTypeRightMouseDown) ? BJ_PRESS : BJ_RELEASE;
            bj_push_button_event(&window->common, BJ_BUTTON_RIGHT, action,
                                 (int)loc.x, (int)loc.y);
            break;
        }
        case NSEventTypeMouseMoved:
        case NSEventTypeLeftMouseDragged:
        case NSEventTypeRightMouseDragged: {
            NSPoint loc = [event locationInWindow];
            bj_push_cursor_event(&window->common, (int)loc.x, (int)loc.y);
            break;
        }
    }
}
```

### Step 4.4: Implement Key Mapping

Add after event dispatcher:

```objc
static bj_key cocoa_keycode_to_bj_key(unsigned short keyCode) {
    // Lookup table: macOS key code → Banjo VK code
    static const bj_key keymap[128] = {
        [0x00] = BJ_KEY_A,       [0x01] = BJ_KEY_S,
        [0x02] = BJ_KEY_D,       [0x03] = BJ_KEY_F,
        [0x04] = BJ_KEY_H,       [0x05] = BJ_KEY_G,
        [0x06] = BJ_KEY_Z,       [0x07] = BJ_KEY_X,
        [0x08] = BJ_KEY_C,       [0x09] = BJ_KEY_V,
        [0x0B] = BJ_KEY_B,       [0x0C] = BJ_KEY_Q,
        [0x0D] = BJ_KEY_W,       [0x0E] = BJ_KEY_E,
        [0x0F] = BJ_KEY_R,       [0x10] = BJ_KEY_Y,
        [0x11] = BJ_KEY_T,       [0x12] = BJ_KEY_1,
        [0x13] = BJ_KEY_2,       [0x14] = BJ_KEY_3,
        [0x15] = BJ_KEY_4,       [0x16] = BJ_KEY_6,
        [0x17] = BJ_KEY_5,       [0x19] = BJ_KEY_9,
        [0x1A] = BJ_KEY_7,       [0x1C] = BJ_KEY_8,
        [0x1D] = BJ_KEY_0,       [0x1F] = BJ_KEY_O,
        [0x20] = BJ_KEY_U,       [0x22] = BJ_KEY_I,
        [0x23] = BJ_KEY_P,       [0x24] = BJ_KEY_RETURN,
        [0x25] = BJ_KEY_L,       [0x26] = BJ_KEY_J,
        [0x28] = BJ_KEY_K,       [0x2D] = BJ_KEY_N,
        [0x2E] = BJ_KEY_M,       [0x30] = BJ_KEY_TAB,
        [0x31] = BJ_KEY_SPACE,   [0x33] = BJ_KEY_BACKSPACE,
        [0x35] = BJ_KEY_ESCAPE,  [0x37] = BJ_KEY_LWIN,
        [0x38] = BJ_KEY_LSHIFT,  [0x3A] = BJ_KEY_LMENU,
        [0x3B] = BJ_KEY_LCONTROL,[0x3C] = BJ_KEY_RSHIFT,
        [0x3D] = BJ_KEY_RMENU,   [0x3E] = BJ_KEY_RCONTROL,
        [0x7B] = BJ_KEY_LEFT,    [0x7C] = BJ_KEY_RIGHT,
        [0x7D] = BJ_KEY_DOWN,    [0x7E] = BJ_KEY_UP,
    };

    return (keyCode < 128) ? keymap[keyCode] : BJ_KEY_UNKNOWN;
}
```

Add needed headers at top:
```objc
#include <banjo/event.h>
```

### Step 4.5: Handle Window Close

Update `BanjoView` implementation:

```objc
@implementation BanjoView
- (BOOL)acceptsFirstResponder {
    return YES;
}

// Add window delegate method
- (BOOL)windowShouldClose:(NSWindow*)sender {
    cocoa_window* window = (cocoa_window*)objc_getAssociatedObject(
        self, "banjo_window"
    );
    if (window) {
        bj_set_window_should_close(&window->common);
    }
    return NO;  // Don't actually close - Banjo controls lifecycle
}
@end
```

Update `cocoa_create_window` (after setting contentView):
```objc
[nsWindow setDelegate:view];  // View handles window delegate methods
```

Add needed header:
```objc
#include <banjo/window.h>
```

### Step 4.6: Test Events

```bash
cmake --build build
./build/examples/event_polling
```

**Expected behavior**:
- Press keys → see key events printed
- Move mouse → see cursor events
- Click → see button events
- Close button → window closes properly

**Troubleshooting**:
- Keys don't work: Check `acceptsFirstResponder` returns YES
- Mouse doesn't work: Verify event types in switch statement
- Crash on close: Check window delegate is set

---

## Phase 5: Framebuffer Rendering (2-3 hours)

**Goal**: Display pixel buffer contents in the window.

**Why last**: Rendering is the most complex part, requiring functional window and events.

### Theory: Software Rendering on macOS

**Approach**: Create a pixel buffer (malloc), draw to it, copy to window.

**Core Graphics API**:
- `CGImageCreate`: Create image from pixel data
- `CGContextDrawImage`: Draw image to graphics context
- `NSGraphicsContext`: Cocoa wrapper around Core Graphics context

**Pixel format**: We'll use `XRGB8888` (32-bit, 8 bits per channel, X=unused).

**Drawing flow**:
1. Allocate pixel buffer
2. App draws to buffer (Banjo's bitmap operations)
3. `flush_framebuffer` triggers view redraw
4. View's `drawRect:` converts buffer to `CGImage` and draws it

**References**:
- [Core Graphics](https://developer.apple.com/documentation/coregraphics)
- [Quartz 2D Programming Guide](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/Introduction/Introduction.html)
- [CGImage](https://developer.apple.com/documentation/coregraphics/cgimage)

### Implementation

### Step 5.1: Add Framebuffer Fields to Window

Update `cocoa_window` structure:

```objc
typedef struct {
    struct bj_window_t common;
    NSWindow*          window;
    BanjoView*         view;
    void*              pixel_buffer;     // NEW
    int                buffer_width;     // NEW
    int                buffer_height;    // NEW
} cocoa_window;
```

### Step 5.2: Update BanjoView to Access Buffer

Update `BanjoView` interface:

```objc
@interface BanjoView : NSView
@property (nonatomic, assign) cocoa_window* banjoWindow;
@end
```

### Step 5.3: Implement Framebuffer Creation

Add before `cocoa_poll_events`:

```objc
static bj_bitmap* cocoa_create_window_framebuffer(
    bj_video_layer* p_layer,
    const bj_window* p_abstract_window,
    bj_error** p_error
) {
    @autoreleasepool {
        cocoa_window* window = (cocoa_window*)p_abstract_window;

        // Get window size
        int width, height;
        cocoa_get_window_size(p_layer, p_abstract_window, &width, &height);

        // Free old buffer if exists
        if (window->pixel_buffer) {
            bj_free(window->pixel_buffer);
        }

        // Allocate pixel buffer
        bj_pixel_mode mode = BJ_PIXEL_MODE_XRGB8888;
        size_t stride = bj_compute_bitmap_stride(width, mode);
        window->pixel_buffer = bj_malloc(stride * height);
        window->buffer_width = width;
        window->buffer_height = height;

        // Store window pointer in view for drawing
        window->view.banjoWindow = window;

        bj_info("Framebuffer created: %dx%d", width, height);

        // Create bitmap wrapping the buffer
        return bj_create_bitmap_from_pixels(
            window->pixel_buffer, width, height, mode, stride
        );
    }
}
```

Add needed header:
```objc
#include <banjo/bitmap.h>
#include <banjo/pixel.h>
#include <banjo/memory.h>
```

### Step 5.4: Implement Framebuffer Flushing

Add before `cocoa_create_window_framebuffer`:

```objc
static void cocoa_flush_window_framebuffer(
    bj_video_layer* p_layer,
    const bj_window* p_abstract_window
) {
    @autoreleasepool {
        cocoa_window* window = (cocoa_window*)p_abstract_window;
        [window->view setNeedsDisplay:YES];
        [window->view displayIfNeeded];
    }
}
```

### Step 5.5: Implement View Drawing

Update `BanjoView` implementation:

```objc
@implementation BanjoView

- (void)drawRect:(NSRect)dirtyRect {
    if (!_banjoWindow || !_banjoWindow->pixel_buffer) {
        // No framebuffer yet - fill with black
        [[NSColor blackColor] setFill];
        NSRectFill(dirtyRect);
        return;
    }

    int width = _banjoWindow->buffer_width;
    int height = _banjoWindow->buffer_height;
    void* pixels = _banjoWindow->pixel_buffer;

    // Create Core Graphics image from pixel buffer
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = [[NSGraphicsContext currentContext] CGContext];

    CGDataProviderRef provider = CGDataProviderCreateWithData(
        NULL, pixels, width * height * 4, NULL
    );

    CGImageRef image = CGImageCreate(
        width, height,                          // dimensions
        8, 32,                                  // bits per component/pixel
        width * 4,                              // bytes per row
        colorSpace,
        kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little,
        provider, NULL, false,
        kCGRenderingIntentDefault
    );

    // Flip coordinate system (Cocoa Y-axis goes up, bitmap goes down)
    CGContextSaveGState(context);
    CGContextTranslateCTM(context, 0, height);
    CGContextScaleCTM(context, 1.0, -1.0);

    // Draw image
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);

    CGContextRestoreGState(context);

    // Cleanup
    CGImageRelease(image);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (BOOL)windowShouldClose:(NSWindow*)sender {
    cocoa_window* window = _banjoWindow;
    if (window) {
        bj_set_window_should_close(&window->common);
    }
    return NO;
}

@end
```

### Step 5.6: Update Window Creation

In `cocoa_create_window`, after creating window struct:

```objc
window->pixel_buffer = NULL;
window->buffer_width = 0;
window->buffer_height = 0;
```

### Step 5.7: Update Window Deletion

In `cocoa_delete_window`, before closing window:

```objc
if (window->pixel_buffer) {
    bj_free(window->pixel_buffer);
}
```

### Step 5.8: Update init Function

In `cocoa_init_video`, replace remaining NULLs:

```objc
layer->create_window_framebuffer = cocoa_create_window_framebuffer;
layer->flush_window_framebuffer = cocoa_flush_window_framebuffer;
```

### Step 5.9: Fix Mouse Y Coordinate

Update mouse event handling in `cocoa_dispatch_event` to flip Y:

```objc
case NSEventTypeLeftMouseDown:
case NSEventTypeLeftMouseUp: {
    NSPoint loc = [event locationInWindow];
    int action = (type == NSEventTypeLeftMouseDown) ? BJ_PRESS : BJ_RELEASE;
    int x = (int)loc.x;
    int y = window->buffer_height - (int)loc.y;  // FLIP Y
    bj_push_button_event(&window->common, BJ_BUTTON_LEFT, action, x, y);
    break;
}
// Repeat for right mouse and motion events...
```

### Step 5.10: Test Rendering

```bash
cmake --build build
./build/examples/drawing_2d
```

**Expected result**: You see colored shapes rendered in the window!

**Success criteria**:
- Window displays framebuffer contents
- Drawing updates when you call flush
- Colors are correct (not inverted/wrong channels)
- No tearing or flickering

**Troubleshooting**:
- Black screen: Check `pixel_buffer` is not NULL in `drawRect`
- Wrong colors: Verify pixel format flags (`kCGImageAlphaNoneSkipFirst`)
- Upside down: Check Y-flip in `drawRect`
- Crash: Ensure buffer is allocated before drawing

---

## Phase 6: Audio (Future Work)

Audio is not part of the video backend. Banjo has a separate audio system (`src/audio_*.c`).

For macOS audio support, you'd create `src/audio_coreaudio.m` using the **Core Audio** framework, but that's beyond this plan's scope.

---

## Testing Checklist

Once all phases are complete:

- [ ] Build succeeds with `-DBJ_FEATURE_COCOA=ON`
- [ ] Window appears with correct title and size
- [ ] Keyboard events register (try `examples/event_polling`)
- [ ] Mouse clicks work
- [ ] Mouse movement tracked
- [ ] Close button sets should_close flag
- [ ] Framebuffer renders correctly (`examples/drawing_2d`)
- [ ] Multiple windows work (`examples/moonlander` has multiple?)
- [ ] No memory leaks (run with Instruments or `leaks` tool)

---

## Common Issues and Solutions

**"Framework not found: Cocoa"**:
- You're not on macOS
- CMake option not enabled: add `-DBJ_FEATURE_COCOA=ON`

**"Undefined symbols" when linking**:
- Missing `#import <Cocoa/Cocoa.h>`
- Missing framework in CMakeLists

**Window appears but doesn't accept input**:
- Check `acceptsFirstResponder` returns YES
- Check window delegate is set

**Events not received**:
- Verify `poll_events` is being called each frame
- Check event masking in `nextEventMatchingMask`

**Framebuffer is black**:
- Verify `pixel_buffer` is allocated
- Check `drawRect` is being called (add NSLog)
- Ensure `setNeedsDisplay:YES` is called

**Colors are wrong**:
- Check `kCGImageAlphaNoneSkipFirst` flag
- Verify byte order matches `BJ_PIXEL_MODE_XRGB8888`

**Upside-down rendering**:
- Check Y-flip in `drawRect` (CTM scaling)

---

## References and Further Reading

**Apple Documentation**:
- [Cocoa Fundamentals Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/CocoaFundamentals/)
- [App Programming Guide for macOS](https://developer.apple.com/library/archive/documentation/General/Conceptual/MOSXAppProgrammingGuide/Introduction/Introduction.html)
- [Event Handling Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/EventOverview/)

**Objective-C**:
- [The Objective-C Programming Language](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ObjectiveC/)
- [Memory Management Programming Guide](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/MemoryMgmt/)

**Graphics**:
- [Quartz 2D Programming Guide](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/)

**Tools**:
- [Xcode Debugging](https://developer.apple.com/documentation/xcode/debugging/)
- [Instruments (profiling)](https://help.apple.com/instruments/)

---

## Final Implementation Checklist

Files created:
- [ ] `src/video_cocoa.m`

Files modified:
- [ ] `src/video.c` (2 lines: extern + array entry)
- [ ] `CMakeLists.txt` (2 additions: option + framework link)

Functions implemented:
- [ ] `cocoa_init_video`
- [ ] `cocoa_end_video`
- [ ] `cocoa_create_window`
- [ ] `cocoa_delete_window`
- [ ] `cocoa_poll_events`
- [ ] `cocoa_dispatch_event`
- [ ] `cocoa_get_window_size`
- [ ] `cocoa_create_window_framebuffer`
- [ ] `cocoa_flush_window_framebuffer`
- [ ] `cocoa_keycode_to_bj_key`
- [ ] `BanjoView` class with `drawRect:`, `acceptsFirstResponder`, `windowShouldClose:`

---

You now have a complete, incremental plan to implement macOS video support for Banjo. Good luck!
