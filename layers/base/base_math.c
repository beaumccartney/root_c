#include <math.h>

internal F32 mix_1f32(F32 a, F32 b, F32 t)
{
	return a + (b - a) * Clamp(0.f, t, 1.f);
}

internal F64 mix_1f64(F64 a, F64 b, F64 t)
{
	return a + (b - a) * Clamp(0., t, 1.);
}

internal Vec2F32 vec_2f32(F32 x, F32 y)                {return (Vec2F32){x, y};}
internal Vec2F32 splat_2f32(F32 e)                     {return (Vec2F32){e, e};}
internal Vec2F32 add_2f32(Vec2F32 a, Vec2F32 b)        {return (Vec2F32){a.x+b.x, a.y+b.y};}
internal Vec2F32 sub_2f32(Vec2F32 a, Vec2F32 b)        {return (Vec2F32){a.x-b.x, a.y-b.y};}
internal Vec2F32 mul_2f32(Vec2F32 a, Vec2F32 b)        {return (Vec2F32){a.x*b.x, a.y*b.y};}
internal Vec2F32 div_2f32(Vec2F32 a, Vec2F32 b)        {return (Vec2F32){a.x/b.x, a.y/b.y};}
internal Vec2F32 scale_2f32(Vec2F32 v, F32 s)          {return (Vec2F32){v.x*s,   v.y*s  };}
internal F32 dot_2f32(Vec2F32 a, Vec2F32 b)            {return a.x*b.x + a.y*b.y;}
internal F32 length_squared_2f32(Vec2F32 v)            {return v.x*v.x + v.y*v.y;}
internal F32 length_2f32(Vec2F32 v)                    {return sqrt_f32(length_squared_2f32(v));}
internal Vec2F32 normalize_2f32(Vec2F32 v)             {return scale_2f32(v, 1.f/length_2f32(v));}
internal Vec2F32 mix_2f32(Vec2F32 a, Vec2F32 b, F32 t) {return (Vec2F32){mix_1f32(a.x, b.x, t), mix_1f32(a.y, b.y, t)};}

internal Vec2S32 vec_2s32(S32 x, S32 y)                {return (Vec2S32){x, y};}
internal Vec2S32 splat_2s32(S32 e)                     {return (Vec2S32){e, e};}
internal Vec2S32 add_2s32(Vec2S32 a, Vec2S32 b)        {return (Vec2S32){a.x+b.x, a.y+b.y};}
internal Vec2S32 sub_2s32(Vec2S32 a, Vec2S32 b)        {return (Vec2S32){a.x-b.x, a.y-b.y};}
internal Vec2S32 mul_2s32(Vec2S32 a, Vec2S32 b)        {return (Vec2S32){a.x*b.x, a.y*b.y};}
internal Vec2S32 div_2s32(Vec2S32 a, Vec2S32 b)        {return (Vec2S32){a.x/b.x, a.y/b.y};}
internal Vec2S32 scale_2s32(Vec2S32 v, S32 s)          {return (Vec2S32){v.x*s,   v.y*s  };}
internal S32 dot_2s32(Vec2S32 a, Vec2S32 b)            {return a.x*b.x + a.y*b.y;}
internal S32 length_squared_2s32(Vec2S32 v)            {return v.x*v.x + v.y*v.y;}
internal S32 length_2s32(Vec2S32 v)                    {return (S32)sqrt_f64((F64)length_squared_2s32(v));}
internal Vec2S32 normalize_2s32(Vec2S32 v)             {return scale_2s32(v, 1/length_2s32(v));}
internal Vec2S32 mix_2s32(Vec2S32 a, Vec2S32 b, F32 t) {return (Vec2S32){(S32)mix_1f32((F32)a.x, (F32)b.x, t), (S32)mix_1f32((F32)a.y, (F32)b.y, t)};}

