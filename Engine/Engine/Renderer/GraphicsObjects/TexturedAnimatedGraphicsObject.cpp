#include "TexturedAnimatedGraphicsObject.h"

#include "../../Animation/Clip.h"
#include "../../Animation/Armature.h"
#include "../Model/Model.h"
#include "../../Time/TimeManager.h"
#include "../../Utils/Logger.h"
#include "../Cameras/CameraManager.h"
#include "../Memory/UniformBuffer.h"
#include "../Images/Texture.h"
#include "../../Animation/Animation.h"

#include <vector>

TexturedAnimatedGraphicsObject::TexturedAnimatedGraphicsObject(Model* const m, Texture* const tex) :
	GraphicsObject(m),
	texture(tex),
	animation(new Animation(model->GetBakedAnimation(0)))
{
	shaderName = "TexturedAnimated";
	InitializeDescriptorSets();
}

TexturedAnimatedGraphicsObject::~TexturedAnimatedGraphicsObject()
{
}

void TexturedAnimatedGraphicsObject::CreateTextures()
{
	textures.push_back(texture);
	texture->SetBinding(1U);
}

void TexturedAnimatedGraphicsObject::CreateUniformBuffers()
{
	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);
}

void TexturedAnimatedGraphicsObject::Update()
{
	mvp.model = glm::mat4(1.0f);

	const Camera& cam = CameraManager::GetCamera("MainCamera");

	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.projection[1][1] *= -1;

	for (unsigned int i = 0; i < model->GetArmature()->GetInvBindPose().size(); i++)
	{
		mvp.invBindPose[i] = model->GetArmature()->GetInvBindPose()[i];
	}

	animation->Update(mvp.pose);

	uniformBuffers[0]->SetData(&mvp);
}

bool TexturedAnimatedGraphicsObject::ToggleLoopAnimation()
{
	return false;
}

void TexturedAnimatedGraphicsObject::SetAnimationSpeed(float animationSpeed)
{
	
}

void TexturedAnimatedGraphicsObject::SetClip(unsigned int clipIndex)
{

}
