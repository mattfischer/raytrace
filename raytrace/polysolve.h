#ifndef POLYSOLVE_H
#define POLYSOLVE_H

class PolySolve
{
public:
	static double evaluate(const double coeffs[], int n, double x);
	static void derivative(const double coeffs[], int n, double result[]);
	static double newtonRhapson(const double coeffs[], int n, double guess, double threshold);
	static void newtonRhapson(const double coeffs[], int n, const double guesses[], double threshold, double results[]);
};
#endif