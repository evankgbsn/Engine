#ifndef TRACK_H
#define TRACK_H

#include "Frame.h"

#include <vector>
#include <glm/glm.hpp>

template<typename T, size_t N>
class Track
{

public:

	Track();

	Track(const Interpolation& interpolationType);

	~Track();

	Track(const Track&) = default;

	Track& operator=(const Track&) = default;

	Track(Track&&) = default;

	Track& operator=(Track&&) = default;

	Frame<N>& operator[](unsigned int index);

	void SetInterpolation(Interpolation newInterpolation);

	Interpolation GetInterpolation() const;

	unsigned int Size() const;

	void SetSize(unsigned int newSize);

	const float& GetStartTime() const;

	const float& GetEndTime() const;

	T Sample(float time, bool isLooping);

	T Hermite(float time, const T& p0, const T& p1, const T& t0, const T& t1) const;

	unsigned int FrameIndex(float time, bool isLooping) const;

	float AdjustTimeToFitTrack(float time, float isLooping) const;

	T Cast(float* value) const;

private:

	T SampleConstant(float time, bool isLooping);
	
	T SampleLinear(float time, bool isLooping);
	
	T SampleCubic(float time, bool isLooping);

	Interpolation interpolation;

	std::vector<Frame<N>> frames;

};

typedef Track<float, 1> ScalarTrack;
typedef Track<glm::vec3, 3> VectorTrack;
typedef Track<glm::quat, 4> QuaternionTrack;

#endif // TRACK_H