#include <QFileInfo>
#include <QTextStream>
#include <QDataStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QImage>
#include <QDebug>

#include "agmesh.h"
#include "aghelper.h"

QHash<QString, QSharedPointer<AGMesh>> AGMesh::meshMap;

QSharedPointer<AGMesh> AGMesh::loadMesh(const QString &inMeshPath, Attributes attributes)
{
    if (inMeshPath.isNull())
        return nullptr;

    const auto meshItr = meshMap.constFind(inMeshPath);
    if (meshItr != meshMap.cend())
        return meshItr.value();

    MeshData result;
    if (inMeshPath.startsWith("#"))
        result = loadPrimitive(inMeshPath, attributes);
    const QFileInfo fi(inMeshPath);
    if (!result.isValid() && fi.exists()) {
        result = loadPLGFile(inMeshPath, attributes);
        if (!result.isValid())
            result = loadCOBFile(inMeshPath, attributes);
        if (!result.isValid())
            result = loadMD2File(inMeshPath, attributes);
        if (!result.isValid())
            result = loadTerrain(inMeshPath, attributes);
    }

    qDebug() << "Loaded mesh:";
    qDebug() << "   Vertices:" << result.numVertices;
    qDebug() << "   Triangles:" << result.vertexIndices.length();
    qDebug() << "   TexCoords:" << result.texCoords.length();
    qDebug() << "   Materials:" << result.materials.length();
    qDebug() << "   Frames:" << result.numFrames;

    if (!result.isValid())
        return nullptr;

    result.normals.resize(result.vertices.length());

    for (int i = 0; i < result.vertexIndices.size(); ++i) {
        QVector4D vertices[3] = {
            result.vertices[result.vertexIndices[i][0]],
            result.vertices[result.vertexIndices[i][1]],
            result.vertices[result.vertexIndices[i][2]],
        };
        const auto a = vertices[0] - vertices[1];
        const auto b = vertices[0] - vertices[2];
        const auto normal = QVector3D::crossProduct(QVector3D(a), QVector3D(b));
        for (int j = 0; j < 3; ++j)
            result.normals[result.vertexIndices[i][j]] += normal;
    }

    QSharedPointer<AGMesh> mesh(new AGMesh());
    mesh->meshData = result;
    meshMap.insert(inMeshPath, mesh);
    return mesh;
}

static constexpr int MD2_MAGIC_NUM = (('I') + ('D' << 8) + ('P' << 16) + ('2' << 24));
static constexpr int MD2_VERSION = 8;

struct MD2Header
{
    quint32 magic;
    quint32 version;
    quint32 skinWidth;
    quint32 skinHeight;
    quint32 frameSize;
    quint32 numSkins;
    quint32 numVertices;
    quint32 numTexCoords;
    quint32 numTriangles;
    quint32 numGlCommands;
    quint32 numFrames;
    quint32 offsetSkins;
    quint32 offsetTexCoords;
    quint32 offsetTriangles;
    quint32 offsetFrames;
    quint32 offsetGlCommands;
    quint32 offsetEnd;
};

struct MD2Triangle
{
    quint16 vertexIndices[3];
    quint16 textureIndices[3];
};

struct MD2Vertex
{
    quint8 vertex[3];
    quint8 lightNormalIndex;
};

struct MD2TexCoord
{
    quint16 s;
    quint16 t;
};

struct MD2Frame
{
    float scale[3];
    float translate[3];
    char name[16];
    MD2Vertex vertices[1];
};

