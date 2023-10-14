#include "Graphics.hpp"

const char* NoGfxException::type() const noexcept
{
    return "NoGfxException";
}

const char* DeviceRemovedException::type() const noexcept
{
    return "DeviceRemovedException";
}
