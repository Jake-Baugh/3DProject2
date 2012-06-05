#ifndef BEZIER_CURVE_HPP
#define BEZIER_CURVE_HPP

#include <vector>
#include <Helper\Global.hpp>

class BezierSegment
{
public:
	BezierSegment(const D3DXVECTOR3& start, const D3DXVECTOR3& point2, const D3DXVECTOR3& point3, const D3DXVECTOR3& end);
	BezierSegment(const std::vector<D3DXVECTOR3>& points);

	// t HAS to be 0.0f <= t <= 1.0f
	D3DXVECTOR3 GetPos(float t) const;
	// index HAS to be 0 <= index < 4
	const D3DXVECTOR3& GetPoint(int index) const;

	void SetPoint(int index, const D3DXVECTOR3& position);

	// This class only uses segments of the third degree.
	static const int C_COUNT = 4;
private:
	std::vector<D3DXVECTOR3> mPoints;
};

class BezierCurve
{
public:
	BezierCurve(const D3DXVECTOR3& start, const D3DXVECTOR3& point2, const D3DXVECTOR3& point3, const D3DXVECTOR3& end);
	BezierCurve(const std::vector<D3DXVECTOR3>& points);

	// t HAS to be 0.0f <= t <= this->GetCount()
	D3DXVECTOR3 GetPos(float t) const;
	const D3DXVECTOR3& GetPoint(int index) const;
	int GetCount() const;
	float GetLength() const;

	void SetPoint(int index, const D3DXVECTOR3& position);
	// secondDistance is the length between the start point and point2
	// (the start point is the end point of the previous segment, FYI)
	void AddSegment(float secondDistance, const D3DXVECTOR3& point3, const D3DXVECTOR3& end);
private:
	std::vector<BezierSegment> mSegments;
};

#endif