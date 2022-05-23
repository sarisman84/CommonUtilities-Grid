#include "stdafx.h"
#include "AnimatedModelInstance.h"
#include "AnimatedModelInstance.h"
#include <tga2d/model/ModelFactory.h>
#include <tga2d/animation/Animation.h>
#include <tga2d/animation/AnimationController.h>
#include <tga2d/drawers/DebugDrawer.h>
#include <tga2d/shaders/ModelShader.h>

using namespace Tga2D;

void AnimatedModelInstance::AnimationInstance::UpdateAnimation(size_t aCurrentFrame, size_t aNextFrame, float anAlpha, unsigned aBoneIdx,
	Matrix4x4f& aParentTransform, Matrix4x4f* outBoneInstanceTransforms)
{
	const Model::Skeleton::Joint& joint = mySkeleton->Joints[aBoneIdx];

	const Transform& currentFrameJointXform = mySkeleton->myAnimations[myAnimation].Frames[aCurrentFrame].LocalTransforms[aBoneIdx];

	Matrix4x4f jointXform = currentFrameJointXform.GetMatrix();

	if(isInterpolating)
	{
		const Transform& nextFrameJointXform = mySkeleton->myAnimations[myAnimation].Frames[aNextFrame].LocalTransforms[aBoneIdx];

		// Interpolate between the frames
		const Vector3f T = Vector3f::Lerp(currentFrameJointXform.GetPosition(), nextFrameJointXform.GetPosition(), anAlpha);
		const Quatf R = Quatf::Slerp(currentFrameJointXform.GetQuaternion(), nextFrameJointXform.GetQuaternion(), anAlpha);
		const Vector3f S = Vector3f::Lerp(currentFrameJointXform.GetScale(), nextFrameJointXform.GetScale(), anAlpha);

		jointXform = Matrix4x4f::CreateScaleMatrix(S) * R.GetRotationMatrix4x4f() * Matrix4x4f::CreateTranslationMatrix(T);
	}

	Matrix4x4f boneTransform = aParentTransform * jointXform.GetTranspose();

	Matrix4x4f Result = Matrix4x4f::CreateIdentityMatrix();

	Result *= boneTransform;
	Result *= joint.BindPoseInverse;		

	outBoneInstanceTransforms[aBoneIdx] = Result;

	for(size_t c = 0; c < joint.Children.size(); c++)
	{
		UpdateAnimation(aCurrentFrame, aNextFrame, anAlpha, joint.Children[c], boneTransform, outBoneInstanceTransforms);
	}
}

void AnimatedModelInstance::AnimationInstance::Init(size_t anAnimation, const Model::Skeleton* aSkeleton)
{
	mySkeleton = aSkeleton;
	myAnimation = anAnimation;
	myFPS = aSkeleton->myAnimations[myAnimation].FramesPerSecond;
	isLooping = aSkeleton->myAnimations[myAnimation].IsLooping;
	isInterpolating = aSkeleton->myAnimations[myAnimation].IsInterpolating;
	
	myFrameTransforms.resize(aSkeleton->Joints.size());
}

void AnimatedModelInstance::AnimationInstance::Update(float aDeltaTime)
{
	if(myState == AnimationState::Playing)
	{
		myTimer += aDeltaTime;

		if(myTimer >= GetAnimation()->Duration)
		{
			if(isLooping)
			{
				myTimer -= GetAnimation()->Duration;
			}
			else
			{
				myTimer = GetAnimation()->Duration;
				myState = AnimationState::Finished;
			}
		}			

		const float frameRate = 1.0f / myFPS;
		const float result = myTimer / frameRate;
		const size_t frame = static_cast<size_t>(std::floor(result));// Which frame we're on
		const float delta = result - static_cast<float>(frame); // How far we have progressed to the next frame.

		size_t nextFrame = frame + 1;
		if(myState == AnimationState::Finished)
		{
			nextFrame = frame;
		}
		else if(nextFrame > GetAnimation()->Length)
			nextFrame = 0;

		// Plow through all the matrices to find the relative transform for each bone.
		Matrix4x4f identity = Matrix4x4f::CreateIdentityMatrix();
		UpdateAnimation(frame, nextFrame, delta, 0, identity, &myFrameTransforms[0]);

		//if(myTimer >= 1.0f / myFPS)
		//{
		//	myTimer = 0;
		//	myCurrentFrame++;
		//	if(myCurrentFrame >= GetAnimation()->Length)
		//	{
		//		if(isLooping)
		//		{
		//			myCurrentFrame = 1;
		//		}
		//		else
		//		{
		//			myCurrentFrame--; // Step back to avoid overflow, just hold the last frame.
		//			myState = AnimationState::Finished;
		//		}
		//	}
		//}

		//if(myTimer >= 1.0f / myFPS)
		//{
		//	myTimer = 0;
		//	myCurrentFrame++;
		//	if(myCurrentFrame >= GetAnimation()->Length)
		//	{
		//		if(isLooping)
		//		{
		//			myCurrentFrame = 1;
		//		}
		//		else
		//		{
		//			myCurrentFrame--; // Step back to avoid overflow, just hold the last frame.
		//			myState = AnimationState::Finished;
		//		}
		//	}

		//	// Plow through all the matrices to find the relative transform for each bone.
		//	Matrix4x4f identity = Matrix4x4f::CreateIdentityMatrix();
		//	UpdateAnimation(aDeltaTime, 0, identity, &myFrameTransforms[0]);
		//}
	}
}

