#pragma once
#include <tga2d/math/Transform.h>

template<typename T>
class Ray2D
{
public:
	Ray2D()
	{
		myOrigin = Tga2D::Vector2<T>();
		myDirection = Tga2D::Vector2<T>();
	}

	Ray2D(const Ray<T> aRay)
	{
		myOrigin = aRay.myOrigin;
		myDirection = aRay.myDirection;
	}

	Ray2D(const Tga2D::Vector2<T>& aFPos, const Tga2D::Vector2<T>& aDir, const float aDistance)
	{
		myOrigin = aFPos;
		myDirection = aDir;
		myLength = aDistance;
	}

	Ray2D(const Tga2D::Vector2<T>& aFPos, const Tga2D::Vector2<T>& aSPos)
	{
		myOrigin = aFPos;
		myDirection = aSPos - aFPos;
		myLength = static_cast<float>(myDirection.Length());
	}

	Tga2D::Vector2<T> GetOrigin()
	{
		return myOrigin;
	}

	Tga2D::Vector2<T> GetDirection()
	{
		return myDirection;
	}

	float GetLength() { return myLength; }

private:
	Tga2D::Vector2<T> myOrigin;
	Tga2D::Vector2<T> myDirection;
	float myLength;
};