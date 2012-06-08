#include <Scene\QuadTree.hpp>

namespace Scene
{
	QuadNode::QuadNode(const Helper::AABB2f& quad, unsigned int levels)
		: mQuad(quad)
	{
		if (levels > 0)
		{
			float halfWidth = mQuad.GetWidth() * 0.5f;
			float halfHeight = mQuad.GetHeight() * 0.5f;

			// Lower left quad = index 0
			Helper::AABB2f childQuad(mQuad.Corners[0].X, mQuad.Corners[0].Y, halfWidth, halfHeight);
			mChildren.push_back(QuadNode(childQuad, levels - 1));

			// Lower right quad = index 1
			childQuad = Helper::AABB2f(mQuad.Corners[0].X + halfWidth, mQuad.Corners[0].Y, halfWidth, halfHeight);
			mChildren.push_back(QuadNode(childQuad, levels - 1));

			// Upper left quad = index 2
			childQuad = Helper::AABB2f(mQuad.Corners[0].X, mQuad.Corners[0].Y + halfHeight, halfWidth, halfHeight);
			mChildren.push_back(QuadNode(childQuad, levels - 1));

			// Upper right quad = index 3
			childQuad = Helper::AABB2f(mQuad.Corners[0].X + halfWidth, mQuad.Corners[0].Y + halfHeight, halfWidth, halfHeight);
			mChildren.push_back(QuadNode(childQuad, levels - 1));
		}
	}

	void QuadNode::AddGeometry(Geometry* geometry)
	{
		if (mChildren.empty())
			mGeometry.push_back(geometry);
		else
		{
			const Helper::AABB3f& geoBox = geometry->GetAABB();
			Helper::AABB2f geoQuad(Helper::Point2f(geoBox.Corners[0].X, geoBox.Corners[0].Z), Helper::Point2f(geoBox.Corners[1].X, geoBox.Corners[1].Z));

			for (int i = 0; i < mChildren.size(); i++)
			{
				if (mChildren[i].mQuad.Intersects(geoQuad))
					mChildren[i].AddGeometry(geometry);
			}
		}
	}

	void QuadNode::GetGeometry(std::set<Geometry*>& outGeometry) const
	{
		if (mChildren.empty())
			for (int i = 0; i < mGeometry.size(); ++i)
				outGeometry.insert(mGeometry[i]);
		else
			for (int i = 0; i < mChildren.size(); ++i)
				mChildren[i].GetGeometry(outGeometry);
		
	}
	
	void QuadNode::GetVisibleGeometry(const Helper::Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection, std::set<Geometry*>& outVisible) const
	{
		if (mChildren.empty())
		{
			for (int i = 0; i < mGeometry.size(); ++i)
				if (Helper::FrustumVsAABB(frustum, frustumPosition, frustumDirection, mGeometry[i]->GetAABB()) != Helper::Collision::Outside)
					outVisible.insert(mGeometry[i]);
		}
		else
		{
			Helper::AABB3f quadBox(Helper::Point3f(mQuad.Corners[0].X, -256.0f, mQuad.Corners[0].Y)
					, Helper::Point3f(mQuad.Corners[1].X, 256.0f, mQuad.Corners[1].Y));

			Helper::Collision c = Helper::FrustumVsAABB(frustum, frustumPosition, frustumDirection, quadBox);

			switch (c)
			{
			case Helper::Collision::Inside:
				GetGeometry(outVisible);
				break;
			case Helper::Collision::Intersects:
				for (int i = 0; i < mChildren.size(); ++i)
					mChildren[i].GetVisibleGeometry(frustum, frustumPosition, frustumDirection, outVisible);
				break;
			}

		}
	}
}