#include <cmath>
#include <vector>
#include <Helper\Primitives.hpp>

namespace Helper
{
	D3DXMATRIX Frustum::CreatePerspectiveProjection() const
	{
		D3DXMATRIX result;
		D3DXMatrixPerspectiveFovLH(&result, FieldOfViewY, AspectRatio, NearDistance, FarDistance);

		return result;
	}

	std::vector<D3DXVECTOR3> TransformFrustum(const Frustum& frustum, const D3DXVECTOR3& position, const D3DXVECTOR3& direction)
	{
		std::vector<D3DXVECTOR3> vertices;

		D3DXMATRIX transform;
		D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 x;
		D3DXVECTOR3 y;
		D3DXVECTOR3 z = direction;

		D3DXVec3Cross(&x, &up, &z);
		D3DXVec3Cross(&y, &z, &x);

		D3DXVec3Normalize(&x, &x);
		D3DXVec3Normalize(&y, &y);
		D3DXVec3Normalize(&z, &z);

		transform.m[0][0] = x.x;
		transform.m[1][0] = y.x;
		transform.m[2][0] = z.x;
		transform.m[3][0] = position.x;

		transform.m[0][1] = x.y;
		transform.m[1][1] = y.y;
		transform.m[2][1] = z.y;
		transform.m[3][1] = position.y;

		transform.m[0][2] = x.z;
		transform.m[1][2] = y.z;
		transform.m[2][2] = z.z;
		transform.m[3][2] = position.z;

		transform.m[0][3] = 0.0f;
		transform.m[1][3] = 0.0f;
		transform.m[2][3] = 0.0f;
		transform.m[3][3] = 1.0f;

		float fovYhalf = 0.5f * frustum.FieldOfViewY;

		float halfHeightNear = frustum.NearDistance * std::tan(fovYhalf);
		float halfWidthNear = frustum.AspectRatio * halfHeightNear;

		float halfHeightFar = frustum.FarDistance * std::tan(fovYhalf);
		float halfWidthFar = frustum.AspectRatio * halfHeightFar;

		D3DXVECTOR3 nearOrigin = D3DXVECTOR3(0.0f, 0.0f, frustum.NearDistance);
		D3DXVECTOR3 farOrigin = D3DXVECTOR3(0.0f, 0.0f, frustum.FarDistance);

		vertices.push_back(D3DXVECTOR3(-halfWidthNear, -halfHeightNear, nearOrigin.z));
		vertices.push_back(D3DXVECTOR3(halfWidthNear, -halfHeightNear, nearOrigin.z));
		vertices.push_back(D3DXVECTOR3(-halfWidthNear, halfHeightNear, nearOrigin.z));
		vertices.push_back(D3DXVECTOR3(halfWidthNear, halfHeightNear, nearOrigin.z));

		vertices.push_back(D3DXVECTOR3(-halfWidthFar, -halfHeightFar, farOrigin.z));
		vertices.push_back(D3DXVECTOR3(halfWidthFar, -halfHeightFar, farOrigin.z));
		vertices.push_back(D3DXVECTOR3(-halfWidthFar, halfHeightFar, farOrigin.z));
		vertices.push_back(D3DXVECTOR3(halfWidthFar, halfHeightFar, farOrigin.z));

		for (int i = 0; i < 8; ++i)
		{
			D3DXVECTOR4 t = D3DXVECTOR4(vertices[i], 1.0f);
			D3DXVec4Transform(&t, &t, &transform);
			vertices[i] = D3DXVECTOR3(t.x, t.y, t.z);
		}

		return vertices;
	}

	D3DXVECTOR2 ProjectToPlane(const D3DXVECTOR3& vec, unsigned int dimension1, unsigned int dimension2)
	{
		return D3DXVECTOR2(vec[dimension1], vec[dimension2]);
	}
	float ProjectToLine(const D3DXVECTOR2& vec, unsigned int dimension)
	{
		return vec[dimension];
	}

	struct FrustumPlane
	{
		D3DXVECTOR3 Points[4];
		D3DXVECTOR3 Normal;
	};

	struct FrustumPlanePair
	{
		FrustumPlane First;
		FrustumPlane Second;
	};

