/// \file
/// Aggregate include for all container-like objects
#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \defgroup algo Algorithms
/// Data manipulation objects
/// 
/// Banjo API provides a set of containers and algorithm used for memory 
/// storage and manipulation.
/// 
/// | Name            | Summary                         | 
/// |-----------------|---------------------------------|
/// | #bj_array       | C-Style contiguous array        |
/// | #bj_list        | Forward-only linked list        |
/// | #bj_htable      | Unordered associative container |
///
/// Algorithms purpose is to provide your code with organized memory.
/// Choosing the right container for the right purpose entirely depends upon your
/// expected interface and complexity.
///
#include <banjo/array.h>
#include <banjo/list.h>
#include <banjo/htable.h>







