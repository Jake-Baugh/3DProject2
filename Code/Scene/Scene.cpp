#include <Resources\ModelObj.hpp>
#include <Scene\Scene.hpp>

namespace Scene
{
	const Helper::AABB2f Scene::C_SCENE_QUAD(Helper::Point2f(-256.0f, -256.0f), Helper::Point2f(256.0f, 256.0f));

	Scene::Scene(ID3D10Device* device)
		: mDevice(device)
		, mQuadTree(C_SCENE_QUAD, 1)
		, mGround(mDevice, C_SCENE_QUAD.GetWidth() * 0.5f)
	{
		// Hard code some geometry

		mModels.push_back(new Resources::ModelObj(mDevice, "crossReal.obj", "", 1.0f));
		mModels.push_back(new Resources::ModelObj(mDevice, "quadReal.obj", "", 1.0f));
		mModels.push_back(new Resources::ModelObj(mDevice, "ringReal.obj", "", 1.0f));
		mModels.push_back(new Resources::ModelObj(mDevice, "triangleReal.obj", "", 1.0f));
		mModels.push_back(new Resources::ModelObj(mDevice, "bth.obj", "", 1.0f));
		mModels.push_back(new Resources::ModelObj(mDevice, "wallSegment.obj", "", 0.5f));

		const float RADIUS = 200.0f;
		const int N = 3;
		const int MAX_MODEL = 4;
		const double DT = 2 * D3DX_PI / N;
		
		for (int i = 0; i < N; ++i)
		{
			D3DXMATRIX world;
			D3DXMatrixTranslation(&world, RADIUS * cos(i * DT), 2.0f, RADIUS * sin(i * DT));

			mGeometry.push_back(new Geometry(mDevice, mModels[i % MAX_MODEL], world));
		}

		D3DXMATRIX world;
		D3DXMatrixRotationX(&world, D3DX_PI * 0.5f);
		mGeometry.push_back(new Geometry(mDevice, mModels[4], world));

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
	}

	void Scene::DrawDeferred(const Camera::Camera& camera, const Helper::Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection)
	{
		std::set<Geometry*> visibleGeometry;

		mQuadTree.GetVisibleGeometry(frustum, frustumPosition, frustumDirection, visibleGeometry);

		mGround.Draw(camera);
		for (std::set<Geometry*>::iterator it = visibleGeometry.begin(); it != visibleGeometry.end(); ++it)
		{
			(*it)->DrawDeferred(camera);
			(*it)->DrawAABB(camera);
		}
	}

	void Scene::DrawForwarded(const Camera::Camera& camera)
	{
		const float RADIUS = 100.0f;
		const int N = 4;
		const double DT = 2 * D3DX_PI / N;

		const D3DXCOLOR COLORS[] = { D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)
								   , D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)
								   , D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)
								   , D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) };
		for (int i = 0; i < N; ++i)
		{
			D3DXMATRIX world;
			D3DXMatrixTranslation(&world, RADIUS * cos(i * DT), 0.0f, RADIUS * sin(i * DT));

			mModels.back()->Bind();
			mModels.back()->SetTintColor(COLORS[i]);
			mModels.back()->DrawForwarded(world, camera);
		}
	}
}