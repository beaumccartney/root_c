// REVIEW(beau): use <simd/*.h> on macos?
#ifndef BASE_MATH_H
#define BASE_MATH_H

typedef union Vec2F32 Vec2F32;
union Vec2F32
{
	struct { F32 x, y; };
	F32 v[2];
};

typedef union Vec2S32 Vec2S32;
union Vec2S32
{
	struct { S32 x, y;         };
	struct { S32 column, line; };
	S32 v[2];
};

typedef union Vec2S64 Vec2S64;
union Vec2S64
{
	struct { S64 x, y;         };
	struct { S64 column, line; };
	S64 v[2];
};

typedef union Vec2U64 Vec2U64;
union Vec2U64
{
	struct { U64 x, y;         };
	struct { U64 column, line; };
	U64 v[2];
};

typedef union Vec3F32 Vec3F32;
union Vec3F32
{
	// xyz
	struct { F32 x, y, z;         };
	struct { Vec2F32 xy; F32 _z0; };
	struct { F32 _x0; Vec2F32 yz; };

	// rgb
	struct { F32 r, g, b;         };
	struct { Vec2F32 rg; F32 _b0; };
	struct { F32 _r0; Vec2F32 gb; };
	F32 v[3];
};

typedef union Vec3S32 Vec3S32;
union Vec3S32
{
	// xyz
	struct { S32 x, y, z;         };
	struct { Vec2S32 xy; S32 _z0; };
	struct { S32 _x0; Vec2S32 yz; };

	// rgb
	struct { S32 r, g, b;         };
	struct { Vec2S32 rg; S32 _b0; };
	struct { S32 _r0; Vec2S32 gb; };
	S32 v[3];
};

typedef union Vec4F32 Vec4F32;
union Vec4F32
{
	// xyzw
	struct { F32 x, y, z, w;         };
	struct { Vec2F32 xy; Vec2F32 zw; };
	struct { Vec3F32 xyz; F32 _w0;   };
	struct { F32 _x0; Vec3F32 yzw;   };

	// rgba
	struct { F32 r, g, b, a;         };
	struct { Vec2F32 rg; Vec2F32 ba; };
	struct { Vec3F32 rgb; F32 _a0;   };
	struct { F32 _r0; Vec3F32 gba;   };
	F32 v[4];
};

typedef union Mat3x3F32 Mat3x3F32;
union Mat3x3F32
{
	struct
	{
		F32 x0, x1, x2,
		    y0, y1, y2,
		    z0, z1, z2;
	};
	F32 v[3][3];
};

typedef union Mat4x4F32 Mat4x4F32;
union Mat4x4F32
{
	struct
	{
		F32 x0, x1, x2, x3,
		    y0, y1, y2, y3,
		    z0, z1, z2, z3,
		    w0, w1, w2, w3;
	};
	F32 v[4][4];
};

// REVIEW: typedef to Vec4F32?
typedef union QuatF32 QuatF32;
union QuatF32
{
	Vec4F32 xyzw;
	struct { Vec2F32 xy, zw;     };
	struct { Vec3F32 xyz; F32 w; };
	struct { F32 x; Vec3F32 yzw; };
	struct { F32 _x, y, z, _w;   };
	F32 v[4];
};

typedef union Rng1F32 Rng1F32;
union Rng1F32
{
	struct { F32 min, max; };
	F32 v[2];
};

typedef union Rng1S32 Rng1S32;
union Rng1S32
{
	struct { S32 min, max; };
	S32 v[2];
};

typedef union Rng1S64 Rng1S64;
union Rng1S64
{
	struct { S64 min, max; };
	S64 v[2];
};

typedef union Rng1U64 Rng1U64;
union Rng1U64
{
	struct { U64 min, max; };
	U64 v[2];
};

typedef union Rng2F32 Rng2F32;
union Rng2F32
{
	struct { Vec2F32 min, max;   };
	struct { Vec2F32 p0, p1;     };
	struct { F32 x0, y0, x1, y1; };
	Vec2F32 v[2];
};

typedef union Rng2S32 Rng2S32;
union Rng2S32
{
	struct { Vec2S32 min, max;   };
	struct { Vec2S32 p0, p1;     };
	struct { S32 x0, y0, x1, y1; };
	Vec2S32 v[2];
};

