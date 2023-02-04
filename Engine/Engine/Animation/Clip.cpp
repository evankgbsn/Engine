#include "Clip.h"

#include "Pose.h"

Clip::Clip() :
    name("No name given"),
    startTime(0.0f),
    endTime(0.0f),
    isLooping(true)
{
}

Clip::~Clip()
{
}

TransformTrack& Clip::operator[](unsigned int joint)
{
    for (unsigned int i = 0; i < tracks.size(); i++)
    {
        if (tracks[i].GetId() == joint)
        {
            return tracks[i];
        }
    }

    tracks.push_back(TransformTrack());
    tracks[tracks.size() - 1].SetId(joint);
    return tracks[tracks.size() - 1];
}

unsigned int Clip::GetIdAtIndex(unsigned int index) const
{
    return tracks[index].GetId();
}

void Clip::SetIdAtIndex(unsigned int index, unsigned int id)
{
    tracks[index].SetId(id);
}

unsigned int Clip::Size() const
{
    return 0;
}

float Clip::Sample(Pose& outPose, float time)
{
    if (GetDuration() == 0.0f)
    {
        return 0.0f;
    }

    time = AdjustTimeToFitRange(time);

    unsigned int size = static_cast<unsigned int>(tracks.size());
    for (unsigned int i = 0; i < size; i++)
    {
        unsigned int j = tracks[i].GetId(); // Joint
        Math::Transform local = outPose.GetLocalTransform(j);
        Math::Transform animated = tracks[i].Sample(local, time, isLooping);
        outPose.SetLocalTransform(j,animated);
    }
    return time;
}

void Clip::RecalculateDuration()
{
    startTime = 0;
    endTime = 0;
    bool startSet = false;
    bool endSet = false;

    unsigned int tracksSize = static_cast<unsigned int>(tracks.size());
    for (unsigned int i = 0; i < tracksSize; i++)
    {
        if (tracks[i].IsValid())
        {
            float st = tracks[i].GetStartTime();
            float et = tracks[i].GetEndTime();

            if (st < startTime || !startSet)
            {
                startTime = st;
                startSet = true;
            }

            if (et > endTime || !endSet)
            {
                endTime = et;
                endSet = true;
            }
        }
    }
}

const std::string& Clip::GetName() const
{
    return name;
}

void Clip::SetName(const std::string& n)
{
    name = n;
}

float Clip::GetDuration() const
{
    return endTime - startTime;
}

float Clip::GetStartTime() const
{
    return startTime;
}

float Clip::GetEndTime() const
{
    return endTime;
}

bool Clip::IsLooping() const
{
    return isLooping;
}

void Clip::SetIsLooping(const bool& loop)
{
    isLooping = loop;
}

float Clip::AdjustTimeToFitRange(float time) const
{
    if (isLooping)
    {
        float duration = endTime - startTime;
        if (duration <= 0)
        {
            return 0.0f;
        }

        time = fmod(time - startTime, duration);

        if (time < 0)
        {
            time += duration;
        }

        time += startTime;
    }
    else
    {
        if (time < startTime)
        {
            time = startTime;
        }
        else if (time > endTime)
        {
            time = endTime;
        }
    }

    return time;
}
