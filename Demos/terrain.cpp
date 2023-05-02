#include "terrain.h"

#include "AlmGine/agviewport.h"
#include "AlmGine/agcamera.h"

Terrain::Terrain(QObject *parent)
    : AGModel(parent)
{
}

qreal Terrain::terrainWidth() const
{
    return m_terrainWidth;
}

qreal Terrain::terrainHeight() const
{
    return m_terrainHeight;
}

int Terrain::sourceWidth() const
{
    return m_sourceWidth;
}

int Terrain::sourceHeight() const
{
    return m_sourceHeight;
}

qreal Terrain::gclearance() const
{
    return m_gclearance;
}

qreal Terrain::delta() const
{
    return m_delta;
}

void Terrain::setTerrainWidth(qreal terrainWidth)
{
    if (qFuzzyCompare(terrainWidth, m_terrainWidth))
        return;

    m_terrainWidth = terrainWidth;
    emit terrainWidthChanged();
}

void Terrain::setTerrainHeight(qreal terrainHeight)
{
    if (qFuzzyCompare(terrainHeight, m_terrainHeight))
        return;

    m_terrainHeight = terrainHeight;
    emit terrainHeightChanged();
}

void Terrain::setSourceWidth(int sourceWidth)
{
    if (m_sourceWidth == sourceWidth)
        return;

    m_sourceWidth = sourceWidth;
    emit sourceWidthChanged();
}

void Terrain::setSourceHeight(int sourceHeight)
{
    if (m_sourceHeight == sourceHeight)
        return;

    m_sourceHeight = sourceHeight;
    emit sourceHeightChanged();
}

void Terrain::setGclearance(qreal gclearance)
{
    if (qFuzzyCompare(gclearance, m_gclearance))
        return;

    m_gclearance = gclearance;
    emit gclearanceChanged();
}

void Terrain::updateWorldVertices(QList<QVector4D> &vertices, AGViewport *viewport)
{
    AGCamera *camera = viewport->camera();

    int ivar1 = m_sourceWidth;
    int ivar2 = m_sourceHeight;

    qreal fvar1 = m_terrainWidth / (qreal)(ivar1 - 1);
    qreal fvar2 = m_terrainHeight / (qreal)(ivar2 - 1);

    int cellX = (camera->position().x() + m_terrainWidth / 2) / fvar1;
    int cellY = (camera->position().z() + m_terrainHeight / 2) / fvar2;
\
    qreal delta = qQNaN();

    if ((cellX >= 0 && cellX < ivar1) && (cellY >= 0 && cellY < ivar2)) {
        int v0 = cellX + cellY * ivar2;
        int v1 = v0 + 1;
        int v2 = v1 + ivar2;
        int v3 = v0 + ivar2;

        qreal y0 = v0 < vertices.size() ? vertices[v0].y() : 0;
        qreal y1 = v1 < vertices.size() ? vertices[v1].y() : 0;
        qreal y2 = v2 < vertices.size() ? vertices[v2].y() : 0;
        qreal y3 = v3 < vertices.size() ? vertices[v3].y() : 0;

        qreal th = 0.25 * (y0 + y1 + y2 + y3);
        delta = th - (camera->position().y() - m_gclearance);
    }

    if (!qFuzzyCompare(delta, m_delta)) {
        m_delta = delta;
        emit deltaChanged();
    }
}
