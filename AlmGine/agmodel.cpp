#include <QQmlContext>
#include <QQmlFile>

#include "agmodel.h"
#include "agmesh.h"
#include "aghelper.h"
#include "agcamera.h"
#include "agrenderer.h"
#include "aglight.h"
#include "agviewport.h"

AGModel::AGModel(QObject *parent)
    : AGObject(parent)
{
    m_shadowMaterial.setType(AGMaterial::Shadow);
    m_shadowMaterial.setColor(QColor(0, 0, 0, 100));
}

QUrl AGModel::source() const
{
    return m_source;
}

AGMesh::Attributes AGModel::attributes() const
{
    return m_attributes;
}

bool AGModel::cullBackface() const
{
    return m_cullBackface;
}

QQmlListProperty<AGMaterial> AGModel::materials()
{
    return QQmlListProperty<AGMaterial>(this,
                                     nullptr,
                                     AGModel::qmlAppendMaterial,
                                     AGModel::qmlMaterialsCount,
                                     AGModel::qmlMaterialAt,
                                     AGModel::qmlClearMaterials);
}

bool AGModel::castsShadow() const
{
    return m_castsShadow;
}

QSharedPointer<AGMesh> AGModel::mesh() const
{
    return m_mesh;
}

qreal AGModel::frameValue() const
{
    return m_frameValue;
}

void AGModel::setSource(const QUrl &source)
{
    if (m_source == source)
        return;

    m_source = source;
    m_meshDirty = true;
    emit sourceChanged();
}

void AGModel::setAttributes(AGMesh::Attributes attributes)
{
    if (m_attributes == attributes)
        return;

    m_attributes = attributes;
    m_meshDirty = true;
    emit attributesChanged();
}

void AGModel::setCullBackface(bool enable)
{
    if (m_cullBackface == enable)
        return;

    m_cullBackface = enable;
    emit cullBackfaceChanged();
}

void AGModel::setCastsShadow(bool castsShadow)
{
    if (m_castsShadow == castsShadow)
        return;

    m_castsShadow = castsShadow;
    emit castsShadowChanged();
}

void AGModel::setFrameValue(qreal frameValue)
{
    if (qFuzzyCompare(frameValue, m_frameValue))
        return;

    m_frameValue = frameValue;
    emit frameValueChanged();
}

QString AGModel::translateMeshSource(QObject *contextObject)
{
    QString fragment;
    if (m_source.hasFragment()) {
        bool isNumber = false;
        m_source.fragment().toInt(&isNumber);
        fragment = QStringLiteral("#") + m_source.fragment();
        if (!isNumber)
            return fragment;
    }

    const auto context = qmlContext(contextObject);
    const auto resolvedUrl = context ? context->resolvedUrl(m_source) : m_source;
    const auto qmlSource = QQmlFile::urlToLocalFileOrQrc(resolvedUrl);
    return qmlSource.isEmpty() ? m_source.path() : qmlSource;
}

bool AGModel::isBackfaceCulled(AGCamera *camera, const QVector3D &normal, const QVector3D &vertex) const
{
    if (!m_cullBackface)
        return false;
    return QVector3D::dotProduct(normal, camera->position() - vertex) <= 0;
}

