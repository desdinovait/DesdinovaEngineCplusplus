#include "DEGeneralIncludes.h"
#include "DESoundSystem.h"

FMOD::System    *DESoundSystem::system;
FMOD::ChannelGroup *DESoundSystem::masterGroup;

DLL_API bool DESoundSystem::GetCPUUsage( float *dsp, float *stream, float *update, float *total)
{
	if (isCreated)
	{
		result = system->getCPUUsage(dsp, stream, update, total);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

DLL_API bool DESoundSystem::GetChannelsPlaying(int *channels)
{
	if (isCreated)
	{
		result = system->getChannelsPlaying(channels);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}


DLL_API bool DESoundSystem::Create(int maxChannels,int sw_max, int  hw_min2d, int  hw_max2d, int  hw_min3d, int  hw_max3d)
{   
	isCreated = false;
	DELog::LogInfo("<br><b>Sound System Initialization</b>");

    //Create a System object and initialize.
    result = FMOD::System_Create(&system);
    if (result==FMOD_OK)
	{
			DELog::LogInfo("<li>SoundSystem->Create(...): The function completed successfully");
		
			unsigned int version;
			result = system->getVersion(&version);
			if (version < FMOD_VERSION)
			{
				DELog::LogError("<li>SoundSystem->GetVersion(...): Old version of FMOD Sound System. Check DLL version and try again.");
			}
			else
			{
				//Setta i canali Software massimi
				if (sw_max!=0)	system->setSoftwareChannels(sw_max);
				
				//Setta i canali hardware minimi e massimi
				if ((hw_min2d!=0)&&(hw_max2d!=0)&&(hw_min3d!=0)&&(hw_max3d!=0))
				{
					system->setHardwareChannels(hw_min2d, hw_max2d, hw_min3d, hw_max3d);
				}


				DELog::LogInfo("<li>SoundSystem->GetVersion(...): The function completed successfully");
				result = system->init(maxChannels, FMOD_INIT_NORMAL, 0);
				if (result==FMOD_OK)
				{
						DELog::LogInfo("<li>SoundSystem->Init(...): The function completed successfully");				
						result = system->getMasterChannelGroup(&masterGroup);
						if (result==FMOD_OK)
						{
							DELog::LogInfo("<li>SoundSystem->GetMasterChannelGroup(...): The function completed successfully");
							float dsp, stream, update, total;
							system->getCPUUsage(&dsp, &stream, &update, &total);
							int a,b,c,d;
							system->getHardwareChannels(&a, &b, &c);
							system->getSoftwareChannels(&d);
							DELog::LogInfo("<li>SoundSystem Hardware Channels (2D/3D/Tot): %d / %d / %d", a, b, c);
							DELog::LogInfo("<li>SoundSystem Software Channels: %d", d);
							isCreated = true;
						}
						else
						{
							DELog::LogError("<li>SoundSystem->GetMasterChannelGroup(...): %s", FMOD_ErrorString(result));
							isCreated = false;
						}				
				}
				else
				{
					DELog::LogError("<li>SoundSystem->init(...): %s", FMOD_ErrorString(result));
				}
			}
	}
	else
	{
		DELog::LogError("<li>SoundSystem->create(...): %s", FMOD_ErrorString(result));
	}
	
	if (isCreated)	DELog::LogInfo("<li>Created successfully");	
	else			DELog::LogError("<li>NOT Created");

	return isCreated;
}

DLL_API bool DESoundSystem::Set3DSettings(float dopplerscale, float distancefactor, float rolloffscale)
{    
	if (isCreated)
	{
		system->set3DSettings(dopplerscale, distancefactor, rolloffscale);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

DLL_API bool DESoundSystem::Set3DNumListeners(int numListerners)
{    
	if (isCreated)
	{
		//If the number of listeners is set to more than 1, then panning and doppler are turned off. All sound effects will be mono.
		//FMOD uses a 'closest sound to the listener' method to determine what should be heard in this case.
		system->set3DNumListeners(numListerners);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

DLL_API bool DESoundSystem::Set3DListenerAttributes(int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up)
{    
	if (isCreated)
	{
		system->set3DListenerAttributes(listener, pos, vel, forward, up);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}


DLL_API bool DESoundSystem::SetSpeakerMode(FMOD_SPEAKERMODE speakermode)
{    
	if (isCreated)
	{
		system->setSpeakerMode(speakermode);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}


DLL_API bool DESoundSystem::GetHardwareChannels(int *num2d, int *num3d, int *total)
{    
	if (isCreated)
	{
		system->getHardwareChannels(num2d, num3d, total);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

DLL_API bool DESoundSystem::GetSoftwareChannels(int *num)
{    
	if (isCreated)
	{
		system->getSoftwareChannels(num);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

DLL_API bool DESoundSystem::GetMasterGroup(DESoundChannelGroup *masterGroup)
{    
	masterGroup->group = DESoundSystem::masterGroup;
	return true;
}

DLL_API bool DESoundSystem::Update()
{
	if (isCreated)
	{
		system->update();
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

DLL_API bool DESoundSystem::Close()
{    
 	if (isCreated)
	{
		result = system->close();
		if (result!=FMOD_OK)
		{		
			DELog::LogWarning("<li>SoundSystem->Close(...): %s", FMOD_ErrorString(result));		
		}
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

DLL_API bool DESoundSystem::Release()
{   

	try
	{
		result = system->release();
		if (result!=FMOD_OK)
		{		
			DELog::LogWarning("<li>SoundSystem->Release(...): %s", FMOD_ErrorString(result));		
		}
		isCreated=false;
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>SoundSystem : Unknown statement error - Probably NOT Released");		
		return false;
	}
	return false;
}

