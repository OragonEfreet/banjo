# Using Banjo Objects {#objects}

\brief How to initialize and dispose Banjo Object

All data types in Banjo API, such as #BjArray and #BjHashTable, strictly follow the same usage pattern.

For example, to create a new array, the simplest is to call #bj_array_new:

```c
BjArrayInfo array_info = { // Array of `int` elements
    .bytes_payload = sizeof(int)
};

BjArray* array = bj_array_new(array_info, 0);
```

- The first parameter is an instance of #BjArrayInfo and is used to configure the array.
- The second parameter is an optional set of allocators, #BjAllocationCallbacks.

Once the object is not needed anymore, you can call #bj_array_del to destroy the object:

```c
bj_array_del(array);
```

This is by far the simplest form of manipulating objects in the API.

\dot "Objects lifecycle"
digraph workflow {
    rankdir=LR
    
    node [shape=box, fontname="Courier New"];
    
    start [shape="point"]
    ready [label="Ready"];
    freed [label="Freed",shape="point"];
    
    start -> ready [label="new"]   
    ready -> freed [label="del"]   
}
\enddot

To see more in details what happens behind **new** and **del**, read the following sections.

# The Complete Workflow

The full workflow of an object's type is shown here:

\dot
digraph workflow {
    rankdir=TB
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    {rank=same;start;ready;freed}
    
    
    start [shape="point", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    freed [shape="point", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    uninitialized [label="Uninitialized", color="black", fontcolor="black"]
    ready [label="Ready", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    empty [label="Empty", color="black", fontcolor="black"]
    
    uninitialized -> ready [label="init", color="black", fontcolor="black"]
    start -> uninitialized [label="alloc", color="black", fontcolor="black"]   
    start -> ready [label="new", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    ready -> empty [label="reset", color="black", fontcolor="black"]
    empty -> empty [label="reset", style="dashed"]
    empty -> ready [label="init", style="dashed"]
    empty -> freed [label="free", color="black", fontcolor="black"]
    uninitialized -> freed [label="free", style="dashed"]
    ready -> freed [label="del", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    empty -> freed [label="del", style="dashed"]
}
\enddot

## Allocation

First, the object's memory size is allocated using #bj_malloc.
Since the size of a type isn't known by default from a user code, each type provide its own allocation function, for example: #bj_array_alloc.

\dot
digraph workflow {
    rankdir=LR
    node [shape=box];
    
    start [shape="point", color="grey"]
    uninitialized [label="Uninitialized"]
    
    start -> uninitialized [label="alloc"]
}
\enddot

For every type `BjType/bj_type` [`BjArray/bj_array`]:

* The **alloc** function is `BjType* bj_type_alloc(const BjAllocationCallbacks*)` [#bj_array_alloc].
* `bj_type_alloc` **will** return an *uninitialized* `Type` [#BjArray].

\warning Passing any *uninitialized* object as a parameter to a function is undefined behaviour.
The only exception are the type's matching `*_init` and `*_del` functions and #bj_free.

## Initialization

An *uninitialized* must be properly initialized before being used in any other function:

\dot
digraph workflow {
    rankdir=LR
    node [shape=box];
    
    start [shape="point", color="grey"]
    uninitialized [label="Uninitialized"]
    ready [label="Ready"]
    
    uninitialized -> ready [label="init"]
    start -> uninitialized [label="alloc", color="grey", fontcolor="grey"]   
}
\enddot

For every type `BjType/bj_type` [`BjArray/bj_array`]:

* The **init** function is `void bj_type_init(BjType*, const BjTypeInfo*, const BjAllocationCallbacks*)` [#bj_array_init].
  * The `BjTypeInfo` [#BjArrayInfo] structure is used to decide how the object is initialized and depends on the object type.
  * When the paramter is _0_ or invalid, the **init** function sets the object to an empty state.
* The returned `Type` [#BjArray] is *ready*.
* `bj_type_init` **will** return a *ready* `Type` [#BjArray].

\warning Unless explicitely otherwise by the type's documentation, calling the **init** function on an already initialized object **will** result in a memory leak.
\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    uninitialized [label="Uninitialized"]
    ready [label="Ready", color="red", fontcolor="red"]
    
    ready -> ready [label="init", color="red", fontcolor="red"]
    uninitialized -> ready [label="init"]
    start -> uninitialized [label="alloc"]   
}
\enddot

## New

The **new** function is an aggregation of both Allocation and Initialization.
This is just a convenience function that is used to perform both operation in one go.

\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point", color="black", fontcolor="black"]
    uninitialized [label="Uninitialized"]
    ready [label="Ready", color="black", fontcolor="black"]
    
    uninitialized -> ready [label="init"]
    start -> uninitialized [label="alloc"]   

    start -> ready [label="new", color="black", fontcolor="black"]
}
\enddot

For every type `BjType/bj_type` [`BjArray/bj_array`]:

* The **new** function is `BjType* bj_type_new(const BjTypeInfo*, const BjAllocationCallbacks*)` [#bj_array_new].
* `bj_type_new` **will** return a *ready* `Type` [#BjArray].

For example, the two code snippets are equivalent:

```c
BjArray* array = bj_array_new(array_info, 0);
```

```c
BjArray* array = bj_array_alloc(p_allocator);
bj_array_init(array, p_allocator, p_info);
```

Most Banjo function expecting a `Type` parameter expect it to be *ready*.

## Reset

Before an object itself is destroyed (using #bj_free), its underlying memory must be released.
The operation is done by calling the **reset** function for the type.

The function can be called on a *ready* or *empty* object:

\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    uninitialized [label="Uninitialized"]
    ready [label="Ready", color="black", fontcolor="black"]
    empty [label="Empty", color="black", fontcolor="balck"]
    
    uninitialized -> ready [label="init"]
    start -> uninitialized [label="alloc"]   
    start -> ready [label="new"]
    ready -> empty [label="reset", color="black", fontcolor="black"]

    empty -> empty [label="reset", color="black", fontcolor="black"]
    empty -> ready [label="init", color="black", fontcolor="black"]
}
\enddot


For every type `BjType/bj_type` [`BjArray/bj_array`]:

* The **reset** function is `void bj_type_reset(BjType*)`
* `bj_type_reset` **will** return an *empty* `Type` [#BjArray].
* `bj_type_reset` can be called on **any object**, including an *empty* object.
* `bj_type_reset` frees the underlying memory and sets its entire content to using #bj_memset.
* The function uses the object's #BjAllocationCallbacks set in `bj_type_init` [#bj_array_init] to release the memory.

Moreover, any function expecting a `BjType` as a paremeter can receive an *empty* object with the least consequence possible.
The actual effect of sending an *empty* object depends on the function and is documented in the API reference.

\warning Calling the **reset** function on an uninitialized object will cause undefined behaviour.
\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    uninitialized [label="Uninitialized"]
    ready [label="Ready"]
    empty [label="Empty"]
    ub [label="?", shape="plaintext", color="red", fontcolor="red"]
    
    uninitialized -> ready [label="init"]
    start -> uninitialized [label="alloc"]   
    start -> ready [label="new"]
    ready -> empty [label="reset"]
    empty -> empty [label="reset"]
    empty -> ready [label="init"]
    uninitialized -> ub  [label="reset", color="red", fontcolor="red"]
}
\enddot


## Free

The **fre** function releases the memory directly used by the object's structure.
This is done by calling the general-purpose #bj_free function on an *empty* or *uninitialized* object:

\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    freed [shape="point", color="black", fontcolor="black"]
    uninitialized [label="Uninitialized", color="black", fontcolor="black"]
    ready [label="Ready"]
    empty [label="Empty", color="black", fontcolor="black"]
    
    uninitialized -> ready [label="init"]
    start -> uninitialized [label="alloc"]   
    start -> ready [label="new"]
    ready -> empty [label="reset"]
    empty -> empty [label="reset"]
    empty -> ready [label="init"]

    empty -> freed [label="free", color="black", fontcolor="black"]
    uninitialized -> freed [label="free", color="black", fontcolor="black"]
}
\enddot

For every type `BjType/bj_type` [`BjArray/bj_array`]:

* The **free** function is #bj_free *(Same function for everything)*
* The function uses the object's #BjAllocationCallbacks set in `bj_type_init` [#bj_array_init] to release the memory.
* #bj_free **will not** leak any memory when called on an *empty* object.

\warning Unless explicited otherwise by the type's documentation, calling the #bj_free on a *ready* object **will** result in a memory leak.
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
    ready [label="Ready", color="black", fontcolor="black"]
    empty [label="Empty"]
    
    uninitialized -> ready [label="init"]
    start -> uninitialized [label="alloc"]   
    start -> ready [label="new"]
    ready -> empty [label="reset"]
    empty -> empty [label="reset"]
    empty -> ready [label="init"]
    empty -> freed [label="free"]
    uninitialized -> freed [label="free"]
    ready -> freed [label="free", color="red", fontcolor="red"]
}
\enddot

## Deleting

The **del** function is a convenience function that performs both a **reset** and a **free** on an object.
It can be called from an *ready* or *empty* object:

\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    start [shape="point"]
    freed [shape="point", color="black", fontcolor="black"]
    uninitialized [label="Uninitialized"]
    ready [label="Ready", color="black", fontcolor="black"]
    empty [label="Empty", color="black", fontcolor="black"]
    
    uninitialized -> ready [label="init"]
    start -> uninitialized [label="alloc"]   
    start -> ready [label="new"]
    ready -> empty [label="reset"]
    empty -> empty [label="reset"]
    empty -> ready [label="init"]
    empty -> freed [label="free"]
    uninitialized -> freed [label="free"]
    
    ready -> freed [label="del", color="black", fontcolor="black"]
    empty -> freed [label="del", color="black", fontcolor="black"]
}
\enddot

For every type `BjType/bj_type` [`BjArray/bj_array`]:

* The **del** function is: `void bj_type_del(Type*)` [#bj_array_del].
* `bj_type_del` is a `bj_type_reset` [#bj_array_reset] followed by a #bj_free.

For example, the two code snippets are equivalent:

```c
bj_array_del(array);
```

```c
bj_array_reset(array);
bj_free(array);
```

# Stack-Allocated Objects

By default, Banjo API provides dynamically allocated objects.
If you defined the `BJ_NO_OPAQUE` macro definition, you have access to the type definition from your code and can thus directly use objects from the stack.

In this case, only **init** and **reset** are needed:

```c
BjArrayInfo array_info = { // Array of `int` elements
    .bytes_payload = sizeof(int)
};
BjArray array; 

bj_array_init(&array, array_info, 0);

// ... using the array ...

bj_array_reset(&array);
```

The comes with a simplified workflow:

\dot
digraph workflow {
    rankdir=LR
    node [shape=box, color="grey", fontcolor="grey"];
    edge [fontname="Courier New", color="grey", fontcolor="grey"];
    
    
    start [shape="point"]
    end [shape="point"];
    uninitialized [label="Uninitialized", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    ready [label="Ready", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    empty [label="Empty", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    
    
    start -> uninitialized
    empty -> end
    uninitialized -> ready [label="init", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    ready -> empty [label="reset", color="forestgreen", fontcolor="forestgreen", penwidth=2]
    empty -> empty [label="reset", style="dashed"]
    empty -> ready [label="init", style="dashed"]
}
\enddot
