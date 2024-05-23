# Using Banjo Objects {#objects}

\brief How to initialize and dispose Banjo Object

All data types in Banjo API, such as #bj_array and #bj_htable, strictly follow the same usage pattern.

To create any object, the simplest is to use #bj_new:

```c
bj_array* arr = bj_new(array, default_t, int);
```

- The first parameter is the type of object to create, matches the pointer name without the `bj_` prefix (`array`: #bj_array).
- The second parameter is a name used to determine which initializer is used.
  Here, we use `default_t`.
- The following parameters (here `int`) depend on the object type and are forwarded to the initializer of the object.

Once the object is not needed anymore, you can call #bj_del to destroy the object:

```c
bj_del(array, arr);
```

This is by far the simplest form of manipulating objects in the API.

\dot "Objects lifecycle"
digraph workflow {
    rankdir=LR
    
    node [shape=box, fontname="Courier New"];
    
    start [shape="point"]
    valid [label="Valid"];
    freed [label="Freed",shape="point"];
    
    start -> valid [label="new"]   
    valid -> freed [label="del"]   
}
\enddot

To see more in details what happens behind **new** and **del**, read the following sections.

# Separating allocation and initialization

#bj_new and #bj_del are convenience function-like macro.
The code in the previous section is the same as:

\code{c}
bj_array* arr = bj_malloc(sizeof(bj_array))
bj_array_init_default_t(arr, int);
\endcode

Conversely, the following code is the same as using #bj_del:

\code{c}
bj_array_reset(arr);
bj_free(arr);
\endcode

# Allocating on the Stack

If you want to initialize an object directly from the stack, using only `init` and `reset` instead:

```c
bj_array array; 

bj_array_init_with_capacity(&array, int, 10);

// ... using the array ...

bj_array_reset(&array);
```


\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    
    start [shape="point"]
    end [shape="point"];
    uninitialized [label="Uninitialized", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    valid [label="Valid", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    nil [label="Nil", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    
    
    start -> uninitialized
    nil -> end
    uninitialized -> valid [label="init", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    valid -> nil [label="reset", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    nil -> nil [label="reset", style="dashed"]
    nil -> valid [label="init", style="dashed"]
}
\enddot


# Object States

All along their lifecycle, an object can be described as in one of these qualifiers:

\anchor uninitialized

## Uninitialized

If you create and object directly onto the stack, or allocated it yourself, the object's memory contains undefined content.

\code{c}
// Here, arr is uninitialized:
bj_array arr;
\endcode

\anchor nil

## Nil

When all bytes of a memory object are at _0_, it is _Nil_.
The following guarantees are ensured by the API:

- _Nil_ and zero-initialized are inter-changeable terms.
- When a function expects and object and is given a _Nil_ one, the function _will_ run without crash (but will do nothing).
- When an object is incorrectly created or initialized, it is set to _Nil_.
- It is possible to `bj_free()` a _Nil_ object without any memory leak (since it's all zeros).

## Valid

A valid object has meaningful data and can be used in functions with expected results.
When created properly, an object is always valid to use.

# The Complete Workflow

The full workflow of an object's type is shown here:

\dot
digraph workflow {
    rankdir=TB
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    {rank=same;start;valid;freed}
    
    
    start [shape="point", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    freed [shape="point", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    uninitialized [label="Uninitialized", color="black", fontcolor="black"]
    valid [label="Valid", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    nil [label="Nil", color="black", fontcolor="black"]
    
    uninitialized -> valid [label="init", color="black", fontcolor="black"]
    start -> uninitialized [label="alloc", color="black", fontcolor="black"]   
    start -> valid [label="new", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    valid -> nil [label="reset", color="black", fontcolor="black"]
    nil -> nil [label="reset", style="dashed"]
    nil -> valid [label="init", style="dashed"]
    nil -> freed [label="free", color="black", fontcolor="black"]
    uninitialized -> freed [label="free", style="dashed"]
    valid -> freed [label="del", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    nil -> freed [label="del", style="dashed"]
}
\enddot

## Allocation

First, the object's memory size is allocated using #bj_malloc.

\code{c}
bj_array* arr = bj_malloc(sizeof(bj_array));
\endcode

\dot
digraph workflow {
    rankdir=LR
    node [shape=box];
    
    start [shape="point", color="grey"]
    uninitialized [label="Uninitialized"]
    
    start -> uninitialized [label="alloc"]
}
\enddot

Since the memory block returned by #bj_malloc is \ref uninitialized, the only function you should call on it are `bj_*_init()` and #bj_free.

## Initialization

An *uninitialized* must be properly initialized before being used in any other function.

Each function provide their own set of initialization function.

For example, the following initializes an array for 10 int elements:
\code{c}
bj_array_init_with_len(arr, sizeof(int), 10);
\endcode

\dot
digraph workflow {
    rankdir=LR
    node [shape=box];
    
    start [shape="point", color="grey"]
    uninitialized [label="Uninitialized"]
    valid [label="Valid"]
    
    uninitialized -> valid [label="init"]
    start -> uninitialized [label="alloc", color="grey", fontcolor="grey"]   
}
\enddot

For every type `bj_type` [#bj_array]:

* The initialization functions all start with `bj_type_init_MOD(bj_type*, ...)`
* At least `bj_type_init_default()` is always available [#bj_array_init_default].
* When the initialization fails, the object is set to \nil.
* The function returns a pointer to the initialized memory.

\warning Unless explicitely otherwise by the type's documentation, calling an **init** function on an already initialized object **will** result in a memory leak.
\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    uninitialized [label="Uninitialized"]
    valid [label="Valid", color="red", fontcolor="red"]
    
    valid -> valid [label="init", color="red", fontcolor="red"]
    uninitialized -> valid [label="init"]
    start -> uninitialized [label="alloc"]   
}
\enddot

## Reset

Before an object itself is destroyed (using #bj_free), its underlying memory must be clean from any memory leak.
The operation is done by calling the **reset** function for the type.

\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    uninitialized [label="Uninitialized"]
    valid [label="Valid", color="black", fontcolor="black"]
    nil [label="Nil", color="black", fontcolor="black"]
    
    uninitialized -> valid [label="init"]
    start -> uninitialized [label="alloc"]   
    start -> valid [label="new"]
    valid -> nil [label="reset", color="black", fontcolor="black"]

    nil -> nil [label="reset", color="black", fontcolor="black"]
    nil -> valid [label="init", color="black", fontcolor="black"]
}
\enddot


For every type `bj_type` [`bj_array`]:

* The **reset** function is `bj_type* bj_type_reset(bj_type*)`
* `bj_type_reset` **will** return an *nil* object.
* `bj_type_reset` can be called on any *valid* and *nil* object, but not on _uninitialized_ objects.
* `bj_type_reset` frees the underlying memory and sets its entire content to _0_ using #bj_memset.

Moreover, any function expecting a `bj_type*` as a paremeter can receive an *nil* object with the least consequence possible.
The actual effect of sending an *nil* object depends on the function and is documented in the API reference.

\warning Calling the **reset** function on an uninitialized object will cause undefined behaviour.
\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    uninitialized [label="Uninitialized"]
    valid [label="Valid"]
    nil [label="Nil"]
    ub [label="?", shape="plaintext", color="red", fontcolor="red"]
    
    uninitialized -> valid [label="init"]
    start -> uninitialized [label="alloc"]   
    start -> valid [label="new"]
    valid -> nil [label="reset"]
    nil -> nil [label="reset"]
    nil -> valid [label="init"]
    uninitialized -> ub  [label="reset", color="red", fontcolor="red"]
}
\enddot


## Free

The **free** function releases the memory directly used by the object's structure.
This is done by calling the general-purpose #bj_free function on an *nil* or *uninitialized* object:

\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    freed [shape="point", color="black", fontcolor="black"]
    uninitialized [label="Uninitialized", color="black", fontcolor="black"]
    valid [label="Valid"]
    nil [label="Nil", color="black", fontcolor="black"]
    
    uninitialized -> valid [label="init"]
    start -> uninitialized [label="alloc"]   
    start -> valid [label="new"]
    valid -> nil [label="reset"]
    nil -> nil [label="reset"]
    nil -> valid [label="init"]

    nil -> freed [label="free", color="black", fontcolor="black"]
    uninitialized -> freed [label="free", color="black", fontcolor="black"]
}
\enddot

For every type `bj_type` [#bj_array]:

* The **free** function is #bj_free *(Same function for everything)*
* #bj_free **will not** leak any memory when called on an *nil* object.

\warning Unless explicited otherwise by the type's documentation, calling the #bj_free on a *valid* object **will** result in a memory leak.
This is because #bj_free only deallocates the structure itself.
The underlying data must be released using the type's **reset** function before.
\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    freed [shape="point", color="black", fontcolor="black"]
    uninitialized [label="Uninitialized"]
    valid [label="Valid", color="black", fontcolor="black"]
    nil [label="Nil"]
    
    uninitialized -> valid [label="init"]
    start -> uninitialized [label="alloc"]   
    start -> valid [label="new"]
    valid -> nil [label="reset"]
    nil -> nil [label="reset"]
    nil -> valid [label="init"]
    nil -> freed [label="free"]
    uninitialized -> freed [label="free"]
    valid -> freed [label="free", color="red", fontcolor="red"]
}
\enddot

