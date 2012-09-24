#pragma once 

#include "../EDRendererD3D/RenderContextWithTexture.h"
#include "../EDRendererD3D/DirectionalLightWithShadow.h"
#include "../EDRendererD3D/PointLightWithShadow.h"
#include "../EDRendererD3D/SpotLightWithShadow.h"
#include "../EDRendererD3D/RenderMesh.h"
#include "../EDRendererD3D/RenderShapeWithTexture.h"
#include "../EDRendererD3D/RenderShapeTarget.h"
#include "../EDRendererD3D/RenderTarget.h"
#include "../EDRendererD3D/CubeRenderTarget.h"
#include "../EDRendererD3D/RenderModel.h"
#include "../EDRendererD3D/PostProcess.h"
#include "../EDRendererD3D/PostProcessWater.h"
#include "../EDRendererD3D/VisibilityQueryInterface.h"
using namespace EDRendererD3D;

#include "../EDUtilities/ContentManager.h"

#include "PostProcessChain.h"

/// Container class for non-data driven rendering components.
class RenderController
{
private:
    /// RenderContext to be used for rendering the 2D objects.
    /// This context is added to the RenderContext Manager with the name 2DContext.
    RenderContext m_2DContext;

    /// A render context used to render directional lights in a deferred renderer.
	ContentHandle<RenderContext> m_DirectionDeferredContextHandle;
    /// A render context used to render point lights in a deferred renderer.
	ContentHandle<RenderContext> m_PointLightContextHandle;
    /// A render context used to render spot lights in a deferred renderer.
	ContentHandle<RenderContext> m_SpotLightContextHandle;

	/// Temporary GUI context handle
	ContentHandle<RenderContext> m_GUIContextHandle;

	ContentHandle<RenderContext> debugLines;

    /// A render context that will be used to set shared parameters.
    RenderContext m_SharedParameterContext;

    /// A collection of contexts that hold data that should cast shadows.
    list<RenderContext *> m_ShadowCasters;
    /// A collection of contexts that hold data to be rendered to basic G-Buffer
    list<RenderContext *> m_GBufferContexts;
    /// A collection of contexts that hold data to be rendered to depth target
    list<RenderContext *> m_DepthCameraContexts;
    /// A collection of contexts that hold data to be rendered without lighting 
    /// during the forward rendering pass
	list<RenderContext *> m_UnlitOpaqueContexts;
	/// A collection of contexts used to render GUI elements
	list<RenderContext *> m_GUIContexts;

	RenderContext* m_pTransparentContext;

    /// The texture to store depth values written by the m_DepthContext.
    RenderTexture *m_pShadowRenderTarget; // currently unused

	/// A directional light instance.
	DirectionalLightWithShadow m_DirectionalLight;
    /// The concatenated view projection of the directional light, 
    /// needed for shadow mapping
    D3DXMATRIX m_DirViewProjection;

	/// The point lights.
    //list<PointLightWithShadow > m_PointLights;

	set<PointLightWithShadow*> visiblePointLightsWithShadow;

    /// A spot lights.
	list<SpotLightWithShadow > m_SpotLights;

	set<SpotLightWithShadow*> visibleSpotLightsWithShadow;

    /// The depth and stencil surface used for point light shadow mapping.
	IDirect3DSurface9 *m_pPointDSShadow;
    const static int thePointShadowSize = 256;

    IDirect3DSurface9 *m_pDirectionalDSShadow;
    const static int theDirectionalShadowSize = 2048;

    IDirect3DSurface9 *m_pSpotDSShadow;
    const static int theSpotShadowSize = 256;

	/// The depth and stencil surface used for deferred rendering G buffers.
	IDirect3DSurface9 *m_pDSGBuffers;

    /// The render target used to render depth values.
    RenderTarget m_GDepthRenderTarget;
    /// The render target used to render diffuse color values.
    RenderTarget m_GDiffuseRenderTarget;
    /// The render target used to render normals.
    RenderTarget m_GNormalRenderTarget;
	/// The render target used for specular color values.
    RenderTarget m_GSpecularRenderTarget;

	/// The render target for rendering all spot light shadow depths
	RenderTarget m_SpotDepths;
    /// A quad mesh that encomposes the screen, in clip space.
    RenderMesh *screenQuadMeshPtr;
    /// A quad mesh that encomposes one quarter of the screen, in clip space.
    RenderMesh quarterScreenQuadMesh;
    /// A shape used to define a full screen render target.
    RenderShapeTarget m_ScreenQuad;
    //char m_ScreenQuadldkhqshldf;

