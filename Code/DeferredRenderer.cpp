#include "DeferredRenderer.hpp"
#include <Helper\r2tk\r2-exception.hpp>

DeferredRenderer::DeferredRenderer(Framework::D3DContext* d3dContext, int width, int height)
	: mD3DContext(d3dContext)
	, mDevice(mD3DContext->GetDevice())
	, mWidth(width)
	, mHeight(height)
{
	// Create the G buffers
	mColorBuffer = CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_BIND_RENDER_TARGET);
	mNormalBuffer = CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_BIND_RENDER_TARGET);
	mDepthStencilBuffer = CreateTexture(DXGI_FORMAT_R32_TYPELESS, D3D10_BIND_DEPTH_STENCIL);

	// Create the render target views and put them into an array
	mColorView = CreateRenderTargetView(mColorBuffer);
	mNormalView = CreateRenderTargetView(mNormalBuffer);

	mRenderTargets.push_back(mColorView);
	mRenderTargets.push_back(mNormalView);


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
	mNormalSRV = CreateShaderResourceView(mNormalBuffer, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mDepthStencilSRV = CreateShaderResourceView(mDepthStencilBuffer, DXGI_FORMAT_R32_FLOAT);
}

DeferredRenderer::~DeferredRenderer() throw()
{
	SafeRelease(mColorBuffer);
	SafeRelease(mColorView);
	SafeRelease(mColorSRV);

	SafeRelease(mNormalBuffer);
	SafeRelease(mNormalView);
	SafeRelease(mNormalSRV);

	SafeRelease(mDepthStencilBuffer);
	SafeRelease(mDepthStencilView);
	SafeRelease(mDepthStencilSRV);
}

void DeferredRenderer::BeginDeferredState()
{
	ID3D10ShaderResourceView* nullSRV = NULL;
	mDevice->PSSetShaderResources(0, 1, &nullSRV);

	mDevice->OMSetRenderTargets(mRenderTargets.size(), &mRenderTargets[0], mDepthStencilView);
	mDevice->ClearRenderTargetView(mColorView, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
	mDevice->ClearRenderTargetView(mNormalView, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
	mDevice->ClearDepthStencilView(mDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
}

void DeferredRenderer::EndDeferredState()
{
	mD3DContext->ResetRenderTarget();
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