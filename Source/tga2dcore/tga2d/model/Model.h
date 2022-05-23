#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <unordered_map>

#include <Tga2d/Math/Vector.h>
#include <Tga2d/Math/Transform.h>

namespace Tga2D
{
	struct AnimationImportDefinition;

	class ModelShader;
struct RenderObjectSharedData;
class TextureResource;

struct BoxSphereBounds
{
	// The radius of the Sphere
	float Radius;
	// The extents of the Box
	Vector3f BoxExtents;
	// The local-space center of the shape.
	Vector3f Center;
};

class Model
{
public:

	friend class ModelFactory;

	struct MeshData
	{
		std::string myName;
		std::string myMaterialName;
		UINT myNumberOfVertices;
		UINT myNumberOfIndices;
		UINT myStride;
		UINT myOffset;
		ID3D11Buffer* myVertexBuffer;
		ID3D11Buffer* myIndexBuffer;
		BoxSphereBounds myBounds;
	};
		
	struct Animation
	{
		struct Frame
		{
			std::vector<Transform> LocalTransforms;
		};

		std::vector<Frame> Frames;
		// The animation length in frames.
		unsigned int Length;

		// The animation duration in seconds.
		float Duration;
		
		std::wstring Name;

		// Default animation FPS
		float FramesPerSecond;

		// Default animation loop setting
		bool IsLooping;

		// Default animation interpolation setting
		bool IsInterpolating;
	};

	struct Skeleton
	{
		std::string Name;

		struct Joint
		{
			Matrix4x4f BindPoseInverse;
			int Parent;
			std::vector<unsigned int> Children;
			std::string Name;

			bool operator==(const Joint& aJoint) const
			{
				const bool A = BindPoseInverse == aJoint.BindPoseInverse;
				const bool B = Parent == aJoint.Parent;
				const bool C = Name == aJoint.Name;
				const bool D = Children == aJoint.Children;

				return A && B && C && D;
			}

			Joint()
				: BindPoseInverse{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, Parent(-1)
			{
			}
		};

		std::vector<Joint> Joints;
		std::unordered_map<std::string, size_t> JointNameToIndex;
		std::vector<std::string> JointName;

		std::unordered_map<std::wstring, size_t> myAnimationNameToIndex;
		std::vector<Animation> myAnimations;

		FORCEINLINE const Joint* GetRoot() const { if(Joints.empty()) return nullptr; return &Joints[0]; }

		bool operator==(const Skeleton& aSkeleton) const
		{
			return Joints == aSkeleton.Joints;
		}
	};

	void Init(MeshData& aMeshData, const std::wstring& aPath);
	void Init(std::vector<MeshData>& someMeshData, const std::wstring& aPath);

	bool LoadAnimation(const AnimationImportDefinition& aDefinition);

	const char* GetMaterialName(int meshIndex) const { return myMeshData[meshIndex].myMaterialName.c_str(); }
	const char* GetMeshName(int meshIndex) const { return myMeshData[meshIndex].myName.c_str(); }

	FORCEINLINE size_t GetMeshCount() const {return myMeshData.size();}
	FORCEINLINE MeshData const& GetMeshData(unsigned int anIndex) { return myMeshData[anIndex]; }
	FORCEINLINE const std::vector<MeshData>& GetMeshDataList() const { return myMeshData; }

	FORCEINLINE const std::wstring& GetPath() { return myPath; }

	FORCEINLINE const Animation* GetAnimation(std::wstring aName) const
	{
		if(const auto found = mySkeleton.myAnimationNameToIndex.find(aName); found != mySkeleton.myAnimationNameToIndex.end())
		{

			return &mySkeleton.myAnimations[found->second];
		}

		return nullptr;
	}
	FORCEINLINE const Animation* GetAnimation(size_t anIndex) const
	{
		if(anIndex < mySkeleton.myAnimations.size())
		{
			return &mySkeleton.myAnimations[anIndex];
		}

		return nullptr;
	}
	FORCEINLINE size_t GetNumAnimations() const { return mySkeleton.myAnimations.size(); }
	FORCEINLINE const Skeleton* GetSkeleton() const { return &mySkeleton; }

private:

	Skeleton mySkeleton;
	std::vector<MeshData> myMeshData;
	std::wstring myPath;
};

} // namespace Tga2D