internal Vec2S64 vec_2s64(S64 x, S64 y)                {return (Vec2S64){x, y};}
internal Vec2S64 splat_2s64(S64 e)                     {return (Vec2S64){e, e};}
internal Vec2S64 add_2s64(Vec2S64 a, Vec2S64 b)        {return (Vec2S64){a.x+b.x, a.y+b.y};}
internal Vec2S64 sub_2s64(Vec2S64 a, Vec2S64 b)        {return (Vec2S64){a.x-b.x, a.y-b.y};}
internal Vec2S64 mul_2s64(Vec2S64 a, Vec2S64 b)        {return (Vec2S64){a.x*b.x, a.y*b.y};}
internal Vec2S64 div_2s64(Vec2S64 a, Vec2S64 b)        {return (Vec2S64){a.x/b.x, a.y/b.y};}
internal Vec2S64 scale_2s64(Vec2S64 v, S64 s)          {return (Vec2S64){v.x*s,   v.y*s  };}
internal S64 dot_2s64(Vec2S64 a, Vec2S64 b)            {return a.x*b.x + a.y*b.y;}
internal S64 length_squared_2s64(Vec2S64 v)            {return v.x*v.x + v.y*v.y;}
internal S64 length_2s64(Vec2S64 v)                    {return (S64)sqrt_f64((F64)length_squared_2s64(v));}
internal Vec2S64 normalize_2s64(Vec2S64 v)             {return scale_2s64(v, 1/length_2s64(v));}
internal Vec2S64 mix_2s64(Vec2S64 a, Vec2S64 b, F32 t) {return (Vec2S64){(S64)mix_1f32((F32)a.x, (F32)b.x, t), (S64)mix_1f32((F32)a.y, (F32)b.y, t)};}

internal Vec2U64 vec_2u64(U64 x, U64 y)                {return (Vec2U64){x, y};}
internal Vec2U64 splat_2u64(U64 e)                     {return (Vec2U64){e, e};}
internal Vec2U64 add_2u64(Vec2U64 a, Vec2U64 b)        {return (Vec2U64){a.x+b.x, a.y+b.y};}
internal Vec2U64 sub_2u64(Vec2U64 a, Vec2U64 b)        {return (Vec2U64){a.x-b.x, a.y-b.y};}
internal Vec2U64 mul_2u64(Vec2U64 a, Vec2U64 b)        {return (Vec2U64){a.x*b.x, a.y*b.y};}
internal Vec2U64 div_2u64(Vec2U64 a, Vec2U64 b)        {return (Vec2U64){a.x/b.x, a.y/b.y};}
internal Vec2U64 scale_2u64(Vec2U64 v, U64 s)          {return (Vec2U64){v.x*s,   v.y*s  };}
internal U64 dot_2u64(Vec2U64 a, Vec2U64 b)            {return a.x*b.x + a.y*b.y;}
internal U64 length_squared_2u64(Vec2U64 v)            {return v.x*v.x + v.y*v.y;}
internal U64 length_2u64(Vec2U64 v)                    {return (U64)sqrt_f64((F64)length_squared_2u64(v));}
internal Vec2U64 normalize_2u64(Vec2U64 v)             {return scale_2u64(v, 1/length_2u64(v));}
internal Vec2U64 mix_2u64(Vec2U64 a, Vec2U64 b, F32 t) {return (Vec2U64){(U64)mix_1f32((F32)a.x, (F32)b.x, t), (U64)mix_1f32((F32)a.y, (F32)b.y, t)};}

internal Vec3F32 vec_3f32(F32 x, F32 y, F32 z)         {return (Vec3F32){x, y, z};}
internal Vec3F32 splat_3f32(F32 e)                     {return (Vec3F32){e, e, e};}
internal Vec3F32 add_3f32(Vec3F32 a, Vec3F32 b)        {return (Vec3F32){a.x+b.x, a.y+b.y, a.z+b.z};}
internal Vec3F32 sub_3f32(Vec3F32 a, Vec3F32 b)        {return (Vec3F32){a.x-b.x, a.y-b.y, a.z-b.z};}
internal Vec3F32 mul_3f32(Vec3F32 a, Vec3F32 b)        {return (Vec3F32){a.x*b.x, a.y*b.y, a.z*b.z};}
internal Vec3F32 div_3f32(Vec3F32 a, Vec3F32 b)        {return (Vec3F32){a.x/b.x, a.y/b.y, a.z/b.z};}
internal Vec3F32 scale_3f32(Vec3F32 v, F32 s)          {return (Vec3F32){v.x*s,   v.y*s,   v.z*s  };}
internal F32 dot_3f32(Vec3F32 a, Vec3F32 b)            {return a.x*b.x + a.y*b.y + a.z*b.z;}
internal F32 length_squared_3f32(Vec3F32 v)            {return v.x*v.x + v.y*v.y + v.z*v.z;}
internal F32 length_3f32(Vec3F32 v)                    {return sqrt_f32(length_squared_3f32(v));}
internal Vec3F32 normalize_3f32(Vec3F32 v)             {return scale_3f32(v, 1.f/length_3f32(v));}
internal Vec3F32 mix_3f32(Vec3F32 a, Vec3F32 b, F32 t) {return (Vec3F32){mix_1f32(a.x, b.x, t), mix_1f32(a.y, b.y, t), mix_1f32(a.z, b.z, t)};}
internal Vec3F32 cross_3f32(Vec3F32 a, Vec3F32 b)      {return (Vec3F32){a.y*b.z - b.y*a.z, a.z*b.x - b.z*a.x, a.x*b.y - b.x*a.y};}
internal Vec3F32 transform_3f32(Vec3F32 v, Mat3x3F32 m)
{
	NotImplemented
	Vec3F32 result = zero_struct;
	return result;
}

