#include <QPainter>
#include <QDebug>

#include "agrenderer.h"
#include "agmaterial.h"

AGRenderer::AGRenderer(int width, int height)
{
    m_image[0] = QImage(width, height, QImage::Format_ARGB32);
    m_zBuffer.resize(width * height);
    clear();
}

QRectF AGRenderer::viewport() const
{
    return QRectF(0, 0, m_image[0].width(), m_image[0].height());
}

void AGRenderer::drawPixel(int x, int y, qreal u, qreal v, qreal z, qreal r, qreal g, qreal b, const QColor &color, const AGTexture *texture)
{
    if (x < 0 || x >= m_image[0].width() || y < 0 || y >= m_image[0].height())
        return;

    QColor tmp = color;
    if (texture) {
        const QColor &textel = texture->pixel(u / z, v / z);
        tmp.setRed(tmp.red() * textel.red() / 255);
        tmp.setGreen(tmp.green() * textel.green() / 255);
        tmp.setBlue(tmp.blue() * textel.blue() / 255);
        tmp.setAlpha(tmp.alpha() * textel.alpha() / 255);
    }

    tmp.setRed(tmp.red() * r);
    tmp.setGreen(tmp.green() * g);
    tmp.setBlue(tmp.blue() * b);

    const auto i = y * m_image[0].width() + x;
    if (z > m_zBuffer[i]) {
        m_zBuffer[i] = z;
        if (tmp.alphaF() < 1.0) {
            auto bg = m_image[0].pixelColor(x, y);
            auto a0 = 1 - (1 - tmp.alphaF()) * (1 - bg.alphaF());
            auto r0 = tmp.redF() * tmp.alphaF() / a0 + bg.redF() * bg.alphaF() * (1 - tmp.alphaF()) / a0;
            auto g0 = tmp.greenF() * tmp.alphaF() / a0 + bg.greenF() * bg.alphaF() * (1 - tmp.alphaF()) / a0;
            auto b0 = tmp.blueF() * tmp.alphaF() / a0 + bg.blueF() * bg.alphaF() * (1 - tmp.alphaF()) / a0;
            tmp = QColor(r0 * 255, g0 * 255, b0 * 255, a0 * 255);
        }
        if (m_drawShadow)
            m_image[1].setPixelColor(x, y, tmp);
        else
            m_image[0].setPixelColor(x, y, tmp);
    }
}

