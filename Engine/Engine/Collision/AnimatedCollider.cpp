#include "AnimatedCollider.h"

#include "OrientedBoundingBoxWithVisualization.h"
#include "SphereWithVisualization.h"
#include "../Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/Model/Vertex.h"
#include "../Animation/Armature.h"

AnimatedCollider::AnimatedCollider(TexturedAnimatedGraphicsObject* const graphicsObject) :
	wrapedGraphics(graphicsObject)
{
	InitializeSphere();
	InitializeOBBs();
}

AnimatedCollider::~AnimatedCollider()
{
	for (const OrientedBoundingBoxWithVisualization* const obb : obbs)
	{
		delete obb;
	}
}

void AnimatedCollider::InitializeOBBs()
{
	std::unordered_map<std::string, std::vector<Vertex>> jointsVertices;

	const Armature* const arms = wrapedGraphics->GetModel()->GetArmature();

	const std::vector<std::string>& jointNames = arms->GetJointNames();

	const std::vector<Vertex>& vertices = wrapedGraphics->GetModel()->GetVertices();

	for (const Vertex& vert : vertices)
	{
		unsigned int jointWithMostInfluence = 0;
		float influence = vert.GetWeights()[jointWithMostInfluence];
		for (unsigned int i = 1; i < 4; ++i)
		{
			if (vert.GetWeights()[i] > influence)
			{
				jointWithMostInfluence = i;
				influence = vert.GetWeights()[i];
			}
		}

		jointsVertices[jointNames[vert.GetInfluences()[jointWithMostInfluence]]].push_back(vert);
	}

	obbs.resize(jointNames.size());

	for (const std::pair<std::string, std::vector<Vertex>>& jointVerts : jointsVertices)
	{
		OrientedBoundingBoxWithVisualization* newObb = new OrientedBoundingBoxWithVisualization(jointVerts.second);

		for (unsigned int i = 0; i < jointNames.size(); ++i)
		{
			if (jointNames[i] == jointVerts.first)
			{
				obbs[i] = newObb;
			}
		}
	}
}

void AnimatedCollider::InitializeSphere()
{
	sphere = new SphereWithVisualization(wrapedGraphics);
}

void AnimatedCollider::ToggleVisibility()
{
	for (const auto& obb : obbs)
	{
		if(obb != nullptr)
			obb->ToggleVisibility();
	}
}

void AnimatedCollider::Update()
{
	unsigned int i = 0;
	for (OrientedBoundingBoxWithVisualization* const obb : obbs)
	{
		if(obb!= nullptr)
			obb->Update(wrapedGraphics->GetTransform() * wrapedGraphics->GetAnimPoseArray()[i] * wrapedGraphics->GetAnimInvBindPoseArray()[i]);
		i++;
	}

	sphere->Update(wrapedGraphics->GetTransform());
}

bool AnimatedCollider::Intersect(const OrientedBoundingBox& other) const
{
	bool intersect = false;
	for (OrientedBoundingBoxWithVisualization* const obb : obbs)
	{
		if (obb != nullptr)
		{
			if (obb->OrientedBoundingBoxIntersect(other))
			{
				obb->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
				intersect = true;
			}
			else
			{
				obb->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			}
			
		}
	}

	return intersect;
}
