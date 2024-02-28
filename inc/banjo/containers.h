/// \file
/// Aggregate include for all container-like objects
#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \defgroup containers Containers
/// Data manipulation objects
/// 
/// Banjo API provides a set of containers used for memory storage and manipulation.
/// 
/// | Name            | Summary                         | 
/// |-----------------|---------------------------------|
/// | #BjArray        | C-Style contiguous array        |
/// | #BjList         | Forward-only linked list        |
/// | #BjHashTable    | Unordered associative container |
///
/// Containers purpose is to provide your code with organized memory.
/// Choosing the right container for the right purpose entirely depends upon your
/// expected interface and complexity.
///
/// \par Memory Allocators
///
/// All allocations and frees performed by a container use the
/// \ref BjAllocationCallbacks provided at creation.
/// If not provided, the container will default to fallback allocators.
///
/// \par Weak and Strong ownership
///
/// Some containers provide a choice between weak and strong ownership of the
/// memory blocks they manipulate.
///
/// When choosing strong ownership (default), any block of memory passed to the 
/// container is copied into its internal memory (using \ref bj_memcpy).
/// The data initially passed to the container from the caller is untouched and
/// not owned by the container.
/// When the data is removed from the container, the latter is responsible for
/// freeing the associated memory.
///
/// In weeak ownership, the container only stores a pointer to the provided data.
/// The container itself can still need to allocate structual information in its
/// internal memory, but the data itself it entirely under the responsibility of
/// the caller.
///
///
#include <banjo/array.h>
#include <banjo/list.h>
#include <banjo/hash_table.h>








