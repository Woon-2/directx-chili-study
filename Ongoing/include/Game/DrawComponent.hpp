#ifndef __DrawComponent
#define __DrawComponent

#include "RenderObjectDesc.hpp"

#include "GFX/PipelineObjects/DrawContext.hpp"

class Renderer;

class IDrawComponent {
public:
    virtual ~IDrawComponent() {}

    virtual const RenderObjectDesc renderObjectDesc() const = 0;
    virtual IDrawContext* drawContext() = 0;
    virtual const IDrawContext* drawContext() const = 0;
    virtual void sync(const Renderer& pRenderer) = 0;

    friend auto operator<=>(const IDrawComponent& lhs,
        const IDrawComponent& rhs) {
        return lhs.renderObjectDesc() <=> rhs.renderObjectDesc();
    }
};

// Prevent instantiation with general type T.
// DrawComponent has to be specialized.
// There's no common implementation between other type of DrawComponents,
// This class template stands for having constraint of naming.
// e.g. to enforce the class which is going to support IDrawComponent interface
// to have a predictable name DrawComponent<Box>,
// rather than unpredictable name BoxDrawComponent.
// Note: The specialization of this class template must inherit IDrawComponent.
template <class T>
class DrawComponent;

#endif  // __DrawComponent