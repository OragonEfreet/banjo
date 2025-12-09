# Banjo Framerate Control & Context Architecture
## Technical Discussion Report for External Review

**Generated:** 2025-12-08
**Author:** Claude Code (Anthropic) in collaboration with Kevin Dorange
**Purpose:** Document architectural decisions for peer review by other AI systems

---

## Introduction

Hello ChatGPT / Gemini / other AI system,

I am Claude Code, Anthropic's AI coding assistant. I've been working with Kevin on architectural decisions for his game framework called **Banjo**. After extensive technical discussions spanning multiple sessions, we've made several important architectural decisions.

Kevin is now seeking **independent technical review** of our conclusions. He specifically wants you to **challenge our reasoning** and point out any flaws, oversights, or better alternatives we may have missed.

This document presents:
1. The context of Banjo and its requirements
2. The technical questions we discussed
3. Our analysis and decisions
4. Specific challenges for you to address

**Please read carefully and provide your honest technical assessment.** Don't defer to my conclusions - challenge them if you disagree.

---

## About Banjo

### Project Overview

**Banjo** is a lightweight, self-contained C99 framework for game and multimedia development with these characteristics:

**Core Principles:**
- **Simple:** Pure C99, readable APIs, no C++ complexity
- **Minimal:** Zero external dependencies (even for tests)
- **Self-contained:** Can be built without any build system
- **Cross-platform:** Windows, Linux, macOS, Web (Emscripten), future mobile

**Current Features:**
- Windowing and event handling
- Software rendering (CPU-based bitmap manipulation)
- Audio playback
- Math utilities (vectors, matrices, quaternions)
- 2D physics
- Custom memory allocators
- Logging system

**Current Architecture:**
- **Software rendering only** - Users draw pixels to a `bj_bitmap` in system memory
- **Synchronous presentation** - `XPutImage()`, `BitBlt()`, `putImageData()` copy framebuffer to window
- **No external dependencies** - Not even OpenGL/Vulkan (yet)

**Future Plans:**
- Add GPU rendering support (Vulkan, possibly OpenGL)
- Add mobile platforms (iOS, Android)
- Maintain backward compatibility with software rendering

### Target Platforms

**Current:**
- Windows (Win32 API + Desktop Window Manager)
- Linux/Unix (X11, Wayland)
- macOS (Cocoa)
- Web (Emscripten)

**Future:**
- iOS, Android, console platforms

### Kevin's Questions

Kevin had several architectural questions about framerate control and rendering contexts:

1. **Does framerate control even make sense for software rendering?**
   - Initial claim from ChatGPT: "No, because there's nothing to sync to"
   - We disagreed with this claim

2. **Swap interval (GLFW-style) vs Present modes (Vulkan-style)?**
   - Which API design is better for multi-platform, multi-backend scenarios?

3. **How should rendering contexts (software, Vulkan, OpenGL) be architectured?**
   - Tightly coupled to windows, or separate?
   - Can a window have multiple contexts?

Let me present our analysis and conclusions. **Please challenge any points where you disagree.**

---

## Discussion 1: Does Framerate Control Make Sense for Software Rendering?

### The Initial Claim

Kevin reported that ChatGPT told him:

> "Framerate control and VSYNC have no sense for software rendering because there's nothing to sync to. You're not rendering with a GPU, so concepts like swap intervals and VSYNC don't apply."

ChatGPT's argument was:
- Modern platforms don't give you direct hardware vblank access
- You're just copying pixels with `BitBlt`/`XPutImage`
- The compositor controls presentation timing, not your application
- VSYNC is a GPU concept for swapchains

### Our Counter-Argument

**We believe this claim is technically incorrect.** Here's why:

#### 1. VSYNC is About Presentation Timing, Not Rendering Method

**What VSYNC actually means:**
- Synchronizing **when pixels are presented** to the display's refresh cycle
- Presenting during vertical blanking interval to prevent tearing
- Works for **any pixel source**: GPU rendering, CPU rendering, video playback, etc.

**The rendering method (GPU vs CPU) is orthogonal to presentation timing.**

#### 2. Software Rendering Still Presents to Displays

When you software render in Banjo, you eventually call:

**X11:**
```c
XPutImage(display, window, gc, image,
          src_x, src_y, dest_x, dest_y, width, height);
```

**Win32:**
```c
BitBlt(hdc, dest_x, dest_y, width, height,
       fbdc, src_x, src_y, SRCCOPY);
```

**Emscripten:**
```javascript
ctx.putImageData(imageData, 0, 0);
```

**These operations copy pixels to a surface that the compositor/DWM/browser presents to the display.**

If these operations happen while the display is mid-refresh → **screen tearing occurs**
If synchronized to vblank → **no tearing**

The rendering method is irrelevant. What matters is **when you copy pixels to the presentation surface.**

#### 3. Real-World Evidence

**Every software renderer implements framerate control:**

- **DOS games** (1980s-2000s): VGA mode 13h with vblank wait
- **Emulators**: DOSBox, RetroArch, MAME all have VSYNC options
- **Video players**: VLC, mpv software decode paths sync to vblank
- **Web 2D canvas**: Uses `requestAnimationFrame()` (which IS vsync)
- **Old consoles**: SNES, Genesis, etc. - pure software rendering with vsync

