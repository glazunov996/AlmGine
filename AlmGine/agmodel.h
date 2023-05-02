#pragma once

#include <QUrl>
#include <QString>
#include <QSharedPointer>
#include <QHash>

#include "agobject.h"
#include "agmesh.h"
#include "agmaterial.h"

class AGViewport;
class AGLight;
class AGCamera;

class AGModel : public AGObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(AGMesh::Attributes attributes READ attributes WRITE setAttributes NOTIFY attributesChanged)
    Q_PROPERTY(bool cullBackface READ cullBackface WRITE setCullBackface NOTIFY cullBackfaceChanged)
    Q_PROPERTY(QQmlListProperty<AGMaterial> materials READ materials CONSTANT)
    Q_PROPERTY(bool castsShadow READ castsShadow WRITE setCastsShadow NOTIFY castsShadowChanged)
    Q_PROPERTY(qreal frameValue READ frameValue WRITE setFrameValue NOTIFY frameValueChanged)

public:
    AGModel(QObject *parent = nullptr);

    QUrl source() const;
    AGMesh::Attributes attributes() const;
    bool cullBackface() const;
    QQmlListProperty<AGMaterial> materials();
    bool castsShadow() const;
    QSharedPointer<AGMesh> mesh() const;
    qreal frameValue() const;

    void updateObject(AGViewport *viewport) override;

public Q_SLOTS:
    void setSource(const QUrl &source);
    void setAttributes(AGMesh::Attributes attributes);
    void setCullBackface(bool enable);
    void setCastsShadow(bool castsShadow);
    void setFrameValue(qreal frameValue);

Q_SIGNALS:
    void sourceChanged();
    void attributesChanged();
    void cullBackfaceChanged();
    void castsShadowChanged();
    void frameValueChanged();

protected:
    virtual void updateWorldVertices(QList<QVector4D> &vertices, AGViewport *viewport);

private:
    QString translateMeshSource(QObject *contextObject);
    bool isBackfaceCulled(AGCamera *camera, const QVector3D &normal, const QVector3D &vertex) const;
    void clipTriangle(AGCamera *camera, const QSharedPointer<AGTriangle> &triangle);
    void lightTriangle(const QVector3D &normal, const QVector3D &firstVertex, const QList<AGLight *> &lights, const QSharedPointer<AGTriangle> &triangle);
    void castShadow(const QList<AGLight *> &lights);
    void loadMeshVertices();
    static void qmlAppendMaterial(QQmlListProperty<AGMaterial> *list, AGMaterial *material);
    static AGMaterial *qmlMaterialAt(QQmlListProperty<AGMaterial> *list, qsizetype index);
    static qsizetype qmlMaterialsCount(QQmlListProperty<AGMaterial> *list);
    static void qmlClearMaterials(QQmlListProperty<AGMaterial> *list);

private:
    QUrl m_source;
    AGMesh::Attributes m_attributes;
    bool m_cullBackface = true;
    QList<AGMaterial *> m_materials;
    QSharedPointer<AGMesh> m_mesh;
    bool m_meshDirty = false;
    bool m_castsShadow = false;
    AGMaterial m_shadowMaterial;
    QList<QVector4D> m_worldVertices;
    QList<QSharedPointer<AGTriangle>> m_worldTriangles;
    QList<QSharedPointer<AGTriangle>> m_dirtyTriangles;
    qreal m_frameValue = 0.0;
};
