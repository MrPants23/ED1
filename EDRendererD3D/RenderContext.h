#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

#include "RenderNode.h"
#include "RenderSetSorted.h"
#include "Light.h"
#include <string>
#include <vector>
#include "VertexFormats.h"

namespace EDRendererD3D
{
	class RenderShape;
	/// An object to define a collection of render states.
	/// The RenderContext class is a specialization of RenderNode that is used to set
	/// any render states that can be shared.  For the fastest rendering possible
	/// RenderNode batches should be sorted in a manner to have as few RenderContexts as possible
	class RenderContext : public RenderNode
	{
	public:
		/// Enumerated stages of rendering that a context might have individual techniques for.
		enum RenderStage{	RS_DEPTH_SPT_LIGHT = 0, RS_DEPTH_PNT_LIGHT0, RS_DEPTH_PNT_LIGHT1, 
							RS_DEPTH_PNT_LIGHT2, RS_DEPTH_PNT_LIGHT3, RS_DEPTH_PNT_LIGHT4, 
							RS_DEPTH_PNT_LIGHT5, RS_DEPTH_DIR_LIGHT, RS_DEPTH_CAMERA, RS_GBUFFER, 
							RS_SPT_LIGHT, RS_PNT_LIGHT, RS_DIR_LIGHT, RS_UNLIT, RS_TRANSPARENT, 
							RS_GUI, RS_MAX };
	protected:
		/// The shdaer effect used by this RenderContext
		ID3DXEffect	*effectPtr;

		/// The linked list of RenderNodes to be rendered using this context.
		/// This is typically filled just RenderShape instances.
		RenderSet *renderSetPtr;

		/// The handle to the model view projection matrix in the shader.
        D3DXHANDLE mvpMatrixHandle;
		/// The handle to the model matrix in the shader.
        D3DXHANDLE modelMatrixHandle;

        D3DXHANDLE viewMatrixHandle;
        D3DXHANDLE invViewProjMatrixHandle;

		/// The handle to the camera position variable in the shader.
        D3DXHANDLE cameraPositionHandle;

     public:
		/// The max number of diffuse textures to support per shape or context.
        const static int Max_Textures = 3;
     protected: 
     	/// Handles to the texture shader variables.  The shader variables are assumed to be named
		/// diffuseTexture0, diffuseTexture1, diffuseTexture2...ect.
        //D3DXHANDLE m_hDiffuse[Max_Textures];
        /// Handle to the ambient occlusion texture shader variables.
        D3DXHANDLE m_hAmbientMap;
        /// Handle to the diffuse texture shader variables.
        D3DXHANDLE m_hDiffuseMap;
        /// Handle to the normal map texture shader variables.
        D3DXHANDLE m_hNormalMap;
        /// Handle to the specular map texture shader variables.
        D3DXHANDLE m_hSpecularMap;
        /// Handle to the emissive map texture shader variables.
        D3DXHANDLE m_hEmissiveMap;
        /// Handle to the reflection map texture shader variables.
        D3DXHANDLE m_hReflectedMap;
		/// The name of the shader technique to use when rendering this context.
		std::string m_TechniqueName;

		/// The lights that are applied to this RenderContext.
        std::vector<Light *> m_pLights;

        /// The number of passes that exist for the current technique.  
        /// Set during render.
        UINT m_Passes;
        
        /// A RenderFunc to be used by other RenderContext RenderFuncs for shared functionality.
		/// Sets render states controlled by this context and renders the contained RenderSet.
		/// \param node - A RenderContext reference to render.
		static void ContextSharedRenderFunc(RenderNode &node);

		/// The vertex format used by this rendercontext
		VertexFormat m_VertexFormat;

		/// The technique names for each stage of rendering for this context
		std::string m_RenderStageTechnique[RS_MAX];

		/// The currently set render stage for this context, used as an index into m_RenderStageTechnique
		RenderStage m_CurrentStage;
	public:

