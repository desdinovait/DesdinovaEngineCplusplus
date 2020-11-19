#ifndef H_DESOUNDDSP_H
#define H_DESOUNDDSP_H

class DESoundDSP
{
	private:
		FMOD_RESULT      result;
		FMOD::DSP *dsp;
	public:
		DLL_API bool CreateDSP(FMOD_DSP_DESCRIPTION *description);
		DLL_API bool CreateDSPByIndex(int index);
		DLL_API bool CreateDSPByType(FMOD_DSP_TYPE type);
		DLL_API FMOD::DSP *GetDSP();
		DLL_API bool Add();
		DLL_API bool Remove();
		DLL_API bool SetParameter(int index, float value);
		DLL_API bool Release();
};

#endif