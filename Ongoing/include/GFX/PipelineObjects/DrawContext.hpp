#ifndef __DrawContext
#define __DrawContext

#define ACTIVATE_DRAWCONTEXT_LOG

#include <d3d11.h>

#include "Game/GFXCMDLogger.hpp"

class GFXPipeline;

class IDrawContext {
public:
    friend class GFXPipeline;

#ifdef ACTIVATE_DRAWCONTEXT_LOG
protected:
    class LogComponent {
    public:
        LogComponent() noexcept
            : category_(), logSrc_(nullptr), bLogEnabled_(false) {}
        
        LogComponent( const void* parent,
            const GFXCMDSourceCategory& category
        ) noexcept
            : category_(category), logSrc_(parent), bLogEnabled_(false) {}

        LogComponent( const void* parent,
            GFXCMDSourceCategory&& category
        ) noexcept
            : category_( std::move(category) ),
            logSrc_(parent), bLogEnabled_(false) {}

        void enableLog() noexcept {
            bLogEnabled_ = true;
        }

        void disableLog() noexcept {
            bLogEnabled_ = false;
        }

        bool logEnabled() const noexcept {
            return bLogEnabled_;
        }

        void logCreate() const {
            logImpl(GFXCMDType::Create);
        }

        void logDraw() const {
            logImpl(GFXCMDType::Draw);
        }

    private:
        void logImpl(GFXCMDType cmdType) const {
            GFXCMDLOG.logCMD( GFXCMDDesc{
                .cmdType = cmdType,
                .sources = { GFXCMDSource{
                    .category = category_,
                    .pSource = logSrc_
                } }
            } );
        }

        GFXCMDSourceCategory category_;
        const void* logSrc_;
        bool bLogEnabled_;
    };
#endif  // ACTIVATE_DRAWCONTEXT_LOG

private:
    virtual void drawCall(GFXPipeline& pipeline) const = 0;
};

class DrawContextBasic : public IDrawContext {
public:
    DrawContextBasic() = default;
    DrawContextBasic(UINT nVertex, UINT startVertexLocation
    #ifdef ACTIVATE_DRAWCONTEXT_LOG
        , bool enableLogOnCreation = true
    #endif
    ) :
    #ifdef ACTIVATE_DRAWCONTEXT_LOG
        logComponent_( this, GFXCMDSourceCategory("DrawContextBasic") ),
    #endif
        numVertex_(nVertex), startVertexLocation_(startVertexLocation) {
    #ifdef ACTIVATE_DRAWCONTEXT_LOG
        if (enableLogOnCreation) {
            logComponent_.enableLog();
        }
        logComponent_.logCreate();
    #endif
    }

    void setNumVertex(UINT nVertex) noexcept {
        numVertex_ = nVertex;
    }

    UINT numVertex() const noexcept {
        return numVertex_;
    }

    void setStartVertexLocation(UINT vertexLocation) noexcept {
        startVertexLocation_ = vertexLocation;
    }

    UINT startVertexLocation() const noexcept {
        return startVertexLocation_;
    }

protected:
    void basicDrawCall(GFXPipeline& pipeline) const;

private:
    void drawCall(GFXPipeline& pipeline) const override;

#ifdef ACTIVATE_DRAWCONTEXT_LOG
    IDrawContext::LogComponent logComponent_;
#endif
    UINT numVertex_;
    UINT startVertexLocation_;
};

class DrawContextIndexed : public IDrawContext {
public:
    DrawContextIndexed() = default;
    DrawContextIndexed(UINT numIndex, UINT startIndexLocation,
        INT baseVertexLocation
    #ifdef ACTIVATE_DRAWCONTEXT_LOG
        , bool enableLogOnCreation = true
    #endif
    ) :
    #ifdef ACTIVATE_DRAWCONTEXT_LOG
        logComponent_( this, GFXCMDSourceCategory("DrawContextIndexed") ),
    #endif
        numIndex_(numIndex), startIndexLocation_(startIndexLocation),
        baseVertexLocation_(baseVertexLocation) {
    #ifdef ACTIVATE_DRAWCONTEXT_LOG
        if (enableLogOnCreation) {
            logComponent_.enableLog();
        }
        logComponent_.logCreate();
    #endif
    }

    void setNumIndex(UINT numIndex) noexcept {
        numIndex_ = numIndex;
    }

    UINT numIndex() const noexcept {
        return numIndex_;
    }

    void setStartIndexLocation(UINT startIndexLocation) noexcept {
        startIndexLocation_ = startIndexLocation;
    }

    UINT startIndexLocation() const noexcept {
        return startIndexLocation_;
    }

    void setbaseVertexLocation(UINT baseVertexLocation) noexcept {
        baseVertexLocation_ = baseVertexLocation;
    }

    INT baseVertexLocation() const noexcept {
        return baseVertexLocation_;
    }

protected:
    void indexedDrawCall(GFXPipeline& pipeline) const;

private:
    void drawCall(GFXPipeline& pipeline) const override;

#ifdef ACTIVATE_DRAWCONTEXT_LOG
    IDrawContext::LogComponent logComponent_;
#endif
    UINT numIndex_;
    UINT startIndexLocation_;
    INT baseVertexLocation_;
};

#endif  // __DrawContext