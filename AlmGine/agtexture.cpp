#include <QImage>
#include <QQmlFile>
#include <QQmlContext>

#include "agtexture.h"
#include "aghelper.h"

AGTexture::AGTexture(QObject *parent) :
    QObject(parent)
{
}

int AGTexture::width() const
{
    return m_image.width();
}

int AGTexture::height() const
{
    return m_image.height();
}

QUrl AGTexture::source() const
{
    return m_source;
}

void AGTexture::setSource(const QUrl &source)
{
    if (m_source == source)
        return;

    const QQmlContext *context = qmlContext(this);
    auto resolvedUrl = (context ? context->resolvedUrl(source) : source);

    if (m_source == resolvedUrl)
        return;

    m_source = resolvedUrl;
    emit sourceChanged();

    loadSource();
}

QColor AGTexture::pixel(int x, int y) const
{
    QColor color;
    if (!m_image.isNull()) {
        const auto index = y * m_image.width() + x;
        color = QColor::fromRgba(m_data[index]);
    }
    return color;
}

void AGTexture::loadSource()
{
    if (!m_source.isEmpty()) {
        const auto path = QQmlFile::urlToLocalFileOrQrc(m_source);
        m_image = QImage(path).convertToFormat(QImage::Format_ARGB32);
        m_data = reinterpret_cast<const unsigned int *>(m_image.constBits());
    }
}
