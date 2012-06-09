#include <Resources\ModelObj.hpp>
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
		mModels.push_back(new Resources::ModelObj(mDevice, "crossReal.obj", ""));
		mModels.push_back(new Resources::ModelObj(mDevice, "quadReal.obj", ""));
		mModels.push_back(new Resources::ModelObj(mDevice, "ringReal.obj", ""));
		mModels.push_back(new Resources::ModelObj(mDevice, "triangleReal.obj", ""));
		mBTHLogo = new Resources::ModelObj(mDevice, "bth.obj", "");

		const float RADIUS = 120.0f;
		const int N = 15;
		const double DT = 2 * D3DX_PI / N;
		
		for (int i = 0; i < N; ++i)
		{
			D3DXMATRIX world;
			D3DXMatrixTranslation(&world, RADIUS * cos(i * DT), 2.0f, RADIUS * sin(i * DT));

			mGeometry.push_back(new Geometry(mDevice, mModels[i % mModels.size()], world));
		}

		D3DXMATRIX world;
		D3DXMatrixRotationX(&world, D3DX_PI * 0.5f);
		mGeometry.push_back(new Geometry(mDevice, mBTHLogo, world));

		for (int i = 0; i < mGeometry.size(); ++i)
		{
			mQuadTree.AddGeometry(mGeometry[i]);
		}
	}

	Scene::~Scene() throw()
	{
		for (int i = 0; i < mGeometry.size(); ++i)
			SafeDelete(mGeometry[i]);
		for (int i = 0; i < mModels.size(); ++i)
			SafeDelete(mModels[i]);
		SafeDelete(mBTHLogo);
	}

	void Scene::Draw(const Camera::Camera& camera, const Helper::Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection)
	{
		/*
		mGround.Draw(camera);
		size_t size = mGeometry.size();
		for (int i = 0; i < size; ++i)
		{
			mGeometry[i]->Draw(camera);
		}
		*/

		std::set<Geometry*> visibleGeometry;

		mQuadTree.GetVisibleGeometry(frustum, frustumPosition, frustumDirection, visibleGeometry);

		mGround.Draw(camera);
		for (std::set<Geometry*>::iterator it = visibleGeometry.begin(); it != visibleGeometry.end(); ++it)
		{
			(*it)->Draw(camera);
		}
	}
}