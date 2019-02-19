#ifndef MODELINFO_H
#define MODELINFO_H

#include <QMatrix4x4>
#include <QSharedPointer>
#include <QString>
#include <QVector3D>
#include <QVector4D>

namespace Model
{
    typedef struct Material
    {
        QString name;
        QVector3D ambient;
        QVector3D diffuse;
        QVector3D specular;
        float shine;
    } Material;

    typedef struct Light
    {
        QVector4D position;
        QVector3D intensity;
    } Light;

    typedef struct Mesh
    {
        QString name;
        unsigned int indexCount;
        unsigned int indexOffset;
        QSharedPointer<Material> material;
    } Mesh;

    typedef struct Node
    {
        QString name;
        QMatrix4x4 transformMatrix;
        QVector<QSharedPointer<Mesh>> meshes;
        QVector<Node> children;
    } Node;
}

#endif
