#include "PerfectAnimatedCollisionVolume.h"

#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/ColoredAnimatedGraphicsObject.h"
#include "../Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"
#include "../Renderer/Model/Model.h"
#include "../Animation/Armature.h"
#include "BoundingSphere.h"


PerfectAnimatedCollisionVolume::PerfectAnimatedCollisionVolume(std::function<void(Entity* owner)> callback, Entity* owner, TexturedAnimatedGraphicsObject* obj) :
    CollisionVolume(callback, owner),
    animatedGraphicsObject(obj),
    jointSpheres(std::unordered_map<std::string, BoundingSphere*>())
{
    GraphicsObjectManager::CreateColoredAnimatedGraphicsObject(obj->GetModel(), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), [this](ColoredAnimatedGraphicsObject* go)
        {
            graphics = go;
            GraphicsObjectManager::WireFrame(graphics, ObjectTypes::GraphicsObjectType::ColoredAnimated);
            graphics->SetClip(animatedGraphicsObject->GetClip());
            Initialize(graphics->GetModel()->GetVertices(), glm::mat4(1.0f), graphics->GetModel());
        });
}

PerfectAnimatedCollisionVolume::~PerfectAnimatedCollisionVolume()
{
}

bool PerfectAnimatedCollisionVolume::Intersect(const CollisionVolume& other) const
{
    return false;
}

bool PerfectAnimatedCollisionVolume::Intersect(const BoundingSphere& other) const
{
    return false;
}

bool PerfectAnimatedCollisionVolume::Intersect(const AxisAlignedBoundingBox& other) const
{
    return false;
}

bool PerfectAnimatedCollisionVolume::Intersect(const OrientedBoundingBox& other) const
{
    return false;
}

void PerfectAnimatedCollisionVolume::Initialize(const std::vector<Vertex, std::allocator<Vertex>>& vertices, const glm::mat4&, const Model* const model)
{
    const std::vector<Vertex>& verts = model->GetVertices();
    const std::vector<unsigned int> indices = model->GetIndices();

    auto jointAssociation = [](const Vertex& v) -> unsigned int
        {
            const glm::ivec4& joints = v.GetInfluences();
            const glm::vec4& weights = v.GetWeights();

            unsigned int jointIndex = 0;
            float max = 0.0f;
            unsigned int j = 0;
            for (j; j < 4; j++)
            {
                if (weights[j] > max)
                {
                    jointIndex = j;
                    max = weights[j];
                }
            }

            return joints[jointIndex];
        };

    auto triangleJointAssociation = [jointAssociation]( const Vertex& p1, const Vertex& p2, const Vertex& p3) -> unsigned int
        {
            unsigned int joint1 = jointAssociation(p1);
            unsigned int joint2 = jointAssociation(p2);
            unsigned int joint3 = jointAssociation(p3);

            if (joint1 == joint2)
                return joint1;

            if (joint1 == joint3)
                return joint1;

            if (joint2 == joint3)
                return joint2;

            return joint1;
        };

    const std::vector<std::string>& jointNames = model->GetArmature()->GetJointNames();

    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
       // Find the joint associated with this triangle.
        const Vertex& p1 = verts[indices[i]];
        const Vertex& p2 = verts[indices[i + 1]];
        const Vertex& p3 = verts[indices[i + 2]];

        unsigned int joint = triangleJointAssociation(p1, p2, p3);

        trianglesOfJoints[jointNames[joint]].push_back(std::vector<const Vertex*>({ &p1, &p2, &p3 }));
    }

    for (const std::pair<std::string, std::vector<std::vector<const Vertex*>>>& jointTriangles : trianglesOfJoints)
    {
        std::vector<Vertex> jointVertices;
        for (const std::vector<const Vertex*>& vertices : jointTriangles.second)
        {
            for (const Vertex* vert : vertices)
            {
                jointVertices.push_back(*vert);
            }
        }

        //jointSpheres[jointTriangles.first] = new BoundingSphere([](Entity* o) {}, owningEntity, jointVertices, glm::mat4(1.0f));
    }

    SetClip(4);
}

void PerfectAnimatedCollisionVolume::ComputeData(const std::vector<Vertex>& verts, const glm::mat4& transform)
{
    if (graphics && animatedGraphicsObject)
    {
        const glm::mat4& newTransform = const_cast<TexturedAnimatedGraphicsObject*>(animatedGraphicsObject)->GetTransform();
        const Model* const model = graphics->GetModel();
        const glm::mat4* animPoseArray = graphics->GetAnimPoseArray();
        const glm::mat4* animInvPoseArray = graphics->GetAnimInvBindPoseArray();
        const std::vector<std::string>& jointNames = model->GetArmature()->GetJointNames();

        graphics->SetTransform(newTransform);

        unsigned int index = 0;
        for (const std::pair<std::string, BoundingSphere*>& sphere : jointSpheres)
        {
            unsigned int jointNameIndex = 0;
            for (unsigned int i = 0; i < jointNames.size(); i++)
            {
                if (sphere.first == jointNames[i])
                    jointNameIndex = i;
            }

            if (sphere.second != nullptr)
                sphere.second->ComputeData(model->GetVertices(), newTransform * animPoseArray[jointNameIndex] * animInvPoseArray[jointNameIndex]);
        }
    }
}

void PerfectAnimatedCollisionVolume::SetClip(unsigned int clipIndex)
{
    if(graphics != nullptr)
        graphics->SetClip(clipIndex);
}

void PerfectAnimatedCollisionVolume::SetAnimationSpeed(float newSpeed)
{
    if (graphics != nullptr)
        graphics->SetAnimationSpeed(newSpeed);
}

void PerfectAnimatedCollisionVolume::SetColor(const glm::vec4 newColor)
{
    graphics->SetColor(newColor);
}

const glm::vec4& PerfectAnimatedCollisionVolume::GetColor() const
{
    return graphics->GetColor();
}

