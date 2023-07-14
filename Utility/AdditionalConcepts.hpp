#ifndef __AdditionalConcepts
#define __AdditionalConcepts

#include <type_traits>

template <class T, class ... Args>
concept contains = (std::is_same_v<T, Args> || ...);

#endif  // __AdditionalConcepts