    /// The projection matrix to be used to represent the light space of the 
    /// directional light.
    float4x4 m_LightProjection;
    float4x4 m_LightView;

	PostProcess m_EdgePostProcess;
	PostProcess m_VerticalBlurPostProcess;
	PostProcess m_HorizontalBlurPostProcess;
	PostProcess m_DepthOfFieldPostProcess;
	PostProcess m_DownSampleX4PostProcess;
	PostProcess m_UpSampleX4PostProcess;
	PostProcess m_BrightPostProcess;
	PostProcess m_VerticalBloomPostProcess;
	PostProcess m_HorizontalBloomPostProcess;
	PostProcess m_CombinePostProcess;

	PostProcess m_PostTestProcess;

	PostProcess m_ToneX2PostProcess;
	PostProcess m_ToneX3PostProcess;
	PostProcess m_ColorEdgeCombinePostProcess;
	PostProcess m_ToneMappingPostProcess;

	PostProcess m_FXAAPostProcess;

	PostProcessWater m_WaterPostProcess;

	list<PostProcessChain *> m_PostProcessChains;

	RenderTarget m_SceneTarget;

    /// A helper method used to initialize RenderContexts.
    /// This should eventually be data driven.
	void CreateContexts();
    /// A helper method used to initialize lights.
    /// This should eventually be data driven.
    void CreateLights();

	void CreateGBuffers();

	void CreateDepthBuffers();

	void CreateSceneTargetsAndViewPorts(UINT viewPorts);

    /// A helper method used to organize rendering camera depth values.
    void RenderCameraDepths();
    /// A helper method used to organize rendering shadow depth values.
	void RenderShadowCasters(RenderContext::RenderStage stage);

    /// Renders shadow depths for the spot light.
    void RenderSpotDepths(SpotLightWithShadow *lightPtr, ID3DXEffect *effectPtr);
    /// Renders shadow depths for the point lights.
    void RenderPointDepths(PointLightWithShadow *lightPtr, ID3DXEffect *effectPtr);
    /// Renders shadow depths for the directional light.
    void RenderDirectionalDepths();

    /// Renders the depth values to a texture target.
	void RenderGDepths();
    /// Renders the G-Buffer values to a texture target.
	void RenderGBuffers();
    /// Renders the deferred lights and processes any forward rendering to the
    /// final render target which will be copied to the screen
    void RenderLightsAndForwardPass();
    /// Initializes blur values
    void SetBlurConstants();
	/// Sets offset values for up and down sampling based on resolution.
	/// This method will need to be called any time the resolution changes.
	void SetDownSampleKernel();

	/// A orginizational method to handle console command creation
	void InitializeConsoleCommands();

	RenderController();
	RenderController(const RenderController&){}
	RenderController& operator=(const RenderController&){ return *this; }

	static RenderController* instancePtr;

	/// RenderTarget for Occlusion/Visibility Queries
	RenderTarget visibilityTestTarget;

	RenderShapeTarget debugBufferShape;

	bool shouldRenderDebugBuffer;
	unsigned int debugBufferToRender;
public:						
	enum GBufferEnum{GB_Depth, GB_Diffuse, GB_Normal, GB_Specular, GB_DirShadow};

	~RenderController();

    RenderModel* m_pTestModel;

	/// Initializes the contexts, textures and lights
	/// \param viewPorts - The number of view ports to use
	void Initialize(UINT viewPorts);
    /// Renders all of the RenderContext instances
	void Render();

    //EDUtilities::Frame &GetTheTestPointLightFrame() { return m_PointLights.front().GetFrame(); }
    //EDUtilities::Frame &GetTheTestSpotLightFrame() { return m_SpotLights.front().GetFrame(); }

    /// Called each time the view changes to build the final view projection
    /// matrix used for the directional light shadow maps.
    void BuildDirViewProjection(const FrustumHull &frustumHull);
    /// Method to create the initial view and projection matrices to be used 
    /// for the directional light. This method only needs to be called when 
    /// the frustum changes size.
    void BuildInitialiDirMatrices(const FrustumHull &frustumHull);