	IntersectionState::IntersectionState FrustumVsAABB(const Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection, const AABB3f& aabb)
	{
		std::vector<FrustumPlanePair> frustumPlanePairs(3);
		std::vector<D3DXVECTOR3> frustumVertices = TransformFrustum(frustum, frustumPosition, frustumDirection);
		std::vector<D3DXVECTOR3> AABBVertices;

		// Add all aabb points
		AABBVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y, aabb.Corners[0].Z));
		AABBVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y, aabb.Corners[0].Z));
		AABBVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[0].Z));
		AABBVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[0].Z));
		AABBVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y, aabb.Corners[1].Z));
		AABBVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y, aabb.Corners[1].Z));
		AABBVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[1].Z));
		AABBVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[1].Z));

		// Front & Back
		frustumPlanePairs[0].First.Points[0] = frustumVertices[0];
		frustumPlanePairs[0].First.Points[1] = frustumVertices[1];
		frustumPlanePairs[0].First.Points[2] = frustumVertices[2];
		frustumPlanePairs[0].First.Points[3] = frustumVertices[3];
		D3DXVec3Cross(&frustumPlanePairs[0].First.Normal, &(frustumVertices[2] - frustumVertices[0]),
					  &(frustumVertices[1] - frustumVertices[0]));

		frustumPlanePairs[0].Second.Points[0] = frustumVertices[4];
		frustumPlanePairs[0].Second.Points[1] = frustumVertices[5];
		frustumPlanePairs[0].Second.Points[2] = frustumVertices[6];
		frustumPlanePairs[0].Second.Points[3] = frustumVertices[7];
		frustumPlanePairs[0].Second.Normal = -frustumPlanePairs[0].First.Normal;

		// Left & Right
		frustumPlanePairs[1].First.Points[0] = frustumVertices[0];
		frustumPlanePairs[1].First.Points[1] = frustumVertices[2];
		frustumPlanePairs[1].First.Points[2] = frustumVertices[4];
		frustumPlanePairs[1].First.Points[3] = frustumVertices[6];
		D3DXVec3Cross(&frustumPlanePairs[1].First.Normal, &(frustumVertices[6] - frustumVertices[4]),
					  &(frustumVertices[0] - frustumVertices[4]));

		frustumPlanePairs[1].Second.Points[0] = frustumVertices[1];
		frustumPlanePairs[1].Second.Points[1] = frustumVertices[3];
		frustumPlanePairs[1].Second.Points[2] = frustumVertices[5];
		frustumPlanePairs[1].Second.Points[3] = frustumVertices[7];
		D3DXVec3Cross(&frustumPlanePairs[1].Second.Normal, &(frustumVertices[3] - frustumVertices[1]),
					  &(frustumVertices[5] - frustumVertices[1]));

		// Top & Bottom
		frustumPlanePairs[2].First.Points[0] = frustumVertices[2];
		frustumPlanePairs[2].First.Points[1] = frustumVertices[3];
		frustumPlanePairs[2].First.Points[2] = frustumVertices[6];
		frustumPlanePairs[2].First.Points[3] = frustumVertices[7];
		D3DXVec3Cross(&frustumPlanePairs[2].First.Normal, &(frustumVertices[6] - frustumVertices[2]),
					  &(frustumVertices[3] - frustumVertices[2]));

		frustumPlanePairs[2].Second.Points[0] = frustumVertices[0];
		frustumPlanePairs[2].Second.Points[1] = frustumVertices[1];
		frustumPlanePairs[2].Second.Points[2] = frustumVertices[4];
		frustumPlanePairs[2].Second.Points[3] = frustumVertices[5];
		D3DXVec3Cross(&frustumPlanePairs[2].Second.Normal, &(frustumVertices[5] - frustumVertices[4]),
					  &(frustumVertices[0] - frustumVertices[4]));

		int inCount = 0;
		for(int p = 0; p < AABBVertices.size(); ++p)
		{
			int pointInCount = 0;
			for(int f = 0; f < frustumPlanePairs.size(); ++f)
			{
				D3DXVECTOR3 v1 = frustumPlanePairs[f].First.Points[0] - AABBVertices[p];
				D3DXVECTOR3 v2 = frustumPlanePairs[f].Second.Points[0] - AABBVertices[p];

				if(D3DXVec3Dot(&v1, &frustumPlanePairs[f].First.Normal) >= 0 &&
					D3DXVec3Dot(&v2, &frustumPlanePairs[f].Second.Normal) >= 0)
					pointInCount++;
				else
					break;
			}

			if(pointInCount == 3)
				inCount++;
		}

		if(inCount == AABBVertices.size())
			return IntersectionState::Inside;
		else if(inCount == 0)
			return IntersectionState::Outside;
		else
			return IntersectionState::Intersecting;
	}

	
	//bool FrustumVsAABB(const Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection, const AABB3f& aabb)
	//{
	//	/*std::vector<D3DXVECTOR3> frustumVertices3D;
	//	std::vector<D3DXVECTOR3> AABBVertices3D;

	//	AABBVertices3D.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y, aabb.Corners[0].Z));
	//	AABBVertices3D.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y, aabb.Corners[0].Z));
	//	AABBVertices3D.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[0].Z));
	//	AABBVertices3D.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[0].Z));
	//	AABBVertices3D.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y, aabb.Corners[1].Z));
	//	AABBVertices3D.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y, aabb.Corners[1].Z));
	//	AABBVertices3D.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[1].Z));
	//	AABBVertices3D.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[1].Z));

	//	D3DXVECTOR3 nearFrustumVertices[4];
	//	D3DXVECTOR3 farFrustumVertices[4];*/
	//	
	//	

	//	TransformFrustum(frustum, frustumPosition, frustumDirection, nearFrustumVertices, farFrustumVertices);

	//	for (int i = 0; i < 3; ++i)
	//	{
	//		std::vector<D3DXVECTOR2> frustumVertices2D
	//	}

	//	/*for (int i = 0; i < 3; ++i)
	//	{
	//		D3DXVECTOR2 nearAABBVertices2D[4];
	//		D3DXVECTOR2 farAABBVertices2D[4];

	//		D3DXVECTOR2 nearFrustumVertices2D[4];
	//		D3DXVECTOR2 farFrustumVertices2D[4];

	//		for (int j = 0; j < 2; ++j)
	//		{
	//			D3DXVECTOR2* currentAABBVertices2D = (j == 0) ? nearAABBVertices2D : farAABBVertices2D;
	//			D3DXVECTOR3* currentAABBVertices = (j == 0) ? nearAABBVertices : farAABBVertices;

	//			D3DXVECTOR2* currentFrustumVertices2D = (j == 0) ? nearFrustumVertices2D : farFrustumVertices2D;
	//			D3DXVECTOR3* currentFrustumVertices = (j == 0) ? nearFrustumVertices : farFrustumVertices;
	//			for (int k = 0; k < 4; ++k)
	//			{
	//				// Note: When i = 1, the vector will be (z, x)
	//				currentFrustumVertices2D[k] = D3DXVECTOR2(currentFrustumVertices[k][(i + 1) % 3], currentFrustumVertices[k][(i + 2) % 3]);
	//				currentAABBVertices2D[k] = D3DXVECTOR2(currentAABBVertices[k][(i + 1) % 3], currentAABBVertices[k][(i + 2) % 3]);

	//				if (i == 1)
	//				{
	//					currentFrustumVertices2D[k] = D3DXVECTOR2(currentFrustumVertices2D[k].y, currentAABBVertices2D[k].x);
	//					currentAABBVertices2D[k] = D3DXVECTOR2(currentAABBVertices2D[k].y, currentAABBVertices2D[k].x);
	//				}
	//			}
	//		}

	//		
	//		for (int n = 0; n < 2; ++n)
	//		{
	//			float minAABB = 10e9;
	//			float maxAABB = -10e9;
	//			float minFrustum = 10e9;
	//			float maxFrustum = -10e9;

	//			for (int j = 0; j < 2; ++j)
	//			{
	//				D3DXVECTOR2* currentAABBVertices = (j == 0) ? nearAABBVertices2D : farAABBVertices2D;
	//				D3DXVECTOR2* currentFrustumVertices = (j == 0) ? nearFrustumVertices2D : farFrustumVertices2D;
	//				
	//				for (int k = 0; k < 4; ++k)
	//				{
	//					float a = currentFrustumVertices[k][(n + 1) % 2];
	//					float b = currentAABBVertices[k][(n + 1) % 2];

	//					minFrustum = (a < minFrustum) ? a : minFrustum;
	//					maxFrustum = (a > maxFrustum) ? a : maxFrustum;

	//					minAABB = (b < minAABB) ? b : minAABB;
	//					maxAABB = (b > maxAABB) ? b : maxAABB; 
	//				}
	//			}

	//			float totalMax = max(maxFrustum, minFrustum);
	//			float totalMin = min(minFrustum, minAABB);
	//			float intervalSize = totalMax - totalMin;
	//			float intervalSum = (maxFrustum - minFrustum) + (maxAABB - minAABB);

	//			if (intervalSize > intervalSum)
	//				return false;
	//			/*
	//			if (maxAABB < minFrustum)
	//				return false;
	//			if (maxFrustum < minAABB)
	//				return false;
	//			*/
	//		}
	//	}*/	

	//	return true;
	//}
}