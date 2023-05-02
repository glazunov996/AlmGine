#include <QPainter>
#include <QDebug>

#include "agviewport.h"
#include "agrenderer.h"
#include "agcamera.h"

AGViewport::AGViewport(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setFlag(ItemHasContents, true);
    m_renderStats = new AGRenderStats();
}

AGViewport::~AGViewport()
{
    delete m_renderStats;
    delete m_renderer;
}

int AGViewport::viewportWidth() const
{
    return m_viewportWidth;
}

int AGViewport::viewportHeight() const
{
    return m_viewportHeight;
}

AGCamera *AGViewport::camera() const
{
    return m_camera;
}

AGRenderStats *AGViewport::renderStats() const
{
    return m_renderStats;
}

QQmlListProperty<AGModel> AGViewport::models()
{
    return QQmlListProperty<AGModel>(this,
                                     nullptr,
                                     AGViewport::qmlAppendModel,
                                     AGViewport::qmlModelsCount,
                                     AGViewport::qmlModelAt,
                                     AGViewport::qmlClearModels);
}

QQmlListProperty<AGLight> AGViewport::lights()
{
    return QQmlListProperty<AGLight>(this,
                                     nullptr,
                                     AGViewport::qmlAppendLight,
                                     AGViewport::qmlLightsCount,
                                     AGViewport::qmlLightAt,
                                     AGViewport::qmlClearLights);
}

void AGViewport::setViewportWidth(int width)
{
    if (m_viewportWidth == width)
        return;

    m_viewportWidth = width;
    emit viewportWidthChanged();
    m_viewportDirty = true;
}

void AGViewport::setViewportHeight(int height)
{
    if (m_viewportHeight == height)
        return;

    m_viewportHeight = height;
    emit viewportHeightChanged();
    m_viewportDirty = true;
}

void AGViewport::setCamera(AGCamera *camera)
{
    if (m_camera == camera)
        return;
    m_camera = camera;
}

void AGViewport::prepareRenderTriangles()
{
    static const auto isTriangleAverageZLessThan = [](AGTriangle *a, AGTriangle *b) {
        qreal z1 = (a->vertices[0].z() + a->vertices[1].z() + a->vertices[2].z()) / 3;
        qreal z2 = (b->vertices[0].z() + b->vertices[1].z() + b->vertices[2].z()) / 3;
        return z1 > z2;
    };

    std::sort(m_renderTriangles.begin(), m_renderTriangles.end(), isTriangleAverageZLessThan);

    QList<AGTriangle *> shadowTriangles;
    for (auto *triangle: m_renderTriangles) {
        if (triangle->material->type() == AGMaterial::Shadow) {
            shadowTriangles.push_back(triangle);
            continue;
        }
        const QVector4D points[3] = {
            m_camera->mapToViewport(triangle->vertices[0]),
            m_camera->mapToViewport(triangle->vertices[1]),
            m_camera->mapToViewport(triangle->vertices[2])
        };
        m_renderer->drawTriangle(
            points[0].x(), points[0].y(), points[0].z(), triangle->texCoords[0].x(), triangle->texCoords[0].y(),
            points[1].x(), points[1].y(), points[1].z(), triangle->texCoords[1].x(), triangle->texCoords[1].y(),
            points[2].x(), points[2].y(), points[2].z(), triangle->texCoords[2].x(), triangle->texCoords[2].y(),
            triangle->litColors, triangle->material->texture(), triangle->material->color()
        );
    }

    m_renderer->beginDrawShadow();

    for (auto *triangle: shadowTriangles) {
        const QVector4D points[3] = {
            m_camera->mapToViewport(triangle->vertices[0]),
            m_camera->mapToViewport(triangle->vertices[1]),
            m_camera->mapToViewport(triangle->vertices[2])
        };
        m_renderer->drawTriangle(
            points[0].x(), points[0].y(), points[0].z(), triangle->texCoords[0].x(), triangle->texCoords[0].y(),
            points[1].x(), points[1].y(), points[1].z(), triangle->texCoords[1].x(), triangle->texCoords[1].y(),
            points[2].x(), points[2].y(), points[2].z(), triangle->texCoords[2].x(), triangle->texCoords[2].y(),
            triangle->litColors, triangle->material->texture(), triangle->material->color()
        );
    }
}

void AGViewport::prepare()
{
    m_renderStats->startRender();
    if (m_camera) {
        const QRectF inViewport = m_renderer->viewport();
        m_camera->calculateViewProjectionMatrix(inViewport);
        qDeleteAll(m_renderTriangles);
        m_renderTriangles.clear();
        for (auto *model : m_models)
            model->updateObject(this);
        prepareRenderTriangles();
    }
}

void AGViewport::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    if (change == ItemSceneChange) {
        if (value.window == m_window)
            return;

        if (m_window)
            disconnect(m_window, &QQuickWindow::beforeRendering, this, &AGViewport::prepare);
        m_window = value.window;
        if (m_window)
            connect(m_window, &QQuickWindow::beforeRendering, this, &AGViewport::prepare);
    }
}

void AGViewport::paint(QPainter *painter)
{
    if (m_viewportDirty) {
        m_viewportDirty = false;
        QObject::disconnect(window());
        delete m_renderer;
        m_renderer = nullptr;
    }

    if (!m_renderer)
        m_renderer = new AGRenderer(m_viewportWidth, m_viewportHeight);

    m_renderer->display(painter, boundingRect());
    m_renderStats->endRender();
    update();
}

void AGViewport::qmlAppendModel(QQmlListProperty<AGModel> *list, AGModel *model)
{
    if (model == nullptr)
        return;
    AGViewport *self = static_cast<AGViewport *>(list->object);
    self->m_models.push_back(model);
}

AGModel *AGViewport::qmlModelAt(QQmlListProperty<AGModel> *list, qsizetype index)
{
    AGViewport *self = static_cast<AGViewport *>(list->object);
    return self->m_models.at(index);
}

qsizetype AGViewport::qmlModelsCount(QQmlListProperty<AGModel> *list)
{
    AGViewport *self = static_cast<AGViewport *>(list->object);
    return self->m_models.count();
}

void AGViewport::qmlClearModels(QQmlListProperty<AGModel> *list)
{
    AGViewport *self = static_cast<AGViewport *>(list->object);
    return self->m_models.clear();
}

void AGViewport::qmlAppendLight(QQmlListProperty<AGLight> *list, AGLight *light)
{
    if (light == nullptr)
        return;
    AGViewport *self = static_cast<AGViewport *>(list->object);
    self->m_lights.push_back(light);
}

AGLight *AGViewport::qmlLightAt(QQmlListProperty<AGLight> *list, qsizetype index)
{
    AGViewport *self = static_cast<AGViewport *>(list->object);
    return self->m_lights.at(index);
}

qsizetype AGViewport::qmlLightsCount(QQmlListProperty<AGLight> *list)
{
    AGViewport *self = static_cast<AGViewport *>(list->object);
    return self->m_lights.count();
}

void AGViewport::qmlClearLights(QQmlListProperty<AGLight> *list)
{
    AGViewport *self = static_cast<AGViewport *>(list->object);
    return self->m_lights.clear();
}
