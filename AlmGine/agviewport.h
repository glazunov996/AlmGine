#pragma once

#include <QQuickWindow>
#include <QQuickPaintedItem>
#include <QSharedPointer>
#include <QImage>
#include <QList>
#include <QVariant>
#include <QPainter>

#include "agrenderstats.h"
#include "agcamera.h"
#include "agmodel.h"
#include "aglight.h"
#include "agtriangle.h"

class AGRenderer;

class AGViewport : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int viewportWidth READ viewportWidth WRITE setViewportWidth NOTIFY viewportWidthChanged)
    Q_PROPERTY(int viewportHeight READ viewportHeight WRITE setViewportHeight NOTIFY viewportHeightChanged)
    Q_PROPERTY(AGCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QQmlListProperty<AGModel> models READ models CONSTANT)
    Q_PROPERTY(QQmlListProperty<AGLight> lights READ lights CONSTANT)
    Q_PROPERTY(AGRenderStats *renderStats READ renderStats CONSTANT)

public:
    AGViewport(QQuickItem *parent = nullptr);
    ~AGViewport() override;

    int viewportWidth() const;
    int viewportHeight() const;
    AGCamera *camera() const;
    QQmlListProperty<AGModel> models();
    QQmlListProperty<AGLight> lights();
    AGRenderStats *renderStats() const;

public Q_SLOTS:
    void setViewportWidth(int width);
    void setViewportHeight(int height);
    void setCamera(AGCamera *camera);
    void prepare();

Q_SIGNALS:
    void cameraChanged();
    void viewportWidthChanged();
    void viewportHeightChanged();

protected:
    void prepareRenderTriangles();
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) override;
    void paint(QPainter *painter) override;

public:
    QList<AGModel *> m_models;
    QList<AGLight *> m_lights;
    AGRenderer *m_renderer = nullptr;
    AGCamera *m_camera = nullptr;
    QList<QSharedPointer<AGTriangle>> m_renderTriangles;

private:
    static void qmlAppendModel(QQmlListProperty<AGModel> *list, AGModel *model);
    static AGModel *qmlModelAt(QQmlListProperty<AGModel> *list, qsizetype index);
    static qsizetype qmlModelsCount(QQmlListProperty<AGModel> *list);
    static void qmlClearModels(QQmlListProperty<AGModel> *list);

    static void qmlAppendLight(QQmlListProperty<AGLight> *list, AGLight *light);
    static AGLight *qmlLightAt(QQmlListProperty<AGLight> *list, qsizetype index);
    static qsizetype qmlLightsCount(QQmlListProperty<AGLight> *list);
    static void qmlClearLights(QQmlListProperty<AGLight> *list);

    QQuickWindow *m_window = nullptr;
    int m_viewportWidth = 320;
    int m_viewportHeight = 240;
    AGRenderStats *m_renderStats = nullptr;
    bool m_viewportDirty = false;
};

