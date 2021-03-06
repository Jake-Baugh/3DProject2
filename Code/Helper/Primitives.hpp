#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include <cmath>
#include <Helper\Global.hpp>

namespace Helper
{
	/**
		Some helper functions for the primitives in the WinAPI and/or DirectX library
	*/
	inline unsigned int GetRectWidth(const RECT& r)
	{
		return r.right - r.left;
	}

	inline unsigned int GetRectHeight(const RECT& r)
	{
		return r.bottom - r.top;
	}

	float RandomFloat(float low, float high);



	/**
		Define a point in 2D space
	*/
	template <typename T>
	struct Point2
	{
		T X;
		T Y;

		Point2();
		Point2(T x, T y);

		bool operator==(const Point2<T>& rhs) const;
		bool operator!=(const Point2<T>& rhs) const;
		Point2<T> operator+(const Point2<T>& rhs) const;
		Point2<T>& operator+=(const Point2<T>& rhs);
		Point2<T> operator-(const Point2<T>& rhs) const;
		Point2<T>& operator-=(const Point2<T>& rhs);
		Point2<T> operator*(T rhs) const;
		Point2<T>& operator*=(T rhs);
	};

	typedef Point2<int> Point2i;
	typedef Point2<float> Point2f;



	/**
		Define a point in 3D space
	*/
	template <typename T>
	struct Point3
	{
		T X;
		T Y;
		T Z;

		Point3();
		Point3(T x, T y, T z);

		const T& operator[](unsigned int index) const;
		T& operator[](unsigned int index);
		bool operator==(const Point3<T>& rhs) const;
		bool operator!=(const Point3<T>& rhs) const;
	};

	typedef Point3<int> Point3i;
	typedef Point3<float> Point3f;


	template <typename T>
	struct Interval
	{
		T Min;
		T Max;

		Interval();
		Interval(T min, T max);

		bool Overlaps(const Interval& rhs) const;
	};


	/**
		Define an axis aligned bounding box, in 2D space
	*/
	template <typename T>
	struct AABB2
	{
		Point2<T> Corners[2];

		AABB2();
		AABB2(T x, T y, T width, T height);
		AABB2(const Point2<T>& p1, const Point2<T>& p2);

		T GetLeft() const;
		T GetBottom() const;
		T GetWidth() const;
		T GetHeight() const;

		bool Intersects(const AABB2<T>& rhs) const;
	};

	typedef AABB2<int> AABB2i;
	typedef AABB2<float> AABB2f;



	/**
		Define an axis aligned bounding box, in 3D space
	*/
	template <typename T>
	struct AABB3
	{
		Point3<T> Corners[2];

		AABB3();
		AABB3(T x, T y, T z, T width, T height, T depth);
		AABB3(const Point3<T>& p1, const Point3<T>& p2);

		T GetLeftPlane() const;
		T GetBottomPlane() const;
		T GetFrontPlane() const;

		T GetWidth() const;
		T GetDepth() const;
		T GetHeight() const;
	};

	typedef AABB3<int> AABB3i;
	typedef AABB3<float> AABB3f;




	/**
		Defines a frustum
	*/
	struct Frustum
	{
		Frustum() : NearDistance(0.0f), FarDistance(0.0f), FieldOfViewY(0.0f), AspectRatio(0.0f) {}

		float NearDistance;
		float FarDistance;
		float FieldOfViewY;
		float AspectRatio;

		/**
			Create a perspective projection matrix from the
			frustum.
		*/
		D3DXMATRIX CreatePerspectiveProjection() const;
	};

	enum Collision
	{
		Outside,
		Intersects,
		Inside
	};

	Collision FrustumVsAABB(const Frustum& frustum, const D3DXVECTOR3& frustumPosition, const D3DXVECTOR3& frustumDirection, const AABB3f& aabb);


	/**
		IMPLEMENTATION
	*/

	template <typename T>
	Point2<T>::Point2() : X(0), Y(0) {}

	template <typename T>
	Point2<T>::Point2(T x, T y)
	{
		X = x;
		Y = y;
	}

	template <typename T>
	bool Point2<T>::operator==(const Point2<T>& rhs) const
	{
		return (X == rhs.X && Y == rhs.Y);
	}

	template <typename T>
	bool Point2<T>::operator!=(const Point2<T>& rhs) const
	{
		return !(*this == rhs);
	}

	template <typename T>
	Point2<T> Point2<T>::operator+(const Point2<T>& rhs) const
	{
		return Point2<T>(X + rhs.X, Y + rhs.Y);
	}

	template <typename T>
	Point2<T>& Point2<T>::operator+=(const Point2<T>& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;

		return *this;
	}

	template <typename T>
	Point2<T> Point2<T>::operator-(const Point2<T>& rhs) const
	{
		return Point2<T>(X - rhs.X, Y - rhs.Y);
	}

