#pragma once
#include "Ray.h"
#include "../Grid/Grid.h"
#include <vector>

namespace Physics
{
	bool IntersectionGridObjectRay(Ray2D<float> aRay, std::vector<std::shared_ptr<GridObject>> someObjects, std::shared_ptr<GridObject>* anOutput)
	{
		for (auto& obj : someObjects)
		{
			float halfExtendsX = obj->mySize.x / 2.f;
			float halfExtendsY = obj->mySize.y / 2.f;


			Tga2D::Vector2f min = { halfExtendsX - obj->myPosition.x , halfExtendsY - obj->myPosition.y };
			Tga2D::Vector2f max = { halfExtendsX + obj->myPosition.x , halfExtendsY + obj->myPosition.y };


	



			float tMinX = (min.x - aRay.GetOrigin().x) / aRay.GetDirection().x;
			float tMaxX = (max.x - aRay.GetOrigin().x) / aRay.GetDirection().x;

			float tMinY = (min.y - aRay.GetOrigin().y) / aRay.GetDirection().y;
			float tMaxY = (max.y - aRay.GetOrigin().y) / aRay.GetDirection().y;


			float tMin = std::max(std::min(tMinX, tMaxX), std::min(tMinY, tMaxY));
			float tMax = std::min(std::max(tMinX, tMaxX), std::max(tMinY, tMaxY));


			if (tMax < 0.0f || tMin > tMax) continue;
			if (tMin < 0.0f)
			{
				anOutput = &obj;
				return true;
			}
		}
		return false;

	}
}