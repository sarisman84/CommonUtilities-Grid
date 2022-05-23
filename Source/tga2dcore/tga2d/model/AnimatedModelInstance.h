#pragma once
#include <map>
#include <memory>
#include <tga2d/math/matrix4x4.h>
#include <tga2d/math/transform.h>
#include <tga2d/EngineDefines.h>
#include <tga2d/model/ModelStatus.h>
#include <tga2d/model/model.h>
#include <tga2d/render/RenderCommon.h>

#include "tga2d/EngineMacros.h"

namespace Tga2D
{
	class AnimatedModel;
	class Animation;
	class ModelShader;
	class TextureResource;

	enum class AnimationState : uint8_t
	{
		NoAnimation,
		NotStarted,
		Playing,
		Stopped,
		Paused,
		Finished,
	};

	struct AnimationImportDefinition
	{
		// Where the animation file is found.
		std::wstring Path;
		// If the animation should loop by default or not.
		bool ShouldLoop = false;
		// If the animation should interpolate between frames or run frame by frame.
		bool ShouldInterpolate = true;
		// If we should override the playback FPS of this animation.
		float OverrideFPS = 0;
	};

	class AnimatedModelInstance : public RenderObjectSharedData
	{
	public:

		struct AnimationInstance
		{
		private:
			size_t myAnimation = 0;
			const Model::Skeleton* mySkeleton = nullptr;
			unsigned int myPreviousFrame = 0;
			unsigned int myCurrentFrame = 0;
			float myTimer = 0;
			float myFPS = 0;
			bool isLooping = false;
			bool isInterpolating = true;
			AnimationState myState = AnimationState::NotStarted;

			std::vector<Matrix4x4f> myFrameTransforms;

			void UpdateAnimation(size_t aCurrentFrame, size_t aNextFrame, float anAlpha, unsigned aBoneIdx, Matrix4x4f& aParentTransform, Matrix4x4f*
			                     outBoneInstanceTransforms);
		public:

			void Init(size_t anAnimation, const Model::Skeleton* aSkeleton);
			void Update(float aDeltaTime);

			/**
			 * Instructs the animation to play.
			 */
			void Play();

			/**
			 * Pauses the animation at the current frame.
			 */
			void Pause();

			/**
			 * Stops and rewinds the current animation to the beginning.
			 */
			void Stop();

			/**
			 * Allows you to change the loop state of this animation
			 * @param shouldLoop If the animation should loop or not.
			 */
			void SetIsLooping(bool shouldLoop);

			void SetFramesPerSecond(float someFPS);

			FORCEINLINE const std::vector<Matrix4x4f>& GetBoneTransforms() { return myFrameTransforms; }
			FORCEINLINE const Model::Animation* GetAnimation() const { return &mySkeleton->myAnimations[myAnimation]; }
			FORCEINLINE bool IsLooping() const { return isLooping; }
			FORCEINLINE AnimationState GetState() const { return myState; }

			void SetFramePosition(unsigned int aFramePosition);
		};

		AnimatedModelInstance();
		~AnimatedModelInstance();
		void Init(std::shared_ptr<Model> aModel);
		bool InitAnimations(const std::vector<AnimationImportDefinition>& someAnimImportDefs);

		FORCEINLINE unsigned int GetAnimationCount() const { return static_cast<unsigned int>(myAnimationInstances.size()); }

		/**
		 * Sets the current animation index and plays that animation if it's valid.
		 * @param anIndex The index of the animation to play. Pass -1 to stop all animations.
		 */
		DEPRECATED("AnimatedModelInstance::SetCurrentAnimationIndex is deprecated and will be removed in a future release. Please use AnimatedModelInstance::PlayAnimation instead.")
		void SetCurrentAnimationIndex(int anIndex);

		void SetCurrentAnimationTime(unsigned int anAnimationFrame);
		ModelStatus Update(float aDelta);
		//void RegisterAnimationEventCallback(const std::string& aEvent, std::function<void()> aFunctionToCall);
		void Render(ModelShader& shader) const;

		const Transform& GetTransform() const { return myTransform; }
		void SetTransform(const Transform& someTransform);
		FORCEINLINE const bool HasAnimation() const { return !myAnimationInstances.empty(); }
		void SetRotation(Rotator someRotation);
		void SetLocation(Vector3f someLocation);
		void SetScale(Vector3f someScale);
		FORCEINLINE bool IsValid() const { return myModel ? true : false; }
		void SetTexture(int meshIndex, int textureIndex, TextureResource* texture) { myTextures[meshIndex][textureIndex] = texture; }

		void PlayAnimation(unsigned int anIndex);
		void PlayAnimation(const std::wstring& anAnimationName);
		void PauseAnimation();
		void StopAnimation();

		FORCEINLINE AnimationState GetAnimationState() const { if(myCurrentAnimationIndex >= 0 && !myAnimationInstances.empty()) { return myAnimationInstances[myCurrentAnimationIndex].GetState(); } return AnimationState::NoAnimation; }
		FORCEINLINE int GetCurrentAnimationIndex() const { return myCurrentAnimationIndex; }
		FORCEINLINE AnimationInstance& GetAnimationInstance(unsigned int anIdx) { return myAnimationInstances[anIdx]; } 

	private:
		friend class ForwardRenderer;

		bool SetAnimIndexNoBlend(int index);
		bool SetAnimIndex(int index);
		void TriggerEvents(int aAnimationIndex, float aAtWhatTime);
		void BoneTransform(Matrix4x4f* Transforms);

		void RefreshAnimationInstances();

		std::vector<AnimationInstance> myAnimationInstances;

		Transform myTransform;
		std::shared_ptr<Model> myModel = nullptr;

		const TextureResource* myTextures[MAX_MESHES_PER_MODEL][4] = {};
		Matrix4x4f myBones[MAX_ANIMATION_BONES];
		Matrix4x4f myBonesDE[MAX_ANIMATION_BONES];

		//int myPrevAnimIndex = 0;
		int myCurrentAnimationIndex = -1;

		// Disabled right now, intent on fixing animations for SP21 ASAP.
		//int myPrevEventTickTime[32];
		//std::map<unsigned int, std::vector<std::function<void()>>> myEventCallbacks;
	};

}
