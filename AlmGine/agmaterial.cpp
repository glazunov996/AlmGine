#include "agmaterial.h"

AGMaterial::AGMaterial(QObject *parent)
    : QObject(parent)
{
}

AGMaterial::Type AGMaterial::type() const
{
    return m_type;
}

QColor AGMaterial::color() const
{
    return m_color;
}

AGTexture *AGMaterial::texture() const
{
    return m_texture;
}

void AGMaterial::setType(Type type)
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void AGMaterial::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged();
}

void AGMaterial::setTexture(AGTexture *texture)
{
    if (m_texture == texture)
        return;

    m_texture = texture;
    emit textureChanged();
}