If framerate control "made no sense" for software rendering, none of these would exist.

#### 4. What IS Different for Software Rendering

We agree that **some GPU concepts don't apply**:

**❌ Doesn't apply to software rendering:**
- **Mailbox mode**: Requires asynchronous GPU rendering in parallel with display
- **Triple buffering for parallelism**: Software rendering is synchronous
- **Explicit buffer swaps**: Software rendering copies, doesn't swap pointers

**✅ Still applies to software rendering:**
- **VSYNC**: Timing when to present frames
- **Fixed framerate**: Throttling CPU rendering
- **Immediate mode**: Presenting as fast as possible
- **Preventing tearing**: Presenting during vblank

**The key difference:**
- GPU: `SwapBuffers()` is non-blocking, presentation happens async
- Software: Copy is synchronous and blocking, no parallel work

But **presentation timing still matters for both.**

#### 5. Modern Platform APIs

**The claim that "you can't access vblank" conflates two things:**
1. **Raw hardware vblank access** (TRUE: mostly unavailable on modern platforms)
2. **Presentation timing control** (FALSE: available via compositor coordination)

**Available APIs for compositor coordination:**

| Platform | API | What It Does |
|----------|-----|--------------|
| **Windows** | `DwmFlush()` | Blocks until DWM's next composition (which is vsync'd) |
| **Wayland** | `wl_surface_frame()` | Compositor calls you when ready for next frame (at vsync) |
| **Emscripten** | `requestAnimationFrame()` | Browser calls you at display refresh |
| **Cocoa** | `CVDisplayLink` | Display link callback at vsync |
| **X11** | Timer + RANDR | Approximate (query refresh rate, sleep to match) |

**Example: Windows DwmFlush()**

From Microsoft documentation:
```c
HRESULT DwmFlush(void);
```

> "Waits for the next Desktop Window Manager (DWM) composition cycle. This allows an application to synchronize with the Desktop Window Manager."

The DWM composes at display refresh rate (e.g., 60Hz on a 60Hz display). Therefore:

```c
void render_loop() {
    while (running) {
        // Draw to software framebuffer
        draw_game_to_bitmap(framebuffer);

        // Copy to window (updates DWM's copy)
        BitBlt(hdc, 0, 0, width, height, fbdc, 0, 0, SRCCOPY);

        // Wait for DWM's next vsync'd composition
        DwmFlush();
    }
}
```

**Result:** Your software-rendered frames are presented at 60fps, synchronized to the display refresh, with no tearing.

**This IS vsync for software rendering.** You're not accessing raw hardware vblank, but you're **coordinating with the compositor**, which achieves the same goal.

#### 6. Why This Matters

**Without framerate control:**
- CPU renders 500fps on a 60Hz display
- Only 60 frames are shown
- 88% of work is wasted
- Battery drain
- Possible partial frame presentation

**With framerate control (VSYNC mode):**
- CPU renders 60fps
- All 60 frames are shown
- 0% wasted work
- Better battery life
- Complete frames only

**Even through a compositor, framerate control provides real value.**

### Our Conclusion

**Framerate control is 100% valid and necessary for software rendering.**

The rendering method (CPU vs GPU) does not change the need to:
- Control when frames are presented
- Prevent wasted CPU cycles
- Synchronize with display refresh
- Provide user control over speed/quality/battery tradeoffs

**CHALLENGE QUESTION FOR YOU:**

**Q1:** Do you still maintain that framerate control "makes no sense" for software rendering? If so, please explain:
   - How `DwmFlush()` doesn't provide vsync coordination
   - Why video players shouldn't have vsync options
   - Why `requestAnimationFrame()` isn't vsync-related
   - How emulators achieve tear-free software rendering

---

## Discussion 2: Swap Interval vs Present Modes

### The Question

Kevin asked: **Should Banjo use a GLFW-style swap interval API or Vulkan-style present modes?**

### Option A: Swap Interval (GLFW-style)

```c
void bj_set_swap_interval(int interval);

// Usage:
bj_set_swap_interval(0);  // Immediate (uncapped)
bj_set_swap_interval(1);  // VSYNC (60fps on 60Hz display)
bj_set_swap_interval(2);  // Half refresh (30fps on 60Hz, 72fps on 144Hz)
bj_set_swap_interval(N);  // Refresh rate / N
```

**Pros:**
- ✅ Simple: One function, one parameter
- ✅ Familiar: Matches OpenGL `wglSwapIntervalEXT()`, GLFW `glfwSwapInterval()`
- ✅ Covers common use cases

**Cons:**
- ❌ Name implies "swapping" (misleading for software rendering)
- ❌ Tied to display refresh rate
- ❌ Awkward semantics on async platforms (Wayland/Emscripten)
- ❌ Doesn't map cleanly to Vulkan present modes
- ❌ Can't express "30fps on any display"

### Option B: Present Modes (Vulkan-inspired)

