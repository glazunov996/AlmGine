#pragma once

#include <QQuickItem>
#include <QObject>
#include <QQmlParserStatus>
#include <QUrl>
#include <QImage>
#include <QColor>

class AGTexture : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int width READ width CONSTANT)
    Q_PROPERTY(int height READ height CONSTANT)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

public:
    AGTexture(QObject *parent = nullptr);

    int width() const;
    int height() const;
    QUrl source() const;
    QColor pixel(int x, int y) const;

public Q_SLOTS:
    void setSource(const QUrl &source);

Q_SIGNALS:
    void sourceChanged();

private:
    void loadSource();

    QUrl m_source;
    QImage m_image;
    const unsigned int *m_data = nullptr;
};

