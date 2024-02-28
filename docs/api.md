# API Design {#api}

\brief How to work with Banjo API

## Using Banjo Objects

In Banjo, *object* is the name given to type instances that serve specific purpose, such as #BjArray, #BjHashTable or #BjWorld.

To create an object, the simplest for is to call the `bj_new` function:

```c
BjArray* array = bj_new(BjArray);
```

Deleting an object from memory is done by calling `bj_delete`:

```c
bj_delete(BjArray, array);
```

`bj_new` and `bj_delete` are function-like macros that abstract away more fine-grained operations on the object and its associated memory.

For your own uses, you may need to go with the actual functions instead of the convenience macros.



