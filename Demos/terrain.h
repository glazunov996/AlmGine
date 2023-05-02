#pragma once

#include <QObject>

#include "AlmGine/agmodel.h"

class AGViewport;
class AGLight;

class Terrain : public AGModel
{
    Q_OBJECT
    Q_PROPERTY(qreal terrainWidth READ terrainWidth WRITE setTerrainWidth NOTIFY terrainWidthChanged)
    Q_PROPERTY(qreal terrainHeight READ terrainHeight WRITE setTerrainHeight NOTIFY terrainHeightChanged)
    Q_PROPERTY(int sourceWidth READ sourceWidth WRITE setSourceWidth NOTIFY sourceWidthChanged)
    Q_PROPERTY(int sourceHeight READ sourceHeight WRITE setSourceHeight NOTIFY sourceHeightChanged)
    Q_PROPERTY(qreal gclearance READ gclearance WRITE setGclearance NOTIFY gclearanceChanged)
    Q_PROPERTY(qreal delta READ delta NOTIFY deltaChanged)

public:
    Terrain(QObject *parent = nullptr);

    qreal terrainWidth() const;
    qreal terrainHeight() const;
    int sourceWidth() const;
    int sourceHeight() const;
    qreal gclearance() const;
    qreal delta() const;

    void updateWorldVertices(QList<QVector4D> &vertices, AGViewport *viewport) override;

public Q_SLOTS:
    void setTerrainWidth(qreal terrainWidth);
    void setTerrainHeight(qreal terrainHeight);
    void setSourceWidth(int sourceWidth);
    void setSourceHeight(int sourceHeight);
    void setGclearance(qreal gclearance);

Q_SIGNALS:
    void terrainWidthChanged();
    void terrainHeightChanged();
    void sourceWidthChanged();
    void sourceHeightChanged();
    void gclearanceChanged();
    void deltaChanged();

private:
    qreal m_terrainWidth = 0;
    qreal m_terrainHeight = 0;
    int m_sourceWidth = 40;
    int m_sourceHeight = 40;
    qreal m_gclearance = 75.f;
    qreal m_delta = 0;
};
