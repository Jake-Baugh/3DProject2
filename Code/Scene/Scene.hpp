#ifndef SCENE_HPP
#define SCENE_HPP

#include <Camera\Camera.hpp>
#include <Scene\QuadTree.hpp>
#include <Scene\Geometry.hpp>
#include <Scene\Ground.hpp>

namespace Scene
{
	class Scene
	{
	public:
		Scene(ID3D10Device* device);
		~Scene() throw();

		void DrawDeferred(const Camera::Camera& camera, const Helper::Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection);
		void DrawForwarded(const Camera::Camera& camera);
	private:
		static const Helper::AABB2f C_SCENE_QUAD;

		ID3D10Device* mDevice;

		QuadNode mQuadTree;
		Ground mGround;
		std::vector<Resources::ModelObj*> mModels;
		std::vector<Geometry*> mGeometry;
	};
}

#endif