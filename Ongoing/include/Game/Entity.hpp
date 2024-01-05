#ifndef __Entity
#define __Entity

#include "Chrono.hpp"

class IEntity {
public:
    virtual ~IEntity() = 0 {}
    virtual void update(milliseconds elapsed) = 0;
};

// Entity is the game object represented by code.
// Like DrawComponent<T> class template,
// it stands for having constraints of naming.
// So prevent instantiation with general type T.
// Loader has to be specialized.
template <class T>
class Entity;

// Loader is responsible for adding entities at an arbitrary system.
// Like DrawComponent<T> class template,
// it stands for having constraints of naming.
// So prevent instantiation with general type T.
// Loader has to be specialized.
template <class T>
class Loader;

#endif  // __Entity