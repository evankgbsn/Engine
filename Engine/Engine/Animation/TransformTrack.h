#ifndef TRANSFORMTRACK_H
#define TRANSFORMTRACK_H

#include "Track.h"
#include "../Math/Transform.h"

class TransformTrack
{

public:

	TransformTrack();

	~TransformTrack();

	TransformTrack(const TransformTrack&) = default;

	TransformTrack& operator=(const TransformTrack&) = default;

	TransformTrack(TransformTrack&&) = default;

	TransformTrack& operator=(TransformTrack&&) = default;

	unsigned int GetId() const;

	void SetId(const unsigned int& newId);

	VectorTrack& GetPositionTrack();

	QuaternionTrack& GetRotationTrack();

	VectorTrack& GetScaleTrack();

	float GetStartTime() const;

	float GetEndTime() const;

	bool IsValid() const;

	Math::Transform Sample(const Math::Transform& transform, float time, bool isLooping);

private:

	unsigned int id;

	VectorTrack position;
	
	QuaternionTrack rotation;
	
	VectorTrack scale;

};

#endif // TRANSFORMTRACK_H