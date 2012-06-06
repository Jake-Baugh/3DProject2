#include <cassert>
#include <cmath>
#include <Helper\BezierCurve.hpp>
#include <Helper\Global.hpp>

BezierSegment::BezierSegment(const D3DXVECTOR3& start, const D3DXVECTOR3& point2, const D3DXVECTOR3& point3, const D3DXVECTOR3& end)
{
	mPoints.reserve(C_COUNT);
	mPoints.push_back(start);
	mPoints.push_back(point2);
	mPoints.push_back(point3);
	mPoints.push_back(end);
}

BezierSegment::BezierSegment(const std::vector<D3DXVECTOR3>& points)
{
	assert(points.size() == C_COUNT);

	mPoints = points;
}

D3DXVECTOR3 BezierSegment::GetPos(float t) const
{
	D3DXVECTOR3 result;
	result = mPoints[0] * pow((1.0f - t), 3.0f);
	result += mPoints[1] * 3 * t * pow((1.0f - t), 2.0f);
	result += mPoints[2] * 3 * pow(t, 2.0f) * (1.0f - t);
	result += mPoints[3] * pow(t, 3.0f);

	return result;
}

const D3DXVECTOR3& BezierSegment::GetPoint(int index) const
{
	return mPoints[index];
}

void BezierSegment::SetPoint(int index, const D3DXVECTOR3& position)
{
	mPoints[index] = position;
}

BezierCurve::BezierCurve(const D3DXVECTOR3& start, const D3DXVECTOR3& point2, const D3DXVECTOR3& point3, const D3DXVECTOR3& end)
{
	mSegments.push_back(BezierSegment(start, point2, point3, end));
}

BezierCurve::BezierCurve(const std::vector<D3DXVECTOR3>& points)
{
	mSegments.push_back(BezierSegment(points));
}

D3DXVECTOR3 BezierCurve::GetPos(float t) const
{
	//t = Clamp(t, 0.0f, GetLength());

	int index = static_cast<int>(t);
	float fractpart, intpart;
	fractpart = modf(t, &intpart);
	if (index >= mSegments.size())
		return mSegments.back().GetPos(1.0f + fractpart);

	return mSegments[index].GetPos(fractpart);
}

const D3DXVECTOR3& BezierCurve::GetPoint(int index) const
{
	int segIndex = index / (BezierSegment::C_COUNT - 1);
	int pointIndex = index % (BezierSegment::C_COUNT - 1);
	if (index > 0 && pointIndex == 0)
		return mSegments[segIndex - 1].GetPoint(3);	// Get the point from the previous spline, to ensure it exists
	else
		return mSegments[segIndex].GetPoint(pointIndex);
}

int BezierCurve::GetCount() const
{
	return (mSegments.size() * 3) + 1;
}

float BezierCurve::GetLength() const
{
	return static_cast<float>(mSegments.size());
}

void BezierCurve::SetPoint(int index, const D3DXVECTOR3& position)
{
	int segIndex = index / (BezierSegment::C_COUNT - 1);
	int pointIndex = index % (BezierSegment::C_COUNT - 1);
	switch (pointIndex)
	{
	case 0:		//The point is a start and/or end point of a segment. Might need to change the point in two segments.
		if (index == 0)	//If the point is the start point
			mSegments[segIndex].SetPoint(pointIndex, position);
		else if (index == GetCount() - 1)	//If the point is the last point of the last segment
			mSegments[segIndex].SetPoint(3, position);
		else	//If the point exists twice (only end points exist once)
		{
			mSegments[segIndex].SetPoint(pointIndex, position);
			mSegments[segIndex - 1].SetPoint(3, position);
		}
		break;
	case 1:
		{		//The point is the second point of a segment. Might need to adjust the third point of previous segment
			D3DXVECTOR3 dir(mSegments[segIndex].GetPoint(pointIndex - 1) - mSegments[segIndex].GetPoint(pointIndex));
			mSegments[segIndex].SetPoint(pointIndex, position);
			if (index > 1)	//If the point isn't from the first segment
			{
				//Get the distance between previous points 2 and end
				FLOAT len = D3DXVec3Length(&(mSegments[segIndex].GetPoint(pointIndex - 1) - mSegments[segIndex - 1].GetPos(2)));
				D3DXVec3Normalize(&dir, &dir);	//Normalize direction to start point of segment
				mSegments[segIndex - 1].SetPoint(2, mSegments[segIndex].GetPoint(0) + (len * dir));	//Correct previous segment's point 2
			}
		}
		break;
	case 2:
		{
			D3DXVECTOR3 dir(mSegments[segIndex].GetPoint(pointIndex + 1) - mSegments[segIndex].GetPoint(pointIndex));
			mSegments[segIndex].SetPoint(pointIndex, position);
			if (index < GetCount() - 1)
			{
				//Get the distance between previous next segment's start and point2
				FLOAT len = D3DXVec3Length(&(mSegments[segIndex + 1].GetPoint(0) - mSegments[segIndex + 1].GetPoint(1)));
				D3DXVec3Normalize(&dir, &dir);
				mSegments[segIndex + 1].SetPoint(1, mSegments[segIndex + 1].GetPoint(0) + (len * dir));
			}
		}
		break;
	}
}

void BezierCurve::AddSegment(float secondDistance, const D3DXVECTOR3& point3, const D3DXVECTOR3& end)
{
	D3DXVECTOR3 point2;
	D3DXVECTOR3 dir = mSegments.back().GetPoint(3) - mSegments.back().GetPoint(2);
	D3DXVec3Normalize(&dir, &dir);
	point2 = mSegments.back().GetPoint(3) + (secondDistance * dir);
	mSegments.push_back(BezierSegment(mSegments.back().GetPoint(3), point2, point3, end));
}