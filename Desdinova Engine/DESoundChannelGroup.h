#ifndef H_DESOUNDCHANNELGROUP_H
#define H_DESOUNDCHANNELGROUP_H

class DESoundChannelGroup
{
	private:
		FMOD_RESULT         result;
		float				mainVolume;
	public:
		FMOD::ChannelGroup *group;
		
		DLL_API bool Create(const char *name);
		DLL_API bool AddChildGroup(DESoundChannelGroup child);
		DLL_API bool AddDSP(DESoundDSP dsp);
		DLL_API FMOD::ChannelGroup *GetGroup();
		DLL_API bool OverrideVolume(float volume);
		DLL_API bool OverrideFrequency(float frequency);
		DLL_API bool OverridePan(float pan);
		DLL_API bool OverrideMute(bool mute);
		DLL_API bool OverridePaused(bool paused);
		DLL_API bool OverrideReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop);
		DLL_API bool OverrideSpeakerMix(float  frontleft, float  frontright, float  center, float  lfe, float  backleft, float  backright, float  sideleft, float  sideright);
		DLL_API bool Override3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
		DLL_API bool SetPitch(float pitch);
		DLL_API bool SetVolume(float volume);
		DLL_API bool GetPitch(float *pitch);
		DLL_API bool GetVolume(float *volume);
		DLL_API bool GetNumChildGroup(int* num);
		DLL_API bool GetNumChannels(int* num);
		DLL_API bool Stop();
		DLL_API bool Invalidate();
		DLL_API bool Restore();
		DLL_API bool Release();
};

#endif