internal Vec4F32 vec_4f32(F32 x, F32 y, F32 z, F32 w)  {return (Vec4F32){x, y, z, w};}
internal Vec4F32 splat_4f32(F32 e)                     {return (Vec4F32){e, e, e, e};}
internal Vec4F32 add_4f32(Vec4F32 a, Vec4F32 b)        {return (Vec4F32){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};}
internal Vec4F32 sub_4f32(Vec4F32 a, Vec4F32 b)        {return (Vec4F32){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};}
internal Vec4F32 mul_4f32(Vec4F32 a, Vec4F32 b)        {return (Vec4F32){a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};}
internal Vec4F32 div_4f32(Vec4F32 a, Vec4F32 b)        {return (Vec4F32){a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w};}
internal Vec4F32 scale_4f32(Vec4F32 v, F32 s)          {return (Vec4F32){v.x*s,   v.y*s,   v.z*s,   v.w*s  };}
internal F32 dot_4f32(Vec4F32 a, Vec4F32 b)            {return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;}
internal F32 length_squared_4f32(Vec4F32 v)            {return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;}
internal F32 length_4f32(Vec4F32 v)                    {return sqrt_f32(length_squared_4f32(v));}
internal Vec4F32 normalize_4f32(Vec4F32 v)             {return scale_4f32(v, 1.f/length_4f32(v));}
internal Vec4F32 mix_4f32(Vec4F32 a, Vec4F32 b, F32 t) {return (Vec4F32){mix_1f32(a.x, b.x, t), mix_1f32(a.y, b.y, t), mix_1f32(a.z, b.z, t), mix_1f32(a.w, b.w, t)};}
internal Vec4F32 transform_4f32(Vec4F32 v, Mat4x4F32 m)
{
	NotImplemented
	Vec4F32 result = zero_struct;
	return result;
}

// TODO(beau): matrix and quaternion ops
//    layout of the matrices? need to consider simd and gfx api layout. perhaps some macro to determine the layout. would be sad

internal Rng1F32 rng_1f32(F32 min, F32 max)          {if (min > max) Swap(F32, min, max); return (Rng1F32){min, max};}
internal Rng1F32 shift_1f32(Rng1F32 r, F32 x)        {return (Rng1F32){r.min+x, r.min+x};}
internal Rng1F32 pad_1f32(Rng1F32 r, F32 x)          {return (Rng1F32){r.min-x, r.min+x};}
internal F32 center_1f32(Rng1F32 r)                  {return (r.min+r.max)/2.f;}
internal B32 contains_1f32(Rng1F32 r, F32 x)         {return r.min <= x && x <= r.max;}
internal F32 dim_1f32(Rng1F32 r)                     {return r.max>r.min ? r.max-r.min : 0.f;}
internal Rng1F32 union_1f32(Rng1F32 a, Rng1F32 b)    {return (Rng1F32){Min(a.min, b.min), Max(a.max, b.max)};}
internal Rng1F32 intersect_1f32(Rng1F32 a, Rng1F32 b){return (Rng1F32){Max(a.min, b.min), Min(a.max, b.max)};}
internal F32 clamp_1f32(Rng1F32 r, F32 v)            {return Clamp(r.min, v, r.max);}

