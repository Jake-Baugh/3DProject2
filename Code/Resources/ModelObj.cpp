#include <Resources\ModelObj.hpp>

#include <sstream>
#include <fstream>
#include <vector>
#include <cassert>

#include <Helper\r2tk\r2-exception.hpp>

namespace Resources
{
	ModelObj::ModelObj(ID3D10Device* device, const std::string& objectFilename, const std::string& glowmapFilename, float alpha)
		: mDevice(device)
		, mDeferredEffect(mDevice, "Resources/Effects/ModelObjDeferred.fx")
		, mForwardEffect(mDevice, "Resources/Effects/ModelObjForward.fx")
		, mData(device, objectFilename)
		, mAlpha(alpha)
		, mTintColor(D3DXCOLOR(1.0, 1.0, 1.0, 1.0))
		, mGlowMap(NULL)
	{
		Framework::Effect::InputLayoutVector inputLayout;
		inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
		inputLayout.push_back(Framework::Effect::InputLayoutElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT));
		inputLayout.push_back(Framework::Effect::InputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT));

		mDeferredEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);
		mForwardEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);
		
		if (glowmapFilename.size() > 0)
		{
			mGlowMap = new Texture(mDevice, glowmapFilename);
		}
		else
		{
			mGlowMap = new Texture(mDevice, "blackPixel.png");		
		}
	}

	ModelObj::~ModelObj() throw()
	{
		SafeDelete(mGlowMap);
	}

	void ModelObj::Bind(unsigned int slot)
	{
		mData.VertexData.Bind(slot);
	}

	void ModelObj::DrawDeferred(const D3DXMATRIX& modelMatrix, const Camera::Camera& camera)
	{
		// Calculate the WVP
		D3DXMATRIX worldViewProjection = modelMatrix * camera.GetViewProjection();

		mDeferredEffect.SetVariable("gWorld", modelMatrix);
		mDeferredEffect.SetVariable("gMVP", worldViewProjection);
		mDeferredEffect.SetVariable("gTexture", mData.MaterialData->GetMaterial(mData.MaterialName)->MainTexture->GetShaderResourceView());
		mDeferredEffect.SetVariable("gGlowMap", mGlowMap->GetShaderResourceView());
		mDeferredEffect.SetVariable("Ka", mData.MaterialData->GetMaterial(mData.MaterialName)->Ambient.x);
		mDeferredEffect.SetVariable("Kd", mData.MaterialData->GetMaterial(mData.MaterialName)->Diffuse.x);
		mDeferredEffect.SetVariable("Ks", mData.MaterialData->GetMaterial(mData.MaterialName)->Specular.x);
		mDeferredEffect.SetVariable("A", mData.MaterialData->GetMaterial(mData.MaterialName)->SpecularExp);
		
		// Draw the buffer, once for each pass
		for(UINT p = 0; p < mDeferredEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mDeferredEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mData.VertexData.GetElementCount(), 0);
		}
	}

	void ModelObj::DrawForwarded(const D3DXMATRIX& modelMatrix, const Camera::Camera& camera)
	{
		// Calculate the WVP
		D3DXMATRIX worldViewProjection = modelMatrix * camera.GetViewProjection();

		mForwardEffect.SetVariable("gMVP", worldViewProjection);
		mForwardEffect.SetVariable("gTexture", mData.MaterialData->GetMaterial(mData.MaterialName)->MainTexture->GetShaderResourceView());
		mForwardEffect.SetVariable("gAlpha", mAlpha);

		// Draw the buffer, once for each pass
		for(UINT p = 0; p < mForwardEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mForwardEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mData.VertexData.GetElementCount(), 0);
		}
	}

	void ModelObj::SetTintColor(const D3DXCOLOR& newColor)
	{
		mTintColor = newColor;
	}

	const Helper::AABB3f& ModelObj::GetAABB() const
	{
		return mData.Box;
	}
}