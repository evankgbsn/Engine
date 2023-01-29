#include "TransformTrack.h"

TransformTrack::TransformTrack() :
	id(),
	position(),
	rotation(),
	scale()
{
}

TransformTrack::~TransformTrack()
{
}

unsigned int TransformTrack::GetId() const
{
	return 0;
}

void TransformTrack::SetId(const unsigned int& newId)
{
}

VectorTrack& TransformTrack::GetPositionTrack()
{
	return position;
}

QuaternionTrack& TransformTrack::GetRotationTrack()
{
	return rotation;
}

VectorTrack& TransformTrack::GetScaleTrack()
{
	return scale;
}

float TransformTrack::GetStartTime() const
{
	return 0.0f;
}

float TransformTrack::GetEndTime() const
{
	return 0.0f;
}

bool TransformTrack::IsValid() const
{
	return false;
}

Math::Transform TransformTrack::Sample(const Math::Transform& refTransform, float time, bool isLooping)
{
	Math::Transform result = refTransform;

	if (position.Size() > 1)
	{
		result.Position() = position.Sample(time, isLooping);
	}

	if (rotation.Size() > 1)
	{
		result.Rotation() = rotation.Sample(time, isLooping);
	}

	if (scale.Size() > 1)
	{
		result.Scale() = scale.Sample(time, isLooping);
	}

	return result;
}
