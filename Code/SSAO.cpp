#include <cmath>
#include <SSAO.hpp>

const D3DXMATRIX SSAO::C_TEXTURE_MATRIX(0.5f, 0.0f,  0.0f, 0.0f,
										0.0f, -0.5f, 0.0f, 0.0f,
										0.0f, 0.0f,  1.0f, 0.0f,
										0.5f, 0.5f,  0.0f, 1.0f);

SSAO::SSAO(ID3D10Device* device, int backBufferWidth, int backBufferHeight)
	: mDevice(device)
	, mFullscreenQuad(mDevice)
	, mEffect(mDevice, "Resources/Effects/SSAO.fx")
	, mBlurEffect(mDevice, "Resources/Effects/SSAOBlur.fx")
	, mRandomTextureSRV(NULL)
	, mOffsets(14)
	, mFrustumFarCorners(4)
	, mScreenSize((float)backBufferWidth, (float)backBufferHeight)
{
	Framework::Effect::InputLayoutVector inputLayout;
	inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
	inputLayout.push_back(Framework::Effect::InputLayoutElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT));
	inputLayout.push_back(Framework::Effect::InputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT));

	mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);

	inputLayout.erase(inputLayout.begin() + 1);

	mBlurEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);
	mBlurEffect.GetTechniqueByIndex(0).GetPassByIndex(1).SetInputLayout(inputLayout);

	/*
	SSAO::Vertex vertices[] = { { D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f) }
							  , { D3DXVECTOR3(-1.0f, +1.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f) }
							  , { D3DXVECTOR3(+1.0f, +1.0f, 0.0f), D3DXVECTOR3(2.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f) }
							  , { D3DXVECTOR3(+1.0f, -1.0f, 0.0f), D3DXVECTOR3(3.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f) } };

	Framework::VertexBuffer::Description bufferDescription;
	bufferDescription.ElementCount = 4;
	bufferDescription.ElementSize = sizeof(SSAO::Vertex);
	bufferDescription.FirstElementPointer = vertices;
	bufferDescription.Topology = Framework::Topology::TriangleStrip;
	bufferDescription.Usage = Framework::Usage::Default;
	*/

	SSAO::Vertex vertices[] = { { D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f) }
							  , { D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DXVECTOR3(3.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f) }
							  , { D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f) }
							  , { D3DXVECTOR3(1.0f, 1.0f, 0.0f), D3DXVECTOR3(2.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f) } };

	Framework::VertexBuffer::Description bufferDesc;
	bufferDesc.ElementCount = 4;
	bufferDesc.ElementSize = sizeof(SSAO::Vertex);
	bufferDesc.Topology = Framework::Topology::TriangleStrip;
	bufferDesc.Usage = Framework::Usage::Default;
	bufferDesc.FirstElementPointer = vertices;

	mFullscreenQuad.SetData(bufferDesc, NULL);

	BuildRandomTexture();
	BuildOffsetVectors();

	ID3D10EffectVariable* arrayVariable = mEffect.GetVariable("gOffsetVectors");
	for (int i = 0; i < mOffsets.size(); ++i)
	{
		ID3D10EffectVariable* v = arrayVariable->GetElement(i);
		v->AsVector()->SetFloatVector(mOffsets[i]);
	}

	mEffect.SetVariable("gScreenWidth", backBufferWidth);
	mEffect.SetVariable("gScreenHeight", backBufferHeight);
}

