#pragma once

#include "agobject.h"

class AGLight : public AGObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type WRITE setLightType NOTIFY typeChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QVector3D worldDirection READ worldDirection WRITE setWorldDirection NOTIFY worldDirectionChanged)
    Q_PROPERTY(qreal constantAttenuation READ constantAttenuation WRITE setConstantAttenuation NOTIFY constantAttenuationChanged)
    Q_PROPERTY(qreal linearAttenuation READ linearAttenuation WRITE setLinearAttenuation NOTIFY linearAttenuationChanged)
    Q_PROPERTY(qreal quadraticAttenuation READ quadraticAttenuation WRITE setQuadraticAttenuation NOTIFY quadraticAttenuationChanged)
    Q_PROPERTY(qreal coneAngle READ coneAngle WRITE setConeAngle NOTIFY coneAngleChanged)
    Q_PROPERTY(qreal innerConeAngle READ innerConeAngle WRITE setInnerConeAngle NOTIFY innerConeAngleChanged)
    Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(bool castsShadow READ castsShadow WRITE setCastsShadow NOTIFY castsShadowChanged)

public:
    AGLight(QObject *parent = nullptr);

    enum Type {
        AmbientLight = 1,
        DirectionalLight = 2,
        PointLight = 3,
        SpotLight = 4
    };
    Q_ENUM(Type)

    Type type() const;
    QColor color() const;
    QVector3D worldDirection() const;
    qreal constantAttenuation() const;
    qreal linearAttenuation() const;
    qreal quadraticAttenuation() const;
    qreal coneAngle() const;
    qreal innerConeAngle() const;
    qreal brightness() const;
    bool castsShadow() const;

public Q_SLOTS:
    void setLightType(AGLight::Type type);
    void setColor(const QColor &color);
    void setWorldDirection(const QVector3D &dir);
    void setConstantAttenuation(qreal constantAttenuation);
    void setLinearAttenuation(qreal linearAttenuation);
    void setQuadraticAttenuation(qreal quadraticAttenuation);
    void setConeAngle(qreal coneAngle);
    void setInnerConeAngle(qreal innerConeAngle);
    void setBrightness(qreal brightness);
    void setCastsShadow(bool castsShadow);

Q_SIGNALS:
    void typeChanged();
    void colorChanged();
    void worldDirectionChanged();
    void constantAttenuationChanged();
    void linearAttenuationChanged();
    void quadraticAttenuationChanged();
    void coneAngleChanged();
    void innerConeAngleChanged();
    void brightnessChanged();
    void castsShadowChanged();

private:
    Type m_type = Type::AmbientLight;
    QColor m_color{Qt::black};
    QVector3D m_worldDirection{0, 0, -1};
    qreal m_constantAttenuation = 0;
    qreal m_linearAttenuation = 0.001;
    qreal m_quadraticAttenuation = 0;
    qreal m_coneAngle = 0;
    qreal m_innerConeAngle = 0;
    qreal m_brightness = 1;
    bool m_castsShadow = false;
};