void AGModel::clipTriangle(AGCamera *camera, const QSharedPointer<AGTriangle> &triangle)
{
    const qreal zTest[3] = {
        camera->zFactor() * triangle->vertices[0].z(),
        camera->zFactor() * triangle->vertices[1].z(),
        camera->zFactor() * triangle->vertices[2].z(),
    };

    if ((triangle->vertices[0].x() > zTest[0] && triangle->vertices[0].x() < -zTest[0])
            && (triangle->vertices[1].x() > zTest[1] && triangle->vertices[1].x() < -zTest[1])
            && (triangle->vertices[2].x() > zTest[2] && triangle->vertices[2].x() < -zTest[2])) {
        return;
    }

    if ((triangle->vertices[0].y() > zTest[0] && triangle->vertices[0].y() < -zTest[0])
            && (triangle->vertices[1].y() > zTest[1] && triangle->vertices[1].y() < -zTest[1])
            && (triangle->vertices[2].y() > zTest[2] && triangle->vertices[2].y() < -zTest[2])) {
        return;
    }

    int flags[3] = {0, 0, 0};
    for (int i = 0; i < 3; ++i)
        flags[i] = triangle->vertices[i].z() > camera->clipFar() ? 1 : triangle->vertices[i].z() < camera->clipNear() ? 2 : 4;
    if ((flags[0] == 1 && flags[1] == 1 && flags[2] == 1)
            || (flags[0] == 2 && flags[1] == 2 && flags[2] == 2)) {
        return;
    }
    int in = 0;
    if (flags[0] == 4)
        in++;
    if (flags[1] == 4)
        in++;
    if (flags[2] == 4)
        in++;
    if ((flags[0] | flags[1] | flags[2]) & 2) {
        int v[3];
        if (in == 1) {
            if (flags[0] == 4) {
                v[0] = 0;
                v[1] = 1;
                v[2] = 2;
            } else if (flags[1] == 4) {
                v[0] = 1;
                v[1] = 2;
                v[2] = 0;
            } else {
                v[0] = 2;
                v[1] = 0;
                v[2] = 1;
            }
            auto p = triangle->vertices[v[1]] - triangle->vertices[v[0]];
            const auto t0 = (camera->clipNear() - triangle->vertices[v[0]].z()) / p.z();
            const auto xi0 = triangle->vertices[v[0]].x() + p.x() * t0;
            const auto yi0 = triangle->vertices[v[0]].y() + p.y() * t0;
            triangle->vertices[v[1]].setX(xi0);
            triangle->vertices[v[1]].setY(yi0);
            triangle->vertices[v[1]].setZ(camera->clipNear());
            p = triangle->vertices[v[2]]- triangle->vertices[v[0]];
            const auto t1 = (camera->clipNear() - triangle->vertices[v[0]].z()) / p.z();
            const auto xi1 = triangle->vertices[v[0]].x() + p.x() * t1;
            const auto yi1 = triangle->vertices[v[0]].y() + p.y() * t1;
            triangle->vertices[v[2]].setX(xi1);
            triangle->vertices[v[2]].setY(yi1);
            triangle->vertices[v[2]].setZ(camera->clipNear());
            const auto ui0 = triangle->texCoords[v[0]].x() + (triangle->texCoords[v[1]].x() - triangle->texCoords[v[0]].x()) * t0;
            const auto vi0 = triangle->texCoords[v[0]].y() + (triangle->texCoords[v[1]].y() - triangle->texCoords[v[0]].y()) * t0;
            triangle->texCoords[v[1]].setX(ui0);
            triangle->texCoords[v[1]].setY(vi0);
            const auto ui1 = triangle->texCoords[v[0]].x() + (triangle->texCoords[v[2]].x() - triangle->texCoords[v[0]].x()) * t1;
            const auto vi1 = triangle->texCoords[v[0]].y() + (triangle->texCoords[v[2]].y() - triangle->texCoords[v[0]].y()) * t1;
            triangle->texCoords[v[2]].setX(ui1);
            triangle->texCoords[v[2]].setY(vi1);
        } else if (in == 2) {
            QSharedPointer<AGTriangle> newTriangle = QSharedPointer<AGTriangle>(new AGTriangle(*triangle));
            if (flags[0] == 2) {
                v[0] = 0;
                v[1] = 1;
                v[2] = 2;
            } else if (flags[1] == 2) {
                v[0] = 1;
                v[1] = 2;
                v[2] = 0;
            } else {
                v[0] = 2;
                v[1] = 0;
                v[2] = 1;
            }
            auto p = triangle->vertices[v[1]] - triangle->vertices[v[0]];
            const auto t0 = (camera->clipNear() - triangle->vertices[v[0]].z()) / p.z();
            const auto x01i = triangle->vertices[v[0]].x() + p.x() * t0;
            const auto y01i = triangle->vertices[v[0]].y() + p.y() * t0;
            p = triangle->vertices[v[2]] - triangle->vertices[v[0]];
            const auto t1 = (camera->clipNear() - triangle->vertices[v[0]].z()) / p.z();
            const auto x02i = triangle->vertices[v[0]].x() + p.x() * t1;
            const auto y02i = triangle->vertices[v[0]].y() + p.y() * t1;
            triangle->vertices[v[0]].setX(x01i);
            triangle->vertices[v[0]].setY(y01i);
            triangle->vertices[v[0]].setZ(camera->clipNear());
            newTriangle->vertices[v[1]].setX(x01i);
            newTriangle->vertices[v[1]].setY(y01i);
            newTriangle->vertices[v[1]].setZ(camera->clipNear());
            newTriangle->vertices[v[0]].setX(x02i);
            newTriangle->vertices[v[0]].setY(y02i);
            newTriangle->vertices[v[0]].setZ(camera->clipNear());
            const auto u01i = triangle->texCoords[v[0]].x() + (triangle->texCoords[v[1]].x() - triangle->texCoords[v[0]].x()) * t0;
            const auto v01i = triangle->texCoords[v[0]].y() + (triangle->texCoords[v[1]].y() - triangle->texCoords[v[0]].y()) * t0;
            const auto u02i = triangle->texCoords[v[0]].x() + (triangle->texCoords[v[2]].x() - triangle->texCoords[v[0]].x()) * t1;
            const auto v02i = triangle->texCoords[v[0]].y() + (triangle->texCoords[v[2]].y() - triangle->texCoords[v[0]].y()) * t1;
            triangle->texCoords[v[0]].setX(u01i);
            triangle->texCoords[v[0]].setY(v01i);
            newTriangle->texCoords[v[0]].setX(u02i);
            newTriangle->texCoords[v[0]].setY(v02i);
            newTriangle->texCoords[v[1]].setX(u01i);
            newTriangle->texCoords[v[1]].setY(v01i);
            m_dirtyTriangles.push_back(newTriangle);
        }
    }
    m_dirtyTriangles.push_back(triangle);
}

