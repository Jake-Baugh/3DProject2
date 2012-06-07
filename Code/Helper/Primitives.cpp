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

	void TransformFrustum(const Frustum& frustum, const D3DXVECTOR3& position, const D3DXVECTOR3& direction, D3DXVECTOR3* nearVertices, D3DXVECTOR3* farVertices)
	{
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

		nearVertices[0] = D3DXVECTOR3(-halfWidthNear, -halfHeightNear, nearOrigin.z);
		nearVertices[1] = D3DXVECTOR3(halfWidthNear, -halfHeightNear, nearOrigin.z);
		nearVertices[2] = D3DXVECTOR3(-halfWidthNear, halfHeightNear, nearOrigin.z);
		nearVertices[3] = D3DXVECTOR3(halfWidthNear, halfHeightNear, nearOrigin.z);

		farVertices[0] = D3DXVECTOR3(-halfWidthFar, -halfHeightFar, farOrigin.z);
		farVertices[1] = D3DXVECTOR3(halfWidthFar, -halfHeightFar, farOrigin.z);
		farVertices[2] = D3DXVECTOR3(-halfWidthFar, halfHeightFar, farOrigin.z);
		farVertices[3] = D3DXVECTOR3(halfWidthFar, halfHeightFar, farOrigin.z);

		for (int i = 0; i < 2; ++i)
		{
			D3DXVECTOR3* currentVertices = (i == 0) ? nearVertices : farVertices;
			for (int k = 0; k < 4; ++k)
			{
				D3DXVECTOR4 t = D3DXVECTOR4(currentVertices[i], 1.0f);
				D3DXVec4Transform(&t, &t, &transform);
				currentVertices[i] = D3DXVECTOR3(t.x, t.y, t.z);
			}
		}
	}

	enum Where
	{
		Above, Below, Left, Right, Near, Far, In
	};

	Where PointInFrustum(const Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection, const D3DXVECTOR3& point)
	{
		D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 vec = point - frustumPosition;

		D3DXVECTOR3 X;
		D3DXVECTOR3 Y;
		D3DXVECTOR3 Z;

		D3DXVec3Cross(&X, &up, &frustumDirection);
		D3DXVec3Cross(&Y, &frustumDirection, &X);
		D3DXVec3Normalize(&X, &X);
		D3DXVec3Normalize(&Y, &Y);
		D3DXVec3Normalize(&Z, &frustumDirection);

		float z = D3DXVec3Dot(&vec, &Z);

		if (frustum.NearDistance > z)
			return Where::Near;
		if (z > frustum.FarDistance)
			return Where::Far;

		float y = D3DXVec3Dot(&vec, &Y);
		float h = z * 2.0f * tan(frustum.FieldOfViewY * 0.5f);
		if (-h * 0.5f > y)
			return Where::Below;
		if (y > h * 0.5f)
			return Where::Above;
		
		float x = D3DXVec3Dot(&vec, &X);
		float w = h * frustum.AspectRatio;
		if (-w * 0.5f > x)
			return Where::Left;
		if (x > w * 0.5f)
			return Where::Right;

		return Where::In;
	}
	
	bool FrustumVsAABB(const Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection, const AABB3f& aabb, const D3DXVECTOR3& translation)
	{
		std::vector<D3DXVECTOR3> boxVertices;
		boxVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y, aabb.Corners[0].Z) + translation);
		boxVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y, aabb.Corners[0].Z) + translation);
		boxVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[0].Z) + translation);
		boxVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[0].Z) + translation);
		boxVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y, aabb.Corners[1].Z) + translation);
		boxVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y, aabb.Corners[1].Z) + translation);
		boxVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X, aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[1].Z) + translation);
		boxVertices.push_back(D3DXVECTOR3(aabb.Corners[0].X + aabb.GetWidth(), aabb.Corners[0].Y + aabb.GetHeight(), aabb.Corners[1].Z) + translation);


		int wheres[] = {0, 0, 0, 0, 0, 0};
		

		for (int i = 0; i < boxVertices.size(); ++i)
		{
			switch(Where w = PointInFrustum(frustum, frustumPosition, frustumDirection, boxVertices[i]))
			{
			case Where::In:
				return true;
			default:
				wheres[w]++;
			}
		}

		for (int i = 0; i < 6; ++i)
		{
			if (wheres[i] > 0 && wheres[i] < boxVertices.size())
				return true;
		}
		return false;

		//TransformFrustum(frustum, frustumPosition, frustumDirection, nearFrustumVertices, farFrustumVertices);



		//for (int i = 0; i < 3; ++i)
		//{
		//	D3DXVECTOR2 nearAABBVertices2D[4];
		//	D3DXVECTOR2 farAABBVertices2D[4];

		//	D3DXVECTOR2 nearFrustumVertices2D[4];
		//	D3DXVECTOR2 farFrustumVertices2D[4];

		//	for (int j = 0; j < 2; ++j)
		//	{
		//		D3DXVECTOR2* currentAABBVertices2D = (j == 0) ? nearAABBVertices2D : farAABBVertices2D;
		//		D3DXVECTOR3* currentAABBVertices = (j == 0) ? nearAABBVertices : farAABBVertices;

		//		D3DXVECTOR2* currentFrustumVertices2D = (j == 0) ? nearFrustumVertices2D : farFrustumVertices2D;
		//		D3DXVECTOR3* currentFrustumVertices = (j == 0) ? nearFrustumVertices : farFrustumVertices;
		//		for (int k = 0; k < 4; ++k)
		//		{
		//			// Note: When i = 1, the vector will be (z, x)
		//			currentFrustumVertices2D[k] = D3DXVECTOR2(currentFrustumVertices[k][(i + 1) % 3], currentFrustumVertices[k][(i + 2) % 3]);
		//			currentAABBVertices2D[k] = D3DXVECTOR2(currentAABBVertices[k][(i + 1) % 3], currentAABBVertices[k][(i + 2) % 3]);

		//			if (i == 1)
		//			{
		//				currentFrustumVertices2D[k] = D3DXVECTOR2(currentFrustumVertices2D[k].y, currentAABBVertices2D[k].x);
		//				currentAABBVertices2D[k] = D3DXVECTOR2(currentAABBVertices2D[k].y, currentAABBVertices2D[k].x);
		//			}
		//		}
		//	}

		//	
		//	for (int n = 0; n < 2; ++n)
		//	{
		//		float minAABB = 10e9;
		//		float maxAABB = -10e9;
		//		float minFrustum = 10e9;
		//		float maxFrustum = -10e9;

		//		for (int j = 0; j < 2; ++j)
		//		{
		//			D3DXVECTOR2* currentAABBVertices = (j == 0) ? nearAABBVertices2D : farAABBVertices2D;
		//			D3DXVECTOR2* currentFrustumVertices = (j == 0) ? nearFrustumVertices2D : farFrustumVertices2D;
		//			
		//			for (int k = 0; k < 4; ++k)
		//			{
		//				float a = currentFrustumVertices[k][(n + 1) % 2];
		//				float b = currentAABBVertices[k][(n + 1) % 2];

		//				minFrustum = (a < minFrustum) ? a : minFrustum;
		//				maxFrustum = (a > maxFrustum) ? a : maxFrustum;

		//				minAABB = (b < minAABB) ? b : minAABB;
		//				maxAABB = (b > maxAABB) ? b : maxAABB; 
		//			}
		//		}

		//		if (max(maxFrustum, maxAABB) - min(minFrustum, minAABB) > (maxFrustum - minFrustum) + (maxAABB - minAABB))
		//			return false;
		//		/*
		//		if (maxAABB < minFrustum)
		//			return false;
		//		if (maxFrustum < minAABB)
		//			return false;
		//		*/
		//	}
		//}	

		//return true;
	}
}