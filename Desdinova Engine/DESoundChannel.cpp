#include "DEGeneralIncludes.h"

DLL_API bool DESoundChannel::SetChannelGroup(DESoundChannelGroup group)
{
	result = channel->setChannelGroup(group.GetGroup());
    if (result != FMOD_OK)
    {
		return false;
	}
	return true;
}

DLL_API bool DESoundChannel::Get3DAttributes(FMOD_VECTOR *pos, FMOD_VECTOR *vel)
{
	result = channel->get3DAttributes(pos, vel);
    if (result != FMOD_OK)
    {
		return false;
	}
	return true;
}

DLL_API bool DESoundChannel::Set3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel)
{
	result = channel->set3DAttributes(pos, vel);
    if (result != FMOD_OK)
    {
		return false;
	}
	return true;
}

DLL_API bool DESoundChannel::SetPaused(bool paused)
{
	result = channel->setPaused(paused);
    if (result != FMOD_OK)
    {
		return false;
	}
	return true;
}    

DLL_API bool DESoundChannel::GetPaused(bool *paused)
{
	result = channel->getPaused(paused);
    if (result != FMOD_OK)
    {
		return false;
	}
	return true;
}   

DLL_API bool DESoundChannel::GetIsVirtual(bool *isvirtual)
{
	result = channel->isVirtual(isvirtual);
    if (result != FMOD_OK)
    {
		return false;
	}
	return true;
} 