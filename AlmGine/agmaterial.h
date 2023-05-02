#pragma once

#include <QObject>
#include <QColor>

#include "agtexture.h"

class AGMaterial : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(AGTexture *texture READ texture WRITE setTexture NOTIFY textureChanged)

public:
    AGMaterial(QObject *parent = nullptr);
    ~AGMaterial() override = default;

    enum Type {
        Constant = 1,
        Flat = 2,
        Gouraud = 3,
        Shadow = 4
    };
    Q_ENUM(Type)

    Type type() const;
    QColor color() const;
    AGTexture *texture() const;

public Q_SLOTS:
    void setType(AGMaterial::Type type);
    void setColor(const QColor &color);
    void setTexture(AGTexture *texture);

Q_SIGNALS:
    void typeChanged();
    void colorChanged();
    void textureChanged();

private:
    Type m_type{Type::Flat};
    QColor m_color{Qt::white};
    AGTexture *m_texture = nullptr;
};