typedef union Rng2S64 Rng2S64;
union Rng2S64
{
	struct { Vec2S64 min, max;   };
	struct { Vec2S64 p0, p1;     };
	struct { S64 x0, y0, x1, y1; };
	Vec2S64 v[2];
};

typedef union Rng2U64 Rng2U64;
union Rng2U64
{
	struct { Vec2U64 min, max;   };
	struct { Vec2U64 p0, p1;     };
	struct { U64 x0, y0, x1, y1; };
	Vec2U64 v[2];
};

typedef union Rng3F32 Rng3F32;
union Rng3F32
{
	struct { Vec3F32 min, max;           };
	struct { Vec3F32 p0, p1;             };
	struct { F32 x0, y0, z0, x1, y1, z1; };
	Vec3F32 v[2];
};

#define abs_s64(v) (S64)llabs(v)

#define sqrt_f32(v)   sqrtf(v)
#define mod_f32(a, b) fmodf((a), (b))
#define pow_f32(b, e) powf((b), (e))
#define ceil_f32(v)   ceilf(v)
#define floor_f32(v)  floorf(v)
#define round_f32(v)  roundf(v)
#define abs_f32(v)    fabsf(v)
#define radians_from_turns_f32(v) ((v)*2*3.1415926535897f)
#define turns_from_radians_f32(v) ((v)/2*3.1415926535897f)
#define degrees_from_turns_f32(v) ((v)*360.f)
#define turns_from_degrees_f32(v) ((v)/360.f)
#define degrees_from_radians_f32(v) (degrees_from_turns_f32(turns_from_radians_f32(v)))
#define radians_from_degrees_f32(v) (radians_from_turns_f32(turns_from_degrees_f32(v)))
#define sin_f32(v)    sinf(radians_from_turns_f32(v))
#define cos_f32(v)    cosf(radians_from_turns_f32(v))
#define tan_f32(v)    tanf(radians_from_turns_f32(v))

#define sqrt_f64(v)   sqrt(v)
#define mod_f64(a, b) fmod((a), (b))
#define pow_f64(b, e) pow((b), (e))
#define ceil_f64(v)   ceil(v)
#define floor_f64(v)  floor(v)
#define round_f64(v)  round(v)
#define abs_f64(v)    fabs(v)
#define radians_from_turns_f64(v) ((v)*2*3.1415926535897)
#define turns_from_radians_f64(v) ((v)/2*3.1415926535897)
#define degrees_from_turns_f64(v) ((v)*360.0)
#define turns_from_degrees_f64(v) ((v)/360.0)
#define degrees_from_radians_f64(v) (degrees_from_turns_f64(turns_from_radians_f64(v)))
#define radians_from_degrees_f64(v) (radians_from_turns_f64(turns_from_degrees_f64(v)))
#define sin_f64(v)    sin(radians_from_turns_f64(v))
#define cos_f64(v)    cos(radians_from_turns_f64(v))
#define tan_f64(v)    tan(radians_from_turns_f64(v))

internal F32 mix_1f32(F32 a, F32 b, F32 t);
internal F64 mix_1f64(F64 a, F64 b, F64 t);

#define v2f32(x, y) vec_2f32((x), (y))
#define vec_2f32(x, y) ((Vec2F32){(x), (y)})
#define splat_2f32(e) vec_2f32((e), (e))
internal Vec2F32 add_2f32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 sub_2f32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 mul_2f32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 div_2f32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 scale_2f32(Vec2F32 v, F32 s);
internal F32 dot_2f32(Vec2F32 a, Vec2F32 b);
internal F32 length_squared_2f32(Vec2F32 v);
internal F32 length_2f32(Vec2F32 v);
internal Vec2F32 normalize_2f32(Vec2F32 v);
internal Vec2F32 mix_2f32(Vec2F32 a, Vec2F32 b, F32 t);

#define v2s32(x, y) vec_2s32((x), (y))
#define vec_2s32(x, y) ((Vec2S32){(x), (y)})
#define splat_2s32(e) vec_2s32((e), (e))
internal Vec2S32 add_2s32(Vec2S32 a, Vec2S32 b);
internal Vec2S32 sub_2s32(Vec2S32 a, Vec2S32 b);
internal Vec2S32 mul_2s32(Vec2S32 a, Vec2S32 b);
internal Vec2S32 div_2s32(Vec2S32 a, Vec2S32 b);
internal Vec2S32 scale_2s32(Vec2S32 v, S32 s);
internal S32 dot_2s32(Vec2S32 a, Vec2S32 b);
internal S32 length_squared_2s32(Vec2S32 v);
internal S32 length_2s32(Vec2S32 v);
internal Vec2S32 normalize_2s32(Vec2S32 v);
internal Vec2S32 mix_2s32(Vec2S32 a, Vec2S32 b, F32 t);