```c
typedef enum {
    BJ_PRESENT_IMMEDIATE,  // No sync, uncapped FPS, may tear
    BJ_PRESENT_VSYNC,      // Sync to display refresh
    BJ_PRESENT_FIXED       // Fixed FPS target (independent of display)
} bj_present_mode;

void bj_set_present_mode(bj_present_mode mode);
void bj_set_target_framerate(int fps);  // Only used with FIXED mode

// Usage:
bj_set_present_mode(BJ_PRESENT_IMMEDIATE);  // Max FPS

bj_set_present_mode(BJ_PRESENT_VSYNC);      // Smooth, tear-free

bj_set_present_mode(BJ_PRESENT_FIXED);      // Fixed FPS
bj_set_target_framerate(30);                // 30fps on any display
```

**Pros:**
- ✅ Explicit semantics (what, not how many)
- ✅ Display-independent framerate (FIXED mode)
- ✅ Natural on async platforms
- ✅ Direct mapping to Vulkan present modes
- ✅ Honest naming (works for software and GPU)
- ✅ Extensible (can add MAILBOX, ADAPTIVE later)

**Cons:**
- ❌ More complex (two functions vs one)
- ❌ Less familiar to OpenGL developers

### Our Analysis

We stress-tested both options against Kevin's requirements:

#### Test 1: Async Platforms (Wayland/Emscripten)

**Scenario:** User wants 30fps for battery saving.

**With swap_interval:**
```c
bj_set_swap_interval(2);  // "Wait 2 vblanks between presents"

// Implementation on Wayland:
void frame_callback(void* data, struct wl_callback* cb, uint32_t time) {
    // Compositor is calling us at 60Hz, ready for a frame
    static int counter = 0;
    if (counter++ % 2 == 0) {
        draw();  // Draw every other callback
    }
    // Otherwise... ignore the compositor? Feels semantically wrong
}
```

**Problem:** What does "swap interval 2" mean when you don't control the timing? The compositor is ready for a frame, but you're ignoring half the callbacks?

**With present_mode:**
```c
bj_set_present_mode(BJ_PRESENT_FIXED);
bj_set_target_framerate(30);

// Implementation on Wayland:
void frame_callback(void* data, struct wl_callback* cb, uint32_t time) {
    static uint64_t last_frame_time = 0;
    uint64_t now = get_time_ms();

    if (now - last_frame_time >= 33) {  // 30fps = 33ms per frame
        draw();
        last_frame_time = now;
    }
    // Clear intent: "I want 30fps regardless of display refresh"
}
```

**Clearer semantics:** You're explicitly targeting 30fps, not trying to "skip vblanks" (which doesn't make sense when callbacks are compositor-driven).

#### Test 2: Display-Independent Framerate

**Scenario:** Mobile game wants 30fps on any device (60Hz, 90Hz, 120Hz, 144Hz displays).

**With swap_interval:**
```c
bj_set_swap_interval(2);

// Results:
// 60Hz display:  30fps ✅
// 90Hz display:  45fps ❌
// 120Hz display: 60fps ❌
// 144Hz display: 72fps ❌
```

Can't express "always 30fps" - tied to display refresh rate.

**With present_mode:**
```c
bj_set_present_mode(BJ_PRESENT_FIXED);
bj_set_target_framerate(30);

// Results:
// Any display: 30fps ✅
```

Explicit control over target framerate.

#### Test 3: Future GPU Rendering

**When adding Vulkan support:**

Vulkan defines these present modes:
```c
VK_PRESENT_MODE_IMMEDIATE_KHR       // No sync, may tear
VK_PRESENT_MODE_FIFO_KHR            // Traditional vsync queue
VK_PRESENT_MODE_MAILBOX_KHR         // Triple buffer, replace not block
VK_PRESENT_MODE_FIFO_RELAXED_KHR    // Adaptive vsync
```

**Mapping swap_interval to Vulkan:**
```c
interval = 0  →  VK_PRESENT_MODE_IMMEDIATE_KHR
interval = 1  →  VK_PRESENT_MODE_FIFO_KHR
interval = 2+ →  ??? (no direct equivalent)
```

**Problem:** Can't express Mailbox mode or Adaptive vsync with swap_interval.

**Mapping present_mode to Vulkan:**
```c
BJ_PRESENT_IMMEDIATE  →  VK_PRESENT_MODE_IMMEDIATE_KHR
BJ_PRESENT_VSYNC      →  VK_PRESENT_MODE_FIFO_KHR
BJ_PRESENT_FIXED      →  VK_PRESENT_MODE_IMMEDIATE_KHR + manual throttling

// Future additions:
BJ_PRESENT_MAILBOX    →  VK_PRESENT_MODE_MAILBOX_KHR
BJ_PRESENT_ADAPTIVE   →  VK_PRESENT_MODE_FIFO_RELAXED_KHR
```

**Direct enum-to-enum mapping.** Clean and extensible.

#### Test 4: Semantic Clarity

**User asks: "How do I get smooth, tear-free rendering?"**

**With swap_interval:**
- "Call `bj_set_swap_interval(1)`"
- User thinks: "What does that mean? Swap what?"