AGMesh::MeshData AGMesh::loadMD2File(const QString &inMeshPath, Attributes attributes)
{
    MeshData result;
    auto device = AGHelper::getStreamForFile(inMeshPath);
    auto buffer = device->readAll();
    auto *header = reinterpret_cast<MD2Header *>(buffer.data());
    if (header->magic != MD2_MAGIC_NUM || header->version != MD2_VERSION)
        return result;

    auto *tex = reinterpret_cast<MD2TexCoord *>(buffer.data() + header->offsetTexCoords);
    for (quint32 i = 0; i < header->numTexCoords; ++i) {
        QVector2D texCoords(
                qreal(tex[i].s) / qreal(header->skinWidth),
                qreal(tex[i].t) / qreal(header->skinHeight));
        if (attributes & Attribute::InvertU)
            texCoords.setX(1 - texCoords.x());
        if (attributes & Attribute::InvertV)
            texCoords.setY(1 - texCoords.y());
        result.texCoords.push_back(texCoords);
    }

    for (quint32 i = 0; i < header->numFrames; ++i) {
        auto *frame = reinterpret_cast<MD2Frame *>(buffer.data() + header->offsetFrames + header->frameSize * i);
        for (quint32 j = 0; j < header->numVertices; ++j) {
            QVector4D vertex;
            vertex.setX(frame->vertices[j].vertex[0] * frame->scale[0] + frame->translate[0]);
            vertex.setY(frame->vertices[j].vertex[1] * frame->scale[1] + frame->translate[1]);
            vertex.setZ(frame->vertices[j].vertex[2] * frame->scale[2] + frame->translate[2]);
            vertex.setW(1);
            if (attributes & Attribute::InvertX)
                vertex.setX(-vertex.x());
            if (attributes & Attribute::InvertY)
                vertex.setY(-vertex.y());
            if (attributes & Attribute::InvertZ)
                vertex.setZ(-vertex.z());
            if (attributes & Attribute::SwapYZ) {
                qreal z = vertex.z();
                vertex.setZ(vertex.y());
                vertex.setY(z);
            }
            if (attributes & Attribute::SwapXZ) {
                qreal z = vertex.z();
                vertex.setZ(vertex.x());
                vertex.setX(z);
            }
            if (attributes & Attribute::SwapXY) {
                qreal y = vertex.y();
                vertex.setY(vertex.x());
                vertex.setX(y);
            }
            result.vertices.push_back(vertex);
        }
    }

    result.numFrames = header->numFrames;
    result.numVertices = header->numVertices;

    auto *face = reinterpret_cast<MD2Triangle *>(buffer.data() + header->offsetTriangles);
    for (quint32 i = 0; i < header->numTriangles; ++i) {
        if (attributes & Attribute::InvertWindingOrder) {
            QVector3D vertexIndex(face[i].vertexIndices[2], face[i].vertexIndices[1], face[i].vertexIndices[0]);
            result.vertexIndices.push_back(vertexIndex);
            QVector3D texCoordIndex(face[i].textureIndices[2], face[i].textureIndices[1], face[i].textureIndices[0]);
            result.texCoordIndices.push_back(texCoordIndex);
        } else {
            QVector3D vertexIndex(face[i].vertexIndices[0], face[i].vertexIndices[1], face[i].vertexIndices[2]);
            result.vertexIndices.push_back(vertexIndex);
            QVector3D texCoordIndex(face[i].textureIndices[0], face[i].textureIndices[1], face[i].textureIndices[2]);
            result.texCoordIndices.push_back(texCoordIndex);
        }
    }

    return result;
}

AGMesh::MeshData AGMesh::loadPLGFile(const QString &inMeshPath, Attributes attributes)
{
    Q_UNUSED(attributes);

    MeshData result;
    auto device = AGHelper::getStreamForFile(inMeshPath);
    QTextStream ts(device.data());
    const QString &firstLine = ts.readLine().trimmed();
    if (!firstLine.contains("begin plg/plx file"))
        return result;

    unsigned int numVertices = 0;
    unsigned int numIndices = 0;

    // get vertices and indices count
    while (!ts.atEnd()) {
        const auto &line = ts.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#"))
            continue;
        const auto &parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.length() < 3)
            continue;
        numVertices = parts[1].toInt();
        numIndices = parts[2].toInt();
        break;
    }

    result.numVertices = numVertices;

    // load vertices
    while (!ts.atEnd()) {
        const auto &line = ts.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#"))
            continue;
        const auto &parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.length() < 3)
            continue;
        result.vertices.push_back(
            QVector4D(parts[0].toDouble(), parts[1].toDouble(), parts[2].toDouble(), 1.0)
        );
        if (result.vertices.length() == numVertices)
            break;
    }

    // load indices
    while (!ts.atEnd()) {
        const auto &line = ts.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#"))
            continue;
        const auto &parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.length() < 5)
            continue;
        result.vertexIndices.push_back(
            QVector3D(parts[2].toInt(), parts[3].toInt(), parts[4].toInt())
        );
        if (result.vertexIndices.length() == numIndices)
            break;
    }
    return result;
}