internal Rng1S32 rng_1s32(S32 min, S32 max)          {if (min > max) Swap(S32, min, max); return (Rng1S32){min, max};}
internal Rng1S32 shift_1s32(Rng1S32 r, S32 x)        {return (Rng1S32){r.min+x, r.min+x};}
internal Rng1S32 pad_1s32(Rng1S32 r, S32 x)          {return (Rng1S32){r.min-x, r.min+x};}
internal S32 center_1s32(Rng1S32 r)                  {return (r.min+r.max)/2;}
internal B32 contains_1s32(Rng1S32 r, S32 x)         {return r.min <= x && x <= r.max;}
internal S32 dim_1s32(Rng1S32 r)                     {return r.max>r.min ? r.max-r.min : 0ll;}
internal Rng1S32 union_1s32(Rng1S32 a, Rng1S32 b)    {return (Rng1S32){Min(a.min, b.min), Max(a.max, b.max)};}
internal Rng1S32 intersect_1s32(Rng1S32 a, Rng1S32 b){return (Rng1S32){Max(a.min, b.min), Min(a.max, b.max)};}
internal S32 clamp_1s32(Rng1S32 r, S32 v)            {return Clamp(r.min, v, r.max);}

internal Rng1S64 rng_1s64(S64 min, S64 max)          {if (min > max) Swap(S64, min, max); return (Rng1S64){min, max};}
internal Rng1S64 shift_1s64(Rng1S64 r, S64 x)        {return (Rng1S64){r.min+x, r.min+x};}
internal Rng1S64 pad_1s64(Rng1S64 r, S64 x)          {return (Rng1S64){r.min-x, r.min+x};}
internal S64 center_1s64(Rng1S64 r)                  {return (r.min+r.max)/2;}
internal B32 contains_1s64(Rng1S64 r, S64 x)         {return r.min <= x && x <= r.max;}
internal S64 dim_1s64(Rng1S64 r)                     {return r.max>r.min ? r.max-r.min : 0ll;}
internal Rng1S64 union_1s64(Rng1S64 a, Rng1S64 b)    {return (Rng1S64){Min(a.min, b.min), Max(a.max, b.max)};}
internal Rng1S64 intersect_1s64(Rng1S64 a, Rng1S64 b){return (Rng1S64){Max(a.min, b.min), Min(a.max, b.max)};}
internal S64 clamp_1s64(Rng1S64 r, S64 v)            {return Clamp(r.min, v, r.max);}

internal Rng1U64 rng_1u64(U64 min, U64 max)          {if (min > max) Swap(U64, min, max); return (Rng1U64){min, max};}
internal Rng1U64 shift_1u64(Rng1U64 r, U64 x)        {return (Rng1U64){r.min+x, r.min+x};}
internal Rng1U64 pad_1u64(Rng1U64 r, U64 x)          {return (Rng1U64){r.min-x, r.min+x};}
internal U64 center_1u64(Rng1U64 r)                  {return (r.min+r.max)/2;}
internal B32 contains_1u64(Rng1U64 r, U64 x)         {return r.min <= x && x <= r.max;}
internal U64 dim_1u64(Rng1U64 r)                     {return r.max>r.min ? r.max-r.min : 0ull;}
internal Rng1U64 union_1u64(Rng1U64 a, Rng1U64 b)    {return (Rng1U64){Min(a.min, b.min), Max(a.max, b.max)};}
internal Rng1U64 intersect_1u64(Rng1U64 a, Rng1U64 b){return (Rng1U64){Max(a.min, b.min), Min(a.max, b.max)};}
internal U64 clamp_1u64(Rng1U64 r, U64 v)            {return Clamp(r.min, v, r.max);}

internal Rng2F32 rng_2f32(Vec2F32 min, Vec2F32 max)  {return (Rng2F32){min, max};}
internal Rng2F32 shift_2f32(Rng2F32 r, Vec2F32 x)    {return (Rng2F32){add_2f32(r.min, x), add_2f32(r.max, x)};}
internal Rng2F32 pad_2f32(Rng2F32 r, F32 x)          {Vec2F32 xv = {x, x}; return (Rng2F32){sub_2f32(r.min, xv), add_2f32(r.max, xv)};}
internal Vec2F32 center_2f32(Rng2F32 r)              {return (Vec2F32){(r.x0+r.x1)/2.f, (r.y0+r.y1)/2.f};}
internal B32 contains_2f32(Rng2F32 r, Vec2F32 x)     {return r.min.x <= x.x && x.x <= r.max.x && r.min.y <= x.y && x.y <= r.max.y;}
internal Vec2F32 dim_2f32(Rng2F32 r)                 {return (Vec2F32){r.max.x>r.min.x?r.max.x-r.min.x:0.f, r.max.y>r.min.y?r.max.y-r.min.y:0.f};}
internal Rng2F32 union_2f32(Rng2F32 a, Rng2F32 b)    {return (Rng2F32){Min(a.min.x, b.min.x), Min(a.min.y, b.min.y), Max(a.max.x, b.max.x), Max(a.max.y, b.max.y)};}
internal Rng2F32 intersect_2f32(Rng2F32 a, Rng2F32 b){return (Rng2F32){Max(a.max.x, b.max.x), Max(a.max.y, b.max.y), Min(a.min.x, b.min.x), Min(a.min.y, b.min.y)};}
internal Vec2F32 clamp_2f32(Rng2F32 r, Vec2F32 v)    {return (Vec2F32){Clamp(r.min.x, v.x, r.max.x), Clamp(r.min.y, v.y, r.max.y)};}