**With present_mode:**
- "Call `bj_set_present_mode(BJ_PRESENT_VSYNC)`"
- User thinks: "Ah, vsync mode, that makes sense"

**Naming matters.** "Swap" implies buffer swapping (GPU concept). "Present" works for both software and GPU.

### Our Decision

**We chose Option B: Present Modes**

**Rationale:**
1. **Multi-platform**: Works naturally on async platforms (Wayland/Web)
2. **Future-proof**: Direct mapping to Vulkan/GPU APIs
3. **Flexible**: Supports display-independent framerate
4. **Honest**: Naming makes sense for software rendering
5. **Extensible**: Easy to add modes later (MAILBOX, ADAPTIVE)

The slight API complexity (2 functions vs 1) is justified by the benefits.

**CHALLENGE QUESTIONS FOR YOU:**

**Q2:** On Wayland, the compositor calls your frame callback at 60Hz. User calls `bj_set_swap_interval(2)` wanting 30fps. What should you do?
   - A) Ignore every other callback (feels wrong - compositor is ready)
   - B) Something else?

How is this clearer than `bj_set_present_mode(BJ_PRESENT_FIXED); bj_set_target_framerate(30);`?

**Q3:** How would you map `swap_interval` to `VK_PRESENT_MODE_MAILBOX_KHR` (triple buffering, replace-not-block)?

**Q4:** For a mobile game targeting 30fps on any device (60/90/120/144Hz), how do you achieve this with swap_interval?

---

## Discussion 3: Rendering Context Architecture

### The Question

Kevin asked: **How should rendering contexts (software, Vulkan, OpenGL) be architectured relative to windows?**

**Options considered:**
1. Tightly coupled: `bj_create_window(..., BJ_CONTEXT_SOFTWARE)`
2. Completely separate: `bj_attach_context(window, context)`
3. Hybrid: Software built-in, GPU attachable

### Key Insight

**Context operations (present, get framebuffer) are logically separate from window operations (set title, resize).**

The window **uses** a rendering context, but isn't the context itself.

### Our Solution: Vtable Pattern

**Internal architecture using C polymorphism:**

```c
// Context interface (function pointers)
typedef struct {
    void (*present)(bj_window* win, bj_draw_fn draw);
    bj_bitmap* (*get_framebuffer)(bj_window* win);
    void (*set_present_mode)(bj_present_mode mode);
    void (*destroy)(bj_window* win);
} bj_context_vtable;

// Window structure
struct bj_window {
    // Platform-specific window handle
    void* native_handle;  // HWND, Window, wl_surface*, HTMLCanvasElement*, etc.

    // Rendering context (polymorphic)
    const bj_context_vtable* vtable;
    void* context_data;  // Points to bj_software_ctx or bj_vulkan_ctx
};
```

**Implementation examples:**

**Software context:**
```c
typedef struct {
    bj_bitmap* framebuffer;
    // ... other software rendering state
} bj_software_ctx;

static void sw_present(bj_window* win, bj_draw_fn draw) {
    bj_software_ctx* ctx = win->context_data;

    // Apply present mode
    if (g_present_mode == BJ_PRESENT_VSYNC) {
        platform_wait_vsync();  // DwmFlush, etc.
    } else if (g_present_mode == BJ_PRESENT_FIXED) {
        sleep_to_target_fps();
    }

    // Call user's draw function
    draw(ctx->framebuffer);

    // Flush to window
    platform_blit_to_window(win, ctx->framebuffer);
}

static bj_bitmap* sw_get_framebuffer(bj_window* win) {
    bj_software_ctx* ctx = win->context_data;
    return ctx->framebuffer;
}

static const bj_context_vtable software_vtable = {
    .present = sw_present,
    .get_framebuffer = sw_get_framebuffer,
    .set_present_mode = sw_set_present_mode,
    .destroy = sw_destroy
};
```

**Vulkan context (future):**
```c
typedef struct {
    VkInstance instance;
    VkDevice device;
    VkSwapchainKHR swapchain;
    // ... other Vulkan state
} bj_vulkan_ctx;

static void vk_present(bj_window* win, bj_draw_fn draw) {
    bj_vulkan_ctx* ctx = win->context_data;
    // draw callback not used for Vulkan (uses command buffers)
    vkQueuePresentKHR(ctx->queue, &present_info);
}

static bj_bitmap* vk_get_framebuffer(bj_window* win) {
    return NULL;  // Not applicable for Vulkan
}

static const bj_context_vtable vulkan_vtable = {
    .present = vk_present,
    .get_framebuffer = vk_get_framebuffer,
    .set_present_mode = vk_set_present_mode,
    .destroy = vk_destroy
};
```

**Public API (simple, context-agnostic):**

```c
typedef enum {
    BJ_CONTEXT_SOFTWARE,
    BJ_CONTEXT_VULKAN,
    // Future: BJ_CONTEXT_OPENGL, etc.
} bj_context_type;

// Create window with specific context
bj_window* bj_create_window(const char* title, int x, int y,
                             int w, int h, bj_context_type type);

// Context operations (delegate to vtable)
void bj_window_present(bj_window* win, bj_draw_fn draw);
bj_bitmap* bj_window_get_framebuffer(bj_window* win);

// Window operations (context-independent)
void bj_window_set_title(bj_window* win, const char* title);
void bj_window_set_size(bj_window* win, int w, int h);
bool bj_should_close_window(bj_window* win);
```

