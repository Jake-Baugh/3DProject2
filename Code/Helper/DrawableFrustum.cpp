#include <Helper\DrawableFrustum.hpp>

namespace Helper
{
	DrawableFrustum::DrawableFrustum(ID3D10Device* device, const Helper::Frustum& frustum, const D3DXVECTOR3& origin, const D3DXVECTOR3& direction)
		: mDevice(device)
		, mFrustum(frustum)
		, mOrigin(origin)
		, mDirection(direction)
		, mVertexBuffer(mDevice)
		, mEffect(mDevice, "Resources/Effects/Debug.fx")
	{
		Framework::Effect::InputLayoutVector inputLayout;
		inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
		mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);

		SetFrustum(frustum);
		UpdateWorld();
	}

	void DrawableFrustum::Update(const D3DXVECTOR3& origin, const D3DXVECTOR3& direction)
	{
		mOrigin = origin;
		mDirection = direction;

		UpdateWorld();
	}

	void DrawableFrustum::SetFrustum(const Frustum& frustum)
	{
		mFrustum = frustum;

		float fovYhalf = 0.5f * mFrustum.FieldOfViewY;

		float halfHeightNear = mFrustum.NearDistance * std::tan(fovYhalf);
		float halfWidthNear = mFrustum.AspectRatio * halfHeightNear;

		float halfHeightFar = mFrustum.FarDistance * std::tan(fovYhalf);
		float halfWidthFar = mFrustum.AspectRatio * halfHeightFar;

		D3DXVECTOR3 nearOrigin = D3DXVECTOR3(0.0f, 0.0f, mFrustum.NearDistance);
		D3DXVECTOR3 farOrigin = D3DXVECTOR3(0.0f, 0.0f, mFrustum.FarDistance);

		D3DXVECTOR3 nearVertices[] = { D3DXVECTOR3(-halfWidthNear, -halfHeightNear, nearOrigin.z)
									 , D3DXVECTOR3(halfWidthNear, -halfHeightNear, nearOrigin.z)
									 , D3DXVECTOR3(-halfWidthNear, halfHeightNear, nearOrigin.z)
									 , D3DXVECTOR3(halfWidthNear, halfHeightNear, nearOrigin.z) };

		D3DXVECTOR3 farVertices[] =  { D3DXVECTOR3(-halfWidthFar, -halfHeightFar, farOrigin.z)
									 , D3DXVECTOR3(halfWidthFar, -halfHeightFar, farOrigin.z)
									 , D3DXVECTOR3(-halfWidthFar, halfHeightFar, farOrigin.z)
									 , D3DXVECTOR3(halfWidthFar, halfHeightFar, farOrigin.z) };

		
		D3DXVECTOR3 vertices[] = { nearVertices[0], nearVertices[1]
								 , nearVertices[1], nearVertices[3]
								 , nearVertices[3], nearVertices[2]
								 , nearVertices[2], nearVertices[0]
								 , nearVertices[0], farVertices[0]
								 , nearVertices[1], farVertices[1]
								 , nearVertices[2], farVertices[2]
								 , nearVertices[3], farVertices[3]
								 , farVertices[0], farVertices[1]
								 , farVertices[1], farVertices[3]
								 , farVertices[3], farVertices[2]
								 , farVertices[2], farVertices[0] };

		Framework::VertexBuffer::Description bufferDescription;
		bufferDescription.ElementCount = sizeof(vertices) / sizeof(vertices[0]);
		bufferDescription.ElementSize = sizeof(D3DXVECTOR3);
		bufferDescription.FirstElementPointer = vertices;
		bufferDescription.Topology = Framework::Topology::LineList;
		bufferDescription.Usage = Framework::Usage::Default;

		mVertexBuffer.SetData(bufferDescription, NULL);
	}


	void DrawableFrustum::Draw(const Camera::Camera& camera)
	{
		mEffect.SetVariable("gWorld", mWorld);
		mEffect.SetVariable("gMVP", mWorld * camera.GetViewProjection());

		mVertexBuffer.Bind();
		for (unsigned int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mVertexBuffer.GetElementCount(), 0);
		}
	}

	void DrawableFrustum::UpdateWorld()
	{
		D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 x;
		D3DXVECTOR3 y;
		D3DXVECTOR3 z = mDirection;

		D3DXVec3Cross(&x, &up, &z);
		D3DXVec3Cross(&y, &z, &x);

		D3DXVec3Normalize(&x, &x);
		D3DXVec3Normalize(&y, &y);
		D3DXVec3Normalize(&z, &z);

		mWorld.m[0][0] = x.x;
		mWorld.m[1][0] = y.x;
		mWorld.m[2][0] = z.x;
		mWorld.m[3][0] = mOrigin.x;

		mWorld.m[0][1] = x.y;
		mWorld.m[1][1] = y.y;
		mWorld.m[2][1] = z.y;
		mWorld.m[3][1] = mOrigin.y;

		mWorld.m[0][2] = x.z;
		mWorld.m[1][2] = y.z;
		mWorld.m[2][2] = z.z;
		mWorld.m[3][2] = mOrigin.z;

		mWorld.m[0][3] = 0.0f;
		mWorld.m[1][3] = 0.0f;
		mWorld.m[2][3] = 0.0f;
		mWorld.m[3][3] = 1.0f;
	}
}