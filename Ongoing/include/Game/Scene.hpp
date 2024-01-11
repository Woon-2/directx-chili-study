#ifndef __Scene
#define __Scene

#include "RCDrawComponent.hpp"
#include "Camera.hpp"

#include "GFX/Core/GraphicsStorage.hpp"

#include <ranges>
#include <algorithm>
#include <vector>
#include <memory>
#include <concepts>

#include "AdditionalRanges.hpp"

#include <cassert>

class Layer {
public:
    void setup() {

    }

    void addBindee(const GFXMappedResource& res) {
        bindees_.push_back(res);
    }

    void addBindee(GFXMappedResource&& res) {
        bindees_.push_back(std::move(res));
    }

    template <std::ranges::range R>
        requires ( std::same_as< std::ranges::range_value_t<R>,
            GFXMappedResource
        > || std::same_as< std::ranges::range_value_t<R>,
            const GFXMappedResource
        > )
    void addBindee(R&& range) {
        if constexpr ( std::ranges::sized_range<R> ) {
            bindees_.reserve( std::size(range) );
        }

        bindees_.insert( bindees_.end(), std::begin(range), std::end(range) );
    }

    // to be added: removeBindee

    auto bindees() noexcept {
        return bindees_ | std::views::transform(
            [](auto& mappedRes) { return mappedRes.get(); }
        ) | dereference();
    }

    const auto bindees() const noexcept {
        return const_cast<Layer*>(this)->bindees();
    }

    void addDrawCmp(RCDrawCmp* drawCmp) {
        drawCmps_.push_back(drawCmp);
    }

    template <std::ranges::range R>
        requires ( std::same_as< std::ranges::range_value_t<R>, RCDrawCmp* >
            || std::same_as< std::ranges::range_value_t<R>, const RCDrawCmp* >
        )
    void addDrawCmp(R&& range) {
        if constexpr ( std::ranges::sized_range<R> ) {
            drawCmps_.reserve( std::size(range) );
        }

        drawCmps_.insert( drawCmps_.end(), std::begin(range), std::end(range) );
    }

    // to be added: removeDrawCmp

    auto drawCmps() noexcept {
        return drawCmps_ | dereference();
    }

    const auto drawCmps() const noexcept {
        return drawCmps_ | dereference();
    }

    template <class TRenderer>
    void sortFor(const TRenderer& renderer) {
        std::ranges::for_each( drawCmps(), [&renderer](auto& dc) {
            dc.sync(renderer);
        } );
        std::ranges::sort( drawCmps_, std::less<>{},
            &IDrawComponent::renderObjectDesc
        );
    }

private:
    std::vector<RCDrawCmp*> drawCmps_;
    std::vector<GFXMappedResource> bindees_;
};

class Scene {
public:
    Scene()
        : layers_(1u), pVision_(nullptr) {}

    Layer& layer(std::size_t idx) noexcept {
        assert(idx < layers_.size());
        return layers_[idx];
    }

    const Layer& layer(std::size_t idx) const noexcept {
        assert(idx < layers_.size());
        return layers_[idx];
    }

    auto& layers() noexcept {
        return layers_;
    }

    const auto& layers() const noexcept {
        return layers_;
    }

    template <class TRenderer>
    void sortFor(const TRenderer& renderer) {
        std::ranges::for_each( layers_, [&renderer](auto& l) {
            l.sortFor(renderer);
        } );
    }

    const CameraVision& vision() const noexcept {
        return *pVision_;
    }

    void setVision(CameraVision& vision) {
        pVision_ = &vision;
    }

private:
    std::vector<Layer> layers_;
    CameraVision* pVision_;
};

#endif  // __Scene