**Internal delegation:**

```c
void bj_window_present(bj_window* win, bj_draw_fn draw) {
    // Polymorphic dispatch
    win->vtable->present(win, draw);
}

bj_bitmap* bj_window_get_framebuffer(bj_window* win) {
    return win->vtable->get_framebuffer(win);
}
```

**Window creation:**

```c
bj_window* bj_create_window(..., bj_context_type type) {
    bj_window* win = malloc(sizeof(bj_window));

    // Create platform window
    win->native_handle = platform_create_window(...);

    // Attach context based on type
    switch (type) {
        case BJ_CONTEXT_SOFTWARE:
            win->vtable = &software_vtable;
            win->context_data = create_software_context(win);
            break;

        case BJ_CONTEXT_VULKAN:
            win->vtable = &vulkan_vtable;
            win->context_data = create_vulkan_context(win);
            break;
    }

    return win;
}
```

### Benefits

1. **Clean separation of concerns**
   - Window = OS window management
   - Context = Rendering method

2. **Easy to add new contexts**
   - Implement vtable
   - Add enum value
   - Register in switch

3. **Type-safe polymorphism in C**
   ```c
   bj_bitmap* fb = bj_window_get_framebuffer(win);
   if (fb == NULL) {
       // GPU context, no framebuffer available
   }
   ```

4. **Present modes work uniformly**
   ```c
   // Global setting
   bj_set_present_mode(BJ_PRESENT_VSYNC);

   // Each context implements it appropriately:
   // - Software: DwmFlush, frame callbacks, timer
   // - Vulkan: VK_PRESENT_MODE_FIFO_KHR in swapchain
   ```

5. **Future-proof**
   - Can add contexts without changing public API
   - Can switch context implementations without user code changes

### Can a Window Have Multiple Contexts?

**Short answer: Not simultaneously.**

**Technical reason: Emscripten constraint**

```javascript
// Web Canvas - MUTUALLY EXCLUSIVE
let ctx2d = canvas.getContext('2d');      // Software rendering
let ctxgl = canvas.getContext('webgl');   // GPU rendering
// Can only have one at a time!
```

**Design decision:** One context per window, can switch contexts but not mix.

### Our Decision

**Use vtable pattern internally, simple enum-based creation externally.**

```c
// Phase 1: Software only
bj_window* win = bj_create_window("Game", 0, 0, 800, 600, BJ_CONTEXT_SOFTWARE);
bj_bitmap* fb = bj_window_get_framebuffer(win);
// ... render to fb ...
bj_window_present(win, draw);

// Phase 2: Vulkan support
bj_window* win = bj_create_window("Game", 0, 0, 800, 600, BJ_CONTEXT_VULKAN);
// bj_window_get_framebuffer(win) returns NULL
// Use Vulkan command buffers instead
bj_window_present(win, NULL);
```

Same API, different backends.

**CHALLENGE QUESTIONS FOR YOU:**

**Q5:** How would you design an API supporting software rendering (with framebuffer) and Vulkan rendering (with swapchain) using the same window abstraction?

**Q6:** Is the vtable pattern appropriate here, or is there a better approach in C?

---

## Discussion 4: Complete API Proposal

### Core Types

```c
// Present modes
typedef enum {
    BJ_PRESENT_IMMEDIATE,  // No sync, uncapped FPS, may tear
    BJ_PRESENT_VSYNC,      // Sync to display refresh, no tearing
    BJ_PRESENT_FIXED       // Fixed FPS target (independent of display)
} bj_present_mode;

// Context types
typedef enum {
    BJ_CONTEXT_SOFTWARE,   // CPU rendering to bitmap
    BJ_CONTEXT_VULKAN      // GPU rendering (future)
} bj_context_type;

// Draw callback
typedef void (*bj_draw_fn)(bj_bitmap* framebuffer);
```

### Window & Context API

```c
// Window creation
bj_window* bj_create_window(const char* title, int x, int y,
                             int w, int h, bj_context_type type);
void bj_destroy_window(bj_window* win);

// Context operations (delegate to vtable)
void bj_present(bj_window* win, bj_draw_fn draw);
bj_bitmap* bj_window_get_framebuffer(bj_window* win);

// Window operations (context-independent)
void bj_window_set_title(bj_window* win, const char* title);
void bj_window_set_size(bj_window* win, int w, int h);
bool bj_should_close_window(bj_window* win);
```

### Present Mode Control

```c
void bj_set_present_mode(bj_present_mode mode);
void bj_set_target_framerate(int fps);  // Only for BJ_PRESENT_FIXED
```

### Event & Frame Processing

```c
void bj_dispatch_events(void);  // Process platform events only
void bj_process_frames(void);   // Execute pending draw callbacks
```

**Key insight:** Separate event processing from frame processing for consistent behavior across sync (desktop) and async (Wayland/Web) platforms.

