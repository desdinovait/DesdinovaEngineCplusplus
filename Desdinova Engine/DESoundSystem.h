#ifndef H_DESOUNDSYSTEM_H
#define H_DESOUNDSYSTEM_H

#include "DESoundChannelGroup.h"
#include "DESoundChannel.h"

class DESoundSystem
{
	private:
		FMOD_RESULT					result;
		bool						isCreated;
	public:
		static FMOD::System			*system;
		static FMOD::ChannelGroup	*masterGroup;
		DLL_API bool				GetCPUUsage( float *dsp, float *stream, float *update, float *total);
		DLL_API bool				GetChannelsPlaying(int *channels);
		DLL_API bool				Create(int maxChannels, int sw_max=0, int hw_min2d=0, int hw_max2d=0, int hw_min3d=0, int hw_max3d=0);
		DLL_API bool				Set3DSettings(float dopplerscale=1.0f, float distancefactor=1.0f, float rolloffscale=1.0f);
		DLL_API bool				Set3DNumListeners(int numListerners=1);
		DLL_API bool				Set3DListenerAttributes(int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up);
		DLL_API bool				SetSpeakerMode(FMOD_SPEAKERMODE speakermode);
		DLL_API bool				GetHardwareChannels(int *num2d, int *num3d, int *total);
		DLL_API bool				GetSoftwareChannels(int *num);
		DLL_API bool				GetMasterGroup(DESoundChannelGroup *masterGroup);
		DLL_API bool				Update();
		DLL_API bool				Close();
		DLL_API bool				Release();
};

#endif