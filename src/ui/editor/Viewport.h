//
// Created by elise.vidal on 1/6/2023.
//

#ifndef SHADEREDITOR_VIEWPORT_H
#define SHADEREDITOR_VIEWPORT_H


class Viewport {
public:
    Viewport() = default;

    ~Viewport() = default;

    Viewport &operator=(const Viewport &) = delete;

    Viewport(const Viewport &) = delete;

    void init();


private:
    int height;
    int width;
};


#endif //SHADEREDITOR_VIEWPORT_H
