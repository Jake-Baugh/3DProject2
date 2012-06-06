#ifndef DRAWABLE_SPLINE_HPP
#define DRAWABLE_SPLINE_HPP

#include <Framework\VertexBuffer.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Helper\BezierCurve.hpp>
#include <Camera\Camera.hpp>

namespace Helper
{
	class DrawableSpline
	{
	public:
		DrawableSpline(ID3D10Device* device, BezierCurve* spline);

		void Draw(const Camera::Camera& camera);
		void Commit();
		//BezierCurve& GetSpline();

	private:
		ID3D10Device* mDevice;
		Framework::VertexBuffer mVertexBuffer;
		Framework::Effect::Effect mEffect;

		BezierCurve* mSpline;

		void CreateBuffer();
	};
}
#endif