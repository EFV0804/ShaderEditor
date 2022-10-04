//
// Created by elise.vidal on 10/3/2022.
//

#ifndef SHADEREDITOR_ALLOCATOR_H
#define SHADEREDITOR_ALLOCATOR_H
#include <memory>
namespace vma{
    class Allocator;
}

class Allocator {
public:
    Allocator(vk::PhysicalDevice pPhysical, vk::Device pDevice, vk::Instance pInstance);
    ~Allocator() = default;

    void init();
    void allocate();
    void map();
    void unmap();
    vma::Allocator& getAllocator(){return *_allocator};

private:
    std::unique_ptr<vma::Allocator> _allocator{};
    vk::PhysicalDevice physicalDevice{};
    vk::Device device{};
    vk::Instance instance{};
};


#endif //SHADEREDITOR_ALLOCATOR_H