AGMesh::MeshData AGMesh::loadCOBFile(const QString &inMeshPath, Attributes attributes)
{
    MeshData result;
    auto device = AGHelper::getStreamForFile(inMeshPath);
    QTextStream ts(device.data());
    const QString &firstLine = ts.readLine().trimmed();
    if (!firstLine.contains("Caligari V00.01ALH"))
        return result;

    static QRegularExpression worldVerticesRegExp(QStringLiteral("^World Vertices (\\d+)$"));
    static QRegularExpression facesRegExp(QStringLiteral("^Faces (\\d+)$"));
    static QRegularExpression textureVerticesRegExp(QStringLiteral("^Texture Vertices (\\d+)$"));
    static QRegularExpression faceVertsFlagsRegExp("^Face verts (\\d+) flags (\\d+) mat (\\d+)$");
    static QRegularExpression faceVertsRegExp(QStringLiteral("^<(\\d+),(\\d+)> <(\\d+),(\\d+)> <(\\d+),(\\d+)>$"));

    unsigned int numVertices = 0;
    unsigned int numIndices = 0;
    unsigned int numTexCoords = 0;

    // get vertices count
    while (!ts.atEnd()) {
        const auto line = ts.readLine().trimmed();
        if (line.isEmpty())
            continue;
        QRegularExpressionMatch match = worldVerticesRegExp.match(line);
        if (!match.hasMatch())
            continue;
        numVertices = match.captured(1).toInt();
        break;
    }

    result.numVertices = numVertices;

    // load vertices
    while (!ts.atEnd()) {
        const auto line = ts.readLine().trimmed();
        if (line.isEmpty())
            continue;
        const auto &parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.length() < 3)
            continue;
        QVector4D vertex(parts[0].toDouble(), parts[1].toDouble(), parts[2].toDouble(), 1.0);
        if (attributes & Attribute::InvertX)
            vertex.setX(-vertex.x());
        if (attributes & Attribute::InvertY)
            vertex.setY(-vertex.y());
        if (attributes & Attribute::InvertZ)
            vertex.setZ(-vertex.z());
        if (attributes & Attribute::SwapYZ) {
            qreal z = vertex.z();
            vertex.setZ(vertex.y());
            vertex.setY(z);
        }
        if (attributes & Attribute::SwapXZ) {
            qreal z = vertex.z();
            vertex.setZ(vertex.x());
            vertex.setX(z);
        }
        if (attributes & Attribute::SwapXY) {
            qreal y = vertex.y();
            vertex.setY(vertex.x());
            vertex.setX(y);
        }
        result.vertices.push_back(vertex);
        if (result.vertices.length() == numVertices)
            break;
    }

    // get texture coordinates count
    while (!ts.atEnd()) {
        const auto line = ts.readLine().trimmed();
        if (line.isEmpty())
            continue;
        QRegularExpressionMatch match = textureVerticesRegExp.match(line);
        if (!match.hasMatch())
            continue;
        numTexCoords = match.captured(1).toInt();
        break;
    }

    // load texture coordinates
    while (!ts.atEnd()) {
        const auto line = ts.readLine().trimmed();
        if (line.isEmpty())
            continue;
        const auto &parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.length() < 2)
            continue;
        QVector2D texCoords(parts[0].toDouble(), parts[1].toDouble());
        if (attributes & Attribute::InvertU)
            texCoords.setX(1 - texCoords.x());
        if (attributes & Attribute::InvertV)
            texCoords.setY(1 - texCoords.y());
        result.texCoords.push_back(texCoords);
        if (result.texCoords.length() == numTexCoords)
            break;
    }

    // get indices count
    while (!ts.atEnd()) {
        const auto line = ts.readLine().trimmed();
        if (line.isEmpty())
            continue;
        QRegularExpressionMatch match = facesRegExp.match(line);
        if (!match.hasMatch())
            continue;
        numIndices = match.captured(1).toInt();
        break;
    }

    // load indices
    while (!ts.atEnd()) {
        auto line = ts.readLine().trimmed();
        if (line.isEmpty())
            continue;
        QRegularExpressionMatch match = faceVertsFlagsRegExp.match(line);
        if (!match.hasMatch())
            continue;
        const int material  = match.captured(3).toInt();
        line = ts.readLine().trimmed();
        if (line.isEmpty())
            continue;
        match = faceVertsRegExp.match(line);
        if (!match.hasMatch())
            continue;
        result.materials.append(material);
        if (attributes & Attribute::InvertWindingOrder) {
            QVector3D vertexIndex(match.captured(5).toInt(), match.captured(3).toInt(), match.captured(1).toInt());
            result.vertexIndices.push_back(vertexIndex);
            QVector3D texCoordIndex(match.captured(6).toInt(), match.captured(4).toInt(), match.captured(2).toInt());
            result.texCoordIndices.push_back(texCoordIndex);
        } else {
            QVector3D vertexIndex(match.captured(1).toInt(), match.captured(3).toInt(), match.captured(5).toInt());
            result.vertexIndices.push_back(vertexIndex);
            QVector3D texCoordIndex(match.captured(2).toInt(), match.captured(4).toInt(), match.captured(6).toInt());
            result.texCoordIndices.push_back(texCoordIndex);
        }
        if (result.vertexIndices.length() == numIndices)
            break;
    }

    return result;
}

