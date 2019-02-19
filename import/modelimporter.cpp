#include "modelimporter.h"

#include <QDebug>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

namespace Model
{
    ModelImporter::ModelImporter() { }

    bool ModelImporter::Load(QString filepath)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filepath.toStdString(),
             aiProcess_GenSmoothNormals |
             aiProcess_CalcTangentSpace |
             aiProcess_Triangulate |
             aiProcess_JoinIdenticalVertices |
             aiProcess_SortByPType
        );

        if(scene == nullptr)
        {
           qDebug() << "Error loading model: " << importer.GetErrorString();
           return false;
        }

        if(scene->HasMaterials()) loadMaterials(scene);

        if(scene->HasMeshes()) loadMeshes(scene);
        else
        {
            qDebug() << "Error loading model: no meshes found";
            return false;
        }

        if(scene->mRootNode != nullptr) loadRootNode(scene);
        else
        {
            qDebug() << "Error loading model: no root node found";
            return false;
        }

        return true;
    }

    void ModelImporter::loadMaterials(const aiScene* scene)
    {
        for(uint i = 0; i < scene->mNumMaterials; i++)
        {
            auto material = processMaterial(scene->mMaterials[i]);
            this->materials.push_back(material);
        }
    }

    void ModelImporter::loadMeshes(const aiScene* scene)
    {
        for(uint i = 0; i < scene->mNumMeshes; i++)
        {
            auto mesh = processMesh(scene->mMeshes[i]);
            this->meshes.push_back(mesh);
        }
    }

    void ModelImporter::loadRootNode(const aiScene* scene)
    {
        Node* root = new Node;
        processNode(scene, scene->mRootNode, nullptr, *root);
        this->rootNode.reset(root);
    }

    QSharedPointer<Material> ModelImporter::processMaterial(aiMaterial* material)
    {
        QSharedPointer<Material> materialData(new Material);

        //Получаем название
        aiString mName;
        material->Get(AI_MATKEY_NAME, mName);
        materialData->name = mName.length > 0 ? mName.C_Str() : "";

        int shadingModel;
        material->Get(AI_MATKEY_SHADING_MODEL, shadingModel);

        aiColor3D amb(0.f,0.f,0.f);
        aiColor3D dif(0.f,0.f,0.f);
        aiColor3D spec(0.f,0.f,0.f);
        float shine = 0.0;

        material->Get(AI_MATKEY_COLOR_AMBIENT, amb);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, dif);
        material->Get(AI_MATKEY_COLOR_SPECULAR, spec);
        material->Get(AI_MATKEY_SHININESS, shine);

        materialData->ambient = QVector3D(amb.r, amb.g, amb.b);
        materialData->diffuse = QVector3D(dif.r, dif.g, dif.b);
        materialData->specular = QVector3D(spec.r, spec.g, spec.b);
        materialData->shine = shine;

        materialData->ambient *= .2;
        if (materialData->shine == 0.0) materialData->shine = 30;

        return materialData;
    }

    QSharedPointer<Mesh> ModelImporter::processMesh(aiMesh* mesh)
    {
        QSharedPointer<Mesh> newMesh(new Mesh);
        newMesh->name = mesh->mName.length > 0 ? mesh->mName.C_Str() : "";

        newMesh->indexOffset = indices.size();
        unsigned int indexCountBefore = indices.size();
        int vertindexoffset = vertices.size()/3;

        // Get Vertices
        if (mesh->mNumVertices > 0)
        {
            for (uint i = 0; i < mesh->mNumVertices; i++)
            {
                aiVector3D& vec = mesh->mVertices[i];

                vertices.push_back(vec.x);
                vertices.push_back(vec.y);
                vertices.push_back(vec.z);
            }
        }

        // Get Normals
        if (mesh->HasNormals())
        {
            for (uint i = 0; i < mesh->mNumVertices; i++)
            {
                aiVector3D& vec = mesh->mNormals[i];
                normals.push_back(vec.x);
                normals.push_back(vec.y);
                normals.push_back(vec.z);
            };
        }

        // Get mesh indexes
        for (uint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace* face = &mesh->mFaces[i];
            if (face->mNumIndices != 3)
            {
                qDebug() << "Warning: Mesh face with not exactly 3 indices, ignoring this primitive.";
                continue;
            }

            indices.push_back(face->mIndices[0] + vertindexoffset);
            indices.push_back(face->mIndices[1] + vertindexoffset);
            indices.push_back(face->mIndices[2] + vertindexoffset);
        }

        newMesh->indexCount = indices.size() - indexCountBefore;
        newMesh->material = materials.at(mesh->mMaterialIndex);

        return newMesh;
    }

    void ModelImporter::processNode(const aiScene* scene, aiNode* node, Node* parent, Node& newNode)
    {
        newNode.name = node->mName.length > 0 ? node->mName.C_Str() : "";
        newNode.transformMatrix = QMatrix4x4(node->mTransformation[0]);

        newNode.meshes.resize(node->mNumMeshes);
        for(uint i = 0; i < node->mNumMeshes; i++)
        {
            uint meshIndex = node->mMeshes[i];
            newNode.meshes[i] = meshes[meshIndex];
        }

        for(uint i = 0; i < node->mNumChildren; i++)
        {
            newNode.children.push_back(Node());
            processNode(scene, node->mChildren[i], parent, newNode.children[i]);
        }
    }
}
