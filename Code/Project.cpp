#include <Project.hpp>
#include <CircleCameraController.hpp>

Project::WindowDescription::WindowDescription()
{
	Description.Caption = "3D Project 2";
	Description.ClientWidth = 1024;
	Description.ClientHeight = 768;
	Description.Resizable = false;
}

Project::ContextDescription::ContextDescription()
{
	Description.Fullscreen = false;
	Description.Viewports.push_back(Framework::D3DContext::Viewport());
}

Project::ProjectionDescription::ProjectionDescription(unsigned int clientWidth, unsigned int clientHeight)
{
	Frustum.NearDistance = 1.0f;
	Frustum.FarDistance = 1000.0f;
	Frustum.FieldOfViewY = D3DX_PI * 0.25;
	Frustum.AspectRatio = static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
}

Project::Project(HINSTANCE instance)
	: Game(instance, WindowDescription().Description, ContextDescription().Description)
	, mCamera(ProjectionDescription(mWindow.GetClientWidth(), mWindow.GetClientHeight()).Frustum.CreatePerspectiveProjection(), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f))
	, mModel(mD3DContext.GetDevice(), "Pacman2.obj")
	, mCameraController(new CircleCameraController(&mCamera, D3DXVECTOR3(0, 0, 0), 15.0f, 20.0f))
	, mGround(mD3DContext.GetDevice())
	, mCurve(D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,2,10), D3DXVECTOR3(0,1,11), D3DXVECTOR3(0,5,15))
	, mCurveBuffer(mD3DContext.GetDevice())
	, mCurveEffect(mD3DContext.GetDevice(), "Resources/Effects/Curve.fx")
{
	std::vector<D3DXVECTOR3> curvePos;
	mCurve.AddSegment(5, D3DXVECTOR3(-3, 5, 10), D3DXVECTOR3(5, 2, 0));
	mCurve.AddSegment(6, D3DXVECTOR3(3, 6, 9), D3DXVECTOR3(9, 18, 12));
	for (float t = 0.0f; t <= mCurve.GetLength(); t += 1.0f/100.0f)
		curvePos.push_back(mCurve.GetPos(t));

	Framework::VertexBuffer::Description desc;
	desc.ElementCount = curvePos.size();
	desc.ElementSize = sizeof(D3DXVECTOR3);
	desc.FirstElementPointer = &curvePos.front();
	desc.Topology = Framework::Topology::LineStrip;
	desc.Usage = Framework::Usage::Default;

	mCurveBuffer.SetData(desc, NULL);

	Framework::Effect::InputLayoutVector inputLayout;
	inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));

	mCurveEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);

	
}

Project::~Project() throw()
{
	SafeDelete(mCameraController);
}

void Project::KeyPressed(Framework::ApplicationWindow* window, int keyCode)
{
	switch (keyCode)
	{
		case VK_ESCAPE:
			Quit();
			break;
	}
}

void Project::Update(float dt)
{
	mCameraController->Update(dt);
	mCamera.Commit();
}

void Project::Draw(float dt)
{
	mCurveEffect.SetVariable("gMVP", mCamera.GetViewProjection());

	mCurveBuffer.Bind();
	for (unsigned int p = 0; p < mCurveEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
	{
		mCurveEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mD3DContext.GetDevice());
		mD3DContext.GetDevice()->Draw(mCurveBuffer.GetElementCount(), 0);
	}
	mGround.Draw(mCamera);
	mModel.Bind();
	mModel.Draw(D3DXVECTOR3(0.0f, 1.0f, 0.0f), mCamera);
}
