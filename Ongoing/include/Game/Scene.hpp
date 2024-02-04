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

    void addBindee(GFXResView res) {
        bindees_.push_back(res);
    }

    template <std::ranges::range R>
        requires GFXResRepresentable< std::ranges::range_value_t<R> >
    void addBindee(R&& range) {
        if constexpr ( std::ranges::sized_range<R> ) {
            bindees_.reserve( std::size(range) );
        }

        bindees_.insert( bindees_.end(), std::begin(range), std::end(range) );
    }

    // to be added: removeBindee

    auto& bindees() noexcept {
        return bindees_;
    }

    const auto& bindees() const noexcept {
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
    std::vector<GFXResView> bindees_;
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

    void addLayer() {
        layers_.emplace_back();
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

// Scene with lights
class LSceneAdapter {
public:
    LSceneAdapter() = default;
    LSceneAdapter(Scene& scene)
        : pRawScene_(&scene) {}

    static constexpr std::size_t lightLayerIdx() noexcept {
        return 0u;
    }

    static constexpr std::size_t meshLayerIdx() noexcept {
        return 1u;
    }

    void sync(Scene& scene) {
        pRawScene_ = &scene;
    }

    void addLight(GFXResView res) {
        lightLayer().addBindee(res);
    }

    void addDrawCmp(RCDrawCmp* drawCmp) {
        meshLayer().addDrawCmp(drawCmp);
    }

    template <class TRenderer>
    void sortFor(const TRenderer& renderer) {
        return pRawScene_->sortFor(renderer);
    }

    const CameraVision& vision() const noexcept {
        return pRawScene_->vision();
    }

    void setVision(CameraVision& vision) {
        pRawScene_->setVision(vision);
    }

    Layer& lightLayer() noexcept {
        return pRawScene_->layer( lightLayerIdx() );
    }

    const Layer& lightLayer() const noexcept {
        return pRawScene_->layer( lightLayerIdx() );
    }

    Layer& meshLayer() noexcept {
        return pRawScene_->layer( meshLayerIdx() );
    }

    const Layer& meshLayer() const noexcept {
        return pRawScene_->layer( meshLayerIdx() );
    }

private:
    Scene* pRawScene_;
};

#endif  // __Scene