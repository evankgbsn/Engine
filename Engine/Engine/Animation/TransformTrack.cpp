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
	return id;
}

void TransformTrack::SetId(const unsigned int& newId)
{
	id = newId;
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
	float result = 0.0f;
	bool isSet = false;

	if (position.Size() > 1)
	{
		result = position.GetStartTime();
		isSet = true;
	}

	if (rotation.Size() > 1)
	{
		float rotationStart = rotation.GetStartTime();
		if (rotationStart < result || !isSet)
		{
			result = rotationStart;
			isSet = true;
		}
	}

	if (scale.Size() > 1)
	{
		float scaleStart = scale.GetStartTime();
		if (scaleStart < result || !isSet)
		{
			result = scaleStart;
			isSet = true;
		}
	}

	return result;
}

float TransformTrack::GetEndTime() const
{
	float result = 0.0f;
	bool isSet = false;

	if (position.Size() > 1)
	{
		result = position.GetEndTime();
		isSet = true;
	}

	if (rotation.Size() > 1)
	{
		float rotationStart = rotation.GetEndTime();
		if (rotationStart > result || !isSet)
		{
			result = rotationStart;
			isSet = true;
		}
	}

	if (scale.Size() > 1)
	{
		float scaleStart = scale.GetEndTime();
		if (scaleStart > result || !isSet)
		{
			result = scaleStart;
			isSet = true;
		}
	}

	return result;
}

bool TransformTrack::IsValid() const
{
	return position.Size() > 1 || rotation.Size() > 1 || scale.Size() > 1;
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
