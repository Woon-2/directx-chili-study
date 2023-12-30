#ifndef __DrawCaller
#define __DrawCaller

#define ACTIVATE_DRAWCALLER_LOG

#include "DrawContext.hpp"

#include <d3d11.h>

#include "Game/GFXCMDLogger.hpp"

#include <vector>
#include <memory>
#include <ranges>
#include <algorithm>

#include "AdditionalRanges.hpp"

class GFXPipeline;

class BasicDrawCaller {
public:
    friend class GFXPipeline;

    // begin of draw context related stuffs.
    // draw context modifiers should be added later.
    // this is minimum implementation.
    void addDrawContext(std::shared_ptr<IDrawContext> pDrawContext) {
        drawContexts_.push_back(pDrawContext);
    }

    void clearDrawContexts() {
        drawContexts_.clear();
    }

    void beforeDrawCall(GFXPipeline& pipeline) const {
        std::ranges::for_each( drawContexts_ | dereference(),
            [&pipeline](IDrawContext& drawContext) {
                drawContext.beforeDrawCall(pipeline);
            }
        );
    }

    void afterDrawCall(GFXPipeline& pipeline) const {
        std::ranges::for_each( drawContexts_ | dereference(),
            [&pipeline](IDrawContext& drawContext) {
                drawContext.afterDrawCall(pipeline);
            }
        );
    }
    // end of draw context related stuffs.

#ifdef ACTIVATE_DRAWCALLER_LOG
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
#endif  // ACTIVATE_DRAWCALLER_LOG

private:
    virtual void drawCall(GFXPipeline& pipeline) const = 0;

    std::vector< std::shared_ptr<IDrawContext> > drawContexts_;
};

class DrawCaller : public BasicDrawCaller {
public:
    DrawCaller() = default;
    DrawCaller(UINT nVertex, UINT startVertexLocation
    #ifdef ACTIVATE_DRAWCALLER_LOG
        , bool enableLogOnCreation = true
    #endif
    ) :
    #ifdef ACTIVATE_DRAWCALLER_LOG
        logComponent_( this, GFXCMDSourceCategory("DrawCaller") ),
    #endif
        numVertex_(nVertex), startVertexLocation_(startVertexLocation) {
    #ifdef ACTIVATE_DRAWCALLER_LOG
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

#ifdef ACTIVATE_DRAWCALLER_LOG
    BasicDrawCaller::LogComponent logComponent_;
#endif
    UINT numVertex_;
    UINT startVertexLocation_;
};

class DrawCallerIndexed : public BasicDrawCaller {
public:
    DrawCallerIndexed() = default;
    DrawCallerIndexed(UINT numIndex, UINT startIndexLocation,
        INT baseVertexLocation
    #ifdef ACTIVATE_DRAWCALLER_LOG
        , bool enableLogOnCreation = true
    #endif
    ) :
    #ifdef ACTIVATE_DRAWCALLER_LOG
        logComponent_( this, GFXCMDSourceCategory("DrawCallerIndexed") ),
    #endif
        numIndex_(numIndex), startIndexLocation_(startIndexLocation),
        baseVertexLocation_(baseVertexLocation) {
    #ifdef ACTIVATE_DRAWCALLER_LOG
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

#ifdef ACTIVATE_DRAWCALLER_LOG
    BasicDrawCaller::LogComponent logComponent_;
#endif
    UINT numIndex_;
    UINT startIndexLocation_;
    INT baseVertexLocation_;
};

#endif  // __DrawCaller