internal Rng2S32 rng_2s32(Vec2S32 min, Vec2S32 max)  {return (Rng2S32){min, max};}
internal Rng2S32 shift_2s32(Rng2S32 r, Vec2S32 x)    {return (Rng2S32){add_2s32(r.min, x), add_2s32(r.max, x)};}
internal Rng2S32 pad_2s32(Rng2S32 r, S32 x)          {Vec2S32 xv = {x, x}; return (Rng2S32){sub_2s32(r.min, xv), add_2s32(r.max, xv)};}
internal Vec2S32 center_2s32(Rng2S32 r)              {return (Vec2S32){(r.x0+r.x1)/2ll, (r.y0+r.y1)/2ll};}
internal B32 contains_2s32(Rng2S32 r, Vec2S32 x)     {return r.min.x <= x.x && x.x <= r.max.x && r.min.y <= x.y && x.y <= r.max.y;}
internal Vec2S32 dim_2s32(Rng2S32 r)                 {return (Vec2S32){r.max.x>r.min.x?r.max.x-r.min.x:0ll, r.max.y>r.min.y?r.max.y-r.min.y:0ll};}
internal Rng2S32 union_2s32(Rng2S32 a, Rng2S32 b)    {return (Rng2S32){Min(a.min.x, b.min.x), Min(a.min.y, b.min.y), Max(a.max.x, b.max.x), Max(a.max.y, b.max.y)};}
internal Rng2S32 intersect_2s32(Rng2S32 a, Rng2S32 b){return (Rng2S32){Max(a.max.x, b.max.x), Max(a.max.y, b.max.y), Min(a.min.x, b.min.x), Min(a.min.y, b.min.y)};}
internal Vec2S32 clamp_2s32(Rng2S32 r, Vec2S32 v)    {return (Vec2S32){Clamp(r.min.x, v.x, r.max.x), Clamp(r.min.y, v.y, r.max.y)};}

internal Rng2S64 rng_2s64(Vec2S64 min, Vec2S64 max)  {return (Rng2S64){min, max};}
internal Rng2S64 shift_2s64(Rng2S64 r, Vec2S64 x)    {return (Rng2S64){add_2s64(r.min, x), add_2s64(r.max, x)};}
internal Rng2S64 pad_2s64(Rng2S64 r, S64 x)          {Vec2S64 xv = {x, x}; return (Rng2S64){sub_2s64(r.min, xv), add_2s64(r.max, xv)};}
internal Vec2S64 center_2s64(Rng2S64 r)              {return (Vec2S64){(r.x0+r.x1)/2ll, (r.y0+r.y1)/2ll};}
internal B32 contains_2s64(Rng2S64 r, Vec2S64 x)     {return r.min.x <= x.x && x.x <= r.max.x && r.min.y <= x.y && x.y <= r.max.y;}
internal Vec2S64 dim_2s64(Rng2S64 r)                 {return (Vec2S64){r.max.x>r.min.x?r.max.x-r.min.x:0ll, r.max.y>r.min.y?r.max.y-r.min.y:0ll};}
internal Rng2S64 union_2s64(Rng2S64 a, Rng2S64 b)    {return (Rng2S64){Min(a.min.x, b.min.x), Min(a.min.y, b.min.y), Max(a.max.x, b.max.x), Max(a.max.y, b.max.y)};}
internal Rng2S64 intersect_2s64(Rng2S64 a, Rng2S64 b){return (Rng2S64){Max(a.max.x, b.max.x), Max(a.max.y, b.max.y), Min(a.min.x, b.min.x), Min(a.min.y, b.min.y)};}
internal Vec2S64 clamp_2s64(Rng2S64 r, Vec2S64 v)    {return (Vec2S64){Clamp(r.min.x, v.x, r.max.x), Clamp(r.min.y, v.y, r.max.y)};}

