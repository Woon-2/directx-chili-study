#ifndef __Entity
#define __Entity

#include "GFXFactory.hpp"
#include "Scene.hpp"

class IEntity {
public:
    virtual void loadAt(GFXFactory factory, Scene& scene) = 0;
};

template <class T>
class Entity : public IEntity {
public:
    Entity(const T& obj)
        : obj_(obj) {}

    Entity(T&& obj)
        : obj_(std::move(obj)) {}

    void loadAt(GFXFactory factory, Scene& scene) override {
        Loader<T>(obj_).loadAt(factory, scene);
    }

private:
    T obj_;
};

#endif  // __Entity