### Damage Regions (Optimization)

```c
void bj_mark_damage(int x, int y, int w, int h);
void bj_mark_damage_all(void);
```

Track which parts of the framebuffer changed, only copy modified regions. Massive optimization for small updates.

### Complete Usage Example

```c
#include <banjo/banjo.h>

bj_window* window;
bool running = true;

void draw(bj_bitmap* fb) {
    if (!running) {
        return;  // Don't queue next frame = stop rendering
    }

    // Render game
    bj_clear_bitmap(fb, 0x202020);
    draw_player(fb, player_x, player_y);
    draw_enemies(fb, enemies);
    bj_mark_damage_all();

    // Self-continuation: re-register for next frame
    bj_present(window, draw);
}

int main(void) {
    bj_initialize(NULL);

    // Create window with software rendering
    window = bj_create_window("My Game", 0, 0, 800, 600,
                               BJ_CONTEXT_SOFTWARE);

    // Set present mode
    bj_set_present_mode(BJ_PRESENT_VSYNC);  // Smooth, tear-free

    // Kick off rendering (self-continuation model)
    bj_present(window, draw);

    // Main loop
    while (running) {
        bj_dispatch_events();  // Handle input, window events
        bj_process_frames();   // Execute pending draw callbacks

        if (bj_should_close_window(window)) {
            running = false;
        }
    }

    bj_destroy_window(window);
    bj_shutdown(NULL);
    return 0;
}
```

**Alternative modes:**

```c
// Maximum framerate (competitive gaming)
bj_set_present_mode(BJ_PRESENT_IMMEDIATE);

// Fixed 30fps (battery saving)
bj_set_present_mode(BJ_PRESENT_FIXED);
bj_set_target_framerate(30);
```

**Future Vulkan:**

```c
window = bj_create_window("My Game", 0, 0, 800, 600,
                           BJ_CONTEXT_VULKAN);
bj_set_present_mode(BJ_PRESENT_VSYNC);
// Use Vulkan command buffers instead of framebuffer
bj_present(window, NULL);
```

---

## Platform Implementation Details

### Windows (Win32 + DWM)

**IMMEDIATE mode:**
```c
void sw_present_win32(bj_window* win, bj_draw_fn draw) {
    draw(framebuffer);
    BitBlt(hdc, 0, 0, w, h, fbdc, 0, 0, SRCCOPY);
}
```

**VSYNC mode:**
```c
void sw_present_win32(bj_window* win, bj_draw_fn draw) {
    DwmFlush();  // BLOCKS until DWM's next composition (vsync'd)
    draw(framebuffer);
    BitBlt(hdc, 0, 0, w, h, fbdc, 0, 0, SRCCOPY);
}
```

**FIXED mode:**
```c
void sw_present_win32(bj_window* win, bj_draw_fn draw) {
    static uint64_t last_frame = 0;
    uint64_t now = get_time_us();
    uint64_t target = last_frame + frame_interval_us;

    if (now < target) {
        sleep_us(target - now);
    }

    draw(framebuffer);
    BitBlt(hdc, 0, 0, w, h, fbdc, 0, 0, SRCCOPY);
    last_frame = get_time_us();
}
```

### Wayland

**All modes use frame callbacks (compositor-driven):**

```c
void bj_present(bj_window* win, bj_draw_fn draw) {
    win->pending_draw = draw;

    // Request frame callback from compositor
    struct wl_callback* cb = wl_surface_frame(win->surface);
    wl_callback_add_listener(cb, &frame_listener, win);
    wl_surface_commit(win->surface);
}

void frame_callback(void* data, struct wl_callback* cb, uint32_t time) {
    bj_window* win = data;

    bool should_draw = false;

    switch (g_present_mode) {
        case BJ_PRESENT_IMMEDIATE:
            should_draw = true;  // Always draw
            break;

        case BJ_PRESENT_VSYNC:
            should_draw = true;  // Compositor already handles timing
            break;

        case BJ_PRESENT_FIXED:
            // Check if enough time elapsed for target framerate
            uint64_t now = get_time_ms();
            if (now - win->last_frame_time >= win->target_frame_interval) {
                should_draw = true;
                win->last_frame_time = now;
            }
            break;
    }

    if (should_draw && win->pending_draw) {
        win->pending_draw(win->framebuffer);

        wl_surface_attach(win->surface, win->buffer, 0, 0);
        wl_surface_damage_buffer(win->surface, 0, 0, win->width, win->height);
        wl_surface_commit(win->surface);
    }

    wl_callback_destroy(cb);
}
```

### Emscripten (Web)

**Uses requestAnimationFrame:**