AGMesh::MeshData AGMesh::loadTerrain(const QString &inMeshPath, Attributes attributes)
{
    Q_UNUSED(attributes);

    MeshData result;
    QImage image;
    if (!image.load(inMeshPath))
        return result;

    int rows = image.height();
    int columns = image.width();

    if (rows != 40 && columns != 40)
        return result;

    qreal rowStep = 1.f / (qreal)(rows - 1);
    qreal columnStep = 1.f / (qreal)(columns - 1);

    const auto *bits = image.constBits();

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            qreal x = (qreal)column * columnStep - 0.5;
            qreal y = (qreal)bits[column + (row * columns)] / 255.f;
            qreal z = (qreal)row * rowStep - 0.5;
            QVector4D vertex(x, y, z, 1.0);
            result.vertices.push_back(vertex);
            QVector2D texCoord((qreal)column / columns, (qreal)row / columns);
            result.texCoords.push_back(texCoord);
        }
    }

    result.numVertices = result.vertices.size();

    int numIndices = (columns - 1) * (rows - 1);
    for (int i = 0; i < numIndices; ++i) {
        int index = (i % (columns - 1)) + (columns * (i / (columns - 1)));
        QVector3D vertexIndex1(index, index + columns, index + columns + 1);
        QVector3D vertexIndex2(index, index + columns + 1, index + 1);
        result.vertexIndices.push_back(vertexIndex1);
        result.vertexIndices.push_back(vertexIndex2);
        result.texCoordIndices.push_back(vertexIndex1);
        result.texCoordIndices.push_back(vertexIndex2);
    }

    return result;
}

AGMesh::MeshData AGMesh::loadPrimitive(const QString &primitive, Attributes attributes)
{
    Q_UNUSED(attributes);

    MeshData result;
    if (primitive == QStringLiteral("#triangle")) {
        result.vertices.push_back(QVector4D(0, 1, 0, 1));
        result.vertices.push_back(QVector4D(1, -1, 0, 1));
        result.vertices.push_back(QVector4D(-1, -1, 0, 1));
        result.normals.push_back(QVector3D(0, 0, 0));
        result.normals.push_back(QVector3D(0, 0, 0));
        result.normals.push_back(QVector3D(0, 0, 0));
        result.vertexIndices.push_back(QVector3D(0, 1, 2));
    }
    result.numVertices = result.vertices.size();
    return result;
}

