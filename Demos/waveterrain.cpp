#include "waveterrain.h"
#include "AlmGine/agtriangle.h"
#include "AlmGine/agviewport.h"
#include "AlmGine/agcamera.h"

#include <QDebug>

WaveTerrain::WaveTerrain(QObject *parent)
    : Terrain(parent)
{
}

qreal WaveTerrain::waterLevel() const
{
    return m_waterLevel;
}

qreal WaveTerrain::waveHeight() const
{
    return m_waveHeight;
}

qreal WaveTerrain::waveRate() const
{
    return m_waveRate;
}

void WaveTerrain::setWaterLevel(qreal waterLevel)
{
    if (qFuzzyCompare(waterLevel, m_waterLevel))
        return;

    m_waterLevel = waterLevel;
    emit waterLevelChanged();
}

void WaveTerrain::setWaveHeight(qreal waveHeight)
{
    if (qFuzzyCompare(waveHeight, m_waveHeight))
        return;

    m_waveHeight = waveHeight;
    emit waveHeightChanged();
}

void WaveTerrain::setWaveRate(qreal waveRate)
{
    if (qFuzzyCompare(waveRate, m_waveRate))
        return;

    m_waveRate = waveRate;
    emit waveRateChanged();
}

qreal WaveTerrain::waveCount() const
{
    return m_waveCount;
}

void WaveTerrain::updateWorldVertices(QList<QVector4D> &vertices, AGViewport *viewport)
{
    qreal fvar2 = terrainHeight() / (qreal)(sourceHeight() - 1);

    for (int i = 0; i < vertices.size(); ++i) {
        QVector4D *vertex = &vertices[i];
        const qreal y = vertex->y();
        if (y < m_waterLevel)
            vertex->setY(y + m_waveHeight * qSin(m_waveCount + (qreal)(i) / (2 * (qreal)fvar2)));
    }

    m_waveCount += m_waveRate * viewport->renderStats()->frameTime() / 10.0;
    emit waveCountChanged();

    Terrain::updateWorldVertices(vertices, viewport);
}
