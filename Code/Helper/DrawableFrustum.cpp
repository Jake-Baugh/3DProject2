#include <Helper\DrawableFrustum.hpp>

namespace Helper
{
	DrawableFrustum::DrawableFrustum(ID3D10Device* device, const Helper::Frustum& frustum, const D3DXVECTOR3& origin, const D3DXVECTOR3& direction)
		: mDevice(device)
		, mFrustum(frustum)
		, mFrustumNormals(6)
		, mFrustumPositions(6)
		, mOrigin(origin)
		, mDirection(direction)
		, mNormalBuffer(mDevice)
		, mFrustumBuffer(mDevice)
		, mEffect(mDevice, "Resources/Effects/Debug.fx")
	{
		Framework::Effect::InputLayoutVector inputLayout;
		inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
		mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);


		
		D3DXVECTOR3 normalVertices[] = { D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f)
									   , D3DXVECTOR3(-0.1f, 0.0f, 0.75f), D3DXVECTOR3(0.0f, 0.0f, 1.0f)
									   , D3DXVECTOR3(0.1f, 0.0f, 0.75f), D3DXVECTOR3(0.0f, 0.0f, 1.0f) };
		/*
		D3DXVECTOR3 normalVertices[] = { D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f)
									   , D3DXVECTOR3(0.75f, 0.0f, -0.1f), D3DXVECTOR3(1.0f, 0.0f, 0.0f)
									   , D3DXVECTOR3(0.75f, 0.0f, 0.1f), D3DXVECTOR3(1.0f, 0.0f, 0.0f) };
		*/

		Framework::VertexBuffer::Description normalBufferDescription;
		normalBufferDescription.ElementCount = 6;
		normalBufferDescription.ElementSize = sizeof(D3DXVECTOR3);
		normalBufferDescription.FirstElementPointer = normalVertices;
		normalBufferDescription.Topology = Framework::Topology::LineList;
		normalBufferDescription.Usage = Framework::Usage::Default;

		mNormalBuffer.SetData(normalBufferDescription, NULL);


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

		mFrustumBuffer.SetData(bufferDescription, NULL);


		/*
		mFrustumNormals[0] = D3DXVECTOR3(0.0, 0.0, -1.0);
		mFrustumNormals[1] = D3DXVECTOR3(0.0, 0.0, 1.0);
		mFrustumNormals[2] = D3DXVECTOR3(-1.0, 0.0, 0.0);
		mFrustumNormals[3] = D3DXVECTOR3(1.0, 0.0, 0.0);
		mFrustumNormals[4] = D3DXVECTOR3(0.0, 1.0, 0.0);
		mFrustumNormals[5] = D3DXVECTOR3(0.0, -1.0, 0.0);
		*/

		
		// Front & Back
		D3DXVec3Cross(&mFrustumNormals[0], &(nearVertices[2] - nearVertices[0]),
					  &(nearVertices[1] - nearVertices[0]));
		mFrustumPositions[0] = D3DXVECTOR3(0.0f, 0.0f, nearOrigin.z - 0.1f);

		mFrustumNormals[1] = -mFrustumNormals[0];
		mFrustumPositions[1] = D3DXVECTOR3(0.0f, 0.0f, nearOrigin.z + 0.1f);


		// Left & Right
		D3DXVec3Cross(&mFrustumNormals[2], &(farVertices[2] - farVertices[0]),
					  &(nearVertices[0] - farVertices[0]));
		mFrustumPositions[2] = D3DXVECTOR3(-halfWidthNear, 0.0f, nearOrigin.z);

		D3DXVec3Cross(&mFrustumNormals[3], &(nearVertices[3] - nearVertices[1]),
					  &(farVertices[1] - nearVertices[1]));
		mFrustumPositions[3] = D3DXVECTOR3(halfWidthNear, 0.0f, nearOrigin.z);

		// Top & Bottom
		D3DXVec3Cross(&mFrustumNormals[4], &(nearVertices[3] - nearVertices[2]), 
					  &(farVertices[2] - nearVertices[2]));
		mFrustumPositions[4] = D3DXVECTOR3(0.0f, halfHeightNear, nearOrigin.z);

		D3DXVec3Cross(&mFrustumNormals[5], &(farVertices[1] - farVertices[0]),
					  &(nearVertices[0] - farVertices[0]));
		mFrustumPositions[5] = D3DXVECTOR3(0.0f, -halfHeightNear, nearOrigin.z);

		for (unsigned int i = 0; i < mFrustumNormals.size(); ++i)
		{
			D3DXVec3Normalize(&mFrustumNormals[i], &mFrustumNormals[i]);
		}
	}


	void DrawableFrustum::Draw(const Camera::Camera& camera)
	{
		mEffect.SetVariable("gWorld", mWorld);
		mEffect.SetVariable("gMVP", mWorld * camera.GetViewProjection());

		mFrustumBuffer.Bind();
		for (unsigned int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mFrustumBuffer.GetElementCount(), 0);
		}

		mNormalBuffer.Bind();
		for (unsigned int i = 0; i < mFrustumNormals.size(); ++i)
		{
			D3DXMATRIX world = GetNormalMatrix(mFrustumPositions[i], mFrustumNormals[i]);
			mEffect.SetVariable("gWorld", world * mWorld);
			mEffect.SetVariable("gMVP", world * mWorld * camera.GetViewProjection());

			for (unsigned int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
			{
				mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
				mDevice->Draw(mNormalBuffer.GetElementCount(), 0);
			}
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

	D3DXMATRIX DrawableFrustum::GetNormalMatrix(const D3DXVECTOR3& position, const D3DXVECTOR3& normal) const
	{
		float length = D3DXVec3Length(&normal);
		float phi = std::asin(normal.y / length);
		float theta = 0;

		if (normal.x < 10e-6 && normal.x > -10e-6)
			if (normal.z > 0)
				theta = D3DX_PI * 0.5;
			else
				theta = 3.0 * D3DX_PI * 0.5;
		else
			theta = std::atan2(normal.z, normal.x);
		theta -= D3DX_PI * 0.5;

		D3DXMATRIX rotation, translation, scale;
		D3DXMatrixRotationYawPitchRoll(&rotation, theta, phi, 0.0f);
		//D3DXMatrixRotationY(&rotation, theta);
		D3DXMatrixTranslation(&translation, position.x, position.y, position.z);
		D3DXMatrixScaling(&scale, length, length, length);

		//return rotation * translation * scale;
		return rotation * translation;
		//return rotation * scale * translation;
		//return translation;
	}
}