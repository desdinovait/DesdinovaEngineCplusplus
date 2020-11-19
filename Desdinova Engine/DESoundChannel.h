#ifndef H_DESOUNDCHANNEL_H
#define H_DESOUNDCHANNEL_H

class DESoundChannel
{
	private:
		FMOD_RESULT		result;
	public:
		FMOD::Channel   *channel;
		DLL_API bool SetChannelGroup(DESoundChannelGroup group);
		DLL_API bool Get3DAttributes(FMOD_VECTOR *pos, FMOD_VECTOR *vel);
		DLL_API bool Set3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
		DLL_API bool SetPaused(bool paused);
		DLL_API bool GetPaused(bool *paused);
		DLL_API bool GetIsVirtual(bool *isvirtual);
};

#endif