void AnimatedModelInstance::AnimationInstance::Play()
{
	myState = AnimationState::Playing;
}

void AnimatedModelInstance::AnimationInstance::Pause()
{
	myState = AnimationState::Paused;
}

void AnimatedModelInstance::AnimationInstance::Stop()
{
	myState = AnimationState::Stopped;
	myCurrentFrame = 0;
}

void AnimatedModelInstance::AnimationInstance::SetIsLooping(bool shouldLoop)
{
	isLooping = shouldLoop;
}

void AnimatedModelInstance::AnimationInstance::SetFramesPerSecond(float someFPS)
{
	myFPS = someFPS;
}

void AnimatedModelInstance::AnimationInstance::SetFramePosition(unsigned aFramePosition)
{
	if(aFramePosition < GetAnimation()->Length)
		myCurrentFrame = aFramePosition;
}

AnimatedModelInstance::AnimatedModelInstance()
{

}

AnimatedModelInstance::~AnimatedModelInstance()
{
	
}

void AnimatedModelInstance::Init(std::shared_ptr<Model> aModel)
{
	myModel = aModel;
	if(myModel->GetNumAnimations() > 0)
	{
		for(size_t i = 0; i < myModel->GetNumAnimations(); i++)
		{
			AnimationInstance animInstance;
			animInstance.Init(i, myModel->GetSkeleton());
			myAnimationInstances.push_back(animInstance);
		}
	}
}

bool AnimatedModelInstance::InitAnimations(const std::vector<AnimationImportDefinition>& someAnimImportDefs)
{
	if(someAnimImportDefs.empty())
		return false;

	for(const AnimationImportDefinition& def : someAnimImportDefs)
	{
		// TODO: This needs to bubble up its result as return value.
		myModel->LoadAnimation(def);
	}

	myAnimationInstances.clear();

	// After this is done we need to instantiate all animations.
	if(myModel->GetNumAnimations() > 0)
	{
		for(size_t i = 0; i < myModel->GetNumAnimations(); i++)
		{
			AnimationInstance animInstance;
			animInstance.Init(i, myModel->GetSkeleton());
			myAnimationInstances.push_back(animInstance);
		}
	}

	return true;
}

void AnimatedModelInstance::SetTransform(const Transform& someTransform)
{
	myTransform = someTransform;
}

void AnimatedModelInstance::SetRotation(Rotator someRotation)
{
	// Really should unroll rotations as well somewhere
	// so we can use -180 to 180 instead of 0 to 360.
	myTransform.SetRotation(someRotation);
}

void AnimatedModelInstance::SetLocation(Vector3f someLocation)
{
	myTransform.SetPosition(someLocation);
}

void AnimatedModelInstance::SetScale(Vector3f someScale)
{
	myTransform.SetScale(someScale);
}

void AnimatedModelInstance::PlayAnimation(unsigned anIndex)
{
	if(myCurrentAnimationIndex >= 0 && !myAnimationInstances.empty())
	{
		if(myCurrentAnimationIndex != static_cast<int>(anIndex))
		{
			StopAnimation();
		}
	}

	if(anIndex < myAnimationInstances.size())
	{
		myCurrentAnimationIndex = static_cast<int>(anIndex);
		myAnimationInstances[myCurrentAnimationIndex].Play();
	}
}

void AnimatedModelInstance::PlayAnimation(const std::wstring& anAnimationName)
{
	for(size_t a = 0; a < myAnimationInstances.size(); a++)
	{
		// TODO: Should be an acceleration map for the instances later on.
		if(myAnimationInstances[a].GetAnimation()->Name == anAnimationName)
		{
			myCurrentAnimationIndex = static_cast<int>(a);
			break;
		}
	}
}

void AnimatedModelInstance::PauseAnimation()
{
	if(myCurrentAnimationIndex >= 0 && !myAnimationInstances.empty())
	{
		myAnimationInstances[myCurrentAnimationIndex].Pause();
	}
}

void AnimatedModelInstance::StopAnimation()
{
	if(myCurrentAnimationIndex >= 0 && !myAnimationInstances.empty())
	{
		myAnimationInstances[myCurrentAnimationIndex].Stop();
	}
}

