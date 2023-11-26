#ifndef __Entity
#define __Entity

#include "Scene.hpp"

class IEntity {
public:
    virtual void loadAt(wrl::ComPtr<ID3D11Device> pDevice, Scene& scene) = 0;
};

template <class T>
class Entity : public IEntity {
public:
    Entity(const T& obj)
        : obj_(obj) {}

    Entity(T&& obj)
        : obj_(std::move(obj)) {}

    void loadAt(wrl::ComPtr<ID3D11Device> pDevice, Scene& scene) override {
        Loader<T>(obj_).loadAt(pDevice, scene);
    }

private:
    T obj_;
};

#endif  // __Entity