	static RenderController* GetInstance()
	{
		if( instancePtr == 0 )
			instancePtr = new RenderController;

		return instancePtr;
	}

	static void DeleteInstance(void)
	{
		delete instancePtr;
		instancePtr = 0;
	}

	void AddRenderContext(RenderContext* pContext);
	void CreatePostProcessEffects();

	//////////////////////////////////////////////////////////////////////////
	/// These methods are for console command handling and are defined 
	/// in RenderControllerCommands.cpp
    void SetDepthOffset(float val);
    void SetEdgeIntensity(float val);
    void SetEdgeWidth(float val);
	void SetFocusDistance(float val);
	void SetFocusRange(float val);
    void SetShadowDepthOffset(float val);
    void SetLumGrey(float val);
    void SetLumWhite(float val);

	void AddDepthOfFieldPostProcess();
	void AddDrawEdgesPostProcess();
	void AddGaussianBlurHPostProcess();
	void AddGaussianBlurVPostProcess();
	void AddDownSamplePostProcess();
	void AddUpSamplePostProcess();
	void AddBloomHPostProcess();
	void AddBloomVPostProcess();
	void AddBrightPassPostProcess();
	void AddCombinePostProcess();
	void AddBloomPostProcess();
	void AddToneCalcPostProcess();
	void AddToneMappingPostProcess();
	void AddAntiAliasingPostProcess();
	void AddFXAAPostProcess();
	void AddWaterPostProcess();
	void AddChainPostProcess();
	void TestSamplingPostProcess();
	void AddPostTestProcess();
	void ClearPostProcessChain();

	// CS - GUI commands
	void ResetGUI();

	/// Adds the passed in post process to the end of the last
	/// post process chain, or if there is none creates one.
	void PushBackPostProcess(PostProcess *process);

	/// Removes the passed in post effect from all chains it is found in.
	void RemovePostProcess(PostProcess *process);

    /// Lab solution method, ignore this
	void RenderGDepthsSolution();
    /// Lab solution method, ignore this
	void RenderGBuffersSolution();
    /// Lab solution method, ignore this
    void RenderLightsAndForwardPassSolution();
    /// Lab solution method, ignore this
    void BuildInitialiDirMatricesSolution(const FrustumHull &frustumHull);
    /// Lab solution method, ignore this
    void BuildDirViewProjectionSolution(const FrustumHull &frustumHull);

	/// Initializes a VisibilityQueryInterface instance, using a single RenderTarget
	/// for all queries and the depth buffer used for general rendering
	/// \param query - The VisibilityQueryInterface to initialize
	void InitializeAVisibilityQuery(VisibilityQueryInterface &query);

	/// Returns a mesh representing the screen
	inline RenderMesh *GetScreenQuadPtr() { return screenQuadMeshPtr; }
	/// Returns the context used to render main scene data
	inline RenderContext *GetScreenContextPtr() { return &m_2DContext; }
	/// Returns the shape used for rendering screen data
	inline RenderShapeTarget *GetScreenShapePtr() { return &m_ScreenQuad; }

	inline DirectionalLight *GetDirLight() { return &m_DirectionalLight; }
	inline RenderContext *GetDirLightContext() { return m_DirectionDeferredContextHandle.GetContent(); }

	//inline list<PointLightWithShadow> & GetPointLights() { return m_PointLights; }
	inline RenderContext *GetPointLightContext() { return m_PointLightContextHandle.GetContent(); }

	inline list<SpotLightWithShadow> & GetSpotLights() { return m_SpotLights; }
	inline RenderContext *GetSpotLightContext() { return m_SpotLightContextHandle.GetContent(); }

	void AddVisibleLight(PointLightWithShadow* light);
	void AddVisibleLight(SpotLightWithShadow* light);

	static inline int GetPointShadowSize(void){ return thePointShadowSize; }
	static inline int GetSpotShadowSize(void){ return theSpotShadowSize; }

	RenderTarget &GetDirectionalShadowRenderTarget() { return m_DirectionalLight.GetRenderTarget(); }
	RenderTarget &GetSpotShadowRenderTarget() { return m_SpotDepths; }

	inline void ToggleDebugRender() { shouldRenderDebugBuffer = !shouldRenderDebugBuffer; }
	inline void SetDebugBufferToRender(unsigned int index) { debugBufferToRender = index; }

	void InitializeDebugBuffers();
	void RenderDebugBuffers();
};