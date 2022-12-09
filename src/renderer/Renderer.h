//
// Created by elise.vidal.
//

#pragma once
#include "VKRenderer.h"


class Renderer {
public:
    Renderer() = default;

    ~Renderer() = default;

    Renderer(const Renderer &) = delete;

    Renderer &operator=(const Renderer &) = delete;

    static Renderer &Get() {
        static Renderer instance;
        return instance;
    }

    void init();
    void loadMeshes(std::vector<Renderable>* renderables);
    void draw(std::vector<Renderable>* renderables);
    void update();
    void updateCamera(CameraBuffer camBuffer);
    void waitIdle();
    void close();

    int getCurrentFrame(){return VKRenderer::Get().currentFrame;} //TODO:make it adaptable
};
