#include <stdlib.h>
#include <math.h>
#include "Vector3D.hpp"

#define Assert( _exp ) ((void)0)

#define FastSqrt(x)			(sqrt)(x)

#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

#define M_PHI		1.61803398874989484820 // golden ratio

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

// MOVEMENT INFO
enum
{
    PITCH = 0,	// up / down
    YAW,		// left / right
    ROLL		// fall over
};

// decls for aligning data

#define DECL_ALIGN(x) __attribute__((aligned(x)))


#define ALIGN16 DECL_ALIGN(16)
#define VALVE_RAND_MAX 0x7fff
#define VectorExpand(v) (v).x, (v).y, (v).z

struct matrix3x4_t
{
    matrix3x4_t() {}
    matrix3x4_t(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23)
    {
        m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
        m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
        m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
    }

    float* operator[](int i) { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
    const float* operator[](int i) const { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
    float* Base() { return &m_flMatVal[0][0]; }
    const float* Base() const { return &m_flMatVal[0][0]; }

    float m_flMatVal[3][4];
};

class VMatrix
{
public:

    VMatrix() {}
    VMatrix(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
    ) {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;

        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;

        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;

        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }



    // array access
    inline float* operator[](int i)
    {
        return m[i];
    }

    inline const float* operator[](int i) const
    {
        return m[i];
    }

    // Get a pointer to m[0][0]
    inline float* Base()
    {
        return &m[0][0];
    }

    inline const float* Base() const
    {
        return &m[0][0];
    }


public:
    // The matrix.
    float		m[4][4];
};

class Vector
{
public:
    float x, y, z;
    Vector(void);
    Vector(float X, float Y, float Z);
    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
    bool IsValid() const;
    float operator[](int i) const;
    float& operator[](int i);
    inline void Zero();
    bool operator==(const Vector& v) const;
    bool operator!=(const Vector& v) const;
    inline Vector& operator+=(const Vector& v);
    inline Vector& operator-=(const Vector& v);
    inline Vector& operator*=(const Vector& v);
    inline Vector& operator*=(float s);
    inline Vector& operator/=(const Vector& v);
    inline Vector& operator/=(float s);
    inline Vector& operator+=(float fl);
    inline Vector& operator-=(float fl);
    inline float	Length() const;
    inline float LengthSqr(void) const
    {
        return (x * x + y * y + z * z);
    }
    bool IsZero(float tolerance = 0.01f) const
    {
        return (x > -tolerance && x < tolerance &&
                y > -tolerance && y < tolerance &&
                z > -tolerance && z < tolerance);
    }
    Vector	Normalize();
    void	NormalizeInPlace();
    inline float	DistTo(const Vector& vOther) const;
    inline float	DistToSqr(const Vector& vOther) const;
    float	Dot(const Vector& vOther) const;
    float	Length2D(void) const;
    float	Length2DSqr(void) const;
    Vector& operator=(const Vector& vOther);
    Vector	operator-(void) const;
    Vector	operator+(const Vector& v) const;
    Vector	operator-(const Vector& v) const;
    Vector	operator*(const Vector& v) const;
    Vector	operator/(const Vector& v) const;
    Vector	operator*(float fl) const;
    Vector	operator/(float fl) const;
    // Base address...
    float* Base();
    float const* Base() const;
};

//===============================================
inline void Vector::Init(float ix, float iy, float iz)
{
    x = ix; y = iy; z = iz;
}
//===============================================
inline Vector::Vector(float X, float Y, float Z)
{
    x = X; y = Y; z = Z;
}
//===============================================
inline Vector::Vector(void) { Zero(); }
//===============================================
inline void Vector::Zero()
{
    x = y = z = 0.0f;
}
//===============================================
inline void VectorClear(Vector& a)
{
    a.x = a.y = a.z = 0.0f;
}
//===============================================
inline Vector& Vector::operator=(const Vector& vOther)
{
    x = vOther.x; y = vOther.y; z = vOther.z;
    return *this;
}
//===============================================
inline float& Vector::operator[](int i)
{
    Assert((i >= 0) && (i < 3));
    return ((float*)this)[i];
}
//===============================================
inline float Vector::operator[](int i) const
{
    Assert((i >= 0) && (i < 3));
    return ((float*)this)[i];
}
//===============================================
inline bool Vector::operator==(const Vector& src) const
{
    return (src.x == x) && (src.y == y) && (src.z == z);
}
//===============================================
inline bool Vector::operator!=(const Vector& src) const
{
    return (src.x != x) || (src.y != y) || (src.z != z);
}
//===============================================
inline void VectorCopy(const Vector& src, Vector& dst)
{
    dst.x = src.x;
    dst.y = src.y;
    dst.z = src.z;
}
//===============================================
inline  Vector& Vector::operator+=(const Vector& v)
{
    x += v.x; y += v.y; z += v.z;
    return *this;
}
//===============================================
inline  Vector& Vector::operator-=(const Vector& v)
{
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}
//===============================================
inline  Vector& Vector::operator*=(float fl)
{
    x *= fl;
    y *= fl;
    z *= fl;

    return *this;
}
//===============================================
inline  Vector& Vector::operator*=(const Vector& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;

    return *this;
}
//===============================================
inline Vector& Vector::operator+=(float fl)
{
    x += fl;
    y += fl;
    z += fl;

    return *this;
}
//===============================================
inline Vector& Vector::operator-=(float fl)
{
    x -= fl;
    y -= fl;
    z -= fl;

    return *this;
}
//===============================================
inline  Vector& Vector::operator/=(float fl)
{
    Assert(fl != 0.0f);
    float oofl = 1.0f / fl;
    x *= oofl;
    y *= oofl;
    z *= oofl;

    return *this;
}
//===============================================
inline  Vector& Vector::operator/=(const Vector& v)
{
    Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
    x /= v.x;
    y /= v.y;
    z /= v.z;

    return *this;
}
//===============================================
inline float Vector::Length(void) const
{


    float root = 0.0f;

    float sqsr = x * x + y * y + z * z;

    root = sqrt(sqsr);

    return root;
}
//===============================================
inline float Vector::Length2D(void) const
{
    float root = 0.0f;

    float sqst = x * x + y * y;

    root = sqrt(sqst);

    return root;
}
//===============================================
inline float Vector::Length2DSqr(void) const
{
    return (x * x + y * y);
}
//===============================================
inline Vector CrossProduct(const Vector& a, const Vector& b)
{
    return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
//===============================================
float Vector::DistTo(const Vector& vOther) const
{
    Vector delta;

    delta.x = x - vOther.x;
    delta.y = y - vOther.y;
    delta.z = z - vOther.z;

    return delta.Length();
}
float Vector::DistToSqr(const Vector& vOther) const
{
    Vector delta;

    delta.x = x - vOther.x;
    delta.y = y - vOther.y;
    delta.z = z - vOther.z;

    return delta.LengthSqr();
}
//===============================================
inline Vector Vector::Normalize()
{
    Vector vector;
    float length = this->Length();

    if (length != 0)
    {
        vector.x = x / length;
        vector.y = y / length;
        vector.z = z / length;
    }
    else
    {
        vector.x = vector.y = 0.0f; vector.z = 1.0f;
    }

    return vector;
}
//===============================================
inline void Vector::NormalizeInPlace()
{
    Vector& v = *this;

    float iradius = 1.f / (this->Length() + 1.192092896e-07F); //FLT_EPSILON

    v.x *= iradius;
    v.y *= iradius;
    v.z *= iradius;
}
//===============================================
inline float VectorNormalize(Vector& v)
{
    Assert(v.IsValid());
    float l = v.Length();
    if (l != 0.0f)
    {
        v /= l;
    }
    else
    {
        // FIXME:
        // Just copying the existing implemenation; shouldn't res.z == 0?
        v.x = v.y = 0.0f; v.z = 1.0f;
    }
    return l;
}
//===============================================
inline float VectorNormalize(float* v)
{
    return VectorNormalize(*(reinterpret_cast<Vector*>(v)));
}
//===============================================
inline Vector Vector::operator+(const Vector& v) const
{
    Vector res;
    res.x = x + v.x;
    res.y = y + v.y;
    res.z = z + v.z;
    return res;
}

//===============================================
inline Vector Vector::operator-(const Vector& v) const
{
    Vector res;
    res.x = x - v.x;
    res.y = y - v.y;
    res.z = z - v.z;
    return res;
}
//===============================================
inline Vector Vector::operator*(float fl) const
{
    Vector res;
    res.x = x * fl;
    res.y = y * fl;
    res.z = z * fl;
    return res;
}
//===============================================
inline Vector Vector::operator*(const Vector& v) const
{
    Vector res;
    res.x = x * v.x;
    res.y = y * v.y;
    res.z = z * v.z;
    return res;
}
//===============================================
inline Vector Vector::operator/(float fl) const
{
    Vector res;
    res.x = x / fl;
    res.y = y / fl;
    res.z = z / fl;
    return res;
}
//===============================================
inline Vector Vector::operator/(const Vector& v) const
{
    Vector res;
    res.x = x / v.x;
    res.y = y / v.y;
    res.z = z / v.z;
    return res;
}
inline float Vector::Dot(const Vector& vOther) const
{
    const Vector& a = *this;

    return(a.x * vOther.x + a.y * vOther.y + a.z * vOther.z);
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------

inline float VectorLength(const Vector& v)
{

    return (float)FastSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

//VECTOR SUBTRAC
inline void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
{


    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
}

//VECTORADD
inline void VectorAdd(const Vector& a, const Vector& b, Vector& c)
{


    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline float* Vector::Base()
{
    return (float*)this;
}

inline float const* Vector::Base() const
{
    return (float const*)this;
}

inline void VectorMAInline(const float* start, float scale, const float* direction, float* dest)
{
    dest[0] = start[0] + direction[0] * scale;
    dest[1] = start[1] + direction[1] * scale;
    dest[2] = start[2] + direction[2] * scale;
}

inline void VectorMAInline(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
    dest.x = start.x + direction.x * scale;
    dest.y = start.y + direction.y * scale;
    dest.z = start.z + direction.z * scale;
}

inline void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
    VectorMAInline(start, scale, direction, dest);
}

inline void VectorMA(const float* start, float scale, const float* direction, float* dest)
{
    VectorMAInline(start, scale, direction, dest);
}


inline unsigned long& FloatBits(float& f)
{
    return *reinterpret_cast<unsigned long*>(&f);
}

inline bool IsFinite(float f)
{
    return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}

/////////////////////////////////////////////////////////////

class AnglesByValue;
class Angles
{
public:
    // Members
    float x, y, z;

    // Construction/destruction
    Angles(void);
    Angles(float X, float Y, float Z);
    //      Angles(RadianEuler const &angles);      // evil auto type promotion!!!

    // Allow pass-by-value
    operator AnglesByValue& () { return *((AnglesByValue*)(this)); }
    operator const AnglesByValue& () const { return *((const AnglesByValue*)(this)); }

    // Initialization
    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
    void Random(float minVal, float maxVal);

    // Got any nasty NAN's?
    bool IsValid() const;
    void Invalidate();

    // array access...
    float operator[](int i) const;
    float& operator[](int i);

    // Base address...
    float* Base();
    float const* Base() const;

    // equality
    bool operator==(const Angles& v) const;
    bool operator!=(const Angles& v) const;

    bool IsZero(float tolerance = 0.01f) const
    {
        return (x > -tolerance && x < tolerance &&
                y > -tolerance && y < tolerance &&
                z > -tolerance && z < tolerance);
    }

    // arithmetic operations
    Angles& operator+=(const Angles& v);
    Angles& operator-=(const Angles& v);
    Angles& operator*=(float s);
    Angles& operator/=(float s);

    // Get the vector's magnitude.
    float   Length() const;
    float   LengthSqr() const;

    // negate the Angles components
    //void  Negate();

    // No assignment operators either...
    Angles& operator=(const Angles& src);

#ifndef VECTOR_NO_SLOW_OPERATIONS
    // copy constructors

    // arithmetic operations
    Angles  operator-(void) const;

    Angles  operator+(const Angles& v) const;
    Angles  operator-(const Angles& v) const;
    Angles  operator*(float fl) const;
    Angles  operator/(float fl) const;
#else

    private:
	// No copy constructors allowed if we're in optimal mode
	Angles(const Angles& vOther);

#endif
};

//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
inline Angles::Angles(void)
{
#ifdef _DEBUG
    #ifdef VECTOR_PARANOIA
	// Initialize to NAN to catch errors
	x = y = z = VEC_T_NAN;
#endif
#endif
}

inline Angles::Angles(float X, float Y, float Z)
{
    x = X; y = Y; z = Z;

}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
inline void Angles::Init(float ix, float iy, float iz)
{
    x = ix; y = iy; z = iz;

}

inline void Angles::Random(float minVal, float maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    z = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);

}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------
inline Angles& Angles::operator=(const Angles& vOther)
{

    x = vOther.x; y = vOther.y; z = vOther.z;
    return *this;
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
inline bool Angles::operator==(const Angles& src) const
{


    return (src.x == x) && (src.y == y) && (src.z == z);
}

inline bool Angles::operator!=(const Angles& src) const
{


    return (src.x != x) || (src.y != y) || (src.z != z);
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
inline Angles& Angles::operator+=(const Angles& v)
{


    x += v.x; y += v.y; z += v.z;
    return *this;
}

inline Angles& Angles::operator-=(const Angles& v)
{


    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}

inline Angles& Angles::operator*=(float fl)
{
    x *= fl;
    y *= fl;
    z *= fl;

    return *this;
}

inline Angles& Angles::operator/=(float fl)
{
    Assert(fl != 0.0f);
    float oofl = 1.0f / fl;
    x *= oofl;
    y *= oofl;
    z *= oofl;

    return *this;
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline float* Angles::Base()
{
    return (float*)this;
}

inline float const* Angles::Base() const
{
    return (float const*)this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline float& Angles::operator[](int i)
{
    Assert((i >= 0) && (i < 3));
    return ((float*)this)[i];
}

inline float Angles::operator[](int i) const
{
    Assert((i >= 0) && (i < 3));
    return ((float*)this)[i];
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float Angles::Length() const
{

    return (float)FastSqrt(LengthSqr());
}


inline float Angles::LengthSqr() const
{

    return x * x + y * y + z * z;
}


//-----------------------------------------------------------------------------
// arithmetic operations (SLOW!!)
//-----------------------------------------------------------------------------
#ifndef VECTOR_NO_SLOW_OPERATIONS

inline Angles Angles::operator-(void) const
{
    return Angles(-x, -y, -z);
}

inline Angles Angles::operator+(const Angles& v) const
{
    Angles res;
    res.x = x + v.x;
    res.y = y + v.y;
    res.z = z + v.z;
    return res;
}

inline Angles Angles::operator-(const Angles& v) const
{
    Angles res;
    res.x = x - v.x;
    res.y = y - v.y;
    res.z = z - v.z;
    return res;
}

inline Angles Angles::operator*(float fl) const
{
    Angles res;
    res.x = x * fl;
    res.y = y * fl;
    res.z = z * fl;
    return res;
}

inline Angles Angles::operator/(float fl) const
{
    Angles res;
    res.x = x / fl;
    res.y = y / fl;
    res.z = z / fl;
    return res;
}

inline Angles operator*(float fl, const Angles& v)
{
    return v * fl;
}

#endif // VECTOR_NO_SLOW_OPERATIONS


//Angles SUBTRAC
inline void AnglesSubtract(const Angles& a, const Angles& b, Angles& c)
{


    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
}

//AnglesADD
inline void AnglesAdd(const Angles& a, const Angles& b, Angles& c)
{


    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
}

inline Angles CalculateAngles(Vector3D from, Vector3D to) {
    Vector3D newDirection = to.Subtract(from);
    newDirection.Normalize();

    float pitch = -asinf(newDirection.z) * (180 / M_PI);
    float yaw = atan2f(newDirection.y, newDirection.x) * (180 / M_PI);

    return Angles(pitch, yaw , 0);
}

/////////////////////////////////////////////////////////////

struct SVector
{
    float x;
    float y;
    float z;
    SVector(float x1, float y1, float z1)
    {
        x = x1;
        y = y1;
        z = z1;
    }

    SVector(Angles q)
    {
        x = q.x;
        y = q.y;
        z = q.z;
    }
};

namespace Math
{
    void NormalizeAngles(Angles& angle);
    double GetFov(const Angles& viewAngle, const Angles& aimAngle);
    double DotProduct(const Vector& v1, const float* v2);
    Angles CalcAngle(Vector3D& src,Vector3D dst);
}
/////////////////////////////////////////////////////////////

inline void Math::NormalizeAngles(Angles& angle)
{
    while (angle.x > 89.0f)
        angle.x -= 180.f;

    while (angle.x < -89.0f)
        angle.x += 180.f;

    while (angle.y > 180.f)
        angle.y -= 360.f;

    while (angle.y < -180.f)
        angle.y += 360.f;
}

inline Angles Math::CalcAngle(Vector3D& src,Vector3D dst)
{
    Angles angle = Angles();
    SVector delta = SVector((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

    double hyp = sqrt(delta.x*delta.x + delta.y * delta.y);

    angle.x = atan(delta.z / hyp) * (180.0f / M_PI);
    angle.y = atan(delta.y / delta.x) * (180.0f / M_PI);
    angle.z = 0;
    if (delta.x >= 0.0) angle.y += 180.0f;

    return angle;
}

inline double Math::GetFov(const Angles& viewAngle, const Angles& aimAngle)
{
    Angles delta = aimAngle - viewAngle;
    NormalizeAngles(delta);

    return sqrt(pow(delta.x, 2.0f) + pow(delta.y, 2.0f));
}

inline double Math::DotProduct(const Vector& v1, const float* v2)
{
    return v1.x * v2[0] + v1.y * v2[1] + v1.z * v2[2];
}

/////////////////////////////////////////////////////////////