		/// Assigns pointers to null and technique name to Basic
		RenderContext();
		/// Cleans up D3D components and dynamic memory.
		~RenderContext();

		RenderContext &operator=(RenderContext &&rhs)
		{
			if(this != &rhs)
			{
				m_TechniqueName = std::move( rhs.m_TechniqueName);
			}
            return *this;
		}

		/// Creates the effect to be used by this context.
		/// \param effectFile - a wide character string defining the name of the 
		/// shader to use for this effect.  A leading L makes an in place string
		/// into a wide character one, ex. L"SomeShader.fx"
		/// \return Returns a buffer to any errors that occured while compiling the shader.
		ID3DXBuffer * CreateEffect(LPCWSTR effectFile);

		/// Adds a light that should apply to this RenderContext.
		/// \param light - The light to be used with this context.
        void AddLight(Light *light) { m_pLights.push_back(light); }

		/// \return Returns the effect being used by this context.
		inline ID3DXEffect *GetEffect() const { return effectPtr; }

		/// Creates the RenderSet to be used by this context.
		/// \param bSorted  - If true creates a render set with sorting
		/// \param SortFunc - A function pointer to the method to be used for sorting.
		///                    This parameter is ignored if bSorted is false.
		inline void CreateRenderSet(bool bSorted = false, 
			bool (*SortFunc)(RenderShape *left, RenderShape *right) = 0)
		{
			if(bSorted)
			{
				renderSetPtr = new RenderSetSorted;
				((RenderSetSorted *)renderSetPtr)->SortFunc = SortFunc;
			}
			else
				renderSetPtr = new RenderSet;
		}
        inline UINT &GetPasses() { return m_Passes; }
		/// \return Returns a reference to the RenderSet being used by this context.
		inline RenderSet &GetRenderSet(void) { return *renderSetPtr; }
		/// Adds a RenderNode to the RenderSet of this context.  This should be called
		/// on all RenderNodes that should be rendered with this context each frame.
		/// \param nodePtr - The RenderNode to be rendered in this batch.
		inline void AddRenderNode(RenderNode *nodePtr)
		{
			renderSetPtr->AddRenderNode(nodePtr);
		}
		/// Clears the RenderSet of this context.
		inline void ClearRenderSet(void) { renderSetPtr->ClearRenderSet();}

		/// Sets the model view projection matrix used in the shader of the effect of this context.
		/// \param mvp - A pointer to the model view projection matrix
        inline void EffectSetMVP(const D3DXMATRIX *mvp) { effectPtr->SetMatrix(mvpMatrixHandle, mvp); }
		/// Sets the model matrix used in the shader of the effect of this context.
		/// \param mvp - A pointer to the model matrix
        inline void EffectSetWorld(const D3DXMATRIX *mvp) { effectPtr->SetMatrix(modelMatrixHandle, mvp); }

        inline void EffectSetAmbientMap(LPDIRECT3DBASETEXTURE9 texture) 
        { 
            effectPtr->SetTexture(m_hAmbientMap, texture); 
        }
        inline void EffectSetDiffuseMap(LPDIRECT3DBASETEXTURE9 texture) 
        { 
            effectPtr->SetTexture(m_hDiffuseMap, texture); 
        }
        inline void EffectSetSpecularMap(LPDIRECT3DBASETEXTURE9 texture) 
        { 
            effectPtr->SetTexture(m_hSpecularMap, texture); 
        }
        inline void EffectSetEmissiveMap(LPDIRECT3DBASETEXTURE9 texture) 
        { 
            effectPtr->SetTexture(m_hEmissiveMap, texture); 
        }
        inline void EffectSetNormalMap(LPDIRECT3DBASETEXTURE9 texture) 
        { 
            effectPtr->SetTexture(m_hNormalMap, texture); 
        }
        inline void EffectSetReflectedMap(LPDIRECT3DBASETEXTURE9 texture) 
        { 
            effectPtr->SetTexture(m_hReflectedMap, texture); 
        }
		/// Sets the technique to use during the next render.
		/// \param name - The new technique name.
		inline void SetTechniqueName(const char *name) { m_TechniqueName = name; }
		/// Sets the technique to use during the next render.
		/// \param name - The new technique name.
		inline void SetTechniqueName(const std::string &name) { m_TechniqueName = name; }
        
