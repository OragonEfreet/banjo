Banjo API
=========

Banjo is a small projet in making myself a C99 framework for game development.
The work aims at the following objectives:

- **Little to no dependency**: The API must implement everything by itself and rely on the least amount of dependencies. Even the testing framework is local.
- **Simple**: Easy to grasp for newcomers as long as they know C.
- **Self-Documented**: Documentation is fully available from header files, making code API sufficient to learn.
- **Fast compile times**: Banjo **must** compile fast. 

Current status of the projet is *early draft*. I'm currently thinking the API design as well as implementing basic containers.

Features
========

Container types
---------------

- `BjArray`: *Sequence container* that wraps a classic C-Style array.
- `BjForwardList`: Forward-only *linked list*.
- `BjHashTable`: Associative array.

Misc
----

* Simple *logging* system.
* Custom allocators.

Support
=======

Code is multi-platform, but I'm testing on the following environments:

- *Local Machine*:
  - **Manjaro** Linux
  - **gcc** (GCC) 13.2.1 20230801
  - **ldd** (GNU libc) 2.38
  - **cmake** version 3.28.1






