#pragma once

#include "../Object.h"
#include "../MyGeometry.h"
#include "../material/MyMaterial.h"
namespace MyOpenGL {
    class MyMesh : public Object {
    public:
        MyMesh(MyGeometry* geometry, MyMaterial* material);
        ~MyMesh();

    public:
        MyGeometry* mGeometry{ nullptr };
        MyMaterial* mMaterial{ nullptr };
    };
}