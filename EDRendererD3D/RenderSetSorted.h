#pragma once
#include "renderset.h"

namespace EDRendererD3D
{
    class RenderShape;
	/// A RenderSet implementation with insertion sorting.
    class RenderSetSorted :
        public RenderSet
    {
    public:

		RenderSetSorted(void) {}
		~RenderSetSorted(void) {}
		/// The address of the function to be used to compare nodes when sorting.
		/// The function should return true if left goes before right.
        bool (*SortFunc)(RenderShape *left, RenderShape *right);

		/// Adds a RenderNode to the render batch and performs an insertion sort.
		/// \param nodePtr - The RenderNode to add to the render batch
        void AddRenderNode(RenderNode *nodePtr);

		/// A comparison function used for insertion sorting based on Z depth.
		/// \param left - The left node to compare.
		/// \param right - The right node to compare.
		/// \return Returns true if the left nodes Z Depth is greater than the rights
        static bool ZSortGreater(RenderShape *left, RenderShape *right);
        /// A comparison function used for insertion sorting based on Z depth.
		/// \param left - The left node to compare.
		/// \param right - The right node to compare.
		/// \return Returns true if the left nodes Z Depth is less than than the rights
		static bool ZSortSmaller(RenderShape *left, RenderShape *right);
        /// A placeholder comparison function used for insertion sorting.
		/// \param left - The left node to compare.
		/// \param right - The right node to compare.
		/// \return Returns true always
		static bool SortReturnTrue(RenderShape *left, RenderShape *right);
        /// A placeholder comparison function used for insertion sorting.
		/// \param left - The left node to compare.
		/// \param right - The right node to compare.
		/// \return Returns false always
		static bool SortReturnFalse(RenderShape *left, RenderShape *right);

        void AddRenderNodeSolution(RenderNode *nodePtr);
    };
}