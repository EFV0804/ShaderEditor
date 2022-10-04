//
// Created by elise on 26/09/2022.
//

#ifndef SHADEREDITOR_SCENE_H
#define SHADEREDITOR_SCENE_H
#include <vector>
#include "Renderable.h"
#include "Renderer.h"


class Scene {
public:
    Scene();
    ~Scene();
    std::vector<Renderable> renderables;
    void load(Renderer* renderer);
    void draw(Renderer* renderer);
    void close();
};


#endif //SHADEREDITOR_SCENE_H
