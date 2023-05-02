#pragma once

#include <QImage>
#include <QList>
#include <QRect>

#include "agmaterial.h"

class QPainter;
class AGTexture;

class AGRenderer
{
public:
    AGRenderer() = default;
    AGRenderer(int width, int height);

    QRectF viewport() const;

    void drawTriangle(int x0, int y0, qreal z0, qreal u0, qreal v0,
                      int x1, int y1, qreal z1, qreal u1, qreal v1,
                      int x2, int y2, qreal z2, qreal u2, qreal v2,
                      const std::array<QColor, 3> &litColors,
                      const AGTexture *texture,
                      const QColor &color);

    void beginDrawShadow();

    void display(QPainter *painter, const QRectF &rect);

private:
    void drawPixel(int x, int y, qreal u, qreal v, qreal z, qreal r, qreal g, qreal b,
                   const QColor &color, const AGTexture *texture);
    void clear();

    QImage m_image[2];
    QList<qreal> m_zBuffer;
    bool m_drawShadow = false;

    friend class AGViewport;
};
