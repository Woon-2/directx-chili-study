#ifndef __InputComponent
#define __InputComponent

#include "Keyboard.hpp"
#include "Mouse.hpp"

template <class CharT>
class IKeyboardInputComponent {
public:
    using MyChar = CharT;
    virtual ~IKeyboardInputComponent() {}
    virtual void receive(const typename Keyboard<MyChar>::Event& ev) = 0;
};

class IMouseInputComponent {
public:
    virtual ~IMouseInputComponent() {}
    virtual void receive(const Mouse::Event& ev) = 0;
};

// InputComponents are responsible for
// handling inputs from input system.
// Like DrawComponent<T> class template,
// it stands for having constraints of naming.
// So prevent instantiation with general type T.
// Loader has to be specialized.

template <class T, class CharT>
class KeyboardInputComponent;

template <class T>
class MouseInputComponent;

#endif  // __InputComponent