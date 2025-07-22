# Development Status {#roadmap}

\brief The features i'm working on

\todo Project is in "early draft" status: designing API, implementing core features.
This list primarily serves as a **development status tracker**, helping to keep track of progress while switching between branches.  
It provides a rough vision of the project's direction but is **not exhaustive**.  
Features are added over time and are **not listed in priority order**.  
Some features may exist in the API but not be listed here, and vice versa.  
- [ ] Basic System Layers
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
    - [ ] Keycode 
      - [X] Windows API
      - [ ] Wayland
      - [ ] Cocoa
      - [X] X11
  - [ ] Mouse Button 
    - [X] Windows API
    - [ ] Wayland
    - [ ] Cocoa
    - [X] X11
  - [ ] Mouse Events 
    - [X] Windows API
    - [ ] Wayland
    - [ ] Cocoa
    - [X] X11
  - [ ] Graphics Context 
    - [X] Windows API
    - [ ] Wayland
    - [ ] Cocoa
    - [X] X11
  - [ ] Event Polling API
    - [X] Generic "bj_event" struct
    - [X] Event buffering structure (linked-list or ring buffer)
    - [X] Poll event fillfunction
    - [X] Dispatch-to-callback function
    - [ ] Polling API
- [ ] Software Rendering
  - [ ] 3D Rendering
    - [ ] Simple Programmable Pipeline
  - [X] Bitmap Structure (\ref bj_bitmap)
  - [X] 2D pixel drawing 
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



