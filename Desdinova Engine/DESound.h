#ifndef H_DESOUND_H
#define H_DESOUND_H

class DESound
{
	private:
		FMOD_RESULT		result;
		FMOD::Sound		*sound;
		FMOD::Channel	*channel;
		bool			isPlayingTemp;
		void			LoadFileIntoMemory(const char *name, void **buff, int *length);
	
	public:
		DLL_API bool	Create(const char *name_or_data, bool inStream, bool fromMemory, FMOD_MODE mode);
		DLL_API bool	Get3DMinMaxDistance(float *minValue, float *maxValue);
		DLL_API bool	Set3DMinMaxDistance(float minValue, float maxValue);
		DLL_API bool	SetMode(FMOD_MODE mode);
		DLL_API bool	Play(bool paused=false);
		DLL_API bool	Restore();
		DLL_API bool	Invalidate();
		DLL_API bool	Release();

		DLL_API bool	SetChannelGroup(DESoundChannelGroup group);
		DLL_API bool	Get3DAttributes(FMOD_VECTOR *pos, FMOD_VECTOR *vel);
		DLL_API bool	Set3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
		DLL_API bool	SetPaused(bool paused);
		DLL_API bool	GetPaused(bool *paused);
		DLL_API bool	GetIsVirtual(bool *isvirtual);
		DLL_API bool	GetIsPlaying(bool *isplaying);
		DLL_API bool	GetLenght(unsigned int *length, FMOD_TIMEUNIT  lengthtype=FMOD_TIMEUNIT_MS);
};

#endif