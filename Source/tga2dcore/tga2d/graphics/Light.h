#pragma once
#include <Tga2d/Math/Color.h>
#include <Tga2d/Math/Transform.h>
#include <Tga2d/Math/Vector.h>

namespace Tga2D
{
class Light
{
protected:
	Color myColor;
	float myIntensity;
	Transform myTransform;

public:

	virtual ~Light() = default;
	
	Light(const Transform& someTransform, const Color& aColor, float aIntensity);

	FORCEINLINE float GetIntensity() const { return myIntensity; }
	FORCEINLINE Color GetColor() const { return myColor; }

	FORCEINLINE Transform GetTransform() const { return myTransform; }
	FORCEINLINE void SetTransform(Transform newTransform) { myTransform = newTransform; }
	FORCEINLINE void SetPosition(Vector3f newPosition) { myTransform.SetPosition(newPosition); }
	FORCEINLINE void SetRotation(Rotator newRotation) { myTransform.SetRotation(newRotation); }
	FORCEINLINE void SetColor(Color newColor) { myColor = newColor; }

};

} // namespace Tga2D