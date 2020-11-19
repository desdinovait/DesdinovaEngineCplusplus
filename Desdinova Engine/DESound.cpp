#include "DEGeneralIncludes.h"

void DESound::LoadFileIntoMemory(const char *name, void **buff, int *length)
{
    FILE *fp = fopen(name, "rb");
    
    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    *buff = malloc(*length);
    fread(*buff, *length, 1, fp);
    
    fclose(fp);
}


DLL_API bool DESound::Create(const char *name_or_data, bool inStream, bool fromMemory, FMOD_MODE mode)
{
	if (fromMemory==false)
	{
		//"Streams" are good for large sounds that are too large to fit into memory and need to be decoded as they play into a small 
		if (inStream)	
		{
			result = DESoundSystem::system->createStream(name_or_data, mode, 0, &sound);
		}
		else			
		{
			result = DESoundSystem::system->createSound(name_or_data, mode, 0, &sound);
		}
	}
	else
	{
		void *buff = 0;
		int length = 0;
		FMOD_CREATESOUNDEXINFO exinfo;
		LoadFileIntoMemory(name_or_data, &buff, &length);
		memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.length = length;
		if (inStream)
		{
			result = DESoundSystem::system->createStream((const char *)buff, mode | FMOD_OPENMEMORY, &exinfo, &sound);
		}
		else			
		{
			result = DESoundSystem::system->createSound((const char *)buff, mode | FMOD_OPENMEMORY, &exinfo, &sound);
			free(buff); // don't need the original memory any more.  Note!  If loading as a stream, the memory must stay active so do not free it!
		}
	}
	
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESound::Get3DMinMaxDistance(float *minValue, float *maxValue)
{
    result = sound->get3DMinMaxDistance(minValue, maxValue);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESound::Set3DMinMaxDistance(float minValue, float maxValue)
{
    result = sound->set3DMinMaxDistance(minValue, maxValue);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESound::Play(bool paused)
{
	result = DESoundSystem::system->playSound(FMOD_CHANNEL_FREE, sound, paused, &channel);
    if (result==FMOD_OK)
	{
		//Associa di default il canale al gruppo Master
		result = channel->setChannelGroup(DESoundSystem::masterGroup);
		if (result==FMOD_OK)	return true;
		else					return false;
	}
	else	
	{
		return false;
	}
}

DLL_API bool DESound::SetMode(FMOD_MODE mode)
{
    result = sound->setMode(mode);
    if (result==FMOD_OK)	return true;
	else					return false;
}


DLL_API bool DESound::GetLenght(unsigned int *length, FMOD_TIMEUNIT  lengthtype)
{
    result = sound->getLength(length, lengthtype);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESound::Invalidate()
{
	try
	{
		channel->isPlaying(&isPlayingTemp);
		if (isPlayingTemp)channel->setPaused(true);
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DESound::Restore()
{
	try
	{
		if (isPlayingTemp)	channel->setPaused(false);
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API bool DESound::Release()
{
    result = channel->stop();
    result = sound->release();
	sound= NULL;
    if (result==FMOD_OK)	return true;
	else					return false;
}






DLL_API bool DESound::SetChannelGroup(DESoundChannelGroup group)
{
	result = channel->setChannelGroup(group.GetGroup());
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESound::Get3DAttributes(FMOD_VECTOR *pos, FMOD_VECTOR *vel)
{
	result = channel->get3DAttributes(pos, vel);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESound::Set3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel)
{
	result = channel->set3DAttributes(pos, vel);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESound::SetPaused(bool paused)
{
	result = channel->setPaused(paused);
    if (result==FMOD_OK)	return true;
	else					return false;
}    

DLL_API bool DESound::GetPaused(bool *paused)
{
	result = channel->getPaused(paused);
    if (result==FMOD_OK)	return true;
	else					return false;
}   

DLL_API bool DESound::GetIsVirtual(bool *isvirtual)
{
	result = channel->isVirtual(isvirtual);
    if (result==FMOD_OK)	return true;
	else					return false;
} 

DLL_API bool DESound::GetIsPlaying(bool *isplaying)
{
	result = channel->isPlaying(isplaying);
    if (result==FMOD_OK)	return true;
	else					return false;
} 