# Framerate Control and Presentation Modes in Banjo

## Executive Summary

This document provides a comprehensive analysis of framerate control strategies for Banjo, a C99 software-rendered game framework. It covers presentation modes, buffering strategies, platform considerations, and multiple architectural approaches for handling rendering and presentation.

**Key Recommendations:**
- Implement 3 presentation modes: Immediate, VSYNC, and Fixed
- Use draw callback architecture with damage region tracking
- Support self-continuation model for async platform compatibility
- Separate event dispatch from frame processing for maximum control
- Start with single buffering, optionally add double buffering later

---

## Table of Contents

1. [Context: Software vs GPU Rendering](#context-software-vs-gpu-rendering)
2. [Presentation Modes Explained](#presentation-modes-explained)
3. [Buffering Strategies](#buffering-strategies)
4. [Platform-Specific Implementation](#platform-specific-implementation)
5. [Architecture Options](#architecture-options)
   - [Traditional Explicit Architecture](#traditional-explicit-architecture)
   - [Draw Callback Architecture](#draw-callback-architecture)
   - [Self-Continuation Model](#self-continuation-model)
6. [Damage Regions and Optimization](#damage-regions-and-optimization)
7. [Wayland Commit Semantics](#wayland-commit-semantics)
8. [Event Dispatch vs Frame Processing](#event-dispatch-vs-frame-processing)
9. [Multi-Window Considerations](#multi-window-considerations)
10. [API Design Proposals](#api-design-proposals)
11. [Code Examples](#code-examples)
12. [Implementation Checklist](#implementation-checklist)

---

## Context: Software vs GPU Rendering

### Banjo's Current Architecture

Banjo performs **software rendering** using CPU-based bitmap manipulation:

- **Framebuffer**: `bj_bitmap` in system memory
- **Rendering**: User draws pixels directly via CPU operations
- **Presentation**: Synchronous memory copy to window surface
  - **X11**: `XPutImage()` copies pixels to X window
  - **Win32**: `BitBlt()` copies from DIB section to window DC
  - **Emscripten**: `putImageData()` copies to HTML canvas

### Key Difference from GPU Rendering

**GPU rendering** (OpenGL/Vulkan/D3D):
- Front buffer: Currently displayed by GPU
- Back buffer(s): Being rendered by GPU
- `SwapBuffers()`: Atomic pointer swap, non-blocking
- Presentation modes control GPU-display synchronization

**Software rendering** (Banjo):
- Framebuffer: In system memory
- `flush_framebuffer()`: **Synchronous memory copy** (blocks until complete)
- Presentation modes control **timing of the copy**, not buffer swaps

**Critical implication:** Some GPU-specific modes (like Mailbox) don't directly apply to software rendering.

---

## Presentation Modes Explained

### Overview

Presentation modes determine **when and how** frames are presented to the display, balancing:
- **Framerate**: How fast frames are rendered
- **Latency**: Delay between input and display
- **Tearing**: Visual artifacts from mid-refresh updates
- **Power consumption**: Unnecessary rendering wastes energy

### Industry Standard Modes (from Vulkan)

These come from Vulkan's presentation mode terminology:

```c
VK_PRESENT_MODE_IMMEDIATE_KHR   // No sync, immediate present
VK_PRESENT_MODE_MAILBOX_KHR     // Sync'd, replaceable queue (triple buffering)
VK_PRESENT_MODE_FIFO_KHR        // Sync'd, 3-frame FIFO queue (traditional VSYNC)
VK_PRESENT_MODE_FIFO_RELAXED_KHR // Adaptive VSYNC
```

For **software rendering**, we simplify to three practical modes:

---

### 1. BJ_PRESENT_IMMEDIATE

**Behavior:**
- Iterate callback runs **as fast as possible** (uncapped)
- Frame presented to display **immediately** without waiting
- Can present mid-refresh → **screen tearing occurs**
- If rendering 500fps on 60Hz monitor: 500 presents/sec, only 60 visible

**Use Cases:**
- Competitive gaming where input latency matters most
- Benchmarking/profiling maximum framerate
- Applications where tearing is acceptable

**Pros:**
- Lowest possible input latency
- Maximum framerate
- Simplest implementation

**Cons:**
- Screen tearing (updates mid-refresh)
- Wasted power (rendering frames that won't be displayed)

**Diagram:**
```
Display Refresh:  |--60Hz--|--60Hz--|--60Hz--|
Render/Present:   |||||||||||||||||||||||||||  (uncapped, tearing)
```

---

### 2. BJ_PRESENT_VSYNC

**Behavior:**
- Iterate callback runs **at display refresh rate** (e.g., 60fps on 60Hz display)
- Present **waits for vertical blanking** before copying framebuffer
- Each frame presented exactly once per refresh
- **No tearing**, smooth presentation, but higher latency

**Use Cases:**
- Visually-focused games and applications
- When smooth presentation matters more than latency
- Battery-powered devices (avoids wasted rendering)

**Pros:**
- No screen tearing
- Smooth, consistent framerate
- Power efficient (only renders when needed)

**Cons:**
- Input latency (frames queued)
- If framerate drops below refresh rate, locks to half-rate (60→30→20→15)
- More complex to implement

**Diagram:**
```
Display Refresh:  |--60Hz--|--60Hz--|--60Hz--|
Render/Present:   |  60fps |  60fps |  60fps |  (synced, no tearing)
Vblank:           ^        ^        ^
```

**Software Rendering VSYNC:**
- Single/double buffer
- Wait for vblank signal (or timer approximation)
- Copy during blanking period
- Lower latency than GPU FIFO

---

### 3. BJ_PRESENT_FIXED

**Behavior:**
- Iterate callback runs at **user-specified framerate** (e.g., 30fps, 60fps, 144fps)
- **Independent of display refresh rate**
- Manual sleep-based throttling
- Can still tear (not synced to vblank)

**Use Cases:**
- Targeting specific framerate for consistency
- Battery savings (cap at 30fps for mobile)
- Deterministic timing for simulations/replays
- Matching other systems (e.g., network tick rate)

**Pros:**
- Precise control over framerate
- Power savings vs uncapped
- Deterministic timing
- Works everywhere (no platform-specific APIs needed)

**Cons:**
- Can still tear (unless fixed FPS == display refresh rate)
- Requires accurate timing

**Diagram:**
```
Display Refresh:  |--60Hz--|--60Hz--|--60Hz--|
Render@30fps:     |---30---|---30---|---30---|  (may tear)
Render@60fps:     |--60----|--60----|--60----|  (synced if precise)
```

---

### What About Mailbox and Adaptive?

#### Mailbox (Triple Buffering)

**Why it doesn't apply to software rendering:**

Mailbox requires **asynchronous parallel work** between GPU and display. Software rendering is **synchronous** - the CPU can't draw the next frame until the copy is complete. There's no parallel work to make mailbox valuable.

**Recommendation:** **Skip mailbox** for software rendering. It's GPU-specific.

#### Adaptive VSYNC (Relaxed FIFO)

**Why it's complex for software rendering:**
- Requires detecting vblank timing accurately
- Same vblank detection problems as VSYNC
- Added complexity for marginal benefit

**Recommendation:** **Skip adaptive** for initial implementation.

---

## Buffering Strategies

### Single Buffer vs Double Buffer

For **software rendering with synchronous copy operations**, both work for all presentation modes.

### Single Buffering (1 Framebuffer)

```c
bj_bitmap* buffer = create_bitmap(width, height);

while (running) {
    user_draw(buffer);              // Draw directly to the only buffer
    flush_framebuffer(buffer);      // Synchronous copy to window
    // Copy complete - can reuse buffer for next frame
}
```

**Characteristics:**
- Simplest possible implementation
- Minimal memory usage
- Works correctly for all presentation modes
- Sufficient when copy is synchronous

### Double Buffering (2 Framebuffers)

```c
bj_bitmap* front = create_bitmap(width, height);
bj_bitmap* back = create_bitmap(width, height);

while (running) {
    user_draw(back);                 // Draw to back buffer
    flush_framebuffer(back);         // Copy back buffer to window
    swap(&front, &back);             // Swap pointers
}
```

**Advantages:**
- Always copying a complete, untouched frame
- Better preparation for future GPU rendering
- "Best practice" architecture

### Buffering vs Tearing

**Important:** Buffering strategy does **NOT** prevent tearing. Tearing is determined by **timing**:

| Mode | Single Buffer | Double Buffer | Tearing? |
|------|---------------|---------------|----------|
| **Immediate** | ✅ Works | ✅ Works | **Yes** (copying at arbitrary time) |
| **VSYNC** | ✅ Works | ✅ Works | **No** (copying during vblank) |
| **Fixed** | ✅ Works | ✅ Works | **Yes** (unless FPS == refresh rate) |

### Recommendation

**Start with single buffering** for simplicity. Optionally add double buffering later for GPU rendering compatibility.

---

## Platform-Specific Implementation

### Platform Behavior Differences

A critical distinction exists between platforms:

**Synchronous Platforms (Desktop X11/Win32):**
- Application controls timing
- Can wait/sleep before drawing
- Synchronous present operations

**Asynchronous Platforms (Wayland/Emscripten):**
- Platform controls timing
- Compositor/browser decides when to draw
- Callback-driven presentation

### The VSYNC Challenge

#### Emscripten (Web)

**Easy:** Browser provides vblank sync automatically via `requestAnimationFrame`.

```javascript
function iterate() {
    user_draw();
    ctx.putImageData(imageData, 0, 0);
    requestAnimationFrame(iterate);  // Calls iterate at next vblank
}
requestAnimationFrame(iterate);
```

**Status:** ✅ Native VSYNC support, always synced to display refresh.

---

#### Windows (Win32)

**Desktop Window Manager (DWM) sync:**

```c
#include <dwmapi.h>  // Link: dwmapi.lib

void implement_vsync_win32() {
    while (running) {
        user_draw();
        BitBlt(hdc, ...);        // Copy framebuffer to window
        DwmFlush();              // Wait for DWM's next vblank
    }
}
```

**Considerations:**
- Requires Windows Vista+ (DWM availability)
- Works reliably for windowed applications
- Fallback: Timer-based

---

#### X11 (Linux/Unix)

**Problem:** X11 has no standard API for software rendering vblank sync.

**Practical solution:** Timer-based approximation.

```c
void implement_vsync_x11_software() {
    // Query display refresh rate (RANDR extension)
    float refresh = query_refresh_rate();
    float frame_time = 1000.0f / refresh;

    uint64_t last_time = get_time_ms();

    while (running) {
        user_draw();
        XPutImage(...);
        XSync(display, False);

        // Sleep to maintain framerate
        uint64_t elapsed = get_time_ms() - last_time;
        if (elapsed < frame_time) {
            sleep_ms(frame_time - elapsed);
        }
        last_time += frame_time;
    }
}
```

**Querying refresh rate:**

```c
#include <X11/extensions/Xrandr.h>

float query_refresh_rate(Display* display, int screen) {
    XRRScreenConfiguration* conf = XRRGetScreenInfo(display,
                                   RootWindow(display, screen));
    short rate = XRRConfigCurrentRate(conf);
    XRRFreeScreenConfigInfo(conf);
    return (float)rate;
}
```

---

#### Wayland

**Fundamentally callback-driven:**

```c
// Request frame callback from compositor
struct wl_callback* callback = wl_surface_frame(surface);
wl_callback_add_listener(callback, &frame_listener, userdata);
wl_surface_commit(surface);

// Compositor calls back when ready for next frame
void frame_callback(void* data, struct wl_callback* cb, uint32_t time) {
    // Compositor says "draw now"
    user_draw();

    // Commit the frame
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage_buffer(surface, ...);
    wl_surface_commit(surface);

    wl_callback_destroy(cb);
}
```

**Key:** Application cannot control timing - compositor decides when frames are drawn.

---

### Platform Summary

| Platform | VSYNC Strategy | Control Model | Complexity |
|----------|---------------|---------------|------------|
| **Emscripten** | `requestAnimationFrame` | Async callback | Easy ✅ |
| **Wayland** | Compositor frame callbacks | Async callback | Medium |
| **Windows** | `DwmFlush()` or timer | Sync | Medium |
| **X11** | Timer-based | Sync | Medium |

---

## Architecture Options

There are multiple valid approaches to structuring the rendering API. Each has trade-offs.

### Traditional Explicit Architecture

**User owns the loop and explicitly calls present:**

```c
int main() {
    bj_initialize();
    bj_window* window = bj_create_window(...);

    bj_set_present_mode(BJ_PRESENT_VSYNC);

    while (!bj_should_close_window(window)) {
        bj_poll_events();

        bj_bitmap* fb = bj_window_framebuffer(window);
        draw_game(fb);

        bj_window_present(window);  // Flushes framebuffer
    }

    bj_destroy_window(window);
    bj_shutdown();
}
```

**Pros:**
- User has full control
- Traditional game loop structure
- Easy to understand
- Explicit about what happens when

**Cons:**
- Doesn't naturally map to async platforms (Wayland/Emscripten)
- User must handle all timing
- Present is just a flush - no opportunity for backend optimization

---

### Draw Callback Architecture

**User provides a draw callback to present, backend controls timing:**

```c
typedef void (*bj_draw_fn)(bj_bitmap* framebuffer);

void draw(bj_bitmap* fb) {
    bj_clear_bitmap(fb, 0);
    draw_game(fb);
    bj_mark_damage_all();  // Mark entire framebuffer dirty
}

int main() {
    while (running) {
        bj_dispatch_events();
        bj_present(window, draw);  // Pass draw callback
    }
}
```

**How it works:**

**Desktop (Sync):**
```c
void bj_present(bj_window* win, bj_draw_fn draw) {
    // Apply present mode timing
    if (present_mode == BJ_PRESENT_VSYNC) {
        wait_for_vblank();
    }

    // Call draw callback
    draw(win->framebuffer);

    // Flush with damage regions
    flush_framebuffer(win, &damage);
}
```

**Wayland (Async):**
```c
void bj_present(bj_window* win, bj_draw_fn draw) {
    // Store callback for later
    win->pending_draw = draw;

    // Request frame from compositor (non-blocking)
    wl_surface_frame(win->surface);
    wl_surface_commit(win->surface);
}

// Later, compositor calls:
void frame_callback(void* data, struct wl_callback* cb, uint32_t time) {
    bj_window* win = data;

    // Call user's draw callback
    win->pending_draw(win->framebuffer);

    // Commit to compositor
    flush_to_wayland(win);
}
```

**Pros:**
- Backend controls WHEN drawing happens
- Natural for async platforms
- Enables damage region optimization
- Same API works on all platforms

**Cons:**
- Less explicit than traditional approach
- Callback storage concerns (addressed by self-continuation)

---

### Self-Continuation Model

**Callback re-registers itself for the next frame (like `requestAnimationFrame`):**

```c
bj_window* window;

void draw(bj_bitmap* fb) {
    if (!running) {
        return;  // Don't queue next frame = stop rendering
    }

    draw_game(fb);
    bj_mark_damage_all();

    bj_present(window, draw);  // Re-register itself for next frame
}

int main() {
    // Kick off rendering
    bj_present(window, draw);

    // Main loop just processes events
    while (running) {
        bj_dispatch_events();
    }
}
```

**Key insight:** The callback is "live" only during execution, then immediately queues the next frame. No stale callback storage.

**Flow:**

1. Initial `bj_present(window, draw)` queues first frame
2. Platform calls `draw()` when ready
3. Inside `draw()`, user calls `bj_present(window, draw)` again
4. This queues the NEXT frame
5. Repeat...

**Pros:**
- ✅ No callback storage concerns (fresh each frame)
- ✅ Perfect match for async platforms
- ✅ Explicit continuation control (don't call = stop)
- ✅ Can change callbacks per frame

**Challenge:** On sync platforms, need to avoid infinite recursion.

**Solution: Trampoline pattern** (queue callbacks, don't execute immediately):

```c
void bj_present(bj_window* win, bj_draw_fn draw) {
    // Store for processing by bj_process_frames()
    win->pending_draw = draw;
    win->has_pending_draw = true;
}

void bj_process_frames(void) {
    for_each_window(win) {
        if (win->has_pending_draw) {
            bj_draw_fn draw = win->pending_draw;
            win->has_pending_draw = false;

            // Apply timing
            if (present_mode == BJ_PRESENT_VSYNC) {
                wait_for_vblank();
            }

            // Call draw (user calls bj_present inside, queuing next)
            draw(win->framebuffer);

            // Flush
            flush_framebuffer(win);
        }
    }
}
```

**Usage:**
```c
bj_present(window, draw);  // Kick off

while (running) {
    bj_dispatch_events();   // Process events
    bj_process_frames();    // Execute pending draws
}
```

---

## Damage Regions and Optimization

### The Concept

Instead of always copying the entire framebuffer, track which regions actually changed and only copy those.

**Example:** Moving a small sprite:
```
Old frame: Sprite at (100, 100)
New frame: Sprite at (110, 100)

Only need to copy:
- Old position (100, 100) to clear it
- New position (110, 100) to draw it
= ~128 pixels instead of 2,073,600 pixels (1080p)
```

### API Design

```c
typedef struct {
    int x, y, w, h;
} bj_rect;

// Mark region as dirty during drawing
void bj_mark_damage(int x, int y, int w, int h);

// Helper: mark entire framebuffer dirty
void bj_mark_damage_all(void);
```

**Usage:**

```c
void draw(bj_bitmap* fb) {
    // Option 1: Full redraw
    bj_clear_bitmap(fb, 0);
    draw_everything(fb);
    bj_mark_damage_all();

    // Option 2: Partial redraw
    draw_sprite(fb, sprite_x, sprite_y);
    bj_mark_damage(sprite_x, sprite_y, sprite_w, sprite_h);

    // Option 3: Multiple regions
    draw_ui(fb, 0, 0);
    bj_mark_damage(0, 0, 200, 600);

    draw_game_area(fb, 200, 0);
    bj_mark_damage(200, 0, 600, 600);
}
```

### Implementation

```c
struct bj_window {
    bj_rect damage;
    bool has_damage;
    // ...
};

static bj_window* g_current_draw_window = NULL;

void bj_mark_damage(int x, int y, int w, int h) {
    bj_window* win = g_current_draw_window;
    bj_rect new_damage = {x, y, w, h};

    if (!win->has_damage) {
        win->damage = new_damage;
        win->has_damage = true;
    } else {
        // Union the rects
        win->damage = bj_rect_union(win->damage, new_damage);
    }
}

void bj_present(bj_window* win, bj_draw_fn draw) {
    win->damage = (bj_rect){0};
    win->has_damage = false;

    g_current_draw_window = win;
    draw(win->framebuffer);
    g_current_draw_window = NULL;

    if (win->has_damage) {
        flush_framebuffer(win, &win->damage);  // Partial copy
    }
    // else: nothing changed, skip present
}
```

### Platform Support

**X11:**
```c
XPutImage(display, window, gc, ximage,
          damage.x, damage.y,    // Source offset
          damage.x, damage.y,    // Dest offset
          damage.w, damage.h);   // Size
```

**Win32:**
```c
BitBlt(hdc, damage.x, damage.y, damage.w, damage.h,
       fbdc, damage.x, damage.y, SRCCOPY);
```

**Wayland:**
```c
wl_surface_damage_buffer(surface, damage.x, damage.y, damage.w, damage.h);
```

**Benefits:**
- Reduced memory bandwidth
- Faster presents for small changes
- Automatic optimization (skip if no damage)

---

## Wayland Commit Semantics

### What is `wl_surface_commit()`?

Wayland surfaces have **double-buffered state** - pending and current. Operations modify pending state, then `commit()` makes them all active **atomically**.

```c
// All pending state (not visible yet)
wl_surface_attach(surface, new_buffer, 0, 0);
wl_surface_damage_buffer(surface, 0, 0, 800, 600);
wl_surface_set_input_region(surface, input_region);

// COMMIT: All changes become current atomically
wl_surface_commit(surface);
```

**Why this matters:**
1. **Atomicity** - Multiple properties change together
2. **Synchronization** - Frame callbacks work with commit
3. **Subsurfaces** - Parent/child surfaces update together

### Should Banjo Have `bj_commit()`?

**Wayland needs commit because:**
- Multiple pending properties (buffer, damage, input region, scale, etc.)
- Async protocol between client and compositor
- Subsurface synchronization
- Complex state management

**Banjo's situation:**
- One framebuffer (simpler state)
- Software rendering (synchronous operations)
- No subsurfaces (yet)
- Draw callback is atomic

### Recommendation: Implicit Commit

**Simple approach:** Callback return = commit.

```c
void draw(bj_bitmap* fb) {
    draw_game(fb);
    bj_mark_damage_all();
    // Returning = implicit "framebuffer ready"
}

bj_present(window, draw);  // Calls draw, then presents if damage exists
```

**Optional explicit commit** (if needed later):

```c
void bj_commit(void);  // Explicit "ready to present"

void draw(bj_bitmap* fb) {
    if (should_skip) {
        return;  // No commit = no present
    }

    draw_game(fb);
    bj_mark_damage_all();
    bj_commit();  // Explicit commit
}
```

**For now:** Implicit commit is simpler and sufficient.

---

## Event Dispatch vs Frame Processing

### The Problem

On **async platforms** (Wayland), event pumping triggers frame callbacks. On **sync platforms** (Desktop), you control when frames are drawn.

Should `bj_dispatch_events()` also process frames?

### Option 1: Combined (Simple but Inconsistent)

```c
void bj_dispatch_events() {
    platform_poll_events();  // Wayland: frame callbacks fire here
}

// Wayland: Draws happen during event dispatch
// Desktop: Draws don't happen
```

**Problem:** Inconsistent behavior across platforms.

### Option 2: Separate (Consistent, Recommended)

```c
void bj_dispatch_events(void);  // ONLY process platform events
void bj_process_frames(void);   // Execute pending draw callbacks
```

**Implementation:**

**Wayland:**
```c
void wayland_frame_callback(...) {
    bj_window* win = data;
    win->compositor_ready = true;  // Just set flag, don't draw yet
}

void bj_dispatch_events() {
    wl_display_dispatch(display);  // Sets compositor_ready flags
}

void bj_process_frames() {
    for_each_window(win) {
        if (win->compositor_ready && win->pending_draw) {
            win->compositor_ready = false;
            win->pending_draw(win->framebuffer);
            flush_to_wayland(win);
        }
    }
}
```

**Desktop:**
```c
void bj_dispatch_events() {
    platform_poll_events();  // X11/Win32 events only
}

void bj_process_frames() {
    for_each_window(win) {
        if (win->pending_draw) {
            // Apply timing
            if (present_mode == BJ_PRESENT_VSYNC) {
                wait_for_vblank();
            }

            win->pending_draw(win->framebuffer);
            flush_framebuffer(win);
        }
    }
}
```

**Usage:**
```c
bj_present(window, draw);  // Kick off

while (running) {
    bj_dispatch_events();   // All platforms: set flags
    bj_process_frames();    // All platforms: actually draw
}
```

**Benefits:**
- ✅ Consistent behavior across platforms
- ✅ User controls when frames are processed
- ✅ Can do work between events and rendering
- ✅ No accidental infinite loops in immediate mode

---

## Multi-Window Considerations

### The Challenge

```c
bj_window* win1;
bj_window* win2;

void draw1(bj_bitmap* fb) {
    draw_window1(fb);
    bj_mark_damage_all();
    bj_present(win1, draw1);
}

void draw2(bj_bitmap* fb) {
    draw_window2(fb);
    bj_mark_damage_all();
    bj_present(win2, draw2);
}

// Kick off both
bj_present(win1, draw1);
bj_present(win2, draw2);

while (running) {
    bj_dispatch_events();
    bj_process_frames();  // Processes both windows
}
```

### Present Mode Handling

**Application-wide present mode (recommended):**
- One present mode for entire application
- First window waits for vblank (if VSYNC)
- Subsequent windows in same frame don't wait again

```c
void bj_process_frames() {
    static bool vblank_waited_this_frame = false;
    static uint64_t last_frame_time = 0;
    uint64_t now = get_time_ms();

    if (now - last_frame_time > 1) {
        vblank_waited_this_frame = false;
    }

    for_each_window(win) {
        if (win->pending_draw) {
            // Wait only once per frame
            if (present_mode == BJ_PRESENT_VSYNC && !vblank_waited_this_frame) {
                wait_for_vblank();
                vblank_waited_this_frame = true;
                last_frame_time = now;
            }

            win->pending_draw(win->framebuffer);
            flush_framebuffer(win);
        }
    }
}
```

**Per-window present mode:**
- Complex, requires threading or scheduling
- Rare use case
- Skip for initial implementation

### Async Platforms with Multiple Windows

Each window gets its own frame callback:

```c
// Wayland: Each surface has independent frame callback
wl_surface_frame(win1->surface);
wl_surface_frame(win2->surface);

// Compositor decides timing per window
// (Might be different on multi-monitor setups)
```

---

## API Design Proposals

### Proposal A: Traditional Explicit

**Simple, user owns loop:**

```c
void bj_set_present_mode(bj_present_mode mode);
void bj_window_present(bj_window* window);

// Usage
bj_set_present_mode(BJ_PRESENT_VSYNC);
while (running) {
    bj_poll_events();
    bj_bitmap* fb = bj_window_framebuffer(window);
    draw_game(fb);
    bj_window_present(window);
}
```

**Pros:** Explicit, traditional
**Cons:** Doesn't map well to async platforms

---

### Proposal B: Draw Callback with Self-Continuation

**Backend controls timing, callback re-registers:**

```c
typedef void (*bj_draw_fn)(bj_bitmap* framebuffer);

void bj_mark_damage(int x, int y, int w, int h);
void bj_mark_damage_all(void);
void bj_present(bj_window* window, bj_draw_fn draw_callback);
void bj_dispatch_events(void);
void bj_process_frames(void);

// Usage
void draw(bj_bitmap* fb) {
    draw_game(fb);
    bj_mark_damage_all();
    bj_present(window, draw);  // Self-continuation
}

bj_present(window, draw);  // Kick off
while (running) {
    bj_dispatch_events();
    bj_process_frames();
}
```

**Pros:** Works naturally on all platforms, damage optimization
**Cons:** More complex, callback-based

---

### Proposal C: Two-Phase Present

**Separate "request" from "draw" from "commit":**

```c
typedef struct {
    bj_bitmap* framebuffer;
    bj_window* window;
} bj_present_context;

bj_present_context* bj_begin_present(bj_window* window);
void bj_end_present(bj_present_context* ctx, const bj_rect* damage);

// Usage
while (running) {
    bj_dispatch_events();

    bj_present_context* ctx = bj_begin_present(window);  // Waits if needed
    draw_game(ctx->framebuffer);
    bj_rect damage = {0, 0, 800, 600};
    bj_end_present(ctx, &damage);
}
```

**Pros:** Most explicit, clear timing
**Cons:** Verbose, three function calls per frame

---

### Proposal D: Hybrid (Explicit + Callback)

**Provide both APIs, user chooses:**

```c
// Explicit (traditional)
void bj_window_present(bj_window* window);

// Callback (advanced)
void bj_present(bj_window* window, bj_draw_fn draw);
void bj_dispatch_events(void);
void bj_process_frames(void);

// Damage tracking (works with both)
void bj_mark_damage(int x, int y, int w, int h);
```

**Pros:** Flexibility, gradual adoption
**Cons:** More API surface

---

### Recommended Approach

**Start with Proposal B (Draw Callback with Self-Continuation):**
- Natural for async platforms
- Enables damage optimization
- Self-continuation solves callback storage
- Can add explicit API later if needed

**API:**
```c
typedef void (*bj_draw_fn)(bj_bitmap* framebuffer);

void bj_set_present_mode(bj_present_mode mode);
void bj_set_target_framerate(int fps);

void bj_mark_damage(int x, int y, int w, int h);
void bj_mark_damage_all(void);

void bj_present(bj_window* window, bj_draw_fn draw_callback);
void bj_dispatch_events(void);
void bj_process_frames(void);
```

---

## Code Examples

### Example 1: Basic Self-Continuation (VSYNC)

```c
#include <banjo/banjo.h>

bj_window* window;
bool running = true;

void draw(bj_bitmap* fb) {
    if (!running) {
        return;  // Don't queue next frame
    }

    static float angle = 0.0f;

    bj_clear_bitmap(fb, 0x202020);
    draw_sprite(fb, 400, 300, angle);
    bj_mark_damage_all();

    angle += 6.0f;  // 6 degrees per frame at 60fps

    bj_present(window, draw);  // Queue next frame
}

int main() {
    bj_initialize(NULL);
    window = bj_create_window("VSYNC Demo", 0, 0, 800, 600, 0);

    bj_set_present_mode(BJ_PRESENT_VSYNC);

    // Kick off rendering
    bj_present(window, draw);

    while (running) {
        bj_dispatch_events();
        bj_process_frames();

        if (bj_should_close_window(window)) {
            running = false;
        }
    }

    bj_destroy_window(window);
    bj_shutdown(NULL);
    return 0;
}
```

---

### Example 2: Damage Region Optimization

```c
void draw(bj_bitmap* fb) {
    static int sprite_x = 100;
    static int sprite_y = 100;
    static int old_x = 100;
    static int old_y = 100;

    // Clear old position
    bj_fill_rect(fb, old_x, old_y, 64, 64, 0);
    bj_mark_damage(old_x, old_y, 64, 64);

    // Update position
    sprite_x++;

    // Draw new position
    draw_sprite(fb, sprite_x, sprite_y);
    bj_mark_damage(sprite_x, sprite_y, 64, 64);

    old_x = sprite_x;
    old_y = sprite_y;

    // Only ~128 pixels copied instead of entire framebuffer!
    bj_present(window, draw);
}
```

---

### Example 3: Multiple Windows

```c
bj_window* game_window;
bj_window* debug_window;

void draw_game(bj_bitmap* fb) {
    if (!running) return;

    draw_game_scene(fb);
    bj_mark_damage_all();
    bj_present(game_window, draw_game);
}

void draw_debug(bj_bitmap* fb) {
    if (!running) return;

    draw_debug_info(fb);
    bj_mark_damage_all();
    bj_present(debug_window, draw_debug);
}

int main() {
    game_window = bj_create_window("Game", 0, 0, 800, 600, 0);
    debug_window = bj_create_window("Debug", 820, 0, 400, 600, 0);

    bj_set_present_mode(BJ_PRESENT_VSYNC);

    bj_present(game_window, draw_game);
    bj_present(debug_window, draw_debug);

    while (running) {
        bj_dispatch_events();
        bj_process_frames();  // Processes both windows
    }
}
```

---

### Example 4: Immediate Mode (Uncapped)

```c
void draw(bj_bitmap* fb) {
    if (!running) return;

    static int frame = 0;
    bj_clear_bitmap(fb, frame++ % 256);
    bj_mark_damage_all();

    bj_present(window, draw);
}

int main() {
    bj_set_present_mode(BJ_PRESENT_IMMEDIATE);
    bj_present(window, draw);

    while (running) {
        bj_dispatch_events();
        bj_process_frames();  // Runs as fast as possible
    }

    // Thousands of fps!
}
```

---

### Example 5: Fixed Framerate (30fps)

```c
int main() {
    bj_set_present_mode(BJ_PRESENT_FIXED);
    bj_set_target_framerate(30);

    bj_present(window, draw);

    while (running) {
        bj_dispatch_events();
        bj_process_frames();  // Sleeps to maintain 30fps
    }
}
```

---

### Example 6: Switching Between Render Modes

```c
void game_draw(bj_bitmap* fb) {
    draw_game(fb);
    bj_mark_damage_all();

    if (entering_menu) {
        bj_present(window, menu_draw);  // Switch to menu
    } else {
        bj_present(window, game_draw);  // Continue game
    }
}

void menu_draw(bj_bitmap* fb) {
    draw_menu(fb);
    bj_mark_damage_all();

    if (exiting_menu) {
        bj_present(window, game_draw);  // Back to game
    } else {
        bj_present(window, menu_draw);  // Continue menu
    }
}
```

---

## Implementation Checklist

### Phase 1: Core Infrastructure

- [ ] Define `bj_present_mode` enum (Immediate, VSYNC, Fixed)
- [ ] Add `bj_set_present_mode()` and `bj_set_target_framerate()` APIs
- [ ] Implement high-precision timing functions
  - [ ] Linux: `clock_gettime(CLOCK_MONOTONIC)`
  - [ ] Windows: `QueryPerformanceCounter()`
- [ ] Add VSYNC support
  - [ ] Win32: `DwmFlush()`
  - [ ] X11: Timer-based with RANDR query
  - [ ] Emscripten: `requestAnimationFrame` (verify existing)
  - [ ] Wayland: Frame callbacks

### Phase 2: Draw Callback Architecture

- [ ] Define `bj_draw_fn` typedef
- [ ] Implement `bj_present(window, draw_fn)`
  - [ ] Desktop: Store callback for trampoline
  - [ ] Emscripten: Register with rAF
  - [ ] Wayland: Request frame callback
- [ ] Add `bj_dispatch_events()` (events only)
- [ ] Add `bj_process_frames()` (execute pending draws)
  - [ ] Apply present mode timing
  - [ ] Call draw callbacks
  - [ ] Flush framebuffers

### Phase 3: Damage Regions

- [ ] Define `bj_rect` struct
- [ ] Add `bj_mark_damage(x, y, w, h)`
- [ ] Add `bj_mark_damage_all()` helper
- [ ] Implement rect union for accumulation
- [ ] Update flush functions to use damage regions
  - [ ] X11: `XPutImage()` with partial region
  - [ ] Win32: `BitBlt()` with partial region
  - [ ] Wayland: `wl_surface_damage_buffer()`
  - [ ] Emscripten: `putImageData()` with partial region

### Phase 4: Multi-Window Support

- [ ] Per-window pending draw state
- [ ] Shared vblank wait (first window per frame)
- [ ] Independent frame callbacks per window (async platforms)
- [ ] Test with 2+ windows

### Phase 5: Testing & Examples

- [ ] Example: Basic VSYNC with self-continuation
- [ ] Example: Immediate mode uncapped
- [ ] Example: Fixed 30fps
- [ ] Example: Damage region optimization
- [ ] Example: Multiple windows
- [ ] Example: Switching draw callbacks
- [ ] Test on all platforms
- [ ] Performance benchmarks

### Phase 6: Optional Enhancements

- [ ] Add `bj_get_display_refresh_rate()` query
- [ ] Add `bj_has_hardware_vsync()` capability query
- [ ] Optional explicit `bj_commit()` for advanced users
- [ ] Double buffering support
- [ ] Frame time statistics API
- [ ] Adaptive VSYNC mode

---

## Performance Considerations

### Software Rendering Bottlenecks

At 1920x1080 @ 60fps:
- Pixels to copy: 2,073,600 pixels
- At 32bpp: ~8MB per frame
- At 60fps: ~480 MB/sec memory bandwidth

**Implication:** The memory copy itself takes 10-15ms. VSYNC timing accuracy is less critical than with GPU rendering.

### Damage Region Impact

**Full screen copy:** 8MB per frame
**Sprite movement:** ~512 bytes per frame (64x64 sprite)
**Speedup:** ~16,000x reduction in memory bandwidth

**Use cases for damage regions:**
- Tools/editors (UI updates)
- Sprite-based games (small moving objects)
- HUD overlays

### When to Use Each Mode

| Scenario | Mode | Reason |
|----------|------|--------|
| Competitive FPS | Immediate | Minimum input lag |
| Visual showcase | VSYNC | Smooth, tear-free |
| Mobile/battery | Fixed @ 30fps | Power savings |
| Tool/editor | VSYNC + damage | Smooth, efficient |
| Benchmarking | Immediate | Max performance |

---

## Comparison with Other Frameworks

### SDL2
- `SDL_RenderPresent()` - Simple flush
- VSYNC via hint: `SDL_HINT_RENDER_VSYNC`
- Limited control over presentation

### SFML
- `window.setFramerateLimit(60)` - Fixed mode
- `window.setVerticalSyncEnabled(true)` - VSYNC
- Explicit but basic

### Raylib
- `SetTargetFPS(60)` - Fixed mode
- VSYNC is platform-dependent
- Very simple API

### Banjo's Approach

**Advantages:**
- Explicit presentation mode control
- Natural async platform support
- Damage region optimization
- Self-continuation for callback freshness
- Maximum flexibility

---

## References

### Technical Documentation

- **Vulkan**: [VK_KHR_swapchain Present Modes](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPresentModeKHR.html)
- **Wayland**: [wl_surface Protocol](https://wayland.freedesktop.org/docs/html/apa.html#protocol-spec-wl_surface)
- **X11 RANDR**: [RandR Protocol Specification](https://www.x.org/releases/current/doc/randrproto/randrproto.txt)
- **DWM API**: [Desktop Window Manager](https://docs.microsoft.com/en-us/windows/win32/dwm/dwm-overview)
- **Emscripten**: [requestAnimationFrame](https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop)

### Articles

- **Screen Tearing**: [TechSpot Guide](https://www.techspot.com/article/2192-screen-tearing-fix-pc-gaming/)
- **VSYNC Latency**: [Blur Busters](https://blurbusters.com/faq/vsync/)
- **Game Loop Timing**: [Fix Your Timestep](https://gafferongames.com/post/fix_your_timestep/)

### Banjo-Specific

- Video backend: `inc/banjo/video.h`
- X11 impl: `src/x11/video.c`
- Win32 impl: `src/win32/video.c`
- Emscripten impl: `src/emscripten/video.c`
- Callback system: `src/main_callbacks.c`

---

## Conclusion

Banjo's framerate control system should embrace:

1. **Three practical modes**: Immediate, VSYNC, Fixed
2. **Draw callback architecture**: Natural for async platforms
3. **Self-continuation model**: Solves callback storage, matches browser/Wayland
4. **Damage region tracking**: Massive optimization potential
5. **Separated concerns**: `bj_dispatch_events()` vs `bj_process_frames()`

**Recommended API:**

```c
// Core
typedef void (*bj_draw_fn)(bj_bitmap* framebuffer);
void bj_present(bj_window* window, bj_draw_fn draw);
void bj_dispatch_events(void);
void bj_process_frames(void);

// Present modes
void bj_set_present_mode(bj_present_mode mode);
void bj_set_target_framerate(int fps);

// Damage regions
void bj_mark_damage(int x, int y, int w, int h);
void bj_mark_damage_all(void);
```

**Usage pattern:**

```c
void draw(bj_bitmap* fb) {
    draw_game(fb);
    bj_mark_damage_all();
    bj_present(window, draw);  // Self-continuation
}

bj_present(window, draw);  // Kick off
while (running) {
    bj_dispatch_events();
    bj_process_frames();
}
```

This approach provides maximum flexibility while working naturally across all platforms.

---

**Document Version**: 2.0
**Last Updated**: 2025-12-06
**Author**: Generated from extensive technical discussion
**Status**: Comprehensive architectural recommendations
