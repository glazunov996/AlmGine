#include "aglight.h"

AGLight::AGLight(QObject *parent)
    : AGObject(parent)
{
}

AGLight::Type AGLight::type() const
{
    return m_type;
}

QColor AGLight::color() const
{
    return m_color;
}

QVector3D AGLight::worldDirection() const
{
    return m_worldDirection;
}

qreal AGLight::constantAttenuation() const
{
    return m_constantAttenuation;
}

qreal AGLight::linearAttenuation() const
{
    return m_linearAttenuation;
}

qreal AGLight::quadraticAttenuation() const
{
    return m_quadraticAttenuation;
}

qreal AGLight::coneAngle() const
{
    return m_coneAngle;
}

qreal AGLight::innerConeAngle() const
{
    return m_innerConeAngle;
}

qreal AGLight::brightness() const
{
    return m_brightness;
}

bool AGLight::castsShadow() const
{
    return m_castsShadow;
}

void AGLight::setLightType(Type type)
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void AGLight::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged();
}

void AGLight::setWorldDirection(const QVector3D &dir)
{
    const auto &normDir = dir.normalized();
    if (m_worldDirection == normDir)
        return;

    m_worldDirection = normDir;
    emit worldDirectionChanged();
}

void AGLight::setConstantAttenuation(qreal constantAttenuation)
{
    if (qFuzzyCompare(m_constantAttenuation, constantAttenuation))
        return;

    m_constantAttenuation = constantAttenuation;
    emit constantAttenuationChanged();
}

void AGLight::setLinearAttenuation(qreal linearAttenuation)
{
    if (qFuzzyCompare(m_linearAttenuation, linearAttenuation))
        return;

    m_linearAttenuation = linearAttenuation;
    emit linearAttenuationChanged();
}

void AGLight::setQuadraticAttenuation(qreal quadraticAttenuation)
{
    if (qFuzzyCompare(m_quadraticAttenuation, quadraticAttenuation))
        return;

    m_quadraticAttenuation = quadraticAttenuation;
    emit quadraticAttenuationChanged();
}

void AGLight::setConeAngle(qreal coneAngle)
{
    if (coneAngle < 0)
        coneAngle = 0.f;
    else if (coneAngle > 180.f)
        coneAngle = 180.f;

    if (qFuzzyCompare(m_coneAngle, coneAngle))
        return;

    m_coneAngle = coneAngle;
    emit coneAngleChanged();
}

void AGLight::setInnerConeAngle(qreal innerConeAngle)
{
    if (innerConeAngle < 0)
        innerConeAngle = 0.f;
    else if (innerConeAngle > 180.f)
        innerConeAngle = 180.f;

    if (qFuzzyCompare(m_innerConeAngle, innerConeAngle))
        return;

    m_innerConeAngle = innerConeAngle;
    emit innerConeAngleChanged();
}

void AGLight::setBrightness(qreal brightness)
{
    if (qFuzzyCompare(m_brightness, brightness))
        return;

    m_brightness = brightness;
    emit brightnessChanged();
}

void AGLight::setCastsShadow(bool castsShadow)
{
    if (m_castsShadow == castsShadow)
        return;

    m_castsShadow = castsShadow;
    emit castsShadowChanged();
}
