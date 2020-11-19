#include "DEGeneralIncludes.h"


DLL_API bool DEInterpolationLinear::Setup(float from, float to, float time)
{
	if(time < 0)
	{
		return false;
	}
	_remainingTime = time;
	_value = from;
	_step = (to-from)/time; // Calculate distance per second.
	return true;
}
DLL_API bool DEInterpolationLinear::Interpolate(float deltaTime)
{
	_remainingTime -= deltaTime;
	_value += _step*deltaTime;
	return (_remainingTime <= 0);
}
DLL_API float DEInterpolationLinear::GetValue()
{
	return _value;
}



DLL_API bool DEInterpolationEaseOutShift::Setup(int from, int to, int shift)
{
	if(shift <= 0)
	{
		return false;
	}
	_value = from;
	_target = to;
	_shift = shift;
	return true;
}
DLL_API bool DEInterpolationEaseOutShift::Interpolate() // Note: Not time dependent.
{
	int oldValue = _value;
	if(_shift > 0)
	{
		_value = (_value * ((1 << _shift) - 1) + _target) >> _shift;
	}
	// lots more likely than with float version.
	return (_value == oldValue);
}
DLL_API int DEInterpolationEaseOutShift::GetValue()
{
	return _value;
}



DLL_API bool DEInterpolationEaseOutDivide::Setup(float from, float to, float divisor)
{
	if(divisor <= 0)
	{
		return false;
	}
	_value = from;
	_target = to;
	_divisor = divisor;
	return true;
}
DLL_API bool DEInterpolationEaseOutDivide::Interpolate() // Note: Not time dependent.
{
	float oldValue = _value;
	if(_divisor > 0) 
	{
		_value = (_value * (_divisor-1.0f) + _target)/_divisor;
	}
	// Not likely to be true very often.
	return (_value == oldValue);
}
DLL_API float DEInterpolationEaseOutDivide::GetValue()
{
	return _value;
}


	
DLL_API bool DEInterpolationEaseInOut::Setup(float from, float to, float time)
{
	if(time <= 0)
	{
		return false;
	}
	_value = from;
	_target = to;
	_speed = 0.0f;
	// derived from x=x0 + v0*t + a*t*t/2
	_acceleration = (to-from)/(time*time/4);
	_remainingTime = _totalTime = time;
	return true;
}
DLL_API bool DEInterpolationEaseInOut::Interpolate(float deltaTime)
{
	_remainingTime -= deltaTime;
	if(_remainingTime < _totalTime/2)
	{
		// Deceleration
		_speed -= _acceleration * deltaTime;
	}
	else
	{
		// Acceleration
		_speed += _acceleration * deltaTime;
	}
	_value += _speed*deltaTime;
	return (_remainingTime <= 0);
}
DLL_API float DEInterpolationEaseInOut::GetValue()
{
	return _value;
}
