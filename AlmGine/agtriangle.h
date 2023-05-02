#pragma once

#include <QVector4D>
#include <QVector3D>
#include <QVector2D>
#include <QColor>

#include "agtexture.h"

class AGMaterial;

class AGTriangle
{
public:
    AGTriangle() = default;
    AGTriangle(const AGTriangle &other)
        : vertices(other.vertices),
          texCoords(other.texCoords),
          normals(other.normals),
          litColors(other.litColors),
          material(other.material)
    {
    }
    AGTriangle &operator=(const AGTriangle &other) noexcept
    {
        if (this != &other) {
            vertices = other.vertices;
            texCoords = other.texCoords;
            normals = other.normals;
            litColors = other.litColors;
            material = other.material;
        }
        return *this;
    }

    std::array<QVector4D, 3> vertices;
    std::array<QVector2D, 3> texCoords;
    std::array<QVector3D, 3> normals;
    std::array<QColor, 3> litColors;
    AGMaterial *material = nullptr;
};