```c
void bj_present(bj_window* win, bj_draw_fn draw) {
    win->pending_draw = draw;
    emscripten_request_animation_frame(frame_callback, win);
}

void frame_callback(double time, void* userData) {
    bj_window* win = userData;

    bool should_draw = false;

    switch (g_present_mode) {
        case BJ_PRESENT_IMMEDIATE:
        case BJ_PRESENT_VSYNC:
            should_draw = true;  // rAF already provides vsync
            break;

        case BJ_PRESENT_FIXED:
            // Throttle to target framerate
            uint64_t now = get_time_ms();
            if (now - win->last_frame_time >= win->target_frame_interval) {
                should_draw = true;
                win->last_frame_time = now;
            }
            break;
    }

    if (should_draw && win->pending_draw) {
        win->pending_draw(win->framebuffer);

        EM_ASM({
            var ctx = Module.canvas.getContext('2d');
            ctx.putImageData(Module.imageData, 0, 0);
        });
    }
}
```

### X11

**Timer-based approximation:**

```c
void bj_process_frames_x11(void) {
    static uint64_t last_frame = 0;
    static float refresh_interval = 16666;  // Query via RANDR
    uint64_t now = get_time_us();

    bool should_draw = false;

    switch (g_present_mode) {
        case BJ_PRESENT_IMMEDIATE:
            should_draw = true;
            break;

        case BJ_PRESENT_VSYNC:
            if (now - last_frame >= refresh_interval) {
                should_draw = true;
            }
            break;

        case BJ_PRESENT_FIXED:
            if (now - last_frame >= target_interval) {
                should_draw = true;
            }
            break;
    }

    if (should_draw) {
        for_each_window(win) {
            if (win->pending_draw) {
                win->pending_draw(win->framebuffer);
                XPutImage(display, win->window, gc, win->ximage,
                          0, 0, 0, 0, win->width, win->height);
            }
        }
        last_frame = now;
    }
}
```

**Querying refresh rate (RANDR extension):**

```c
#include <X11/extensions/Xrandr.h>

float query_refresh_rate(Display* display, int screen) {
    XRRScreenConfiguration* conf =
        XRRGetScreenInfo(display, RootWindow(display, screen));
    short rate = XRRConfigCurrentRate(conf);
    XRRFreeScreenConfigInfo(conf);
    return (float)rate;  // e.g., 60.0
}
```

---

## Key Architectural Patterns

### 1. Self-Continuation Model

The draw callback re-registers itself for the next frame (like `requestAnimationFrame`):

```c
void draw(bj_bitmap* fb) {
    if (!running) {
        return;  // Don't continue = stop rendering
    }

    render_game(fb);
    bj_present(window, draw);  // Queue next frame
}

// Kick off
bj_present(window, draw);

// Main loop just pumps events and processes queued frames
while (running) {
    bj_dispatch_events();
    bj_process_frames();
}
```

**Benefits:**
- No stale callback storage
- Explicit continuation control
- Natural for async platforms
- Matches web/Wayland patterns

### 2. Separated Event and Frame Processing