#define v2s64(x, y) vec_2s64((x), (y))
#define vec_2s64(x, y) ((Vec2S64){(x), (y)})
#define splat_2s64(e) vec_2s64((e), (e))
internal Vec2S64 add_2s64(Vec2S64 a, Vec2S64 b);
internal Vec2S64 sub_2s64(Vec2S64 a, Vec2S64 b);
internal Vec2S64 mul_2s64(Vec2S64 a, Vec2S64 b);
internal Vec2S64 div_2s64(Vec2S64 a, Vec2S64 b);
internal Vec2S64 scale_2s64(Vec2S64 v, S64 s);
internal S64 dot_2s64(Vec2S64 a, Vec2S64 b);
internal S64 length_squared_2s64(Vec2S64 v);
internal S64 length_2s64(Vec2S64 v);
internal Vec2S64 normalize_2s64(Vec2S64 v);
internal Vec2S64 mix_2s64(Vec2S64 a, Vec2S64 b, F32 t);

#define v2u64(x, y) vec_2u64((x), (y))
#define vec_2u64(x, y) ((Vec2U64){(x), (y)})
#define splat_2u64(e) vec_2u64((e), (e))
internal Vec2U64 add_2u64(Vec2U64 a, Vec2U64 b);
internal Vec2U64 sub_2u64(Vec2U64 a, Vec2U64 b);
internal Vec2U64 mul_2u64(Vec2U64 a, Vec2U64 b);
internal Vec2U64 div_2u64(Vec2U64 a, Vec2U64 b);
internal Vec2U64 scale_2u64(Vec2U64 v, U64 s);
internal U64 dot_2u64(Vec2U64 a, Vec2U64 b);
internal U64 length_squared_2u64(Vec2U64 v);
internal U64 length_2u64(Vec2U64 v);
internal Vec2U64 normalize_2u64(Vec2U64 v);
internal Vec2U64 mix_2u64(Vec2U64 a, Vec2U64 b, F32 t);

#define v3f32(x, y, z) vec_3f32((x), (y), (z))
#define vec_3f32(x, y, z) ((Vec3F32){(x), (y), (z)})
#define splat_3f32(e) vec_3f32((e), (e), (e))
internal Vec3F32 add_3f32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 sub_3f32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 mul_3f32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 div_3f32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 scale_3f32(Vec3F32 v, F32 s);
internal F32 dot_3f32(Vec3F32 a, Vec3F32 b);
internal F32 length_squared_3f32(Vec3F32 v);
internal F32 length_3f32(Vec3F32 v);
internal Vec3F32 normalize_3f32(Vec3F32 v);
internal Vec3F32 mix_3f32(Vec3F32 a, Vec3F32 b, F32 t);
internal Vec3F32 cross_3f32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 transform_3f32(Vec3F32 v, Mat3x3F32 m);

#define v3s32(x, y, z) vec_3s32((x), (y), (z))
#define vec_3s32(x, y, z) ((Vec3S32){(x), (y), (z)})
#define splat_3s32(e) vec_3s32((e), (e), (e))
internal Vec3S32 add_3s32(Vec3S32 a, Vec3S32 b);
internal Vec3S32 sub_3s32(Vec3S32 a, Vec3S32 b);
internal Vec3S32 mul_3s32(Vec3S32 a, Vec3S32 b);
internal Vec3S32 div_3s32(Vec3S32 a, Vec3S32 b);
internal Vec3S32 scale_3s32(Vec3S32 v, S32 s);
internal S32 dot_3s32(Vec3S32 a, Vec3S32 b);
internal S32 length_squared_3s32(Vec3S32 v);
internal S32 length_3s32(Vec3S32 v);
internal Vec3S32 normalize_3s32(Vec3S32 v);
internal Vec3S32 mix_3s32(Vec3S32 a, Vec3S32 b, F32 t);

