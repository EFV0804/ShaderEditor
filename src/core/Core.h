//
// Created by elise.vidal.
//

#pragma once


class Core {
public:
    Core() = delete;

    ~Core() = delete;

    Core(const Core &) = delete;

    Core &operator=(const Core &) = delete;

    static void init();

    static void close();

};