void SSAO::DrawSSAOBuffer(const Camera::Camera& camera, const Helper::Frustum& frustum, ID3D10ShaderResourceView* positionBuffer, ID3D10ShaderResourceView* normalBuffer, ID3D10ShaderResourceView* depthBuffer)
{
	BuildFrustumFarCorners(frustum);

	ID3D10EffectVariable* arrayVariable = mEffect.GetVariable("gFrustumFarCorners");
	for (int i = 0; i < 4; ++i)
	{
		ID3D10EffectVariable* v = arrayVariable->GetElement(i);
		v->AsVector()->SetFloatVector(mFrustumFarCorners[i]);
	}

	mEffect.SetVariable("gView", camera.GetView());
	mEffect.SetVariable("gProjTex", camera.GetProjection() * C_TEXTURE_MATRIX);

	mEffect.SetVariable("gPositionBuffer", positionBuffer);
	mEffect.SetVariable("gNormalBuffer", normalBuffer);
	mEffect.SetVariable("gDepthBuffer", depthBuffer);
	mEffect.SetVariable("gRandomBuffer", mRandomTextureSRV);

	mFullscreenQuad.Bind();
	for (unsigned int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
	{
		mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
		mDevice->Draw(mFullscreenQuad.GetElementCount(), 0);
	}

	

}

void SSAO::BlurSSAOBuffer(const Camera::Camera& camera, ID3D10ShaderResourceView* normalBuffer, ID3D10ShaderResourceView* depthBuffer, ID3D10ShaderResourceView* SSAOBuffer)
{
	mBlurEffect.SetVariable("gView", camera.GetView());
	mBlurEffect.SetVariable("gNormalBuffer", normalBuffer);
	mBlurEffect.SetVariable("gDepthBuffer", depthBuffer);
	mBlurEffect.SetVariable("gInputImage", SSAOBuffer);
	mBlurEffect.SetVariable("gTexelWidth", 1.0f / mScreenSize.x);
	mBlurEffect.SetVariable("gTexelHeight", 1.0f / mScreenSize.y);

	for (unsigned int p = 0; p < mBlurEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
	{
		mBlurEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
		mDevice->Draw(mFullscreenQuad.GetElementCount(), 0);
	}
}

void SSAO::BuildRandomTexture()
{
	D3D10_TEXTURE2D_DESC description;
	description.Width = 256;
	description.Height = 256;
	description.MipLevels = 1;
	description.ArraySize = 1;
	description.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	description.SampleDesc.Count = 1;
	description.SampleDesc.Quality = 0;
	description.Usage = D3D10_USAGE_IMMUTABLE;
	description.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	description.CPUAccessFlags = 0;
	description.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA data = { 0 };
	data.SysMemPitch = 256 * sizeof(D3DXCOLOR);

	D3DXCOLOR* color = new D3DXCOLOR[256 * 256];
	for (int i = 0; i < 256; ++i)
	{
		for (int j = 0; j < 256; ++j)
		{
			color[i * 256 + j] = D3DXCOLOR(Helper::RandomFloat(-1.0f, 1.0f), Helper::RandomFloat(-1.0f, 1.0f), Helper::RandomFloat(-1.0f, 1.0f), 0.0f);
		}
	}
	data.pSysMem = color;

	ID3D10Texture2D* texture;
	mDevice->CreateTexture2D(&description, &data, &texture);
	mDevice->CreateShaderResourceView(texture, NULL, &mRandomTextureSRV);

	SafeDeleteArray(color);
	SafeRelease(texture);
}

void SSAO::BuildOffsetVectors()
{
	// 8 cube corners
	mOffsets[0] = D3DXVECTOR4(+1.0f, +1.0f, +1.0f, 0.0f);
	mOffsets[1] = D3DXVECTOR4(-1.0f, -1.0f, -1.0f, 0.0f);

	mOffsets[2] = D3DXVECTOR4(-1.0f, +1.0f, +1.0f, 0.0f);
	mOffsets[3] = D3DXVECTOR4(+1.0f, -1.0f, -1.0f, 0.0f);

	mOffsets[4] = D3DXVECTOR4(+1.0f, +1.0f, -1.0f, 0.0f);
	mOffsets[5] = D3DXVECTOR4(-1.0f, -1.0f, +1.0f, 0.0f);

	mOffsets[6] = D3DXVECTOR4(-1.0f, +1.0f, -1.0f, 0.0f);
	mOffsets[7] = D3DXVECTOR4(+1.0f, -1.0f, +1.0f, 0.0f);

	// 6 centers of cube faces
	mOffsets[8] = D3DXVECTOR4(-1.0f, 0.0f, 0.0f, 0.0f);
	mOffsets[9] = D3DXVECTOR4(+1.0f, 0.0f, 0.0f, 0.0f);

	mOffsets[10] = D3DXVECTOR4(0.0f, -1.0f, 0.0f, 0.0f);
	mOffsets[11] = D3DXVECTOR4(0.0f, +1.0f, 0.0f, 0.0f);

	mOffsets[12] = D3DXVECTOR4(0.0f, 0.0f, -1.0f, 0.0f);
	mOffsets[13] = D3DXVECTOR4(0.0f, 0.0f, +1.0f, 0.0f);

	for (int i = 0; i < 14; ++i)
	{
		float s = Helper::RandomFloat(0.25f, 1.0f);
		D3DXVec4Normalize(&mOffsets[i], &mOffsets[i]);
		mOffsets[i] = s * mOffsets[i];
	}
}

void SSAO::BuildFrustumFarCorners(const Helper::Frustum& frustum)
{
	float halfHeight = frustum.FarDistance * std::tan(0.5f * frustum.FieldOfViewY);
	float halfWidth = halfHeight * frustum.AspectRatio;

	mFrustumFarCorners[0] = D3DXVECTOR4(-halfWidth, -halfHeight, frustum.FarDistance, 0.0f);
	mFrustumFarCorners[1] = D3DXVECTOR4(-halfWidth, +halfHeight, frustum.FarDistance, 0.0f);
	mFrustumFarCorners[2] = D3DXVECTOR4(+halfWidth, +halfHeight, frustum.FarDistance, 0.0f);
	mFrustumFarCorners[3] = D3DXVECTOR4(+halfWidth, -halfHeight, frustum.FarDistance, 0.0f);
}