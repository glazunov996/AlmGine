#pragma once

#include <QQuickItem>
#include <QQmlParserStatus>
#include <QVector3D>
#include <QMatrix4x4>

#include "agutils.h"

class AGViewport;

class AGObject: public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QVector3D pivot READ pivot WRITE setPivot NOTIFY pivotChanged)
    Q_PROPERTY(QVector3D forward READ forward NOTIFY forwardChanged)
    Q_PROPERTY(QVector3D right READ right NOTIFY rightChanged)
    Q_PROPERTY(QVector3D up READ up NOTIFY upChanged)
    Q_INTERFACES(QQmlParserStatus)

public:
    AGObject(QObject *parent = nullptr);

    bool visible() const;
    QVector3D position() const;
    QVector3D rotation() const;
    QVector3D scale() const;
    QVector3D pivot() const;
    QVector3D forward() const;
    QVector3D right() const;
    QVector3D up() const;
    QMatrix4x4 worldMatrix() const;

    Q_INVOKABLE QVector4D mapToWorld(const QVector4D &point) const;
    Q_INVOKABLE QVector3D mapDirectionToWorld(const QVector3D &dir) const;
    Q_INVOKABLE QVector3D mapDirectionFromWorld(const QVector3D &dir) const;

    virtual void updateObject(AGViewport *viewport);
    bool calculateWorldMatrix();

public Q_SLOTS:
    void setVisible(bool visible);
    void setPosition(const QVector3D &position);
    void setRotation(const QVector3D &rotation);
    void setScale(const QVector3D &scale);
    void setPivot(const QVector3D &pivot);

Q_SIGNALS:
    void visibleChanged();
    void positionChanged();
    void rotationChanged();
    void scaleChanged();
    void pivotChanged();
    void forwardChanged();
    void rightChanged();
    void upChanged();

protected:
    void classBegin() override;
    void componentComplete() override;
    bool isComponentComplete() const;

public:
    bool m_visible = true;
    QVector3D m_position;
    Rotation m_rotation;
    QVector3D m_scale;
    QVector3D m_pivot;
    QVector3D m_forward;
    QVector3D m_right;
    QVector3D m_up;
    QMatrix4x4 m_worldMatrix;
    bool m_worldMatrixDirty = true;
    bool m_componentComplete = false;
};
