//
// Created by elise.vidal on 10/3/2022.
//

#include "Allocator.h"
#include "vk_mem_alloc.hpp"


Allocator::Allocator(vk::PhysicalDevice pPhysical, vk::Device pDevice, vk::Instance pInstance):
physicalDevice{pPhysical},
device{pDevice},
instance{pInstance}{

}

void Allocator::allocate() {

}

void Allocator::map() {

}

void Allocator::unmap() {

}

void Allocator::init() {

}
