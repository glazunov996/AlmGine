#include <QDebug>

#include "agrenderstats.h"

AGRenderStats::AGRenderStats(QObject *parent)
    : QObject(parent)
{
    m_frameTimer.start();
}

int AGRenderStats::fps() const
{
    return m_fps;
}

float AGRenderStats::frameTime() const
{
    return m_results.frameTime;
}

float AGRenderStats::renderTime() const
{
    return m_results.renderTime;
}

float AGRenderStats::maxFrameTime() const
{
    return m_maxFrameTime;
}

void AGRenderStats::startRender()
{
    m_renderStartTime = timestamp();
}

void AGRenderStats::endRender(bool dump)
{
    ++m_frameCount;
    m_results.frameTime = timestamp();
    m_internalMaxFrameTime = qMax(m_results.frameTime, m_internalMaxFrameTime);

    m_secTimer += m_results.frameTime;
    m_notifyTimer += m_results.frameTime;

    m_results.renderTime = m_results.frameTime - m_renderStartTime;

    const float notifyInterval = 200.0f;
    if (m_notifyTimer >= notifyInterval) {
        m_notifyTimer -= notifyInterval;

        if (m_results.frameTime != m_notifiedResults.frameTime) {
            m_notifiedResults.frameTime = m_results.frameTime;
            emit frameTimeChanged();
        }

        if (m_results.renderTime != m_notifiedResults.renderTime) {
            m_notifiedResults.renderTime = m_results.renderTime;
            emit renderTimeChanged();
        }
    }

    const float fpsInterval = 1000.0f;
    if (m_secTimer >= fpsInterval) {
        m_secTimer -= fpsInterval;

        m_fps = m_frameCount;
        m_frameCount = 0;
        emit fpsChanged();

        m_maxFrameTime = m_internalMaxFrameTime;
        m_internalMaxFrameTime = 0;
        emit maxFrameTimeChanged();
    }

    m_frameTimer.restart();

    if (dump)
        qDebug("Render took: %f ms", m_results.renderTime);
}

float AGRenderStats::timestamp() const
{
    return m_frameTimer.nsecsElapsed() / 1000000.0f;
}
