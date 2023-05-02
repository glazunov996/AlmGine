#pragma once

#include <QSharedPointer>
#include <QIODevice>
#include <QList>
#include <QVector3D>
#include <QHash>

#include "agtriangle.h"

class AGMesh
{
    Q_GADGET

public:
    enum class Attribute {
        InvertX = 0x00000001,
        InvertY = 0x00000002,
        InvertZ = 0x00000004,
        SwapYZ = 0x00000008,
        SwapXZ = 0x00000010,
        SwapXY = 0x00000020,
        InvertWindingOrder = 0x00000040,
        InvertU = 0x00000080,
        InvertV = 0x00000100,
        SwapUV = 0x00000200
    };
    Q_DECLARE_FLAGS(Attributes, Attribute)
    Q_FLAG(Attributes)

    struct MeshData {
        QList<QVector4D> vertices;
        QList<QVector3D> vertexIndices;
        QList<QVector2D> texCoords;
        QList<QVector3D> texCoordIndices;
        QList<QVector3D> normals;
        QList<int> materials;
        int numFrames = 0;
        int numVertices = 0;
        bool isValid() const { return !vertexIndices.isEmpty(); }
    };

    static QSharedPointer<AGMesh> loadMesh(const QString &inMeshPath, Attributes attributes);
    bool isValid() const { return meshData.isValid(); }

private:
    static MeshData loadMD2File(const QString &inMeshPath, Attributes attributes);
    static MeshData loadPLGFile(const QString &inMeshPath, Attributes attributes);
    static MeshData loadCOBFile(const QString &inMeshPath, Attributes attributes);
    static MeshData loadPrimitive(const QString &primitive, Attributes attributes);
    static MeshData loadTerrain(const QString &inMeshPath, Attributes attributes);

    MeshData meshData;
    static QHash<QString, QSharedPointer<AGMesh>> meshMap;

    friend class AGModel;
};

