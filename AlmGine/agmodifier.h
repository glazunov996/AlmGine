#pragma once

#include <QObject>
#include <QPointer>

#include "agrenderstats.h"
#include "agtriangle.h"

class AGModifier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float frameTime READ frameTime CONSTANT)

public:
    AGModifier(QObject *parent = nullptr);

    float frameTime() const;

Q_SIGNALS:
    void modifyTriangle(AGTriangle *triangle);

private:
    QPointer<AGRenderStats> m_renderStats;

    friend class AGModel;
};