void AGModel::lightTriangle(const QVector3D &normal, const QVector3D &firstVertex, const QList<AGLight *> &lights,  const QSharedPointer<AGTriangle> &triangle)
{
    const AGMaterial *material = triangle->material;
    if (material->type() == AGMaterial::Constant) {
        triangle->litColors[0] = material->color();
        triangle->litColors[1] = material->color();
        triangle->litColors[2] = material->color();
        return;
    }
    if (material->type() == AGMaterial::Flat) {
        triangle->normals[0] = normal;
        triangle->normals[1] = normal;
        triangle->normals[2] = normal;
    } else {
        triangle->normals[0] = mapToWorld(triangle->normals[0].toVector4D()).toVector3D();
        triangle->normals[1] = mapToWorld(triangle->normals[1].toVector4D()).toVector3D();
        triangle->normals[2] = mapToWorld(triangle->normals[2].toVector4D()).toVector3D();
    }
    float litColors[3][4] = {
        {0.f, 0.f, 0.f, 0.f},
        {0.f, 0.f, 0.f, 0.f},
        {0.f, 0.f, 0.f, 0.f}
    };

    float tmpColor[4];
    material->color().getRgbF(&tmpColor[0], &tmpColor[1], &tmpColor[2], &tmpColor[3]);
    for (const auto *light: lights) {
        if (!light->visible())
            continue;
        float lightColor[4];
        light->color().getRgbF(&lightColor[0], &lightColor[1], &lightColor[2], &lightColor[3]);
        qreal diffuse[3] = {0, 0, 0};
        if (light->type() == AGLight::DirectionalLight) {
            for (int j = 0; j < 3; ++j) {
                const auto dp = QVector3D::dotProduct(triangle->normals[j], light->worldDirection());
                if (dp > 0)
                    diffuse[j] = dp / triangle->normals[j].length();
            }
        } else if (light->type() == AGLight::PointLight) {
            const auto pointDirection = light->position() - firstVertex;
            for (int j = 0; j < 3; ++j) {
                const auto dp = QVector3D::dotProduct(triangle->normals[j], pointDirection);
                if (dp > 0) {
                    const auto distance = pointDirection.length();
                    const auto attenuation = light->constantAttenuation() +
                                             light->linearAttenuation() * distance +
                                             light->quadraticAttenuation() * distance * distance;
                    diffuse[j] = dp / (triangle->normals[j].length() * distance * attenuation);
                }
            }
        } else if (light->type() == AGLight::SpotLight) {
            for (int j = 0; j < 3; ++j) {
                const auto dp = QVector3D::dotProduct(triangle->normals[j], light->worldDirection());
                if (dp > 0) {
                    const auto spotDirection = firstVertex - light->position();
                    const auto distance = spotDirection.length();
                    const auto dpsl = QVector3D::dotProduct(spotDirection, light->worldDirection());
                    if (dpsl > 0) {
                        const auto attenuation = light->constantAttenuation() +
                                                 light->linearAttenuation() * distance +
                                                 light->quadraticAttenuation() * distance * distance;
                        diffuse[j] = dp * dpsl / (triangle->normals[j].length() * distance * attenuation);
                    }
                }
            }
        } else if (light->type() == AGLight::AmbientLight){
            diffuse[0] = 1;
            diffuse[1] = 1;
            diffuse[2] = 1;
        }
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k)
                litColors[j][k] += (lightColor[k] * tmpColor[k] * diffuse[j] * light->brightness());
        }
    }
    for (int j = 0; j < 3; ++j) {
        for (int k = 0; k < 4; ++k)
            litColors[j][k] = qMin(qMax(litColors[j][k], 0.f), 1.0);
        triangle->litColors[j] = QColor::fromRgbF(litColors[j][0], litColors[j][1], litColors[j][2], litColors[j][3]);
    }
}

