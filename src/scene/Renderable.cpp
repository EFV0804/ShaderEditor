//
// Created by elise on 26/09/2022.
//

#include "Renderable.h"

Renderable::Renderable(const Mesh& pMesh, const Material& pMaterial, glm::mat4 pTransform):mesh{pMesh}, material{pMaterial}, transform{pTransform} {

}

void Renderable::load() {

    // load mesh
    // load material
    // apply transforms
}