	template <typename T>
	Point2<T>& Point2<T>::operator-=(const Point2<T>& rhs)
	{
		X -= rhs.X;
		Y -= rhs.Y;

		return *this;
	}

	template <typename T>
	Point2<T> Point2<T>::operator*(T rhs) const
	{
		return Point2<T>(X * rhs, Y * rhs);
	}

	template <typename T>
	Point2<T>& Point2<T>::operator*=(T rhs)
	{
		X *= rhs;
		Y *= rhs;

		return *this;
	}



	template <typename T>
	Point3<T>::Point3() : X(0), Y(0), Z(0) {}

	template <typename T>
	Point3<T>::Point3(T x, T y, T z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	template <typename T>
	const T& Point3<T>::operator[](unsigned int index) const
	{
		switch(index)
		{
			case 0:
				return X;
			case 1:
				return Y;
			case 2:
				return Z;
		}
	}

	template <typename T>
	T& Point3<T>::operator[](unsigned int index)
	{
		switch(index)
		{
			case 0:
				return X;
			case 1:
				return Y;
			case 2:
				return Z;
		}
	}

	template <typename T>
	bool Point3<T>::operator==(const Point3<T>& rhs) const
	{
		return (X == rhs.X && Y == rhs.Y && Z == rhs.Z);
	}

	template <typename T>
	bool Point3<T>::operator!=(const Point3<T>& rhs) const
	{
		return !(*this == rhs);
	}


	template <typename T>
	Interval<T>::Interval()
		: Min(0)
		, Max(0)
	{}

	template <typename T>
	Interval<T>::Interval(T min, T max)
		: Min(min)
		, Max(max)
	{}

	template <typename T>
	bool Interval<T>::Overlaps(const Interval& rhs) const
	{
		T length1 = Max - Min;
		T length2 = rhs.Max - rhs.Min;

		T totLength = max(Max, rhs.Max) - min(Min, rhs.Min);

		return (totLength < length1 + length2);
	}




	template <typename T>
	AABB2<T>::AABB2() {}

	template <typename T>
	AABB2<T>::AABB2(T x, T y, T width, T height)
	{
		Corners[0] = Point2<T>(x, y);
		Corners[1] = Point2<T>(x + width, y + height);
	}

	template <typename T>
	AABB2<T>::AABB2(const Point2<T>& p1, const Point2<T>& p2)
	{
		Corners[0] = p1;
		Corners[1] = p2;
	}



	template <typename T>
	T AABB2<T>::GetLeft() const
	{
		return std::min(Corners[0].X, Corners[1].X);
	}

	template <typename T>
	T AABB2<T>::GetBottom() const
	{
		return std::min(Corners[0].Y, Corners[1].Y);
	}

	template <typename T>
	T AABB2<T>::GetWidth() const
	{
		return std::abs(Corners[0].X - Corners[1].X);
	}

	template <typename T>
	T AABB2<T>::GetHeight() const
	{
		return std::abs(Corners[0].Y - Corners[1].Y);
	}

	template <typename T>
	bool AABB2<T>::Intersects(const AABB2<T>& rhs) const
	{
		Interval<T> intervalsX[] = { Interval<T>(Corners[0].X, Corners[1].X), Interval<T>(rhs.Corners[0].X, rhs.Corners[1].X) };
		Interval<T> intervalsY[] = { Interval<T>(Corners[0].Y, Corners[1].Y), Interval<T>(rhs.Corners[0].Y, rhs.Corners[1].Y) };

		return (intervalsX[0].Overlaps(intervalsX[1]) && intervalsY[0].Overlaps(intervalsY[1]));
	}





	template <typename T>
	AABB3<T>::AABB3() {}

	template <typename T>
	AABB3<T>::AABB3(T x, T y, T z, T width, T depth, T height)
	{
		Corners[0] = Point3<T>(x, y, z);
		Corners[1] = Point3<T>(x + width, y + height, z + depth);
	}

	template <typename T>
	AABB3<T>::AABB3(const Point3<T>& p1, const Point3<T>& p2)
	{
		Corners[0] = p1;
		Corners[1] = p2;
	}


	template <typename T>
	T AABB3<T>::GetLeftPlane() const
	{
		return std::min(Corners[0].X, Corners[1].X);
	}

	template <typename T>
	T AABB3<T>::GetBottomPlane() const
	{
		return std::min(Corners[0].Y, Corners[1].Y);
	}

	template <typename T>
	T AABB3<T>::GetFrontPlane() const
	{
		return std::min(Corners[0].Z, Corners[1].Z);
	}


	template <typename T>
	T AABB3<T>::GetWidth() const
	{
		return std::abs(Corners[0].X - Corners[1].X);
	}

	template <typename T>
	T AABB3<T>::GetDepth() const
	{
		return std::abs(Corners[0].Z - Corners[1].Z);
	}

	template <typename T>
	T AABB3<T>::GetHeight() const
	{
		return std::abs(Corners[0].Y - Corners[1].Y);
	}
}

#endif