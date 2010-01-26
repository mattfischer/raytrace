#include "polysolve.h"

#include <math.h>

double PolySolve::evaluate(const double coeffs[], int n, double x)
{
	double result = 0;
	for(int i=n-1; i>=0; i--)
	{
		result *= x;
		result += coeffs[i];
	}

	return result;
}

void PolySolve::derivative(const double coeffs[], int n, double result[])
{
	for(int i=1; i<n; i++)
	{
		result[i - 1] = i * coeffs[i];
	}
}

double PolySolve::newtonRhapson(const double coeffs[], int n, double guess, double threshold)
{
	double *deriv = new double[n-1];
	double oldVal, newVal;
	int iter;

	derivative(coeffs, n, deriv);

	newVal = guess;
	iter = 0;
	do
	{
		oldVal = newVal;
		newVal = oldVal - evaluate(coeffs, n, oldVal) / evaluate(deriv, n-1, oldVal);
		iter++;
	} while(abs(oldVal - newVal) > threshold && iter < 50);

	delete[] deriv;

	if(iter == 50) return HUGE_VAL;
	else return newVal;
}

void PolySolve::newtonRhapson(const double coeffs[], int n, const double guesses[], double threshold, double results[])
{
	for(int i=0; i<n-1; i++)
	{
		results[i] = newtonRhapson(coeffs, n, guesses[i], threshold);
	}
}