#include "SpotLightBehavior.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDGOCA/GameObject.h"
#include "../EDCollision/CollisionLibrary.h"

#include "../EDRendererD3D/RenderModel.h"
#include "../EDRendererD3D/RenderShape.h"

#include "../EDRendererD3D/DebugRenderer.h"
#include "RenderController.h"

using namespace EDRendererD3D;

using namespace EDCollision;

namespace
{
	DeclareMessage( MSG_FRUSTUM_TEST );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
	DeclareMessage( MSG_GET_RENDERABLE_COUNT );
}

namespace Behaviors
{
	SpotLightBehavior::SpotLightBehavior(void)
	{
		lightPtr = new SpotLightWithShadow();
		lightPtr->SetName("SpotLight0");
		lightPtr->LoadModel( "GDModel/SpotLight/mdl_SpotLight.xml" );
		lightPtr->SetCutOff(.98f);
		lightPtr->SetExponent(1);
		lightPtr->SetDiffuseColor(&D3DXVECTOR4(1, 1, 1, 1));
		lightPtr->SetAmbientColor(&D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1));
		lightPtr->SetEnabled(true);
		lightPtr->SetAttenuation(&D3DXVECTOR3(1.0f, 0.0125f, 0.0125f), 25);
		lightPtr->SetSpecularColor(&D3DXVECTOR4(1, 1, 1, 1));
		lightPtr->SetSpecularIntensity(3.0f);
		lightPtr->SetSpecularPower(36.0f);
		//lightPtr->CreateRenderTarget(RenderController::GetSpotShadowSize(), RenderController::GetSpotShadowSize());
		lightPtr->SetRenderTarget(&RenderController::GetInstance()->GetSpotShadowRenderTarget());
	}

	void SpotLightBehavior::OnAdd(void)
	{
		this->gameObject->BuildLocalAabb();
	}

	void SpotLightBehavior::OnRemove(void)
	{
	}

	SpotLightBehavior::~SpotLightBehavior(void)
	{
		delete lightPtr;
	}

	void SpotLightBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MSG_FRUSTUM_TEST, OnFrustumTest );
		AddMessageHandler( MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb );
		AddMessageHandler( MSG_GET_RENDERABLE_COUNT, OnGetRenderableCount );
	}

	void SpotLightBehavior::OnGetRenderableCount( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		MessageT<unsigned int>* message = (MessageT<unsigned int>*)msg;
		message->value += 1;
	}

	void SpotLightBehavior::OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		SpotLightBehavior* lightBehaviorPtr = (SpotLightBehavior*)pInvokingBehavior;

		if( lightBehaviorPtr->lightPtr == 0 )
			return;

		MessageT<FrustumHull*>* message = (MessageT<FrustumHull*>*)msg;

		Aabb tempAabb = lightBehaviorPtr->gameObject->GetAabb();

		if( message->value->Collides( &tempAabb ) )
		{
			//DebugRenderer::GetInstance()->DrawAabb( tempAabb );

			const D3DXVECTOR3& attenuation = lightBehaviorPtr->lightPtr->GetAttenuation();

			float adjacent = (-attenuation.y + sqrt( attenuation.y * attenuation.y 
				- 4 * attenuation.z * (attenuation.x - 255.f))) / (2 * attenuation.z);

			float radAngle = acos(lightBehaviorPtr->lightPtr->GetCutOff());
			float tanr = tan(radAngle);
			float opposite = tanr * adjacent;

			float4x4 newMat(lightBehaviorPtr->gameObject->GetWorldTransform());
			newMat.XAxis *= opposite;
			newMat.YAxis *= opposite;
			newMat.ZAxis *= adjacent;

			*lightBehaviorPtr->lightPtr->GetWorldMatrixPtr() = *(D3DXMATRIX *)&newMat;

			RenderController::GetInstance()->AddVisibleLight(lightBehaviorPtr->lightPtr);
		}
	}

	void SpotLightBehavior::OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		SpotLightBehavior* lightBehaviorPtr = (SpotLightBehavior*)pInvokingBehavior;
		MessageT<Aabb*>* message = (MessageT<Aabb*>*)msg;

		// Determine the size of the aabb, including scaling of the mesh to be used for rendering
		Aabb localAabb = 
			lightBehaviorPtr->lightPtr->GetRenderModel()->GetShapes()[0]->GetRenderMesh()->GetAabb();

		const D3DXVECTOR3& attenuation = lightBehaviorPtr->lightPtr->GetAttenuation();

		float adjacent = (-attenuation.y + sqrt( attenuation.y * attenuation.y 
			- 4 * attenuation.z * (attenuation.x - 255.f))) / (2 * attenuation.z);

		float radAngle = acos(lightBehaviorPtr->lightPtr->GetCutOff());
		float tanr = tan(radAngle);
		float opposite = tanr * adjacent;
		
		float3 scale( opposite, opposite, adjacent );

		//localAabb.max = localAabb.max * (scale * 0.5f);
		//localAabb.min = localAabb.min * (scale * 0.5f);

		localAabb.max.x *= scale.x * 0.5f;
		localAabb.max.y *= scale.y * 0.5f;
		localAabb.max.z *= scale.z * 0.5f;

		localAabb.min.x *= scale.x * 0.5f;
		localAabb.min.y *= scale.y * 0.5f;
		localAabb.min.z *= scale.z * 0.5f;

		for(unsigned int i = 0; i < 3; ++i)
		{
			if( message->value->min.v[i] > localAabb.min.v[i] )
				message->value->min.v[i] = localAabb.min.v[i];
			if( message->value->max.v[i] < localAabb.max.v[i] )
				message->value->max.v[i] = localAabb.max.v[i];
		}
	}
}