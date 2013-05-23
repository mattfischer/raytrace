#ifndef POLYSOLVE_H
#define POLYSOLVE_H

namespace Math {

class PolySolve
{
public:
	static float evaluate(const float coeffs[], int n, float x);
	static void derivative(const float coeffs[], int n, float result[]);
	static float newtonRhapson(const float coeffs[], int n, float guess, float threshold);
	static void newtonRhapson(const float coeffs[], int n, const float guesses[], float threshold, float results[]);
};

}
#endif