#define v4f32(x, y, z, w) vec_4f32((x), (y), (z), (w))
#define vec_4f32(x, y, z, w) ((Vec4F32){(x), (y), (z), (w)})
#define splat_4f32(e) vec_4f32((e), (e), (e), (e))
internal Vec4F32 add_4f32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 sub_4f32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 mul_4f32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 div_4f32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 scale_4f32(Vec4F32 v, F32 s);
internal F32 dot_4f32(Vec4F32 a, Vec4F32 b);
internal F32 length_squared_4f32(Vec4F32 v);
internal F32 length_4f32(Vec4F32 v);
internal Vec4F32 normalize_4f32(Vec4F32 v);
internal Vec4F32 mix_4f32(Vec4F32 a, Vec4F32 b, F32 t);
internal Vec4F32 transform_4f32(Vec4F32 v, Mat4x4F32 m);

#define mat_3x3f32(diagonal) ((Mat3x3F32){.v[0][0] = diagonal, .v[1][1] = diagonal, .v[2][2] = diagonal})
internal Mat3x3F32 transpose_3x3f32(Mat3x3F32 m);
internal Mat3x3F32 make_translate_3x3f32(Vec2F32 delta);
internal Mat3x3F32 make_scale_3x3f32(Vec2F32 scale);
internal Mat3x3F32 scale_3x3f32(Mat3x3F32 m, F32 scale);
internal Mat3x3F32 mul_3x3f32(Mat3x3F32 a, Mat3x3F32 b);
internal F32 det_3x3f32(Mat3x3F32 m);
internal Mat3x3F32 inverse_3x3f32(Mat3x3F32 m);

#define mat_4x4f32(diagonal) ((Mat4x4F32){.v[0][0] = diagonal, .v[1][1] = diagonal, .v[2][2] = diagonal, .v[3][3] = diagonal})
internal Mat4x4F32 transpose_4x4f32(Mat4x4F32 m);
internal Mat4x4F32 make_translate_4x4f32(Vec3F32 delta);
internal Mat4x4F32 make_scale_4x4f32(Vec3F32 scale);
internal Mat4x4F32 scale_4x4f32(Mat4x4F32 m, F32 scale);
internal Mat4x4F32 mul_4x4f32(Mat4x4F32 a, Mat4x4F32 b);
internal F32 det_4x4f32(Mat4x4F32 m);
internal Mat4x4F32 inverse_4x4f32(Mat4x4F32 m);
internal Mat4x4F32 make_perspective_4x4f32(F32 fov, F32 aspect_ratio, F32 near_z, F32 far_z);
internal Mat4x4F32 make_orthographic_4x4f32(F32 left, F32 right, F32 bottom, F32 top, F32 near_z, F32 far_z);
internal Mat4x4F32 make_look_at_4x4f32(Vec3F32 eye, Vec3F32 center, Vec3F32 up);
internal Mat4x4F32 make_rotate_4x4f32(Vec3F32 axis, F32 turns);
internal Mat4x4F32 derotate_4x4f32(Mat4x4F32 mat);

// REVIEW: macros for just using vec4 operations?
internal QuatF32 make_quat_f32(F32 x, F32 y, F32 z, F32 w);
internal QuatF32 quat_from_axis_angle_f32(Vec3F32 axis, F32 turns);

internal QuatF32 add_quatf32(QuatF32 a, QuatF32 b);
internal QuatF32 sub_quatf32(QuatF32 a, QuatF32 b);
internal QuatF32 mul_quatf32(QuatF32 a, QuatF32 b);
internal QuatF32 scale_quatf32(QuatF32 a, F32 scale);
internal QuatF32 normalize_quatf32(QuatF32 q);
internal QuatF32 mix_quatf32(QuatF32 a, QuatF32 b, F32 t);
internal F32 dot_quatf32(QuatF32 a, QuatF32 b);
internal Mat4x4F32 mat4x4_from_quatf32(QuatF32 q);

#define r1f32(min, max) rng_1f32((min), (max))
internal Rng1F32 rng_1f32(F32 min, F32 max);
internal Rng1F32 shift_1f32(Rng1F32 r, F32 x);
internal Rng1F32 pad_1f32(Rng1F32 r, F32 x);
internal F32 center_1f32(Rng1F32 r);
internal B32 contains_1f32(Rng1F32 r, F32 x);
internal F32 dim_1f32(Rng1F32 r);
internal Rng1F32 union_1f32(Rng1F32 a, Rng1F32 b);
internal Rng1F32 intersect_1f32(Rng1F32 a, Rng1F32 b);
internal F32 clamp_1f32(Rng1F32 r, F32 v);

