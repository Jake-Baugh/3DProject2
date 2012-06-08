#ifndef QUAD_TREE_HPP
#define QUAD_TREE_HPP

#include <Helper\Global.hpp>
#include <Helper\Primitives.hpp>
#include <Resources\ModelObj.hpp>
#include <Scene\Geometry.hpp>
#include <vector>
#include <set>

namespace Scene
{
	class QuadNode
	{
	public:
		QuadNode(const Helper::AABB2f& quad, unsigned int levels);

		void AddGeometry(Geometry* geometry);
		void GetGeometry(std::set<Geometry*>& outGeometry) const;
		void GetVisibleGeometry(const Helper::Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection, std::set<Geometry*>& outVisible) const;
	private:
		Helper::AABB2f mQuad;
		std::vector<QuadNode> mChildren;
		std::vector<Geometry*> mGeometry;
	};
}
#endif