internal Rng2U64 rng_2u64(Vec2U64 min, Vec2U64 max)  {return (Rng2U64){min, max};}
internal Rng2U64 shift_2u64(Rng2U64 r, Vec2U64 x)    {return (Rng2U64){add_2u64(r.min, x), add_2u64(r.max, x)};}
internal Rng2U64 pad_2u64(Rng2U64 r, U64 x)          {Vec2U64 xv = {x, x}; return (Rng2U64){sub_2u64(r.min, xv), add_2u64(r.max, xv)};}
internal Vec2U64 center_2u64(Rng2U64 r)              {return (Vec2U64){(r.x0+r.x1)/2ull, (r.y0+r.y1)/2ull};}
internal B32 contains_2u64(Rng2U64 r, Vec2U64 x)     {return r.min.x <= x.x && x.x <= r.max.x && r.min.y <= x.y && x.y <= r.max.y;}
internal Vec2U64 dim_2u64(Rng2U64 r)                 {return (Vec2U64){r.max.x>r.min.x?r.max.x-r.min.x:0ull, r.max.y>r.min.y?r.max.y-r.min.y:0ull};}
internal Rng2U64 union_2u64(Rng2U64 a, Rng2U64 b)    {return (Rng2U64){Min(a.min.x, b.min.x), Min(a.min.y, b.min.y), Max(a.max.x, b.max.x), Max(a.max.y, b.max.y)};}
internal Rng2U64 intersect_2u64(Rng2U64 a, Rng2U64 b){return (Rng2U64){Max(a.max.x, b.max.x), Max(a.max.y, b.max.y), Min(a.min.x, b.min.x), Min(a.min.y, b.min.y)};}
internal Vec2U64 clamp_2u64(Rng2U64 r, Vec2U64 v)    {return (Vec2U64){Clamp(r.min.x, v.x, r.max.x), Clamp(r.min.y, v.y, r.max.y)};}

internal Rng3F32 rng_3f32(Vec3F32 min, Vec3F32 max)  {return (Rng3F32){min, max};}
internal Rng3F32 shift_3f32(Rng3F32 r, Vec3F32 x)    {return (Rng3F32){add_3f32(r.min, x), add_3f32(r.max, x)};}
internal Rng3F32 pad_3f32(Rng3F32 r, F32 x)          {Vec3F32 xv = {x, x ,x}; return (Rng3F32){sub_3f32(r.min, xv), add_3f32(r.max, xv)};}
internal Vec3F32 center_3f32(Rng3F32 r)              {return (Vec3F32){(r.x0+r.x1)/2.f, (r.y0+r.y1)/2.f, (r.z0+r.z1)/2.f};}
internal B32 contains_3f32(Rng3F32 r, Vec3F32 x)     {return r.min.x <= x.x && x.x <= r.max.x && r.min.y <= x.y && x.y <= r.max.y && r.min.z <= x.z && x.z <= r.max.z ;}
internal Vec3F32 dim_3f32(Rng3F32 r)                 {return (Vec3F32){r.max.x>r.min.x?r.max.x-r.min.x:0.f, r.max.y>r.min.y?r.max.y-r.min.y:0.f, r.max.z>r.min.z?r.max.z-r.min.z:0.f};}
internal Rng3F32 union_3f32(Rng3F32 a, Rng3F32 b)    {return (Rng3F32){Min(a.min.x, b.min.x), Min(a.min.y, b.min.y), Min(a.min.z, b.min.z), Max(a.max.x, b.max.x), Max(a.max.y, b.max.y), Max(a.max.z, b.max.z)};}
internal Rng3F32 intersect_3f32(Rng3F32 a, Rng3F32 b){return (Rng3F32){Max(a.min.x, b.min.x), Max(a.min.y, b.min.y), Max(a.min.z, b.min.z), Min(a.max.x, b.max.x), Min(a.max.y, b.max.y), Min(a.max.z, b.max.z)};}
internal Vec3F32 clamp_3f32(Rng3F32 r, Vec3F32 v)    {return (Vec3F32){Clamp(r.min.x, v.x, r.max.x), Clamp(r.min.y, v.y, r.max.y), Clamp(r.min.z, v.z, r.max.z)};}
