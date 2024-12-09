#ifndef BASE_MATH_H
#define BASE_MATH_H

typedef union Vec2F32 Vec2F32;
union Vec2F32
{
	struct { F32 x, y; };
	F32 v[2];
};

typedef union Vec3F32 Vec3F32;
union Vec3F32
{
	struct { F32 x, y, z; };
	struct { Vec2F32 xy; F32 _z0; };
	struct { F32 _x0; Vec2F32 yz; };
	F32 v[3];
};

typedef union Vec4F32 Vec4F32;
union Vec4F32
{
	struct { F32 x, y, z, w; };
	struct { Vec2F32 xy; Vec2F32 zw; };
	struct { Vec3F32 xyz; F32 _w0; };
	struct { F32 _x0; Vec3F32 yzw; };
	F32 v[4];
};

typedef struct Mat3x3F32 Mat3x3F32;
struct Mat3x3F32
{
	F32 v[3][3];
};

typedef struct Mat4x4F32 Mat4x4F32;
struct Mat4x4F32
{
	F32 v[4][4];
};

typedef union Rng1F32 Rng1F32;
union Rng1F32
{
	struct { F32 min, max; };
	F32 v[2];
};

typedef union Rng2F32 Rng2F32;
union Rng2F32
{
	struct { Vec2F32 min, max };
	struct { Vec2F32 p0, p1 };
	struct { F32 x0, y0, x1, y1 };
	Vec2F32 v[2];
};

llabs()

#endif // BASE_MATH_H
