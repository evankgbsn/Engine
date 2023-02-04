#ifndef FRAME_H
#define FRAME_H

enum class Interpolation
{
	Constant,
	Linear,
	Cubic
};

template<size_t N>
class Frame
{

public:

	Frame();

	~Frame();

	Frame(const Frame&) = default;

	Frame& operator=(const Frame&) = default;

	Frame(Frame&&) = default;

	Frame& operator=(Frame&&) = default;

	void SetTime(const float& newTime);

	const float& GetTime() const;

	float* const GetValue();

	float* const GetOutTangent();

	float* const GetInTangent();

private:

	float value[N];
	float inTangent[N];
	float outTangent[N];
	float time;
};

typedef Frame<1> ScalarFrame;
typedef Frame<3> VectorFrame;
typedef Frame<4> QuaternionFrame;

#endif // FRAME_H

template<size_t N>
inline Frame<N>::Frame()
{
}

template<size_t N>
inline Frame<N>::~Frame()
{
}

template<size_t N>
inline void Frame<N>::SetTime(const float& newTime)
{
	time = newTime;
}

template<size_t N>
inline const float& Frame<N>::GetTime() const
{
	return time;
}

template<size_t N>
inline float* const Frame<N>::GetValue()
{
	return value;
}

template<size_t N>
inline float* const Frame<N>::GetOutTangent()
{
	return outTangent;
}

template<size_t N>
inline float* const Frame<N>::GetInTangent()
{
	return inTangent;
}
