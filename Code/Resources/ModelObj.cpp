#include <Resources\ModelObj.hpp>

#include <sstream>
#include <fstream>
#include <vector>
#include <cassert>

#include <Helper\r2tk\r2-exception.hpp>

namespace Resources
{
	ModelObj::ModelObj(ID3D10Device* device, const std::string& filename)
		: mDevice(device)
		, mEffect(device, "Resources/Effects/ModelObj.fx")
		, mData(device, filename)
		, mScale(1.0f)
		, mTintColor(D3DXCOLOR(1.0, 1.0, 1.0, 1.0))
	{
		Framework::Effect::InputLayoutVector inputLayout;
		inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
		inputLayout.push_back(Framework::Effect::InputLayoutElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT));
		inputLayout.push_back(Framework::Effect::InputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT));

		mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);
	}

	void ModelObj::Bind(unsigned int slot)
	{
		mData.VertexData.Bind(slot);
	}

	void ModelObj::Draw(const D3DXVECTOR3& drawPosition, const Helper::Camera& camera)
	{
		// Calculate the world matrix. Use worldViewProjection as temporary storage. Think green.
		D3DXMATRIX world;
		D3DXMATRIX worldViewProjection;
		D3DXMatrixScaling(&world, mScale, mScale, mScale);		
		D3DXMatrixTranslation(&worldViewProjection, drawPosition.x, drawPosition.y, drawPosition.z);
		world *= worldViewProjection;

		// Calculate the REAL worldViewProjection.
		worldViewProjection = world * camera.GetViewProjection();

		mEffect.SetVariable("g_matWorld", world);
		mEffect.SetVariable("g_matWVP", worldViewProjection);
		mEffect.SetVariable("g_modelTintColor", static_cast<D3DXVECTOR4>(mTintColor));
		mEffect.SetVariable("g_modelTexture", mData.MaterialData->GetMaterial(mData.MaterialName)->MainTexture->GetShaderResourceView());
		
		// DEBUG: get light position elsewhere
		mEffect.SetVariable("g_lightDirection", D3DXVECTOR4(50, 50, 0, 0));

		// Draw the buffer, once for each pass
		for(UINT p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mData.VertexData.GetElementCount(), 0);
		}
	}

	void ModelObj::Draw(const D3DXMATRIX& modelMatrix, const Helper::Camera& camera)
	{
		D3DXMATRIX worldViewProjection;

		// Calculate the REAL worldViewProjection.
		worldViewProjection = modelMatrix * camera.GetViewProjection();

		mEffect.SetVariable("g_matWorld", modelMatrix);
		mEffect.SetVariable("g_matWVP", worldViewProjection);
		mEffect.SetVariable("g_modelTintColor", static_cast<D3DXVECTOR4>(mTintColor));
		mEffect.SetVariable("g_modelTexture", mData.MaterialData->GetMaterial(mData.MaterialName)->MainTexture->GetShaderResourceView());

		// DEBUG: get light position elsewhere
		mEffect.SetVariable("g_lightDirection", D3DXVECTOR4(50, 50, 0, 0));

		// Draw the buffer, once for each pass
		for(UINT p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mData.VertexData.GetElementCount(), 0);
		}
	}

	void ModelObj::SetScale(float newScale)
	{
		mScale = newScale;
	}

	void ModelObj::SetTintColor(D3DXCOLOR newColor)
	{
		mTintColor = newColor;
	}
}