void AnimatedModelInstance::RefreshAnimationInstances()
{
	std::vector<AnimationInstance> myOldAnimationInstances = myAnimationInstances;
	myAnimationInstances.clear();

	if(myModel->GetNumAnimations() > 0)
	{
		for(size_t i = 0; i < myModel->GetNumAnimations(); i++)
		{
			bool bFound = false;
			for(size_t e = 0; e < myOldAnimationInstances.size(); e++)
			{
				if(myOldAnimationInstances[e].GetAnimation()->Name == myModel->GetSkeleton()->myAnimations[i].Name)
				{
					// Re-use existing definition.
					myAnimationInstances.push_back(myOldAnimationInstances[e]);
					bFound = true;
					break;
				}
			}

			if(!bFound)
			{
				AnimationInstance animInstance;
				animInstance.Init(i, myModel->GetSkeleton());
				myAnimationInstances.push_back(animInstance);
			}
		}
	}
}

ModelStatus AnimatedModelInstance::Update(float aDelta)
{
	// This isn't a good place to do this but I have no ideas right now.
	// I feel the modelfactory should keep a multimap of all instances or something
	// since some instances, such as animated ones, need local copies of certain data.
	// I.e. we don't want all animated instances of the same model to play the same animation.
	if(GetAnimationCount() != myModel->GetNumAnimations())
	{
		RefreshAnimationInstances();
	}

	ModelStatus status = ModelStatus::Nothing;

	if(myCurrentAnimationIndex > -1 && myCurrentAnimationIndex < myAnimationInstances.size())
	{
		AnimationInstance& animInstance = myAnimationInstances[myCurrentAnimationIndex];
		animInstance.Update(aDelta);

		const std::vector<Matrix4x4f>& frameTransforms = animInstance.GetBoneTransforms();
		memcpy(&myBones[0], frameTransforms.data(), sizeof(Matrix4x4f) * frameTransforms.size());
	}
	else
	{
		// if there is no animation to play, reset to default transforms
		for (int i = 0; i < MAX_ANIMATION_BONES; i++)
			myBones[i] = Matrix4x4f();
	}

	return status;
}

void Tga2D::AnimatedModelInstance::SetCurrentAnimationIndex(int anIndex)
{
	if (myAnimationInstances.empty())
	{
		ERROR_PRINT("%s", "SetCurrentAnimationIndex: no animation is loaded");
		return;
	}

	if(anIndex < static_cast<int>(myAnimationInstances.size()))
	{
		myCurrentAnimationIndex = anIndex;
	}
}

void Tga2D::AnimatedModelInstance::SetCurrentAnimationTime(unsigned int anAnimationFrame)
{
	if (myAnimationInstances.empty())
	{
		ERROR_PRINT("%s", "SetCurrentAnimationTime: no animation is loaded");
		return;
	}

	if(myCurrentAnimationIndex < 0)
	{
		ERROR_PRINT("%s", "SetCurrentAnimationTime: Not currently playing any animations.");
		return;
	}

	if(anAnimationFrame < myAnimationInstances[myCurrentAnimationIndex].GetAnimation()->Length)
	{
		myAnimationInstances[myCurrentAnimationIndex].SetFramePosition(anAnimationFrame);
	}

}

// Disabled right now, intent on fixing animations for SP21 ASAP.
//void Tga2D::AnimatedModelInstance::RegisterAnimationEventCallback(const std::string& aEvent, std::function<void()> aFunctionToCall)
//{
//	aiString eventString(aEvent);
//	myEventCallbacks[myAnimation->controller->m_EventMapping[eventString]].push_back(aFunctionToCall);
//}

void AnimatedModelInstance::Render(ModelShader& shader) const
{
	const std::vector<Model::MeshData>& meshData = myModel->GetMeshDataList();

	for (int j = 0; j < meshData.size(); j++)
	{
		shader.Render(*this, myTextures[j], meshData[j], myTransform.GetMatrix(), myBones);
	}
}

// Disabled right now, intent on fixing animations for SP21 ASAP.
//void AnimatedModelInstance::TriggerEvents(int aAnimationIndex, float aAtWhatTime)
//{
//	int eventFrameIndex = (int)aAtWhatTime;
//	if (myPrevEventTickTime[aAnimationIndex] == eventFrameIndex)
//		return;
//
//	myPrevEventTickTime[aAnimationIndex] = eventFrameIndex;
//
//	for (auto& it : myAnimation->controller->myEventFrames[aAnimationIndex][eventFrameIndex].myEvents)
//	{
//		if (it.second.isTriggerFrame)
//		{
//			for (auto& listener : myEventCallbacks[it.first])
//			{
//				listener();
//			}
//		}
//	}
//}
