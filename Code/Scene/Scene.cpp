#include <Scene\Scene.hpp>

namespace Scene
{
	const Helper::AABB2f Scene::C_SCENE_QUAD(Helper::Point2f(-256.0f, -256.0f), Helper::Point2f(256.0f, 256.0f));

	Scene::Scene(ID3D10Device* device)
		: mDevice(device)
		, mQuadTree(C_SCENE_QUAD, 5)
		, mGround(mDevice, C_SCENE_QUAD.GetWidth() * 0.5f)
	{
		// Hard code some geometry
		
		//mGeometry.push_back(Geometry(mDevice, 
	}

	void Scene::Draw(const Camera::Camera& camera, const Helper::Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection)
	{
		std::set<Geometry*> visibleGeometry;

		mQuadTree.GetVisibleGeometry(frustum, frustumPosition, frustumDirection, visibleGeometry);

		mGround.Draw(camera);
		for (std::set<Geometry*>::iterator it = visibleGeometry.begin(); it != visibleGeometry.end(); ++it)
		{
			(*it)->Draw(camera);
		}
	}
}