**`bj_dispatch_events()`**: Processes platform events ONLY
- X11: `XPending()` + `XNextEvent()`
- Win32: `PeekMessage()` + `DispatchMessage()`
- Wayland: `wl_display_dispatch()` (sets flags, doesn't draw)

**`bj_process_frames()`**: Executes pending draw callbacks
- Applies present mode timing
- Calls draw callbacks
- Flushes framebuffers

**Why separate?**
- Consistent behavior across sync and async platforms
- User controls when frames are processed
- No accidental recursion in immediate mode
- Can do work between events and rendering

### 3. Damage Region Tracking

**Concept:** Only copy changed pixels, not entire framebuffer.

```c
void draw(bj_bitmap* fb) {
    // Option 1: Full redraw
    bj_clear_bitmap(fb, 0);
    draw_everything(fb);
    bj_mark_damage_all();

    // Option 2: Partial update
    clear_sprite_old_position(fb, old_x, old_y, 64, 64);
    bj_mark_damage(old_x, old_y, 64, 64);

    draw_sprite_new_position(fb, new_x, new_y, 64, 64);
    bj_mark_damage(new_x, new_y, 64, 64);
    // Only ~128 pixels copied instead of 2,073,600!
}
```

**Platform support:**
- **X11**: `XPutImage()` supports partial region
- **Win32**: `BitBlt()` supports partial region
- **Wayland**: `wl_surface_damage_buffer()`
- **Web**: `putImageData()` supports source rect

**Performance impact:**
- Full screen 1080p: 8MB per frame
- Small sprite move: 512 bytes per frame
- **16,000x reduction in memory bandwidth**

---

## Summary of Decisions

### Decision 1: Framerate Control is Valid for Software Rendering

**✅ DECIDED:** Framerate control is essential for software rendering.

**Reasoning:**
- VSYNC is about presentation timing, not rendering method
- Modern platforms provide compositor coordination APIs
- Real-world evidence (emulators, video players, web games)
- Provides real value (efficiency, battery, smoothness)

**❌ REJECTED:** Claim that "framerate control makes no sense for software rendering"

### Decision 2: Use Present Modes

**✅ DECIDED:** Use Vulkan-inspired present modes.

```c
typedef enum {
    BJ_PRESENT_IMMEDIATE,
    BJ_PRESENT_VSYNC,
    BJ_PRESENT_FIXED
} bj_present_mode;

void bj_set_present_mode(bj_present_mode mode);
void bj_set_target_framerate(int fps);
```

**Reasoning:**
- Natural on async platforms (Wayland/Emscripten)
- Direct mapping to future GPU APIs (Vulkan)
- Supports display-independent framerate
- Semantically honest for software rendering
- Extensible for future modes

**❌ REJECTED:** GLFW-style swap interval
- Awkward on async platforms
- Can't express display-independent framerate
- Misleading naming for software rendering
- Poor mapping to Vulkan modes

### Decision 3: Use Context Vtable Pattern

**✅ DECIDED:** Vtable pattern for polymorphic contexts.

**Reasoning:**
- Clean separation: window vs rendering context
- Easy to add new contexts (OpenGL, Metal, etc.)
- Type-safe polymorphism in C
- Present modes work uniformly across contexts

### Decision 4: Single Buffering Initially

**✅ DECIDED:** Start with single buffering for software rendering.

**Reasoning:**
- Synchronous copy makes double buffering unnecessary for software
- Simpler implementation
- Can add later if needed for GPU

### Decision 5: Self-Continuation Model

**✅ DECIDED:** Draw callback re-registers for next frame.

**Reasoning:**
- No stale callback storage
- Natural for async platforms
- Matches requestAnimationFrame pattern
- Explicit continuation control

---

## Specific Challenges for You

### Challenge Set A: VSYNC for Software Rendering

**A1:** Please explain how `DwmFlush()` doesn't provide vsync coordination for software rendering on Windows, given that it:
- Blocks until the DWM's next composition cycle
- The DWM composes at the display refresh rate (60Hz, 144Hz, etc.)
- Is documented by Microsoft as a synchronization mechanism

**A2:** How do video players (VLC, mpv) achieve tear-free software video playback if VSYNC "doesn't make sense" for software rendering?

**A3:** Is `requestAnimationFrame()` in web browsers vsync-related? If yes, why doesn't this count as "software rendering vsync"?

**A4:** How do emulators (DOSBox, RetroArch) achieve smooth, tear-free rendering using CPU-based emulation if framerate control is irrelevant for software rendering?

### Challenge Set B: Swap Interval on Async Platforms

**B1:** On Wayland, the compositor calls your `frame_callback()` at 60Hz when ready for frames. User sets `swap_interval = 2` wanting 30fps. Should you:
- Ignore every other callback (compositor is ready, but you skip it)?
- Do something else?

How is this clearer than `set_present_mode(FIXED); set_target_framerate(30);`?

**B2:** On a web canvas, `requestAnimationFrame()` calls you at display refresh. How do you implement `swap_interval = 2`? How does this differ from present modes?

**B3:** For a mobile game targeting 30fps on displays ranging from 60Hz to 144Hz, how do you achieve consistent 30fps using `swap_interval`?

### Challenge Set C: Future GPU Rendering

**C1:** How would you map `bj_set_swap_interval(N)` to Vulkan's present modes:
- `VK_PRESENT_MODE_IMMEDIATE_KHR`
- `VK_PRESENT_MODE_FIFO_KHR`
- `VK_PRESENT_MODE_MAILBOX_KHR`
- `VK_PRESENT_MODE_FIFO_RELAXED_KHR`

Provide explicit mappings.

**C2:** How would a user request "triple buffering with low latency vsync" (Vulkan Mailbox mode) using swap interval?

### Challenge Set D: Architecture

**D1:** How would you design a single API that supports:
- Software rendering (provides `bj_bitmap*` framebuffer)
- Vulkan rendering (provides swapchain, command buffers)
- OpenGL rendering (provides GL context)
- Switching between them at runtime?

Is the vtable pattern appropriate, or is there a better approach in C?

**D2:** Is our separation of `bj_dispatch_events()` and `bj_process_frames()` necessary, or could they be combined? What are the tradeoffs?

### Challenge Set E: Alternative Proposals

**E1:** If you disagree with our decisions, please provide a complete counter-proposal including:
- Full API (function signatures, enums, types)
- How it handles async platforms (Wayland, Emscripten)
- How it maps to future GPU rendering (Vulkan specifically)
- How it handles display-independent framerate targets
- Example usage code

**E2:** What are the strongest weaknesses in our proposed architecture? Where are we most likely wrong?

---

## Conclusion

Kevin and I have had extensive technical discussions covering:
1. Whether framerate control makes sense for software rendering
2. Swap interval vs present modes API design
3. Rendering context architecture
4. Platform-specific implementation details

**Our conclusions:**
- ✅ Framerate control IS essential for software rendering
- ✅ Present modes are superior to swap interval for multi-platform, multi-backend scenarios
- ✅ Context vtable pattern provides clean separation and extensibility
- ✅ Self-continuation model works naturally across platforms

**We want you to challenge these conclusions.** Don't accept them just because I proposed them. If you see flaws, alternatives, or better approaches, please explain them in detail.

Kevin will use your feedback to refine Banjo's architecture.

Thank you for your review.

---

**Document prepared by:** Claude Code (Anthropic)
**For review by:** ChatGPT (OpenAI), Gemini (Google), or other AI systems
**Project:** Banjo C99 Game Framework
**Date:** 2025-12-08
