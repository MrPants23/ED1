#include "../UnitTest++/src/UnitTest++.h"

#include "../EDUtilities/float3.h"
using namespace EDUtilities;

float3 theTestObj(42.0f, 49.5f, .1f);

struct float3Fixture
{
	float3Fixture() {  }
	~float3Fixture() { theTestObj = float3(42.0f, 49.5f, .1f); }
};

TEST(float3Constructor)
{
	CHECK_EQUAL(42.0f, theTestObj.x);
	CHECK_EQUAL(49.5f, theTestObj.y);
	CHECK_EQUAL(0.1f,  theTestObj.z);
}

TEST(float3UnionStructure)
{
	CHECK_CLOSE(42.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(42.0f, theTestObj.r, 0.001f);
	CHECK_CLOSE(42.0f, theTestObj.v[0], 0.001f);

	CHECK_CLOSE(49.5f, theTestObj.y, 0.001f);
	CHECK_CLOSE(49.5f, theTestObj.g, 0.001f);
	CHECK_CLOSE(49.5f, theTestObj.v[1], 0.001f);

	CHECK_CLOSE(.1f, theTestObj.z, 0.001f);
	CHECK_CLOSE(.1f, theTestObj.b, 0.001f);
	CHECK_CLOSE(.1f, theTestObj.v[2], 0.001f);
}

TEST(float3Assignment)
{
	float3 leftHand;
	leftHand = theTestObj;

	CHECK_CLOSE(42.0f, leftHand.x, 0.001f);
	CHECK_CLOSE(49.5f, leftHand.y, 0.001f);
	CHECK_CLOSE(0.1f,  leftHand.z, 0.001f);
}

TEST(float3CopyConstructor)
{
	float3 leftHand = theTestObj;

	CHECK_CLOSE(42.0f, leftHand.x, 0.001f);
	CHECK_CLOSE(49.5f, leftHand.y, 0.001f);
	CHECK_CLOSE(0.1f,  leftHand.z, 0.001f);
}

TEST(float3Add)
{
	float3 rightHand(1.0f, 2.0f, 3.0f);
	float3 leftHand = rightHand + theTestObj;

	CHECK_CLOSE(43.0f, leftHand.x, 0.001f);
	CHECK_CLOSE(51.5f, leftHand.y, 0.001f);
	CHECK_CLOSE(3.1f,  leftHand.z, 0.001f);
}

TEST(float3Subtract)
{
	float3 rightHand(1.0f, 2.0f, 3.0f);
	float3 leftHand = rightHand - theTestObj;

	CHECK_CLOSE(-41.0f, leftHand.x, 0.001f);
	CHECK_CLOSE(-47.5f, leftHand.y, 0.001f);
	CHECK_CLOSE(2.9f,   leftHand.z, 0.001f);
}

TEST(float3Multiply)
{
	float3 rightHand(1.0f, 2.0f, 3.0f);
	float3 leftHand(3.0f, 2.0f, 1.0f);
	float3 result = leftHand * rightHand;

	CHECK_CLOSE(3.0f, result.x, 0.001f);
	CHECK_CLOSE(4.0f, result.y, 0.001f);
	CHECK_CLOSE(3.0f, result.z, 0.001f);
}

TEST(float3Divide)
{
	float3 rightHand(2.0f, 8.0f, 8.0f);
	float3 leftHand(2.0f, 2.0f, 4.0f);
	float3 result = leftHand / rightHand;

	CHECK_CLOSE(1.0f,  result.x, 0.001f);
	CHECK_CLOSE(0.25f, result.y, 0.001f);
	CHECK_CLOSE(0.5f,  result.z, 0.001f);
}

TEST(float3ScalarMultiply)
{
	float3 leftHand(3.0f, 2.0f, 1.0f);
	float3 result = leftHand * 5;

	CHECK_CLOSE(15.0f, result.x, 0.001f);
	CHECK_CLOSE(10.0f, result.y, 0.001f);
	CHECK_CLOSE(5.0f,  result.z, 0.001f);
}

TEST(float3ScalarDivide)
{
	float3 leftHand(15.0f, 10.0f, 5.0f);
	float3 result = leftHand / 5;

	CHECK_CLOSE(3.0f, result.x, 0.001f);
	CHECK_CLOSE(2.0f, result.y, 0.001f);
	CHECK_CLOSE(1.0f, result.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3AddAssignment)
{
	float3 rightHand(1.0f, 2.0f, 3.0f);
	theTestObj += rightHand;

	CHECK_CLOSE(43.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(51.5f, theTestObj.y, 0.001f);
	CHECK_CLOSE(3.1f,  theTestObj.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3SubtractAssignment)
{
	float3 rightHand(1.0f, 2.0f, 3.0f);
	theTestObj -= rightHand;

	CHECK_CLOSE(41.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(47.5f, theTestObj.y, 0.001f);
	CHECK_CLOSE(-2.9f, theTestObj.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3ScalarAssignment)
{
	theTestObj *= 2.0f;

	CHECK_CLOSE(84.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(99.0f, theTestObj.y, 0.001f);
	CHECK_CLOSE(0.2f, theTestObj.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3MakeZero)
{
	theTestObj.makeZero();

	CHECK_CLOSE(0.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(0.0f, theTestObj.y, 0.001f);
	CHECK_CLOSE(0.0f, theTestObj.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3Negate)
{
	theTestObj.negate();

	CHECK_EQUAL(-42.0f, theTestObj.x);
	CHECK_EQUAL(-49.5f, theTestObj.y);
	CHECK_EQUAL(-0.1f,  theTestObj.z);
}

TEST(float3InvokingDotProduct)
{
	float3 rightHand(1.0f, 2.0f, 3.0f);
	
	CHECK_CLOSE(141.3, theTestObj.dotProduct(rightHand), 0.001f);
}

TEST(float3DotProduct)
{
	float3 rightHand(1.0f, 2.0f, 3.0f);
	
	CHECK_CLOSE(141.3, DotProduct(theTestObj, rightHand), 0.001f);
}

TEST(float3CrossProduct)
{
	float3 rightHand(0.0f, 1.0f, 0.0f);
	float3 leftHand(1.0f, 0.0f, 0.0f);
	float3 result; 
	CrossProduct(result, leftHand, rightHand);

	CHECK_CLOSE(0, result.x, 0.001f);
	CHECK_CLOSE(0, result.y, 0.001f);
	CHECK_CLOSE(1, result.z, 0.001f);
}

TEST(float3Magnitude)
{
	CHECK_CLOSE(64.91733, theTestObj.magnitude(), 0.001f);
}

TEST_FIXTURE(float3Fixture, float3Normalize)
{
	float3 returnResult = theTestObj.normalize();

	CHECK_CLOSE(1, theTestObj.magnitude(), 0.001f);
	CHECK_CLOSE(1, returnResult.magnitude(), 0.001f);
}
