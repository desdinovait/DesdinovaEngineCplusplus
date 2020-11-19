#ifndef H_INTERPOLATIONS_H
#define H_INTERPOLATIONS_H

// The linear interpolation class calculates a constant velocity at
// setup time, then applies that velocity to the data point based on
// the specified time slice (usually one frame).
// It is frame rate independent, meaning varying frame rates should
// give consistent results.
class DEInterpolationLinear
{
	private:
		float _value;
		float _step;
		float _remainingTime;

	public:
		DLL_API bool Setup(float from, float to, float time);
		DLL_API bool Interpolate(float deltaTime);
		DLL_API float GetValue();
};

// This is a version of the following equation:
// a = (a*(n-1) + b)/n
// or an example which has been simplified and had constants added:
// a = (a*7 + b)/8
// or
// a = (a*7 + b)>>3
// The second form is about as fast as you can get on hardware which 
// does not have good floating point support.
// This gives a heavy weighting to the original value, but will
// tend toward the destination value over time.
// It gets rather strong aliasing problems when the source and 
// destination are close to each other, and the shift value must 
// be integer based, which restricts the rates at which you 
// approach the destination. It is frame rate dependent, meaning 
// varying frame rates will give varying results.
class DEInterpolationEaseOutShift
{
	private:
		int _value;
		int _target;
		int _shift;

	public:
		DLL_API bool Setup(int from, int to, int shift);
		DLL_API bool Interpolate();
		DLL_API int GetValue();
};

// This is roughly equal to the integer shift interpolation method, 
// but uses floating point multiplies and divides based on the same 
// equation:
// a = (a*(n-1) + b) / n
// In this case, the equation is used directly instead of being 
// converted into shifts. This gives it a wider number of possible 
// rates of movement. It is frame rate dependent, meaning varying 
// frame rates will give varying results.
class DEInterpolationEaseOutDivide : DEInterpolationEaseOutShift
{
	private:
		float _value;
		float _target;
		float _divisor;
	
	public:
		DLL_API bool Setup(float from, float to, float divisor);
		DLL_API bool Interpolate();
		DLL_API float GetValue();
};

// Ease In and Ease Out Interpolation is done by calculating the 
// acceleration which will cover exactly half the distance in half 
// the specified time. That acceleration is applied for the first 
// half, then the inverse is applied for the second half to slow 
// down so the data point stops as it reaches the target point.
class DEInterpolationEaseInOut
{
	private:
		float _value;
		float _target;
		float _remainingTime;
		float _totalTime;
		float _speed;
		float _acceleration;

	public:
		DLL_API bool Setup(float from, float to, float time);
		DLL_API bool Interpolate(float deltaTime);
		DLL_API float GetValue();

};

#endif