#include "Math/PolySolve.hpp"

#include <math.h>

namespace Math {
	float PolySolve::evaluate(const float coeffs[], int n, float x)
	{
		float result = 0;
		for(int i=n-1; i>=0; i--)
		{
			result *= x;
			result += coeffs[i];
		}

		return result;
	}

	void PolySolve::derivative(const float coeffs[], int n, float result[])
	{
		for(int i=1; i<n; i++)
		{
			result[i - 1] = i * coeffs[i];
		}
	}

	float PolySolve::newtonRhapson(const float coeffs[], int n, float guess, float threshold)
	{
		float *deriv = new float[n-1];
		float oldVal, newVal;
		int iter;

		derivative(coeffs, n, deriv);

		newVal = guess;
		iter = 0;
		do
		{
			oldVal = newVal;
			newVal = oldVal - evaluate(coeffs, n, oldVal) / evaluate(deriv, n-1, oldVal);
			iter++;
		} while(fabs(oldVal - newVal) > threshold && iter < 50);

		delete[] deriv;

		if(iter == 50) return HUGE_VAL;
		else return newVal;
	}

	void PolySolve::newtonRhapson(const float coeffs[], int n, const float guesses[], float threshold, float results[])
	{
		for(int i=0; i<n-1; i++)
		{
			results[i] = newtonRhapson(coeffs, n, guesses[i], threshold);
		}
	}
}