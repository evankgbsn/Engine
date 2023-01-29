#include "Track.h"

#include "../Math/Math.h"
#include <glm/gtx/quaternion.hpp>

template Track<float, 1>;
template Track<glm::vec3, 3>;
template Track<glm::quat, 4>;

namespace TrackHelpers
{

	inline float Interpolate(float a, float b, float time)
	{
		return a + (b - a) * time;
	}

	inline glm::vec3 Interpolate(const glm::vec3& a, const glm::vec3& b, float time)
	{
		return Math::Lerp(a, b, time);
	}

	inline glm::quat Interpolate(const glm::quat& a, const glm::quat& b, float time)
	{
		glm::quat result = glm::mix(a, b, time);

		if (glm::dot(a, b) < 0.0f)
		{
			result = glm::mix(a, -b, time);
		}

		return glm::normalize(result);
	}

	inline void Neighborhood(const glm::quat& a, glm::quat& b)
	{
		if (glm::dot(a, b) < 0)
		{
			b = -b;
		}
	}

	inline void Neighborhood(const float& a, float& b) {};

	inline void Neighborhood(const glm::vec3& a, glm::vec3& b) {};

	inline float AdjustHermiteResult(float f)
	{
		return f;
	}

	inline glm::vec3 AdjustHermiteResult(const glm::vec3& v)
	{
		return v;
	}

	inline glm::quat AdjustHermiteResult(const glm::quat& q)
	{
		return glm::normalize(q);
	}

};

template<typename T, unsigned int N>
inline Track<T, N>::Track() :
	interpolation(Interpolation::Linear)
{
}

template<typename T, unsigned int N>
inline Track<T, N>::Track(const Interpolation& interpolationType) :
	interpolation(interpolationType)
{
}

template<typename T, unsigned int N>
inline Track<T, N>::~Track()
{
}

template<typename T, unsigned int N>
inline Frame<N>& Track<T, N>::operator[](unsigned int index)
{
	return frames[index];
}

template<typename T, unsigned int N>
inline void Track<T, N>::SetInterpolation(Interpolation newInterpolation)
{
	interpolation = newInterpolation;
}

template<typename T, unsigned int N>
inline Interpolation Track<T, N>::GetInterpolation() const
{
	return interpolation;
}

template<typename T, unsigned int N>
inline unsigned int Track<T, N>::Size() const
{
	return static_cast<unsigned int>(frames.size());
}

template<typename T, unsigned int N>
inline void Track<T, N>::SetSize(unsigned int newSize)
{
	frames.resize(newSize);
}

template<typename T, unsigned int N>
inline const float& Track<T, N>::GetStartTime() const
{
	return frames[0].GetTime();
}

template<typename T, unsigned int N>
inline const float& Track<T, N>::GetEndTime() const
{
	return frames[frames.size() - 1].GetTime();
}

template<typename T, unsigned int N>
inline float Track<T, N>::AdjustTimeToFitTrack(float time, float isLooping) const
{
	if (frames.size() <= 1U)
	{
		return 0.0f;
	}

	float startTime = GetStartTime();
	float endTime = GetEndTime();
	float duration = endTime - startTime;

	if (duration <= 0.0f)
	{
		return 0.0f;
	}

	if (isLooping)
	{
		time = fmod(time - startTime, duration);

		if (time < 0.0f)
		{
			time += duration;
		}

		time += startTime;
	}
	else
	{
		if (time < 0.0f)
		{
			time = startTime;
		}
		else if (time > duration)
		{
			time = endTime;
		}

	}

	return time;
}

template<> float Track<float, 1>::Cast(float* value) const
{
	return value[0];
}

template<> glm::vec3 Track<glm::vec3, 3>::Cast(float* value) const
{
	return glm::vec3(value[0], value[1], value[2]);
}

template<> glm::quat Track<glm::quat, 4>::Cast(float* value) const
{
	glm::quat tmp(value[0], value[1], value[2], value[3]);
	return glm::normalize(tmp);
}

template<typename T, unsigned int N>
inline T Track<T, N>::SampleConstant(float time, bool isLooping)
{
	unsigned int frame = FrameIndex(time, isLooping);

	if (frame < 0 || frame >= Size())
	{
		return T();
	}

	return Cast(&frames[frame].GetValue()[0]);
}

