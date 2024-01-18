#include "Game/InputSystem.hpp"

#include "App/ChiliWindow.hpp"

AppMousePoint MousePointConverter::
convert(const Mouse::Point& pt) const {
    return AppMousePoint{
        .x = ( pt.x - static_cast<int>(client_.x) ) / static_cast<float>(client_.width)
            * (right() - left()) + left(),
        .y = ( pt.y - static_cast<int>(client_.y) ) / static_cast<float>(client_.height)
            * (bottom() - top()) + top()
    };
}

template class InputSystem<CHAR>;
template class InputSystem<WCHAR>;