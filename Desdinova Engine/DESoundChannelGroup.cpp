#include "DEGeneralIncludes.h"

DLL_API bool DESoundChannelGroup::Create(const char *name)
{
	result = DESoundSystem::system->createChannelGroup(name, &group);
    if (result==FMOD_OK)
	{
		mainVolume = 1.0f;
		result = DESoundSystem::masterGroup->addGroup(group);
		if (result==FMOD_OK)	return true;
		else					return false;
	}
	else
	{
		return false;
	}
	return false;
}


DLL_API bool DESoundChannelGroup::AddChildGroup(DESoundChannelGroup child)
{
	result = group->addGroup(child.GetGroup());
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::AddDSP(DESoundDSP dsp)
{
	result = group->addDSP(dsp.GetDSP());
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API FMOD::ChannelGroup *DESoundChannelGroup::GetGroup()
{
	return group;
}

DLL_API bool DESoundChannelGroup::Invalidate()
{
	try
	{
		group->setVolume(0.0f);
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DESoundChannelGroup::Restore()
{
	try
	{
		group->setVolume(mainVolume);
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DESoundChannelGroup::Release()
{
	mainVolume=0;
    result = group->release();
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::OverrideVolume(float volume)
{
	result = group->overrideVolume(volume);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::OverrideFrequency(float frequency)
{
	result = group->overrideFrequency(frequency);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::OverridePan(float pan)
{
	result = group->overridePan(pan);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::OverrideMute(bool mute)
{
	result = group->overrideMute(mute);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::OverridePaused(bool paused)
{
	result = group->overridePaused(paused);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::OverrideReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop)
{
	result = group->overrideReverbProperties(prop);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::Override3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel)
{
	result = group->override3DAttributes(pos, vel);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::OverrideSpeakerMix(float  frontleft, float  frontright, float  center, float  lfe, float  backleft, float  backright, float  sideleft, float  sideright)
{
	result = group->overrideSpeakerMix(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);
    if (result==FMOD_OK)	return true;
	else					return false;
}



DLL_API bool DESoundChannelGroup::SetPitch(float pitch)
{
	result = group->setPitch(pitch);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::SetVolume(float volume)
{
	mainVolume = volume;
	result = group->setVolume(volume);
    if (result==FMOD_OK)	return true;
	else					return false;
}


DLL_API bool DESoundChannelGroup::GetPitch(float *pitch)
{
	result = group->getPitch(pitch);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::GetVolume(float* volume)
{
	result = group->getVolume(volume);
    if (result==FMOD_OK)	return true;
	else					return false;
}


DLL_API bool DESoundChannelGroup::GetNumChannels(int* num)
{
	result = group->getNumChannels(num);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundChannelGroup::GetNumChildGroup(int* num)
{
	result = group->getNumGroups(num);
    if (result==FMOD_OK)	return true;
	else					return false;
}


DLL_API bool DESoundChannelGroup::Stop()
{
	result = group->stop();
    if (result==FMOD_OK)	return true;
	else					return false;
}