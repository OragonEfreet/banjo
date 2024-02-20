# API Design {#api}

\brief How to work with Banjo API

## Objects and Handles

In Banjo, the term _object_ refers to any structured memory, such as an array or a linked list.
Most of the time, objects are abstracted away and cannot be manipulated directly by the user except through an identifier, called _handle_.
This handle is usually an opaque pointer to the underlying memory.

The API follows a consistent terminology and naming convention for such structures.
For example, the concept of a **world** object is provided through an _handle_ called `BjWorld`.
When a handle is _0_ or `BJ_NULL_HANDLE`, it does not represent any valid object:

```c
BjWorld world = BJ_NULL_HANDLE;
```

To **create** a world object, you need to call a specific function called `bj_world_create()`.
This function needs to be provided an info structure that describes how the object must be created.
This structure is `BjWorldInfo` and its definition depends on the object type:

```c
BjWorldInfo info = {
    .option1 = ...,
    .option2 = ...,
    ...
};

world = bj_world_create(&info, 0);
```



When the object is not needed anymore, we need to **destroy** the object using `bj_world_destroy()`:

```c
bj_world_destroy(world);
```

All functions performing directly on a **world** object will be prefixed with *bj_world_*.

In the API reference, you will see a lot of such object/handle APIs.
For example, `BjArray` and `BjHashTable`.

 Each time, they are provided with a small table resuming the list of mandatory types/functions associated to it.
 While the symbol names can easily be infered from the common name of the object (in this example, **world**), such table help identifying quickly the tools you need to manipulate this object type.

 **Name**             | **world**                 
----------------------|-----------------------
 **Info Type**        | `BjWorldInfo` 
 **Create Function**  | `bj_world_create()`  
 **Destroy Function** | `bj_world_destroy()` 

The **create** and **destroy** functions always have the same signatures relative to the object they manipulate:

```c
BjWorld bj_world_create(BjWorldInfo*, BjAllocationCallbacks*);
void bj_world_destroy(BjWorld);
```

# API Naming Conventions

The API uses a few naming conventions to help user quickly identify the usage and scope of each symbol:

 **Type**                 | **casing**            | **Example**
--------------------------|-----------------------|----------------
 **Files**                | `snake_case`          | `forward_list.h`
 **Functions**            | `snake_case`          | `bj_memcpy()`
 **Function-like Macros** | `snake_case`          | `BJ_MAKE_VERSION`
 **Parameters**           | `snake_case`          | `mem_size`
 **Structs**              | `PascalCase`          | `BjHashTableInfo`
 **Typedefs**             | `PascalCase`          | `BjArray`
 **Constant Macros**      | `CAPITAL_CASE`        | `BJ_MAKE_VERSION`

Additionally:

* All functions and functionèlike macros start with `bj_`
* All structures and typedefs start with `Bj`
  * Typedefs to function pointers end with `FunctionPtr`. For example: `BjHashFunctionPtr`.
* All constants macro start with `BJ_`.














