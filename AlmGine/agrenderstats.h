#pragma once

#include <QObject>
#include <QElapsedTimer>

class AGRenderStats : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int fps READ fps NOTIFY fpsChanged)
    Q_PROPERTY(float frameTime READ frameTime NOTIFY frameTimeChanged)
    Q_PROPERTY(float renderTime READ renderTime NOTIFY renderTimeChanged)
    Q_PROPERTY(float maxFrameTime READ maxFrameTime NOTIFY maxFrameTimeChanged)

public:
    AGRenderStats(QObject *parent = nullptr);

    int fps() const;
    float frameTime() const;
    float renderTime() const;
    float maxFrameTime() const;

    void startRender();
    void endRender(bool dump = false);

Q_SIGNALS:
    void fpsChanged();
    void frameTimeChanged();
    void renderTimeChanged();
    void maxFrameTimeChanged();

private:
    float timestamp() const;

    QElapsedTimer m_frameTimer;
    int m_frameCount = 0;
    float m_secTimer = 0;
    float m_notifyTimer = 0;
    float m_renderStartTime = 0;

    float m_internalMaxFrameTime = 0;
    float m_maxFrameTime = 0;

    int m_fps = 0;

    struct Results {
        float frameTime = 0;
        float renderTime = 0;
    };

    Results m_results;
    Results m_notifiedResults;
};
