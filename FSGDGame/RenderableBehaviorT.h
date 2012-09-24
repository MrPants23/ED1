#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/RenderShape.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"

using namespace EDGOCA;
using namespace EDRendererD3D;

namespace Behaviors
{
	template<class DerivedRenderable>
	class RenderableBehaviorT : public BehaviorT<DerivedRenderable>
	{
		RenderShapeBase* renderShape;

	public:
		RenderableBehaviorT(void) : renderShape(0) {}

		virtual ~RenderableBehaviorT(void)
		{
			delete renderShape;
		}

		static void OnSetRenderShape( IBehavior* pInvokingBehavior, IMessage* msg )
		{
			RenderableBehaviorT* pRenderable = (RenderableBehaviorT*)pInvokingBehavior;
			MessageT< pair<BehaviorNameKey, string> >* message = (MessageT< pair<BehaviorNameKey, string> >*)msg;

			if( message->value.first != pRenderable->GetBehaviorNameKey() )
				return;

			if( pRenderable->renderShape != 0 )
			{
				delete pRenderable->renderShape;
				pRenderable->renderShape = 0;
			}

			if( message->value.second.length() != 0 )
			{
				std::vector< ContentHandle<RenderForm> > formVector;
				ContentHandle<RenderForm> formHandle = ContentManager::LoadXML<RenderForm>( message->value.second.c_str() );

				if( formHandle.GetContent() != 0 )
				{
					formVector.push_back( formHandle );
				
					pRenderable->renderShape = formHandle.GetContent()->GetNewRenderShape();
					pRenderable->renderShape->Initialize( formVector );
					pRenderable->renderShape->SetWorldMatrix( (D3DXMATRIX*)&pRenderable->gameObject->GetWorldTransform() );

					RenderContext* pContext = pRenderable->renderShape->GetRenderContext();
					RenderController::GetInstance()->AddRenderContext( pContext );
				}
			}

			pRenderable->gameObject->BuildLocalAabb();
		}

		static void OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg )
		{
			RenderableBehaviorT* pRenderable = (RenderableBehaviorT*)pInvokingBehavior;
			MessageT<FrustumHull*>* message = (MessageT<FrustumHull*>*)msg;

			if( pRenderable->renderShape == 0 )
				return;

			Aabb tempAabb = pRenderable->gameObject->GetAabb();
			if( message->value->Collides( &tempAabb ) )
			{
				pRenderable->renderShape->SetWorldMatrix( (D3DXMATRIX*)&pRenderable->gameObject->GetWorldTransform() );
				pRenderable->renderShape->AddToContextSet();
			}
		}

		static void OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg )
		{
			RenderableBehaviorT* pRenderable = (Renderable*)pInvokingBehavior;
			MessageT<Aabb*>* message = (MessageT<Aabb*>*)msg;

			if( pRenderable->renderShape == 0 )
				return;

			const Aabb& localAabb = pRenderable->renderShape->GetRenderMesh()->GetAabb();
			for(unsigned int i = 0; i < 3; ++i)
			{
				if( message->value->min.v[i] > localAabb.min.v[i] )
					message->value->min.v[i] = localAabb.min.v[i];
				if( message->value->max.v[i] < localAabb.max.v[i] )
					message->value->max.v[i] = localAabb.max.v[i];
			}
		}

		static void OnGetRenderableCount( IBehavior* pInvokingBehavior, IMessage* msg )
		{
			MessageT<unsigned int>* message = (MessageT<unsigned int>*)msg;
			message->value += 1;
		}
	};
}