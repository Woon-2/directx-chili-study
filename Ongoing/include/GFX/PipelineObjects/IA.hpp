#ifndef __IA
#define __IA

struct GFXVertex {
    float x;
    float y;
    float z;
};

struct GFXNormal {
    float x;
    float y;
    float z;
};

struct GFXTexCoord {
    float u;
    float v;
};

struct GFXColor {
    float r;
    float g;
    float b;
    float a;
};

using GFXIndex = unsigned short;

#endif __IA