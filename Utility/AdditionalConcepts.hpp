#ifndef __AdditionalConcepts
#define __AdditionalConcepts

#include <type_traits>

template <class T, class ... Args>
concept contains = (std::is_same_v<T, Args> || ...);

template <class T>
concept pointable = std::is_pointer_v<std::remove_cvref_t<T>> ||
    requires (T & t) {
    *t;
    t.operator->();
};

#endif  // __AdditionalConcepts