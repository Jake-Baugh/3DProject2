#include <Scene\Geometry.hpp>

namespace Scene
{
	Geometry::Geometry(ID3D10Device* device, Resources::ModelObj* model, const D3DXMATRIX& world)
		: mDevice(device)
		, mModel(model)
		, mDrawableAABB(mDevice, mAABB)
	{
		SetWorld(world);
	}

	const Helper::AABB3f& Geometry::GetAABB() const
	{
		return mAABB;
	}
	
	void Geometry::SetWorld(const D3DXMATRIX& world)
	{
		mWorld = world;
		mAABB = CreateAABB();
		mDrawableAABB.SetBox(mAABB);
	}

	const D3DXMATRIX& Geometry::GetWorld() const
	{
		return mWorld;
	}

	void Geometry::Draw(const Camera::Camera& camera)
	{
		mModel->Draw(mWorld, camera);
	}

	void Geometry::DrawAABB(const Camera::Camera& camera)
	{
		D3DXMATRIX identity;
		D3DXMatrixIdentity(&identity);

		mDrawableAABB.Draw(camera, identity);
	}

	Helper::AABB3f Geometry::CreateAABB() const
	{
		const Helper::AABB3f& worldAABB = mModel->GetAABB();
		float width = worldAABB.GetWidth();
		float height = worldAABB.GetHeight();

		std::vector<D3DXVECTOR3> vertices(8);
		vertices[0] = D3DXVECTOR3(worldAABB.Corners[0].X, worldAABB.Corners[0].Y, worldAABB.Corners[0].Z);
		vertices[1] = D3DXVECTOR3(worldAABB.Corners[0].X, worldAABB.Corners[0].Y + height, worldAABB.Corners[0].Z);
		vertices[2] = D3DXVECTOR3(worldAABB.Corners[0].X + width, worldAABB.Corners[0].Y + height, worldAABB.Corners[0].Z);
		vertices[3] = D3DXVECTOR3(worldAABB.Corners[0].X + width, worldAABB.Corners[0].Y, worldAABB.Corners[0].Z);
		vertices[4] = D3DXVECTOR3(worldAABB.Corners[0].X, worldAABB.Corners[0].Y, worldAABB.Corners[1].Z);
		vertices[5] = D3DXVECTOR3(worldAABB.Corners[0].X, worldAABB.Corners[0].Y + height, worldAABB.Corners[1].Z);
		vertices[6] = D3DXVECTOR3(worldAABB.Corners[0].X + width, worldAABB.Corners[0].Y + height, worldAABB.Corners[1].Z);
		vertices[7] = D3DXVECTOR3(worldAABB.Corners[0].X + width, worldAABB.Corners[0].Y, worldAABB.Corners[1].Z);

		D3DXVECTOR3 minV(10e9, 10e9, 10e9);
		D3DXVECTOR3 maxV = -minV;

		for (int i = 0; i < vertices.size(); ++i)
		{
			D3DXVECTOR4 v(vertices[i], 1.0f);
			D3DXVec4Transform(&v, &v, &mWorld);

			minV.x = min(minV.x, v.x);
			minV.y = min(minV.y, v.y);
			minV.z = min(minV.z, v.z);

			maxV.x = max(maxV.x, v.x);
			maxV.y = max(maxV.y, v.y);
			maxV.z = max(maxV.z, v.z);
		}
		
		return Helper::AABB3f(Helper::Point3f(minV.x, minV.y, minV.z), Helper::Point3f(maxV.x, maxV.y, maxV.z));
	}
}