#ifndef __Bindable
#define __Bindable

using RID = unsigned;

class GFXPipeline;

class IBindable {
public:
    friend class GFXPipeline;

private:
    virtual void bind(GFXPipeline& pipeline) = 0;
};

#endif  // __Bindable