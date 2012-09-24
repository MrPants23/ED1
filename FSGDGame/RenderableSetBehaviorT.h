#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/RenderShape.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDGOCA/GameObject.h"
#include "../EDCollision/CollisionLibrary.h"
#include "../EDRendererD3D/DebugRenderer.h"

#include "../EDRendererD3D/ViewPortManager.h"
using namespace EDRendererD3D;

using namespace EDGOCA;
using namespace EDRendererD3D;
using namespace EDCollision;

const UINT globalNumLODRadii = 2;
const float globalLODRadii[globalNumLODRadii] = { 0.01f, 0.005f };
const float globalHysteresis = 0.001f;

namespace Behaviors
{
	typedef unsigned int RenderShapeKey;

	typedef vector< string > RenderShapeLODPaths;
	typedef list< pair< RenderShapeLODPaths, TransformKey > > RenderShapePathList;

	typedef list< pair< RenderShape *, TransformKey > > RenderShapeList;

	template<class DerivedRenderable>
	class RenderableSetBehaviorT : public BehaviorT<DerivedRenderable>
	{
		RenderShapeList renderShapeList;

	public:
		RenderableSetBehaviorT(void) {}

		virtual ~RenderableSetBehaviorT(void)
		{
			while( !renderShapeList.empty() )
			{
				delete renderShapeList.front().first;

				renderShapeList.pop_front();
			}
		}

		static void OnSetRenderShapeList( IBehavior* invokingBehavior, IMessage* iMsg )
		{
			RenderableSetBehaviorT* pThis = (RenderableSetBehaviorT*)invokingBehavior;

			MessageT< RenderShapePathList* >* message = (MessageT< RenderShapePathList* >*)iMsg;

			while( !pThis->renderShapeList.empty() )
			{
				delete pThis->renderShapeList.front().first;

				pThis->renderShapeList.pop_front();
			}
			
			RenderShapePathList* renderShapePathList = message->value;
			
			RenderShapePathList::iterator shapePathIter = renderShapePathList->begin();
			for(; shapePathIter != renderShapePathList->end(); ++shapePathIter )
			{
				std::vector< ContentHandle<RenderForm> > formVector;
				for(unsigned int i = 0; i < shapePathIter->first.size(); ++i)
				{
					ContentHandle<RenderForm> formHandle = ContentManager::LoadXML<RenderForm>( shapePathIter->first[i].c_str() );
					
					if( formHandle.GetContent() != 0 )
						formVector.push_back( formHandle );
				}

				if( !formVector.empty() )
				{
					pair< RenderShape*, TransformKey > newShapePair;
					newShapePair.second = shapePathIter->second;

					newShapePair.first = (RenderShape*)formVector[0].GetContent()->GetNewRenderShape();
					newShapePair.first->Initialize( formVector );
					newShapePair.first->SetWorldMatrix( (D3DXMATRIX*)&pThis->gameObject->GetWorldTransform( shapePathIter->second ) );

					for(unsigned int i = 0; i < formVector.size(); ++i )
					{
						RenderContext* pContext = newShapePair.first->GetRenderContext(i);
						RenderController::GetInstance()->AddRenderContext( pContext );
					}

					pThis->renderShapeList.push_back( newShapePair );
				}
			}

			pThis->gameObject->BuildLocalAabb();
		}

		static void OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg )
		{
			RenderableSetBehaviorT* pThis = (RenderableSetBehaviorT*)pInvokingBehavior;
			MessageT<FrustumHull*>* message = (MessageT<FrustumHull*>*)msg;

			Aabb tempAabb = pThis->gameObject->GetAabb();

			if( message->value->Collides( &tempAabb ) )
			{
				SetLOD(pThis->renderShapeList, *pThis->gameObject);
			}
		}
		static void SetLODSolution( RenderShapeList &shapeList, GameObject &gameObject);

		static void SetLOD( RenderShapeList &shapeList, GameObject &gameObject)
		{
			SetLODSolution(shapeList, gameObject);
		}

		static void OnBuildLocalAabb( IBehavior* invokingBehavior, IMessage* msg )
		{
			RenderableSetBehaviorT* pThis = (RenderableSetBehaviorT*)invokingBehavior;
			MessageT<Aabb*>* message = (MessageT<Aabb*>*)msg;

			RenderShapeList::iterator shapeListIter = pThis->renderShapeList.begin();
			for(; shapeListIter != pThis->renderShapeList.end(); ++shapeListIter)
			{
				Aabb tempAabb = shapeListIter->first->GetRenderMesh()->GetAabb();

				for(unsigned int i = 0; i < 3; ++i)
				{
					if( message->value->min.v[i] > tempAabb.min.v[i] )
						message->value->min.v[i] = tempAabb.min.v[i];
					if( message->value->max.v[i] < tempAabb.max.v[i] )
						message->value->max.v[i] = tempAabb.max.v[i];
				}
				//Sphere tempSphere = shapeListIter->first->GetLocalSphere();

				//const float4x4& xform = pThis->gameObject->GetLocalTransform( shapeListIter->second );

				//if( xform != pThis->gameObject->GetLocalTransform() )
				//	tempSphere.Transform( xform );

				//for(unsigned int i = 0; i < 3; ++i)
				//{
				//	if( message->value->min.v[i] > tempSphere.center.v[i] - tempSphere.radius )
				//		message->value->min.v[i] = tempSphere.center.v[i] - tempSphere.radius;

				//	if( message->value->max.v[i] < tempSphere.center.v[i] + tempSphere.radius )
				//		message->value->max.v[i] = tempSphere.center.v[i] + tempSphere.radius;
				//}
			}
		}

		static void OnGetRenderableCount( IBehavior* pInvokingBehavior, IMessage* msg )
		{
			MessageT<unsigned int>* message = (MessageT<unsigned int>*)msg;
			message->value += 1;
		}
	};
}