#ifndef FLOAT3_CLASS_H
#define FLOAT3_CLASS_H
#include <math.h>

#pragma push_macro("new")
#undef new
#include <d3dx9math.h>
#pragma pop_macro("new")
#include "InternalOutput.h"

namespace EDUtilities
{
    // A simple class used to define a 3d floating point vector
    // with common functionality
    class float3
    {
    public:

        // Used to allow the data in the vector to 
        // be accessed in different ways
        union
        {
            // Access the vector as an array
            float v[3];

            // Access the vector's individual components
            struct
            {
                float x;
                float y;
                float z;
            };
			struct 
			{
                float r;
                float g;
                float b;
			};
        };

        // Constructors
        float3() { x = y = z = 0; }
        float3(float _x, float _y, float _z)
        { x = _x; y = _y; z = _z; }

        inline float3 operator+(const float3 &v) const
        {
            return float3(x + v.x, y + v.y, z + v.z);
        }
        inline float3 operator-(const float3 &v) const
        {
            return float3(x - v.x, y - v.y, z - v.z);
        }
        inline float3 operator*(const float3 &v) const
        {
            return float3(x * v.x, y * v.y, z * v.z);
        }
        inline float3 operator/(const float3 &v) const
        {
            return float3(x / v.x, y / v.y, z / v.z);
        }
        inline float3 operator*(float v) const
        {
            return float3(x * v, y * v, z * v);
        }
        inline float3 operator/(float v) const
        {
            return float3(x / v, y / v, z / v);
        }

        inline float3 &operator=(const float3 &v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }
		inline float3 &operator=(const float3&& v)
        {
			if(this != &v)
			{
				x = std::move(v.x);
				y = std::move(v.y);
				z = std::move(v.z);
			}
            return *this;
        }
        inline float3 &operator+=(const float3 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        inline float3 &operator-=(const float3 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        inline float3 &operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            return *this;
        }

        inline float3 &makeZero()
        {
            x = y = z = 0;
            return *this;
        }

        inline float3 &negate()
        {
            x = -x;
            y = -y;
            z = -z;
            return *this;
        }

        inline float dotProduct(const float3 &v) const
        {
            return x * v.x + y * v.y + z * v.z;
        }

        inline friend float DotProduct(const float3 &a, const float3 &b)
        {
            return a.dotProduct(b);
        }

        inline friend void CrossProduct(float3 &out, const float3 &a, const float3 &b)
        {
            out.x = a.y * b.z - a.z * b.y;
            out.y = a.z * b.x - a.x * b.z;
            out.z = a.x * b.y - a.y * b.x;
        }

        inline float magnitude() const
        {
            return (float)sqrt(dotProduct(*this));
        }

        inline float3 &normalize()
        {
            float invMag = 1 / magnitude();
            *this *= invMag;
            return *this;
        }

		friend InternalOutput &operator<<(InternalOutput &out, 
            const float3 &vec)
		{
			const int BufferSize = 64;
            char buffer[BufferSize];

            sprintf_s(buffer, BufferSize, "%f, %f, %f", vec.x, vec.y, vec.z);

            out << buffer;

            return out;
		}

        operator D3DXVECTOR3 *() { return (D3DXVECTOR3 *)this; }

    };
}
#endif