void AGModel::castShadow(const QList<AGLight *> &lights)
{
    if (!m_castsShadow)
        return;

    QList<QSharedPointer<AGTriangle>> newTriangles;
    for (const auto *light: lights) {
        if (!light->visible() || !light->castsShadow() || light->type() != AGLight::PointLight)
            continue;
        for (auto &triangle: m_worldTriangles) {
            QSharedPointer<AGTriangle> newTriangle = QSharedPointer<AGTriangle>(new AGTriangle(*triangle));
            newTriangle->material = &m_shadowMaterial;
            for (int i = 0; i < 3; ++i) {
                auto vertex = newTriangle->vertices[i];
                auto t0 = -light->position().y() / (vertex.y() - light->position().y());
                vertex.setX(light->position().x() + t0 * (vertex.x() - light->position().x()));
                vertex.setY(10);
                vertex.setZ(light->position().z() + t0 * (vertex.z() - light->position().z()));
                newTriangle->vertices[i] = vertex;
            }
            newTriangles.push_back(newTriangle);
        }
    }

    m_worldTriangles.append(newTriangles);
}

void AGModel::loadMeshVertices()
{
    const auto &meshData = m_mesh->meshData;
    if (meshData.numFrames) {
        qreal offset = m_frameValue - (int)m_frameValue;
        int frame0 = m_frameValue;
        int frame1 = m_frameValue + 1;
        if (frame0 >= meshData.numFrames)
            frame0 = meshData.numFrames - 1;
        if (frame1 >= meshData.numFrames)
            frame1 = meshData.numFrames - 1;
        int index0 = meshData.numVertices * frame0;
        int index1 = meshData.numVertices * frame1;
        for (int i = 0; i < meshData.numVertices; ++i) {
            const auto &vertex0 = meshData.vertices[index0 + i] * (1 - offset);
            const auto &vertex1 = meshData.vertices[index1 + i] * offset;
            const auto &vertex = vertex0 + vertex1;
            m_worldVertices.push_back(mapToWorld(vertex));
        }
    } else {
        for (int i = 0; i < meshData.numVertices; ++i) {
            const auto &vertex = meshData.vertices[i];
            m_worldVertices.push_back(mapToWorld(vertex));
        }
    }
}