template<typename T, unsigned int N>
inline T Track<T, N>::SampleLinear(float time, bool isLooping)
{
	unsigned int thisFrame = FrameIndex(time, isLooping);

	if (thisFrame < 0 || thisFrame >= Size() - 1)
	{
		return T();
	}

	unsigned int nextFrame = thisFrame + 1;

	float trackTime = AdjustTimeToFitTrack(time, isLooping);
	float thisTime = frames[thisFrame].GetTime();
	float frameDelta = frames[nextFrame].GetTime() - thisTime;

	if (frameDelta <= 0.0f)
	{
		return T();
	}

	float t = (trackTime - thisTime) / frameDelta;

	T start = Cast(&frames[thisFrame].GetValue()[0]);
	T end = Cast(&frames[nextFrame].GetValue()[0]);

	return TrackHelpers::Interpolate(start, end, t);
}

template<typename T, unsigned int N>
inline T Track<T, N>::SampleCubic(float time, bool isLooping)
{
	unsigned int thisFrame = FrameIndex(time, isLooping);

	if (thisFrame <= 0 || thisFrame >= Size() - 1)
	{
		return T();
	}

	unsigned int nextFrame = thisFrame + 1;

	float trackTime = AdjustTimeToFitTrack(time, isLooping);
	float thisTime = frames[thisFrame].GetTime();
	float frameDelta = frames[nextFrame].GetTime() - thisTime;

	if (frameDelta <= 0.0f)
	{
		return T();
	}

	float t = (trackTime - thisTime) / frameDelta;

	T point1 = Cast(&frames[thisFrame].GetValue()[0]);
	T tangent1; // = frames[thisFrame].GetOutTangent() * frameDelta
	memcpy(&tangent1, frames[thisFrame].GetOutTangent(), N * sizeof(float));
	tangent1 = tangent1 * frameDelta;

	T point2 = Cast(&frames[nextFrame].GetValue()[0]);
	T tangent2;
	memcpy(&tangent2, frames[nextFrame].GetInTangent(), N * sizeof(float));
	tangent2 = tangent2 * frameDelta;

	return Hermite(time, point1, point2, tangent1, tangent2);
}

template<typename T, unsigned int N>
inline T Track<T, N>::Sample(float time, bool isLooping)
{
	switch (interpolation)
	{
	case Interpolation::Constant:
		return SampleConstant(time, isLooping);
	case Interpolation::Linear:
		return SampleLinear(time, isLooping);
	case Interpolation::Cubic:
		return SampleCubic(time, isLooping);
	default:
		break;
	};

	return T();
}

template<typename T, unsigned int N>
inline T Track<T, N>::Hermite(float time, const T& p0, const T& p1, const T& t0, const T& t1) const
{
	float time2 = time * time;
	float time3 = time2 * time;

	T _p1 = p1;
	TrackHelpers::Neighborhood(p0, _p1);

	float h1 = 2.0f * time3 - 3.0f * time2 + 1.0f;
	float h2 = -2.0f * time3 * 3.0f * time2;
	float h3 = time3 - 2.0f * time2 + time;
	float h4 = time3 - time2;

	T result = p0 * h1 + p1 * h2 + t0 * h3 + t1 * h4;

	return TrackHelpers::AdjustHermiteResult(result);
}

template<typename T, unsigned int N>
inline unsigned int Track<T, N>::FrameIndex(float time, bool isLooping) const
{
	if (frames.size() < 2)
	{
		return -1;
	}

	if (isLooping)
	{
		float startTime = GetStartTime();
		float endTime = GetEndTime();
		float duration = endTime - startTime;

		time = fmod(time - startTime, duration);

		if (time < 0.0f)
		{
			time += duration;
		}

		time += startTime;
	}
	else
	{
		if (time < 0)
		{
			return 0;
		}
		else if (time > frames[frames.size() - 2].GetTime())
		{
			return Size() - 2U;
		}
	}

	for (unsigned int i = Size() - 1; i > 0; i++)
	{
		if (frames[i].GetTime() <= time)
		{
			return i;
		}
	}

	return -1;

}

typedef Track<float, 1> ScalarTrack;
typedef Track<glm::vec3, 3> VectorTrack;
typedef Track<glm::quat, 4> QuaternionTrack;