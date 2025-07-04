# Development Status {#roadmap}

\brief The features i'm working on

\todo Project is in "early draft" status: designing API, implementing core features.
This list primarily serves as a **development status tracker**, helping to keep track of progress while switching between branches.  
It provides a rough vision of the project's direction but is **not exhaustive**.  
Features are added over time and are **not listed in priority order**.  
Some features may exist in the API but not be listed here, and vice versa.  
- [ ] Basic System Layer
  - [ ] Window management (\ref window.c)
    - [X] Windows API
    - [ ] Wayland
    - [ ] Cocoa
    - [X] X11
  - [ ] Keyboard events (\ref events.c)
    - [ ] Text
      - [ ] Windows API
      - [ ] Wayland
      - [ ] X11
      - [ ] Cocoa
    - [ ] Keycode (\ref bj_window_key_event_t)
      - [X] Windows API
      - [ ] Wayland
      - [ ] Cocoa
      - [X] X11
  - [ ] Mouse Button (\ref bj_window_button_event_t)
    - [X] Windows API
    - [ ] Wayland
    - [ ] Cocoa
    - [X] X11
  - [ ] Mouse Events (\ref bj_window_cursor_event_t)
    - [X] Windows API
    - [ ] Wayland
    - [ ] Cocoa
    - [X] X11
  - [ ] Graphics Context (Software Framebuffer)
    - [X] Windows API
    - [ ] Wayland
    - [ ] Cocoa
    - [X] X11
- [ ] Software Rendering
  - [ ] 3D Rendering
    - [ ] Simple Programmable Pipeline
  - [X] Bitmap Structure (\ref bj_bitmap)
  - [X] 2D pixel drawing (\ref game_of_life.c)
  - [X] 2D primitives drawing (\ref drawing_2d.c)
- [ ] BMP File Support
  - [ ] Writing to disk
  - [X] Loading from disk (\ref load_bmp.c)
- [ ] Sound support
  - [X] Windows
  - [X] GNU/Linux
  - [X] Pull based PCM
  - [ ] Push based PCM
  - [ ] WAVE support
  - [ ] MIDI support
- [ ] Frameworks
  - [ ] Entity-Component System
- [ ] Data Structures
  - [ ] Standard Algorithms
  - [X] Sequential Array (\ref array.c)
  - [X] Hash Map (\ref htable.c)
  - [X] Linked List (\ref list.c)
- [X] Make CMake not mandatory.
- [X] Custom Allocators (\ref memory_callbacks.c)
- [X] Logging System (\ref logging.c)
- [X] Time System 