void AGModel::updateObject(AGViewport *viewport)
{
    if (m_source.isEmpty() || !m_materials.length() || !visible())
        return;

    if (m_meshDirty) {
        m_meshDirty = false;
        m_mesh = AGMesh::loadMesh(translateMeshSource(this), m_attributes);
    }

    if (m_mesh.isNull())
        return;

    const auto &meshData = m_mesh->meshData;

    calculateWorldMatrix();
    m_worldTriangles.clear();
    m_dirtyTriangles.clear();
    m_worldVertices.clear();

    loadMeshVertices();
    updateWorldVertices(m_worldVertices, viewport);

    for (int i = 0; i < meshData.vertexIndices.size(); ++i) {
        int material = 0;
        if (i < meshData.materials.length())
            material = meshData.materials[i];
        if (material >= m_materials.length())
            material = m_materials.length() - 1;
        QSharedPointer<AGTriangle> triangle = QSharedPointer<AGTriangle>(new AGTriangle);
        triangle->material = m_materials[material];
        for (int j = 0; j < 3; ++j) {
            triangle->vertices[j] = m_worldVertices[meshData.vertexIndices[i][j]];
            triangle->normals[j] = meshData.normals[meshData.vertexIndices[i][j]];
            if (!meshData.texCoordIndices.isEmpty())
                triangle->texCoords[j] = meshData.texCoords[meshData.texCoordIndices[i][j]];
        }
        m_worldTriangles.push_back(triangle);
    }

    castShadow(viewport->m_lights);

    for (auto &triangle: m_worldTriangles) {
        const auto &a = QVector3D(triangle->vertices[1] - triangle->vertices[0]);
        const auto &b = QVector3D(triangle->vertices[2] - triangle->vertices[0]);
        const auto &normal = QVector3D::crossProduct(a, b);
        const QVector3D &firstVertex = QVector3D(triangle->vertices[0]);
        if (isBackfaceCulled(viewport->m_camera, normal, firstVertex))
            continue;
        lightTriangle(normal, firstVertex, viewport->m_lights, triangle);
        triangle->vertices[0] = viewport->m_camera->mapToCamera(triangle->vertices[0]);
        triangle->vertices[1] = viewport->m_camera->mapToCamera(triangle->vertices[1]);
        triangle->vertices[2] = viewport->m_camera->mapToCamera(triangle->vertices[2]);
        clipTriangle(viewport->m_camera, triangle);
    }

    viewport->m_renderTriangles.append(m_dirtyTriangles);
}

void AGModel::updateWorldVertices(QList<QVector4D> &vertices, AGViewport *viewport)
{
    Q_UNUSED(vertices);
    Q_UNUSED(viewport);
}

void AGModel::qmlAppendMaterial(QQmlListProperty<AGMaterial> *list, AGMaterial *material)
{
    if (material == nullptr)
        return;
    AGModel *self = static_cast<AGModel *>(list->object);
    self->m_materials.push_back(material);
}

AGMaterial *AGModel::qmlMaterialAt(QQmlListProperty<AGMaterial> *list, qsizetype index)
{
    AGModel *self = static_cast<AGModel *>(list->object);
    return self->m_materials.at(index);
}

qsizetype AGModel::qmlMaterialsCount(QQmlListProperty<AGMaterial> *list)
{
    AGModel *self = static_cast<AGModel *>(list->object);
    return self->m_materials.count();
}

void AGModel::qmlClearMaterials(QQmlListProperty<AGMaterial> *list)
{
    AGModel *self = static_cast<AGModel *>(list->object);
    return self->m_materials.clear();
}
