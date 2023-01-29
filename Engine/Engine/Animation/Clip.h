#ifndef CLIP_H
#define CLIP_H

#include "TransformTrack.h"

#include <vector>
#include <string>

class Pose;

class Clip
{

public:

	Clip();

	~Clip();

	Clip(const Clip&) = default;

	Clip& operator=(const Clip&) = default;

	Clip(Clip&&) = delete;

	Clip& operator=(Clip&&) = default;

	TransformTrack& operator[](unsigned int joint);

	unsigned int GetIdAtIndex(unsigned int index) const;

	void SetIdAtIndex(unsigned int index, unsigned int id);

	unsigned int Size() const;

	float Sample(Pose& outPose, float time);

	void RecalculateDuration();

	const std::string& GetName() const;

	void SetName(const std::string& name);

	float GetDuration() const;

	float GetStartTime() const;

	float GetEndTime() const;

	bool IsLooping() const;

	void SetIsLooping(const bool& loop);

	float AdjustTimeToFitRange(float time) const;

private:

	std::string name;

	std::vector<TransformTrack> tracks;

	float startTime;

	float endTime;

	bool isLooping;
};


#endif // CLIP_H