#ifndef MODELIMPORTER_H
#define MODELIMPORTER_H

#include "modelinfo.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>

#include <assimp/light.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>

class ModelImporter
{
    public:
        ModelImporter();
        bool Load(QString filepath);
        void getBufferData(QVector<float>** vertices, QVector<float>** normals, QVector<uint>** indices);
        QSharedPointer<Node> getRoot();

    private:
        void loadMaterials(const aiScene* scene);
        void loadMeshes(const aiScene* scene);
        void loadRootNode(const aiScene* scene);

        QSharedPointer<Material> processMaterial(aiMaterial* material);
        QSharedPointer<Mesh> processMesh(aiMesh* mesh);
        void processNode(const aiScene* scene, aiNode* node, Node* parent, Node& newNode);

        QVector<float> vertices;
        QVector<float> normals;
        QVector<uint> indices;

        QVector<QSharedPointer<Material>> materials;
        QVector<QSharedPointer<Light>> lights;
        QVector<QSharedPointer<Mesh>> meshes;
        QSharedPointer<Node> rootNode;
};

#endif
