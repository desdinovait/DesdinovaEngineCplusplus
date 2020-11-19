#include "DEGeneralIncludes.h"

DLL_API bool DESoundDSP::CreateDSP(FMOD_DSP_DESCRIPTION *description)
{
	result = DESoundSystem::system->createDSP(description, &dsp);
    if (result==FMOD_OK)	return true;
	else					return false;
}
DLL_API bool DESoundDSP::CreateDSPByIndex(int index)
{
    result = DESoundSystem::system->createDSPByIndex(index, &dsp);
    if (result==FMOD_OK)	return true;
	else					return false;
}
DLL_API bool DESoundDSP::CreateDSPByType(FMOD_DSP_TYPE type)
{
    result = DESoundSystem::system->createDSPByType(type, &dsp);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API FMOD::DSP *DESoundDSP::GetDSP()
{
	return dsp;
}

DLL_API bool DESoundDSP::Add()
{
	result = DESoundSystem::system->addDSP(dsp);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundDSP::Remove()
{
    bool active;
	result = dsp->getActive(&active);
    if (result==FMOD_OK)
	{
		if (active)
		{
			result = dsp->remove();
			if (result==FMOD_OK)	return true;
			else					return false;
		}
		return true;
	}
	return false;
}

DLL_API bool DESoundDSP::SetParameter(int index, float value)
{
	result = dsp->setParameter(index, value);
    if (result==FMOD_OK)	return true;
	else					return false;
}

DLL_API bool DESoundDSP::Release()
{
	result = dsp->release();
    if (result==FMOD_OK)	return true;
	else					return false;
}