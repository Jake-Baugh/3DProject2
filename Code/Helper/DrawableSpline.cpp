#include <Helper\DrawableSpline.hpp>

namespace Helper
{
	DrawableSpline::DrawableSpline(ID3D10Device* device, BezierCurve* spline)
		: mDevice(device)
		, mVertexBuffer(mDevice)
		, mEffect(mDevice, "Resources/Effects/Curve.fx")
		, mSpline(spline)
	{
		CreateBuffer();
	}

	void DrawableSpline::Draw(const Camera::Camera& camera)
	{
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);

		mEffect.SetVariable("gMVP", camera.GetViewProjection());
		mEffect.SetVariable("gWorld", world);

		mVertexBuffer.Bind();
		for (unsigned int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mVertexBuffer.GetElementCount(), 0);
		}
	}

	void DrawableSpline::Commit()
	{
		CreateBuffer();
	}

	/*
	BezierCurve& DrawableSpline::GetSpline()
	{
		return mSpline;
	}
	*/

	void DrawableSpline::CreateBuffer()
	{
		std::vector<D3DXVECTOR3> curvePos;
		for (float t = 0.0f; t <= mSpline->GetLength(); t += 1.0f/100.0f)
			curvePos.push_back(mSpline->GetPos(t));

		Framework::VertexBuffer::Description desc;
		desc.ElementCount = curvePos.size();
		desc.ElementSize = sizeof(D3DXVECTOR3);
		desc.FirstElementPointer = &curvePos.front();
		desc.Topology = Framework::Topology::LineStrip;
		desc.Usage = Framework::Usage::Default;

		mVertexBuffer.SetData(desc, NULL);

		Framework::Effect::InputLayoutVector inputLayout;
		inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));

		mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);
	}
}