		inline std::vector<Light *> &GetLights() { return m_pLights; }
        /// A helper method used to retrieve and store shader variable handles.
        void InitializeHandles();
		
        /// Sets the technique name to use for a specific stage of rendering.
		/// \param stage - The rendering stage to set the technique name for.
		/// \param pTechniqueName - The name to assign to the rendering stage specified by stage.
		void SetRenderStageTechnique(RenderStage stage, const char* pTechniqueName)
		{
			m_RenderStageTechnique[stage] = pTechniqueName; 
		}

		/// Sets the current rendering stage of this RenderContext to the specified stage.
		/// \param stage - The rendering stage to set the current stage to.
		void SetRenderStage(RenderStage stage){ m_CurrentStage = stage; }

		/// \return Returns the name of the technique for the currently set rendering stage. Returns NULL if none is set.
		const char* GetRenderStageTechnique(void)
		{ 
			if(0 == m_RenderStageTechnique[m_CurrentStage].length())
				return 0;
			return  m_RenderStageTechnique[m_CurrentStage].c_str(); 
		}

		/// \return Returns the name of the technique for a specific rendering stage. Returns NULL if none is set.
		const char* GetRenderStageTechnique(RenderStage stage){ return ( m_RenderStageTechnique[stage].length() == 0 ? 0 : m_RenderStageTechnique[stage].c_str()); }

		/// A RenderFunc to be used by RenderContext instances.
		/// Sets render states controlled by this context and renders the contained RenderSet.
        /// Sets the stream to the buffer containing positions, normals and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPosNormalUVRenderFunc(RenderNode &node);

        /// A RenderFunc to be used by RenderContext instances.
		/// Sets render states controlled by this context and renders the contained RenderSet.
        /// Sets the stream to the buffer containing positions, normals and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPosNormalTangentUVRenderFunc(RenderNode &node);

        /// A RenderFunc to be used by RenderContext instances.
		/// Sets render states controlled by this context and renders the contained RenderSet.
        /// Sets the stream to the buffer containing only positions
		/// \param node - A RenderContext reference to render.
		static void ContextPositionRenderFunc(RenderNode &node);

		static void ContextPositionColorRenderFunc(RenderNode &node);

        /// A RenderFunc to be used by RenderContext instances.
		/// Sets render states controlled by this context and renders the contained RenderSet.
        /// Sets the stream to the buffer containing positions and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPositionUVRenderFunc(RenderNode &node);
    
        /// A render func used to set shared parameters between shaders.
		/// \param node - A RenderContext reference to render.
        static void ContextSharedParameterFunc(RenderNode &node);

		/// Loads a RenderContext from a specified XML file.
		/// \param xmlFileNamePtr - File path to the RenderContext to load from XML format.
		/// \param indentifier - A unique identifier, unused in this context. Exists only to match function prototype.
		static RenderContext* LoadXML(const char* xmlFileNamePtr, const char* indentifier = 0);

		/// \return Returns the vertex format of this RenderContext
		VertexFormat GetVertexFormat(void){ return m_VertexFormat; }


        ID3DXBuffer *CreateEffectSolution(LPCWSTR effectFile);

        void InitializeHandlesSolution();
    
        static void ContextSharedRenderFuncSolution(RenderNode &node);
        static void ContextPosNormalUVRenderFuncSolution(RenderNode &node);
	    static void ContextPositionRenderFuncSolution(RenderNode &node);
        static void ContextPositionUVRenderFuncSolution(RenderNode &node);
        static void ContextPositionColorRenderFuncSolution(RenderNode &node);
	};
}
