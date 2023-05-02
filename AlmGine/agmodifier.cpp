#include "agmodifier.h"

AGModifier::AGModifier(QObject *parent)
    : QObject(parent)
{
}

float AGModifier::frameTime() const
{
    if (m_renderStats == nullptr)
        return 0;

    return m_renderStats->frameTime();
}
