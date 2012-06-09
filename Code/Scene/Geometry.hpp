#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <Helper\Global.hpp>
#include <Helper\Primitives.hpp>
#include <Helper\DrawableBox.hpp>
#include <Camera\Camera.hpp>
#include <Resources\ModelObj.hpp>

namespace Scene
{
	class Geometry
	{
	public:
		Geometry(ID3D10Device* device, Resources::ModelObj* model, const D3DXMATRIX& world);

		const Helper::AABB3f& GetAABB() const;

		void SetWorld(const D3DXMATRIX& world);
		const D3DXMATRIX& GetWorld() const;

		void DrawDeferred(const Camera::Camera& camera);
		void DrawForwarded(const Camera::Camera& camera);
		void DrawAABB(const Camera::Camera& camera);
	private:
		Helper::AABB3f CreateAABB() const;

		ID3D10Device* mDevice;

		Resources::ModelObj* mModel;
		D3DXMATRIX mWorld;

		Helper::AABB3f mAABB;
		Helper::DrawableBox mDrawableAABB;
	};
}

#endif