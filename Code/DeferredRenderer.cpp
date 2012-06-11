#include "DeferredRenderer.hpp"
#include <Helper\r2tk\r2-exception.hpp>
#include <Helper\r2tk\r2-assert.hpp>

DeferredRenderer::DeferredRenderer(Framework::D3DContext* d3dContext, int width, int height)
	: mD3DContext(d3dContext)
	, mDevice(mD3DContext->GetDevice())
	, mWidth(width)
	, mHeight(height)
	, mLightEffect(mDevice, "Resources/Effects/Light.fx")
	, mBufferEffect(mDevice, "Resources/Effects/GBuffer.fx")
	, mFullscreenQuad(mDevice)
	, mSSAO(mDevice, mD3DContext->GetViewportWidth(mD3DContext->GetActiveViewport()), mD3DContext->GetViewportHeight(mD3DContext->GetActiveViewport()))
	, mSSAOToggle(true)
{
	// Create the fullscreen quad VB
	DeferredRenderer::QuadVertex vertices[] = { { D3DXVECTOR2(-1.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f) }
											  , { D3DXVECTOR2(1.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f) }
											  , { D3DXVECTOR2(-1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) }
											  , { D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f) } };

	Framework::VertexBuffer::Description bufferDesc;
	bufferDesc.ElementCount = 4;
	bufferDesc.ElementSize = sizeof(DeferredRenderer::QuadVertex);
	bufferDesc.Topology = Framework::Topology::TriangleStrip;
	bufferDesc.Usage = Framework::Usage::Default;
	bufferDesc.FirstElementPointer = vertices;

	mFullscreenQuad.SetData(bufferDesc, NULL);

	Framework::Effect::InputLayoutVector inputLayout;
	inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32_FLOAT));
	inputLayout.push_back(Framework::Effect::InputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT));
	
	mLightEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);
	mBufferEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);


	// Create the G buffers
	mColorBuffer = CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_BIND_RENDER_TARGET);
	mPositionBuffer = CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_BIND_RENDER_TARGET);
	mNormalBuffer = CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_BIND_RENDER_TARGET);
	mMaterialBuffer = CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_BIND_RENDER_TARGET);
	mSSAOBuffer = CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_BIND_RENDER_TARGET);
	mSSAOPostBuffer = CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_BIND_RENDER_TARGET);
	mDepthStencilBuffer = CreateTexture(DXGI_FORMAT_R32_TYPELESS, D3D10_BIND_DEPTH_STENCIL);

	mGBuffers.push_back(mColorBuffer);
	mGBuffers.push_back(mPositionBuffer);
	mGBuffers.push_back(mNormalBuffer);
	mGBuffers.push_back(mMaterialBuffer);
	mGBuffers.push_back(mSSAOBuffer);
	mGBuffers.push_back(mSSAOPostBuffer);
	mGBuffers.push_back(mDepthStencilBuffer);


	// Create the render target views
	mColorView = CreateRenderTargetView(mColorBuffer);
	mPositionView = CreateRenderTargetView(mPositionBuffer);
	mNormalView = CreateRenderTargetView(mNormalBuffer);
	mMaterialView = CreateRenderTargetView(mMaterialBuffer);
	mSSAOView = CreateRenderTargetView(mSSAOBuffer);
	mSSAOPostView = CreateRenderTargetView(mSSAOPostBuffer);

	mRenderTargets.push_back(mColorView);
	mRenderTargets.push_back(mPositionView);
	mRenderTargets.push_back(mNormalView);
	mRenderTargets.push_back(mMaterialView);

	// Create depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
	depthStencilViewDescription.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDescription.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescription.Texture2D.MipSlice = 0;

	HRESULT result = mDevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDescription, &mDepthStencilView);
	if (FAILED(result))
		throw r2ExceptionRuntimeM("Failed to create G depth stencil view");


	// Create the shader resource views
	mColorSRV = CreateShaderResourceView(mColorBuffer, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mPositionSRV = CreateShaderResourceView(mPositionBuffer, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mNormalSRV = CreateShaderResourceView(mNormalBuffer, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mMaterialSRV = CreateShaderResourceView(mMaterialBuffer, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mSSAOSRV = CreateShaderResourceView(mSSAOBuffer, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mSSAOPostSRV = CreateShaderResourceView(mSSAOPostBuffer, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mDepthStencilSRV = CreateShaderResourceView(mDepthStencilBuffer, DXGI_FORMAT_R32_FLOAT);

	mShaderResourceViews.push_back(mColorSRV);
	mShaderResourceViews.push_back(mPositionSRV);
	mShaderResourceViews.push_back(mNormalSRV);
	mShaderResourceViews.push_back(mMaterialSRV);
	mShaderResourceViews.push_back(mSSAOSRV);
	mShaderResourceViews.push_back(mSSAOPostSRV);
	mShaderResourceViews.push_back(mDepthStencilSRV);
}

DeferredRenderer::~DeferredRenderer() throw()
{
	SafeRelease(mColorBuffer);
	SafeRelease(mColorView);
	SafeRelease(mColorSRV);

	SafeRelease(mPositionBuffer);
	SafeRelease(mPositionView);
	SafeRelease(mPositionSRV);

	SafeRelease(mNormalBuffer);
	SafeRelease(mNormalView);
	SafeRelease(mNormalSRV);

	SafeRelease(mMaterialBuffer);
	SafeRelease(mMaterialView);
	SafeRelease(mMaterialSRV);

	SafeRelease(mDepthStencilBuffer);
	SafeRelease(mDepthStencilView);
	SafeRelease(mDepthStencilSRV);

	SafeRelease(mSSAOBuffer);
	SafeRelease(mSSAOView);
	SafeRelease(mSSAOSRV);

	SafeRelease(mSSAOPostBuffer);
	SafeRelease(mSSAOPostView);
	SafeRelease(mSSAOPostSRV);
}

void DeferredRenderer::ToggleSSAO(bool ssaoOn)
{
	mSSAOToggle = ssaoOn;
}

bool DeferredRenderer::GetSSAOToggle() const
{
	return mSSAOToggle;
}

void DeferredRenderer::SetDirectionalLight(const DirectionalLight& light)
{
	mDirectionalLight = light;

	// Update the directional light struct in the shader
	ID3D10EffectVariable* structVariable = mLightEffect.GetVariable("gDirectionalLight");

	structVariable->GetMemberByName("DirectionW")->AsVector()->SetFloatVector((FLOAT*)&mDirectionalLight.Direction);
	structVariable->GetMemberByName("Intensity")->AsVector()->SetFloatVector((FLOAT*)&mDirectionalLight.Intensity);
}

size_t DeferredRenderer::AddPointLight(const PointLight& light)
{
	mPointLights.push_back(light);
	
	// Update the point light array in the shader
	ID3D10EffectVariable* arrayVariable = mLightEffect.GetVariable("gPointLights");
	for (int i = 0; i < mPointLights.size(); ++i)
	{
		ID3D10EffectVariable* structVariable = arrayVariable->GetElement(i);

		structVariable->GetMemberByName("PositionW")->AsVector()->SetFloatVector((FLOAT*)&mPointLights[i].Position);
		
		D3DXVECTOR3 v = mPointLights[i].Intensity;
		structVariable->GetMemberByName("Intensity")->SetRawValue(&v, 0, sizeof(D3DXVECTOR3));

		structVariable->GetMemberByName("Radius")->AsScalar()->SetFloat(mPointLights[i].Radius);
	}
	
	// Update the count of point lights in the shader
	mLightEffect.SetVariable("gPointLightCount", static_cast<int>(mPointLights.size()));
	
	// Return the ID of this point light
	return mPointLights.size() - 1;
}

void DeferredRenderer::SetAmbientLight(const D3DXVECTOR3& light)
{
	mAmbientLight = light;
}


void DeferredRenderer::BeginDeferredState()
{
	std::vector<ID3D10ShaderResourceView*> nullSRVs;
	for (int i = 0; i < mRenderTargets.size(); ++i)
		nullSRVs.push_back(NULL);
	mDevice->PSSetShaderResources(0, mRenderTargets.size(), &nullSRVs[0]);

	mDevice->OMSetRenderTargets(mRenderTargets.size(), &mRenderTargets[0], mDepthStencilView);
	for (int i = 0; i < mRenderTargets.size(); ++i)
	{
		mDevice->ClearRenderTargetView(mRenderTargets[i], D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
	}
	
	mDevice->ClearDepthStencilView(mDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
}

void DeferredRenderer::EndDeferredState(const Camera::Camera& camera, const Helper::Frustum& frustum)
{
	mD3DContext->ResetRenderTarget();

	mDevice->OMSetRenderTargets(1, &mSSAOView, NULL);
	mDevice->ClearRenderTargetView(mSSAOView, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

	if (mSSAOToggle)
	{
		mSSAO.DrawSSAOBuffer(camera, frustum, mPositionSRV, mNormalSRV, mDepthStencilSRV);
		
		//// Unbind from pixel shader
		//ID3D10ShaderResourceView* nullSRV = NULL;
		//mDevice->PSSetShaderResources(0, 1, &nullSRV);
		//
		//mDevice->OMSetRenderTargets(1, &mSSAOPostView, NULL);
		//mDevice->ClearRenderTargetView(mSSAOPostView, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

		//mSSAO.BlurSSAOBuffer(camera, mNormalSRV, mDepthStencilSRV, mSSAOSRV);
	}

	ID3D10ShaderResourceView* nullSRV = NULL;
	mDevice->PSSetShaderResources(0, 1, &nullSRV);
}

void DeferredRenderer::ApplyLightingPhase(const Camera::Camera& camera)
{
	ID3D10RenderTargetView* backBuffer = mD3DContext->GetBackBufferView();
	ID3D10ShaderResourceView* nullSRV = NULL;

	mDevice->PSSetShaderResources(0, 1, &nullSRV);
	mDevice->OMSetRenderTargets(1, &backBuffer, NULL);
	mDevice->ClearRenderTargetView(backBuffer, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

	mLightEffect.SetVariable("gView", camera.GetView());
	mLightEffect.SetVariable("gProjection", camera.GetProjection());

	mLightEffect.SetVariable("gAmbientLightIntensity", mAmbientLight);
	mLightEffect.SetVariable("gEyePositionW", camera.GetPosition());

	mLightEffect.SetVariable("gColorBuffer", mColorSRV);
	mLightEffect.SetVariable("gPositionBuffer", mPositionSRV);
	mLightEffect.SetVariable("gNormalBuffer", mNormalSRV);
	mLightEffect.SetVariable("gMaterialBuffer", mMaterialSRV);
	mLightEffect.SetVariable("gSSAOBuffer", mSSAOSRV);
	mLightEffect.SetVariable("gSSAOToggle", mSSAOToggle);

	mFullscreenQuad.Bind();
	for (unsigned int p = 0; p < mLightEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
	{
		mLightEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
		mDevice->Draw(mFullscreenQuad.GetElementCount(), 0);
	}

	mD3DContext->ResetRenderTarget();
}

void DeferredRenderer::BeginForwardState()
{
	ID3D10RenderTargetView* backBuffer = mD3DContext->GetBackBufferView();
	mDevice->OMSetRenderTargets(1, &backBuffer, mDepthStencilView);
}

void DeferredRenderer::EndForwardState()
{
	mD3DContext->ResetRenderTarget();
}

void DeferredRenderer::RenderBuffer(ID3D10ShaderResourceView* buffer)
{
	bool renderDepth = buffer == mDepthStencilSRV;

	mBufferEffect.SetVariable("gBuffer", buffer);
	mBufferEffect.SetVariable("gDepthMode", renderDepth);

	mFullscreenQuad.Bind();
	for (unsigned int p = 0; p < mBufferEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
	{
		mBufferEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
		mDevice->Draw(mFullscreenQuad.GetElementCount(), 0);
	}
}

ID3D10ShaderResourceView* DeferredRenderer::GetColorBuffer() const
{
	return mColorSRV;
}

ID3D10ShaderResourceView* DeferredRenderer::GetPositionBuffer() const
{
	return mPositionSRV;
}

ID3D10ShaderResourceView* DeferredRenderer::GetNormalBuffer() const
{
	return mNormalSRV;
}

ID3D10ShaderResourceView* DeferredRenderer::GetMaterialBuffer() const
{
	return mMaterialSRV;
}

ID3D10ShaderResourceView* DeferredRenderer::GetDepthStencilBuffer() const
{
	return mDepthStencilSRV;
}

ID3D10ShaderResourceView* DeferredRenderer::GetGBufferByIndex(unsigned int index) const
{

	r2AssertM(index <= GBuffer::Count, "Invalid G buffer index");

	return mShaderResourceViews[index];
}



ID3D10Texture2D* DeferredRenderer::CreateTexture(DXGI_FORMAT format, UINT bindFlags) const
{
	ID3D10Texture2D* result;

	D3D10_TEXTURE2D_DESC textureDescription;
	textureDescription.Width = mWidth;
	textureDescription.Height = mHeight;
	textureDescription.MipLevels = 1;
	textureDescription.ArraySize = 1;
	textureDescription.Format = format;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;
	textureDescription.Usage = D3D10_USAGE_DEFAULT;
	textureDescription.BindFlags = bindFlags | D3D10_BIND_SHADER_RESOURCE;
	textureDescription.CPUAccessFlags = 0;
	textureDescription.MiscFlags = 0;

	if (FAILED(mDevice->CreateTexture2D(&textureDescription, NULL, &result)))
		throw r2ExceptionRuntimeM("Failed to create G buffer texture");

	return result;
}

ID3D10RenderTargetView* DeferredRenderer::CreateRenderTargetView(ID3D10Texture2D* buffer) const
{
	ID3D10RenderTargetView* result;

	D3D10_TEXTURE2D_DESC bufferDesc;
	buffer->GetDesc(&bufferDesc);

	D3D10_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = bufferDesc.Format;
	desc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	if (FAILED(mDevice->CreateRenderTargetView(buffer, &desc, &result)))
		throw r2ExceptionRuntimeM("Failed to create G render target view");

	return result;
}

ID3D10ShaderResourceView* DeferredRenderer::CreateShaderResourceView(ID3D10Texture2D* buffer, DXGI_FORMAT format) const
{
	ID3D10ShaderResourceView* result;

	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
    srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

	if (FAILED(mDevice->CreateShaderResourceView(buffer, &srvDesc, &result)))
		throw r2ExceptionRuntimeM("Failed to create G shader resource view");

	return result;
}