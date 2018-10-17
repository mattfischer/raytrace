#ifndef MATH_COORDINATE_HPP
#define MATH_COORDINATE_HPP

namespace Math {
	class Coordinate
	{
	public:
		Coordinate();
		Coordinate(float x, float y, float z, float w);
	
		float x() const;
		void setX(float x);

		float y() const;
		void setY(float y);

		float z() const;
		void setZ(float z);

		float w() const;
		void setW(float w);

		float &at(int i);
		const float &at(int i) const;

		float &operator()(int i);
		const float &operator()(int i) const;

	protected:
		float mValues[4];
	};
}

#endif