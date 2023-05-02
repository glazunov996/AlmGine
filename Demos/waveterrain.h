#pragma once

#include <QList>

#include "AlmGine/agmodel.h"
#include "terrain.h"

class AGViewport;

class WaveTerrain : public Terrain
{
    Q_OBJECT
    Q_PROPERTY(qreal waveCount READ waveCount NOTIFY waveCountChanged)
    Q_PROPERTY(qreal waterLevel READ waterLevel WRITE setWaterLevel NOTIFY waterLevelChanged)
    Q_PROPERTY(qreal waveHeight READ waveHeight WRITE setWaveHeight NOTIFY waveHeightChanged)
    Q_PROPERTY(qreal waveRate READ waveRate WRITE setWaveRate NOTIFY waveRateChanged)

public:
    WaveTerrain(QObject *parent = nullptr);

    qreal waterLevel() const;
    qreal waveHeight() const;
    qreal waveRate() const;

    qreal waveCount() const;

    void updateWorldVertices(QList<QVector4D> &vertices, AGViewport *viewport) override;

public Q_SLOTS:
    void setWaterLevel(qreal waterLevel);
    void setWaveHeight(qreal waveHeight);
    void setWaveRate(qreal waveRate);

Q_SIGNALS:
    void waterLevelChanged();
    void waveHeightChanged();
    void waveRateChanged();
    void waveCountChanged();

private:
    qreal m_waveCount = 0;
    qreal m_waterLevel = 40.f;
    qreal m_waveHeight = 25.f;
    qreal m_waveRate = 0.1f;
};