#define r1s32(min, max) rng_1s32((min), (max))
internal Rng1S32 rng_1s32(S32 min, S32 max);
internal Rng1S32 shift_1s32(Rng1S32 r, S32 x);
internal Rng1S32 pad_1s32(Rng1S32 r, S32 x);
internal S32 center_1s32(Rng1S32 r);
internal B32 contains_1s32(Rng1S32 r, S32 x);
internal S32 dim_1s32(Rng1S32 r);
internal Rng1S32 union_1s32(Rng1S32 a, Rng1S32 b);
internal Rng1S32 intersect_1s32(Rng1S32 a, Rng1S32 b);
internal S32 clamp_1s32(Rng1S32 r, S32 v);

#define r1s64(min, max) rng_1s64((min), (max))
internal Rng1S64 rng_1s64(S64 min, S64 max);
internal Rng1S64 shift_1s64(Rng1S64 r, S64 x);
internal Rng1S64 pad_1s64(Rng1S64 r, S64 x);
internal S64 center_1s64(Rng1S64 r);
internal B32 contains_1s64(Rng1S64 r, S64 x);
internal S64 dim_1s64(Rng1S64 r);
internal Rng1S64 union_1s64(Rng1S64 a, Rng1S64 b);
internal Rng1S64 intersect_1s64(Rng1S64 a, Rng1S64 b);
internal S64 clamp_1s64(Rng1S64 r, S64 v);

#define r1u64(min, max) rng_1u64((min), (max))
internal Rng1U64 rng_1u64(U64 min, U64 max);
internal Rng1U64 shift_1u64(Rng1U64 r, U64 x);
internal Rng1U64 pad_1u64(Rng1U64 r, U64 x);
internal U64 center_1u64(Rng1U64 r);
internal B32 contains_1u64(Rng1U64 r, U64 x);
internal U64 dim_1u64(Rng1U64 r);
internal Rng1U64 union_1u64(Rng1U64 a, Rng1U64 b);
internal Rng1U64 intersect_1u64(Rng1U64 a, Rng1U64 b);
internal U64 clamp_1u64(Rng1U64 r, U64 v);

#define r2f32(min, max) rng_2f32((min), (max))
#define r2f32p(x, y, z, w) r2f32(v2f32((x), (y)), v2f32((z), (w)))
internal Rng2F32 rng_2f32(Vec2F32 min, Vec2F32 max);
internal Rng2F32 shift_2f32(Rng2F32 r, Vec2F32 x);
internal Rng2F32 pad_2f32(Rng2F32 r, F32 x);
internal Vec2F32 center_2f32(Rng2F32 r);
internal B32 contains_2f32(Rng2F32 r, Vec2F32 x);
internal Vec2F32 dim_2f32(Rng2F32 r);
internal Rng2F32 union_2f32(Rng2F32 a, Rng2F32 b);
internal Rng2F32 intersect_2f32(Rng2F32 a, Rng2F32 b);
internal Vec2F32 clamp_2f32(Rng2F32 r, Vec2F32 v);

#define r2s32(min, max) rng_2s32((min), (max))
#define r2s32p(x, y, z, w) r2s32(v2s32((x), (y)), v2s32((z), (w)))
internal Rng2S32 rng_2s32(Vec2S32 min, Vec2S32 max);
internal Rng2S32 shift_2s32(Rng2S32 r, Vec2S32 x);
internal Rng2S32 pad_2s32(Rng2S32 r, S32 x);
internal Vec2S32 center_2s32(Rng2S32 r);
internal B32 contains_2s32(Rng2S32 r, Vec2S32 x);
internal Vec2S32 dim_2s32(Rng2S32 r);
internal Rng2S32 union_2s32(Rng2S32 a, Rng2S32 b);
internal Rng2S32 intersect_2s32(Rng2S32 a, Rng2S32 b);
internal Vec2S32 clamp_2s32(Rng2S32 r, Vec2S32 v);