void AGRenderer::drawTriangle(int x0, int y0, qreal z0, qreal u0, qreal v0,
                              int x1, int y1, qreal z1, qreal u1, qreal v1,
                              int x2, int y2, qreal z2, qreal u2, qreal v2,
                              const std::array<QColor, 3> &litColors,
                              const AGTexture *texture,
                              const QColor &color)
{
    if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
        return;

    if (((y0 < 0)  && (y1 < 0)  && (y2 < 0))
            || ((y0 >= m_image[0].height()) && (y1 >= m_image[0].height()) && (y2 >= m_image[0].height()))
            || ((x0 < 0) && (x1 < 0) && (x2 < 0))
            || ((x0 >= m_image[0].width()) && (x1 >= m_image[0].width()) && (x2 >= m_image[0].width()))) {
        return;
    }

    int i0 = 0;
    int i1 = 1;
    int i2 = 2;

    if (texture) {
        u0 *= texture->width() - 1;
        u1 *= texture->width() - 1;
        u2 *= texture->width() - 1;

        v0 *= texture->height() - 1;
        v1 *= texture->height() - 1;
        v2 *= texture->height() - 1;
    }

    if (y1 < y0) {
        qSwap(x0, x1);
        qSwap(y0, y1);
        qSwap(u0, u1);
        qSwap(v0, v1);
        qSwap(z0, z1);
        qSwap(i0, i1);
    }

    if (y2 < y0) {
        qSwap(x0, x2);
        qSwap(y0, y2);
        qSwap(u0, u2);
        qSwap(v0, v2);
        qSwap(z0, z2);
        qSwap(i0, i2);
    }

    if (y2 < y1) {
        qSwap(x1, x2);
        qSwap(y1, y2);
        qSwap(u1, u2);
        qSwap(v1, v2);
        qSwap(z1, z2);
        qSwap(i1, i2);
    }

    u0 = u0 / z0;
    u1 = u1 / z1;
    u2 = u2 / z2;

    v0 = v0 / z0;
    v1 = v1 / z1;
    v2 = v2 / z2;

    z0 = 1.0f / z0;
    z1 = 1.0f / z1;
    z2 = 1.0f / z2;

    float r0;
    float g0;
    float b0;
    litColors[i0].getRgbF(&r0, &g0, &b0);

    float r1;
    float g1;
    float b1;
    litColors[i1].getRgbF(&r1, &g1, &b1);

    float r2;
    float g2;
    float b2;
    litColors[i2].getRgbF(&r2, &g2, &b2);

    qreal dyl = 0;
    qreal dxdyl = 0.0f;
    qreal dudyl = 0.0f;
    qreal dvdyl = 0.0f;
    qreal dzdyl = 0.0f;
    qreal drdyl = 0.0f;
    qreal dgdyl = 0.0f;
    qreal dbdyl = 0.0f;
    qreal dyr = 0;
    qreal dxdyr = 0.0f;
    qreal dudyr = 0.0f;
    qreal dvdyr = 0.0f;
    qreal dzdyr = 0.0f;
    qreal drdyr = 0.0f;
    qreal dgdyr = 0.0f;
    qreal dbdyr = 0.0f;
    qreal xl = 0.0f;
    qreal xr = 0.0f;
    qreal ul = 0.0f;
    qreal ur = 0.0f;
    qreal zl = 0.0f;
    qreal vl = 0.0f;
    qreal vr = 0.0f;
    qreal zr = 0.0f;
    qreal rl = 0.0f;
    qreal rr = 0.0f;
    qreal gl = 0.0f;
    qreal gr = 0.0f;
    qreal bl = 0.0f;
    qreal br = 0.0f;
    int ystart = 0;

    int yend = y2;
    if (yend > m_image[0].height() - 1)
        yend = m_image[0].height() - 1;

    int irestart = 0;
    int yrestart = y1;

    if (y1 < 0) {
        dyl = (y2 - y1);
        dxdyl = (x2 - x1) / dyl;
        dudyl = (u2 - u1) / dyl;
        dvdyl = (v2 - v1) / dyl;
        dzdyl = (z2 - z1) / dyl;
        drdyl = (r2 - r1) / dyl;
        dgdyl = (g2 - g1) / dyl;
        dbdyl = (b2 - b1) / dyl;
        dyr = (y2 - y0);
        dxdyr = ((x2 - x0)) / dyr;
        dudyr = ((u2 - u0)) / dyr;
        dvdyr = ((v2 - v0)) / dyr;
        dzdyr = ((z2 - z0)) / dyr;
        drdyr = ((r2 - r0)) / dyr;
        dgdyr = ((g2 - g0)) / dyr;
        dbdyr = ((b2 - b0)) / dyr;
        dyr = -y0;
        dyl = -y1;
        xl = dxdyl * dyl + qreal(x1);
        ul = dudyl * dyl + u1;
        vl = dvdyl * dyl + v1;
        zl = dzdyl * dyl + z1;
        rl = drdyl * dyl + r1;
        gl = dgdyl * dyl + g1;
        bl = dbdyl * dyl + b1;
        xr = dxdyr * dyr + qreal(x0);
        ur = dudyr * dyr + u0;
        vr = dvdyr * dyr + v0;
        zr = dzdyr * dyr + z0;
        rr = drdyr * dyr + r0;
        gr = dgdyr * dyr + g0;
        br = dbdyr * dyr + b0;
        ystart = 0;
        if (dxdyr > dxdyl) {
            qSwap(dxdyl, dxdyr);
            qSwap(dudyl, dudyr);
            qSwap(dvdyl, dvdyr);
            qSwap(dzdyl, dzdyr);
            qSwap(drdyl, drdyr);
            qSwap(dgdyl, dgdyr);
            qSwap(dbdyl, dbdyr);
            qSwap(xl, xr);
            qSwap(ul, ur);
            qSwap(vl, vr);
            qSwap(zl, zr);
            qSwap(rl, rr);
            qSwap(gl, gr);
            qSwap(bl, br);
            qSwap(x1, x2);
            qSwap(y1, y2);
            qSwap(u1, u2);
            qSwap(v1, v2);
            qSwap(z1, z2);
            qSwap(r1, r2);
            qSwap(g1, g2);
            qSwap(b1, b2);
            irestart = 1;
        }
    } else if (y0 < 0) {
        dyl = (y1 - y0);
        dxdyl = (x1 - x0) / dyl;
        dudyl = (u1 - u0) / dyl;
        dvdyl = (v1 - v0) / dyl;
        dzdyl = (z1 - z0) / dyl;
        drdyl = (r1 - r0) / dyl;
        dgdyl = (g1 - g0) / dyl;
        dbdyl = (b1 - b0) / dyl;
        dyr = (y2 - y0);
        dxdyr = (x2 - x0) / dyr;
        dudyr = (u2 - u0) / dyr;
        dvdyr = (v2 - v0) / dyr;
        dzdyr = (z2 - z0) / dyr;
        drdyr = (r2 - r0) / dyr;
        dgdyr = (g2 - g0) / dyr;
        dbdyr = (b2 - b0) / dyr;
        qreal dy = -y0;
        xl = dxdyl * dy + x0;
        ul = dudyl * dy + u0;
        vl = dvdyl * dy + v0;
        zl = dzdyl * dy + z0;
        rl = drdyl * dy + r0;
        gl = dgdyl * dy + g0;
        bl = dbdyl * dy + b0;
        xr = dxdyr * dy + x0;
        ur = dudyr * dy + u0;
        vr = dvdyr * dy + v0;
        zr = dzdyr * dy + z0;
        rr = drdyr * dy + r0;
        gr = dgdyr * dy + g0;
        br = dbdyr * dy + b0;
        ystart = 0;
        if (dxdyr < dxdyl) {
            qSwap(dxdyl, dxdyr);
            qSwap(dudyl, dudyr);
            qSwap(dvdyl, dvdyr);
            qSwap(dzdyl, dzdyr);
            qSwap(drdyl, drdyr);
            qSwap(dgdyl, dgdyr);
            qSwap(dbdyl, dbdyr);
            qSwap(xl, xr);
            qSwap(ul, ur);
            qSwap(vl, vr);
            qSwap(zl, zr);
            qSwap(rl, rr);
            qSwap(gl, gr);
            qSwap(bl, br);
            qSwap(x1, x2);
            qSwap(y1, y2);
            qSwap(u1, u2);
            qSwap(v1, v2);
            qSwap(z1, z2);
            qSwap(r1, r2);
            qSwap(g1, g2);
            qSwap(b1, b2);
            irestart = 1;
        }
    } else {
        dyl = (y1 - y0);
        dxdyl = (x1 - x0) / dyl;
        dudyl = (u1 - u0) / dyl;
        dvdyl = (v1 - v0) / dyl;
        dzdyl = (z1 - z0) / dyl;
        drdyl = (r1 - r0) / dyl;
        dgdyl = (g1 - g0) / dyl;
        dbdyl = (b1 - b0) / dyl;
        dyr = (y2 - y0);
        dxdyr = (x2 - x0) / dyr;
        dudyr = (u2 - u0) / dyr;
        dvdyr = (v2 - v0) / dyr;
        dzdyr = (z2 - z0) / dyr;
        drdyr = (r2 - r0) / dyr;
        dgdyr = (g2 - g0) / dyr;
        dbdyr = (b2 - b0) / dyr;
        xl = x0;
        xr = x0;
        ul = u0;
        ur = u0;
        zl = z0;
        vl = v0;
        vr = v0;
        zr = z0;
        rl = r0;
        rr = r0;
        gl = g0;
        gr = g0;
        bl = b0;
        br = b0;
        ystart = y0;
        if (dxdyr < dxdyl) {
            qSwap(dxdyl, dxdyr);
            qSwap(dudyl, dudyr);
            qSwap(dvdyl, dvdyr);
            qSwap(dzdyl, dzdyr);
            qSwap(drdyl, drdyr);
            qSwap(dgdyl, dgdyr);
            qSwap(dbdyl, dbdyr);
            qSwap(xl, xr);
            qSwap(ul, ur);
            qSwap(vl, vr);
            qSwap(zl, zr);
            qSwap(rl, rr);
            qSwap(gl, gr);
            qSwap(bl, br);
            qSwap(x1, x2);
            qSwap(y1, y2);
            qSwap(u1, u2);
            qSwap(v1, v2);
            qSwap(z1, z2);
            qSwap(r1, r2);
            qSwap(g1, g2);
            qSwap(b1, b2);
            irestart = 1;
        }
    }

    if (x0 < 0 || x0 >= m_image[0].width() - 1 ||
        x1 < 0 || x1 >= m_image[0].width() - 1 ||
        x2 < 0 || x2 >= m_image[0].width() - 1) {
        for (int yi = ystart; yi <= yend; ++yi) {
            int xstart = xl;
            int xend = xr + 0.5;
            qreal ui = ul;
            qreal vi = vl;
            qreal zi = zl;
            qreal ri = rl;
            qreal gi = gl;
            qreal bi = bl;
            qreal dx = xend - xstart;
            qreal du = 0;
            qreal dv = 0;
            qreal dz = 0;
            qreal dr = 0;
            qreal dg = 0;
            qreal db = 0;
            if (dx > 0) {
                du = (ur - ul) / dx;
                dv = (vr - vl) / dx;
                dz = (zr - zl) / dx;
                dr = (rr - rl) / dx;
                dg = (gr - gl) / dx;
                db = (br - bl) / dx;
            } else {
                du = (ur - ul);
                dv = (vr - vl);
                dz = (zr - zl);
                dr = (rr - rl);
                dg = (gr - gl);
                db = (br - bl);
            }
            if (xstart < 0) {
                dx = - xstart;
                ui += dx * du;
                vi += dx * dv;
                zi += dx * dz;
                ri += dx * dr;
                gi += dx * dg;
                bi += dx * db;
                xstart = 0;
            }
            if (xend > m_image[0].width() - 1)
                xend = m_image[0].width() - 1;
            for (int xi = xstart; xi < xend; ++xi) {
                drawPixel(xi, yi, ui, vi, zi, ri, gi, bi, color, texture);
                ui += du;
                vi += dv;
                zi += dz;
                ri += dr;
                gi += dg;
                bi += db;
            }
            xl += dxdyl;
            xr += dxdyr;
            ul += dudyl;
            ur += dudyr;
            vl += dvdyl;
            vr += dvdyr;
            zl += dzdyl;
            zr += dzdyr;
            rl += drdyl;
            rr += drdyr;
            gl += dgdyl;
            gr += dgdyr;
            bl += dbdyl;
            br += dbdyr;
            if (yi == yrestart) {
                if (irestart == 0) {
                    dyl = (y2 - y1);
                    dxdyl = (x2 - x1) / dyl;
                    dudyl = (u2 - u1) / dyl;
                    dvdyl = (v2 - v1) / dyl;
                    dzdyl = (z2 - z1) / dyl;
                    drdyl = (r2 - r1) / dyl;
                    dgdyl = (g2 - g1) / dyl;
                    dbdyl = (b2 - b1) / dyl;
                    xl = x1;
                    ul = u1;
                    vl = v1;
                    zl = z1;
                    rl = r1;
                    gl = g1;
                    bl = b1;
                    xl += dxdyl;
                    ul += dudyl;
                    vl += dvdyl;
                    zl += dzdyl;
                    rl += drdyl;
                    gl += dgdyl;
                    bl += dbdyl;
                } else {
                    dyr = (y1 - y2);
                    dxdyr = (x1 - x2) / dyr;
                    dudyr = (u1 - u2) / dyr;
                    dvdyr = (v1 - v2) / dyr;
                    dzdyr = (z1 - z2) / dyr;
                    drdyr = (r1 - r2) / dyr;
                    dgdyr = (g1 - g2) / dyr;
                    dbdyr = (b1 - b2) / dyr;
                    xr = x2;
                    ur = u2;
                    vr = v2;
                    zr = z2;
                    rr = r2;
                    gr = g2;
                    br = b2;
                    xr += dxdyr;
                    ur += dudyr;
                    vr += dvdyr;
                    zr += dzdyr;
                    rr += drdyr;
                    gr += dgdyr;
                    br += dbdyr;
                }
            }
        }
    } else {
        for (int yi = ystart; yi <= yend; ++yi) {
            int xstart = xl;
            int xend = xr + 0.5;
            qreal ui = ul;
            qreal vi = vl;
            qreal zi = zl;
            qreal ri = rl;
            qreal gi = gl;
            qreal bi = bl;
            qreal dx = xend - xstart;
            qreal du = 0;
            qreal dv = 0;
            qreal dz = 0;
            qreal dr = 0;
            qreal dg = 0;
            qreal db = 0;
            if (dx > 0) {
                du = (ur - ul) / dx;
                dv = (vr - vl) / dx;
                dz = (zr - zl) / dx;
                dr = (rr - rl) / dx;
                dg = (gr - gl) / dx;
                db = (br - bl) / dx;
            } else {
                du = (ur - ul);
                dv = (vr - vl);
                dz = (zr - zl);
                dr = (rr - rl);
                dg = (gr - gl);
                db = (br - bl);
            }
            for (int xi = xstart; xi < xend; ++xi) {
                drawPixel(xi, yi, ui, vi, zi, ri, gi, bi, color, texture);
                ui += du;
                vi += dv;
                zi += dz;
                ri += dr;
                gi += dg;
                bi += db;
            }
            xl += dxdyl;
            xr += dxdyr;
            ul += dudyl;
            ur += dudyr;
            vl += dvdyl;
            vr += dvdyr;
            zl += dzdyl;
            zr += dzdyr;
            rl += drdyl;
            rr += drdyr;
            gl += dgdyl;
            gr += dgdyr;
            bl += dbdyl;
            br += dbdyr;
            if (yi == yrestart) {
                if (irestart == 0) {
                    dyl = (y2 - y1);
                    dxdyl = (x2 - x1) / dyl;
                    dudyl = (u2 - u1) / dyl;
                    dvdyl = (v2 - v1) / dyl;
                    dzdyl = (z2 - z1) / dyl;
                    drdyl = (r2 - r1) / dyl;
                    dgdyl = (g2 - g1) / dyl;
                    dbdyl = (b2 - b1) / dyl;
                    xl = x1;
                    ul = u1;
                    vl = v1;
                    zl = z1;
                    rl = r1;
                    gl = g1;
                    bl = b1;
                    xl += dxdyl;
                    ul += dudyl;
                    vl += dvdyl;
                    zl += dzdyl;
                    rl += drdyl;
                    gl += dgdyl;
                    bl += dbdyl;
                } else {
                    dyr = (y1 - y2);
                    dxdyr = (x1 - x2) / dyr;
                    dudyr = (u1 - u2) / dyr;
                    dvdyr = (v1 - v2) / dyr;
                    dzdyr = (z1 - z2) / dyr;
                    drdyr = (r1 - r2) / dyr;
                    dgdyr = (g1 - g2) / dyr;
                    dbdyr = (b1 - b2) / dyr;
                    xr = x2;
                    ur = u2;
                    vr = v2;
                    zr = z2;
                    rr = r2;
                    gr = g2;
                    br = b2;
                    xr += dxdyr;
                    ur += dudyr;
                    vr += dvdyr;
                    zr += dzdyr;
                    rr += drdyr;
                    gr += dgdyr;
                    br += dbdyr;
                }
            }
        }
    }
}

void AGRenderer::beginDrawShadow()
{
    m_drawShadow = true;
    m_image[1] = m_image[0];
}

void AGRenderer::display(QPainter *painter, const QRectF &rect)
{
    if (m_image[1].isNull())
        return;

    const auto &scaledImage = m_image[1].scaled(
            rect.width() - 0.5, rect.height() - 0.5, Qt::KeepAspectRatioByExpanding);
    const auto x = 0.5 + (rect.width() - scaledImage.width()) / 2.0;
    const auto y = 0.5 + (rect.height() - scaledImage.height()) / 2.0;
    painter->drawImage(x, y, scaledImage);
    clear();
}

void AGRenderer::clear()
{
    m_image[0].fill(0);
    m_zBuffer.fill(0);
    m_drawShadow = false;
}
