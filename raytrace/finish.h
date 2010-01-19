#ifndef FINISH_H
#define FINISH_H

class Finish
{
public:
	Finish();
	virtual ~Finish();

	double ambient() const;
	void setAmbient(double ambient);

	double diffuse() const;
	void setDiffuse(double diffuse);

	double specular() const;
	void setSpecular(double specular);

	double specularPower() const;
	void setSpecularPower(double specularPower);

protected:
	double mAmbient;
	double mDiffuse;
	double mSpecular;
	double mSpecularPower;
};
#endif