#define r2s64(min, max) rng_2s64((min), (max))
#define r2s64p(x, y, z, w) r2s64(v2s64((x), (y)), v2s64((z), (w)))
internal Rng2S64 rng_2s64(Vec2S64 min, Vec2S64 max);
internal Rng2S64 shift_2s64(Rng2S64 r, Vec2S64 x);
internal Rng2S64 pad_2s64(Rng2S64 r, S64 x);
internal Vec2S64 center_2s64(Rng2S64 r);
internal B32 contains_2s64(Rng2S64 r, Vec2S64 x);
internal Vec2S64 dim_2s64(Rng2S64 r);
internal Rng2S64 union_2s64(Rng2S64 a, Rng2S64 b);
internal Rng2S64 intersect_2s64(Rng2S64 a, Rng2S64 b);
internal Vec2S64 clamp_2s64(Rng2S64 r, Vec2S64 v);

#define r2u64(min, max) rng_2u64((min), (max))
#define r2u64p(x, y, z, w) r2u64(v2u64((x), (y)), v2u64((z), (w)))
internal Rng2U64 rng_2u64(Vec2U64 min, Vec2U64 max);
internal Rng2U64 shift_2u64(Rng2U64 r, Vec2U64 x);
internal Rng2U64 pad_2u64(Rng2U64 r, U64 x);
internal Vec2U64 center_2u64(Rng2U64 r);
internal B32 contains_2u64(Rng2U64 r, Vec2U64 x);
internal Vec2U64 dim_2u64(Rng2U64 r);
internal Rng2U64 union_2u64(Rng2U64 a, Rng2U64 b);
internal Rng2U64 intersect_2u64(Rng2U64 a, Rng2U64 b);
internal Vec2U64 clamp_2u64(Rng2U64 r, Vec2U64 v);

#define r3f32(min, max) rng_3f32((min), (max))
#define r3f32p(x0, y0, z0, x1, y1, z1) r3f32(v3f32((x0), (y0), (z0)), v3f32((x0), (y0), (z0)))
internal Rng3F32 rng_3f32(Vec3F32 min, Vec3F32 max);
internal Rng3F32 shift_3f32(Rng3F32 r, Vec3F32 x);
internal Rng3F32 pad_3f32(Rng3F32 r, F32 x);
internal Vec3F32 center_3f32(Rng3F32 r);
internal B32 contains_3f32(Rng3F32 r, Vec3F32 x);
internal Vec3F32 dim_3f32(Rng3F32 r);
internal Rng3F32 union_3f32(Rng3F32 a, Rng3F32 b);
internal Rng3F32 intersect_3f32(Rng3F32 a, Rng3F32 b);
internal Vec3F32 clamp_3f32(Rng3F32 r, Vec3F32 v);

