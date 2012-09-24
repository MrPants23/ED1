#pragma once

#include <vector>
using std::vector;

#include "..\EDRendererD3D\RenderShapeTarget.h"

#include "..\EDUtilities\float4x4.h"
using EDUtilities::float4x4;

namespace EDRendererD3D
{
	class ViewPortManager
	{
		class ViewPort
		{
		public:
			/// The concatenated view and projection matrices to be used with this view port
			float4x4 viewProjection;
			float4x4 view;
			float4x4 projection;
			float4x4 world;
			float nearClip;
			float farClip;
			RenderShapeTarget viewPortQuad;
		};
	public:
		~ViewPortManager(void);

		/// Gets the view projection matrix of the view port that is currently active.
		/// \return Returns the currently active viewport's view projection
		inline const float4x4 &GetActiveViewProjection() const 
		{
			return GetViewProjection(activePort);
		}

		/// Gets the view projection matrix of the view port whose index is passed in.
		/// \param port - the index to the view port to select
		/// \return Returns the selected viewport's view projection
		inline const float4x4 &GetViewProjection(UINT port) const 
		{
			return viewPorts[port].viewProjection;
		}

	    /// Gets the world matrix of the view port that is currently active.
		/// \return Returns the currently active viewport's world projection
		inline const float4x4 &GetActiveWorld() const 
		{
			return GetWorld(activePort);
		}

		/// Gets the world matrix of the view port whose index is passed in.
		/// \param port - the index to the view port to select
		/// \return Returns the selected viewport's world projection
		inline const float4x4 &GetWorld(UINT port) const 
		{
			return viewPorts[port].world;
		}

	    /// Gets the view matrix of the view port that is currently active.
		/// \return Returns the currently active viewport's view projection
		inline const float4x4 &GetActiveView() const 
		{
			return GetView(activePort);
		}

		/// Gets the view matrix of the view port whose index is passed in.
		/// \param port - the index to the view port to select
		/// \return Returns the selected viewport's view projection
		inline const float4x4 &GetView(UINT port) const 
		{
			return viewPorts[port].view;
		}

		/// Gets the projection matrix of the view port that is currently active.
		/// \return Returns the currently active viewport's view projection
		inline const float4x4 &GetActiveProjection() const 
		{
			return GetProjection(activePort);
		}

		/// Gets the projection matrix of the view port whose index is passed in.
		/// \param port - the index to the view port to select
		/// \return Returns the selected viewport's view projection
		inline const float4x4 &GetProjection(UINT port) const 
		{
			return viewPorts[port].projection;
		}

		/// Gets the position of the viewer for the active viewport
		/// \return Returns the position of the viewer
		inline const float3 &GetActiveViewPosition() { return GetPosition(activePort); }

		/// Gets the position of the viewer for the selected viewport
		/// \param port - the index to the view port to select
		/// \return Returns the position of the viewer
		inline const float3 &GetPosition(UINT port ) { return viewPorts[port].world.WAxis; }

		/// Gets the facing direction of the viewer for the active viewport
		/// \return Returns the facing direction of the viewer
		inline const float3 &GetActiveViewForward() { return GetForward(activePort); }

		/// Gets the facing direction of the viewer for the selected viewport
		/// \param port - the index to the view port to select
		/// \return Returns the facing direction of the viewer
		inline const float3 &GetForward(UINT port ) { return viewPorts[port].world.ZAxis; }

		/// Gets the near clip plane distance for the selected viewport
		/// \param port - the index to the view port to select
		/// \return Returns the near clip distance of the selected view port
		inline float GetNearClip(UINT port) { return viewPorts[port].nearClip; }

		/// Gets the near clip plane distance for the active viewport
		/// \return Returns the near clip distance of the active view port
		inline float GetActiveViewNearClip() { return GetNearClip(activePort); }

		/// Gets the far clip plane distance for the selected viewport
		/// \param port - the index to the view port to select
		/// \return Returns the far clip distance of the selected view port
		inline float GetFarClip(UINT port) { return viewPorts[port].farClip; }

		/// Gets the far clip plane distance for the active viewport
		/// \return Returns the far clip distance of the active view port
		inline float GetActiveViewFarClip() { return GetFarClip(activePort); }

		/// Gets the facing direction of the viewer for the active viewport
		/// \param port - the index to the view port to select
		/// \return Returns the facing direction of the viewer
		inline const float3 &GetActiveViewForward(UINT port ) { return viewPorts[port].world.ZAxis; }

		void UpdateViewPort(int port, const float4x4 &_world, const float4x4 &_view, const float4x4 &_projection, float _nearClip, float _farClip);
		void UpdateActiveViewPort(const float4x4 &_world, const float4x4 &_view, const float4x4 &_projection, float _nearClip, float _farClip);

		inline size_t GetNumViewPorts() { return viewPorts.size(); }
		inline void SetActiveViewPort(UINT _activePort) { activePort = _activePort; }
		/// \param _renderMeshPtr - The address of the mesh to be used while rendering this shape.
		/// \param _renderContextPtr - The Address of the context to be used while rendering this shape.
		/// \param _renderTargetPtr - The RenderTarget whose texture should be rendered
		/// \param matrix - Defines where on the screen the view port should be
		void AddViewPort(RenderMesh *_renderMeshPtr, RenderContext *_renderContextPtr, 
			RenderTarget *_renderTargetPtr, const float4x4 &matrix);

		void RenderActiveViewPort();

		static ViewPortManager &GetReference();
		static void DeleteInstance();
	private:
		vector<ViewPort> viewPorts;

		UINT activePort;

		ViewPortManager(void);
		ViewPortManager(const ViewPortManager&);
		ViewPortManager &operator=(const ViewPortManager &);

		static ViewPortManager *instancePtr;

		void CalcViewProjection(UINT port)
		{
			Multiply(viewPorts[port].viewProjection, viewPorts[port].view, viewPorts[port].projection);
		}

	};
}
