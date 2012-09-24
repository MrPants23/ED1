#ifndef FLOAT4X4_CLASS_H
#define FLOAT4X4_CLASS_H

#pragma push_macro("new")
#undef new
#include <d3dx9math.h>
#pragma pop_macro("new")

#include "../EDUtilities/float3.h"

#pragma intrinsic(sin, cos)

namespace EDUtilities
{
    // A class used to define a 4 by 4 floating point 
    // matrix with common functionality
    class float4x4
    {
    public:

        // Used to allow the data in the matrix to 
        // be accessed in different ways
        union
        {
            // The matrix as a 2d array
            float m[4][4];

            // The matrix as a 1d array
            float ma[16];

            // The matrix as individual components
            // Think of them as axis'
            // row vs collum major does not matter
            struct 
            {
                // The X axis
                float Xx, Xy, Xz, Xw;
                // The Y axis
                float Yx, Yy, Yz, Yw;
                // The Z axis
                float Zx, Zy, Zz, Zw;            
                // The W axis - Translation
                float Wx, Wy, Wz, Ww;        
            };

            struct
            {
                // The X axis
                float3 XAxis;            float padX;
                // The Y axis
                float3 YAxis;            float padY;
                // The Z axis
                float3 ZAxis;            float padZ;
                // The W axis - Translation
                float3 WAxis;            float padW;
            };
        };

        // Constructors
        float4x4() {}
        float4x4(   float xx, float xy, float xz, float xw,
            float yx, float yy, float yz, float yw,
            float zx, float zy, float zz, float zw,
            float wx, float wy, float wz, float ww);

        bool operator==(const float4x4 &rightMatrix) const 
		{
			return !memcmp(ma, rightMatrix.ma, 16 * sizeof(float));
		}

        // These methods change the matrix into a specific type of matrix
        float4x4 &makeZero();        
        float4x4 &makeIdentity();        
        // Changes the matrix to be an X rotation matrix
        float4x4 &makeRotationX(float radians);
        // Changes the matrix to be an Y rotation matrix
        float4x4 &makeRotationY(float radians);
        // Changes the matrix to be an Z rotation matrix
        float4x4 &makeRotationZ(float radians);
        // Changes the matrix to be a scale matrix
        float4x4 &makeScale(float xScale, float yScale, float zScale);
        // Changes the matrix to be a perspective matrix
        float4x4 &makePerspectiveLH(float fFov, float fAspect, 
            float fNear, float fFar);

        // Matrix multiply method
        // out = a * b
        // The out cannot be the same as the a or b
        friend void Multiply(float4x4 &out, const float4x4 &a, const float4x4 &b);

        // Matrix multiply method, slower than friend func Multiply
        // temp = this;
        // this = temp * a
        //float4x4 &operator*=(const float4x4 &a);

        // Local rotation methods function as pre multiplies
        // this = this * makeRotation
        float4x4 &rotateLocalX(float radians);
        float4x4 &rotateLocalY(float radians);
        float4x4 &rotateLocalZ(float radians);

        // Global rotation methods function as post multiplies
        // this = makeRotation * this
        float4x4 &rotateGlobalX(float radians);
        float4x4 &rotateGlobalY(float radians);
        float4x4 &rotateGlobalZ(float radians);

        float4x4 &translateLocal(const float3 &tran)
        {
            return translateLocal(tran.x, tran.y, tran.z);
        }
        float4x4 &translateLocal(float x, float y, float z);

        float4x4 &translateGlobal(const float3 &tran)
        {
            return translateGlobal(tran.x, tran.y, tran.z);
        }
        float4x4 &translateGlobal(float x, float y, float z);

        // Scales the x, y and z axis' of the matrix
        float4x4 &scale(const float3 &scaleVec);
        // Scales the x, y and z axis' of the matrix
        float4x4 &scale(float x, float y, float z);
        // Scales the x, y, and z axes of the matrix
        float4x4 &scale(float scalar);

        // Transposes the matrix
        float4x4 &transpose(void);
        // Transposes the orientation of the matrix
        float4x4 &transpose3x3(void);

        friend void RotateVector(float3 &out, const float3 &inVec,
            const float4x4 &inMat);
        friend void TranslateVector(float3 &out, const float3 &inVec,
            const float4x4 &inMat);

        float3 getScale(void) const;

		friend InternalOutput &operator<<(InternalOutput &out, 
            const float4x4 &mat);

        operator D3DXMATRIX *() { return (D3DXMATRIX *)this; }

		operator const D3DXMATRIX *() const { return (const D3DXMATRIX *)this; }

		void RotateVector(float3 &vector);
		void TranslateVector(float3 &vector);
    };
}
#endif