#if !BASE_DISABLE_OMITTED_BASE_TYPES
	typedef Vec2F32   Vec2;
	typedef Vec3F32   Vec3;
	typedef Vec4F32   Vec4;
	typedef Rng1F32   Rng1;
	typedef Rng2F32   Rng2;
	typedef Rng3F32   Rng3;
	typedef Mat3x3F32 Mat3;
	typedef Mat4x4F32 Mat4;

	#define mix1(a, b, t) mix_1f32((a), (b), (t))

	#define v2(x, y) v2f32((x), (y))
	#define splat2(e) splat_2f32((e))
	#define add2(a, b) add_2f32((a), (b))
	#define sub2(a, b) sub_2f32((a), (b))
	#define mul2(a, b) mul_2f32((a), (b))
	#define div2(a, b) div_2f32((a), (b))
	#define scale2(a, b) scale_2f32((a), (b))
	#define dot2(a, b) dot_2f32((a), (b))
	#define length_squared2(v) length_squared_2f32((v))
	#define length2(v) length_2f32((v))
	#define normalize2(v) normalize_2f32((v))
	#define mix2(a, b, t) mix_2f32((a), (b), (t))

	#define v3(x, y, z) v3f32((x), (y), (z))
	#define splat3(e) splat_3f32((e))
	#define add3(a, b) add_3f32((a), (b))
	#define sub3(a, b) sub_3f32((a), (b))
	#define mul3(a, b) mul_3f32((a), (b))
	#define div3(a, b) div_3f32((a), (b))
	#define scale3(a, b) scale_3f32((a), (b))
	#define dot3(a, b) dot_3f32((a), (b))
	#define length_squared3(v) length_squared_2f32((v))
	#define length3(v) length_3f32((v))
	#define normalize3(v) normalize_3f32((v))
	#define mix3(a, b, t) mix_3f32((a), (b), (t))
	#define cross3(a, b) cross_3f32((a), (b))
	#define transform3(v, mat3x3) transform_3f32((v), (mat3x3))

	#define v4(x, y, z, w) v4f32((x), (y), (z), (w))
	#define splat4(e) splat_4f32((e))
	#define add4(a, b) add_4f32((a), (b))
	#define sub4(a, b) sub_4f32((a), (b))
	#define mul4(a, b) mul_4f32((a), (b))
	#define div4(a, b) div_4f32((a), (b))
	#define scale4(a, b) scale_4f32((a), (b))
	#define dot4(a, b) dot_4f32((a), (b))
	#define length_squared4(v) length_squared_4f32((v))
	#define length4(v) length_4f32((v))
	#define normalize4(v) normalize_4f32((v))
	#define mix4(a, b, t) mix_4f32((a), (b), (t))
	#define transform4(v, mat4x4) transform_4f32((v), (mat4x4))

	#define r1(min, max) r1f32((min), (max))
	#define shift1(rng, v) shift_1f32((rng), (v))
	#define pad1(rng, v) pad_1f32((rng), (v))
	#define center1(rng) center_1f32((rng))
	#define contains1(rng, v) contains_1f32((rng), (v))
	#define dim1(rng) dim_1f32((rng))
	#define union1(a, b) union_1f32((a), (b))
	#define intersect1(a, b) intersect_1f32((a), (b))
	#define clamp1(rng, v) clamp_1f32((rng), (v))

	#define r2(min, max) r2f32((min), (max))
	#define r2p(x, y, z, w) r2f32p((x), (y), (z), (w))
	#define shift2(rng, v) shift_2f32((rng), (v))
	#define pad2(rng, v) pad_2f32((rng), (v))
	#define center2(rng) center_2f32((rng))
	#define contains2(rng, v) contains_2f32((rng), (v))
	#define dim2(rng) dim_2f32((rng))
	#define union2(a, b) union_2f32((a), (b))
	#define intersect2(a, b) intersect_2f32((a), (b))
	#define clamp2(rng, v) clamp_2f32((rng), (v))

	#define r3(min, max) r3f32((min), (max))
	#define r3p(x0, y0, z0, x1, y1, z1)3f32p((x0), (y0), (z0), (x1), (y1), (z1))
	#define shift3(rng, v) shift_3f32((rng), (v))
	#define pad3(rng, v) pad_3f32((rng), (v))
	#define center3(rng) center_3f32((rng))
	#define contains3(rng, v) contains_3f32((rng), (v))
	#define dim3(rng) dim_3f32((rng))
	#define union3(a, b) union_3f32((a), (b))
	#define intersect3(a, b) intersect_3f32((a), (b))
	#define clamp3(rng, v) clamp_3f32((rng), (v))

	#define mat3x3(diagonal) mat_3x3f32(diagonal)
	#define mul3x3(a, b) mul_3x3f32((a), (b))
	#define scale3x3(m, scalar) scale_3x3f32((m), (scalar))
	#define inverse3x3(m) inverse_3x3f32((m))

	#define mat4x4(diagonal) mat_3x3f32(diagonal)
	#define mul4x4(a, b) mul_3x3f32((a), (b))
	#define scale4x4(m, scalar) scale_3x3f32((m), (scalar))
	#define inverse4x4(m) inverse_3x3f32((m))

	#define addq(a, b) add_quatf32(QuatF32 a, QuatF32 b)
	#define subq(a, b) sub_quatf32(QuatF32 a, QuatF32 b)
	#define mulq(a, b) mul_quatf32(QuatF32 a, QuatF32 b)
	#define scaleq(a, scale) scale_quatf32(QuatF32 a, F32 scale)
	#define normalizeq(q) normalize_quatf32(QuatF32 q)
	#define mixq(a, b, t) mix_quatf32(QuatF32 a, QuatF32 b, F32 t)
	#define dotq(a, b) dot_quatf32(QuatF32 a, QuatF32 b)
	#define mat4x4_from_quat(q) mat4x4_from_quatf32(QuatF32 q)

#endif // !BASE_DISABLE_OMITTED_BASE_TYPES

#endif // BASE_MATH_H
