/* TODO(beau):
 *  c++ stuff
 */

#ifndef BASE_CORE_H
#define BASE_CORE_H

#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define internal      static // internal linking for functions
#define global        static // internal linking for global vars
#define local_persist static // compile-time allocated and permanent memory for vars in local functions

// read_only
#if OS_MAC && COMPILER_CLANG
	#define read_only __attribute__((section("__DATA,__const")))
#elif COMPILER_MSVC || (COMPILER_CLANG && OS_WINDOWS)
	#pragma section(".rdata$", read)
	#define read_only __declspec(allocate(".rdata$"))
#else
	#define read_only
#endif

// thread local
#if COMPILER_CLANG || COMPILER_GCC
	#define thread_local __thread
#elif COMPILER_MSVC
	#define thread_local __declspec(thread)
#endif

// force inline
#if COMPILER_CLANG || COMPILER_GCC
	#define force_inline __attribute__((always_inline))
#elif COMPILER_MSVC
	#define force_inline __forceinline
#endif

#if COMPILER_CLANG
	#define Expect(expr, val) __builtin_expect((expr), (val))
#else
	#define Expect(expr, val) (expr)
#endif

#define Likely(expr)  Expect(expr,1)
#define Unikely(expr) Expect(expr,0)

#if COMPILER_CLANG || COMPILER_MSVC
	#define AlignOf(T) __alignof(T)
#else
	#error AlignOf not yet defined for this compiler
#endif

#if COMPILER_CLANG
	#define no_return _Noreturn
#elif COMPILER_MSVC
	#define no_return __declspec(noreturn)
#endif

#define this_function_name __func__

#define KB(n)  (((U64)(n)) << 10)
#define MB(n)  (((U64)(n)) << 20)
#define GB(n)  (((U64)(n)) << 30)
#define TB(n)  (((U64)(n)) << 40)
#define Thousand(n)   ((n)*1000)
#define Million(n)    ((n)*1000000)
#define Billion(n)    ((n)*1000000000)

#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define ClampTop(A,B) Min((A),(B))
#define ClampBot(A,B) Max((A),(B))
#define Clamp(A,X,B) (((X)<(A))?(A):((X)>(B))?(B):(X))

#define DeferLoop(begin, end)        for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

#define Member(T,m)                 (((T*)0)->m)
#define OffsetOf(T,m)               IntFromPtr(&Member(T,m))
#define MemberFromOffset(T,ptr,off) (T)((((U8 *)ptr)+(off)))
#define CastFromMember(T,m,ptr)     (T*)(((U8*)ptr) - OffsetOf(T,m))

#define EachIndex(it, count) (U64 it = 0; it < (count); it += 1)
#define EachElement(it, array) (U64 it = 0; it < ArrayCount(array); it += 1)
#define EachEnumVal(type, it) (type it = (type)0; it < type##_COUNT; it = (type)(it+1))
#define EachNonZeroEnumVal(type, it) (type it = (type)1; it < type##_COUNT; it = (type)(it+1))

#define MemoryCopy(dst, src, size)    memmove((dst), (src), (size))
#define MemorySet(dst, byte, size)    memset((dst), (byte), (size))
#define MemoryCompare(a, b, size)     memcmp((a), (b), (size))
#define MemoryStrlen(ptr)             strlen(ptr)

#define MemoryCopyStruct(d,s)  MemoryCopy((d),(s),sizeof(*(d)))
#define MemoryCopyArray(d,s)   MemoryCopy((d),(s),sizeof(d))
#define MemoryCopyTyped(d,s,c) MemoryCopy((d),(s),sizeof(*(d))*(c))

#define MemoryZero(s,z)       memset((s),0,(z))
#define MemoryZeroStruct(s)   MemoryZero((s),sizeof(*(s)))
#define MemoryZeroArray(a)    MemoryZero((a),sizeof(a))
#define MemoryZeroTyped(m,c)  MemoryZero((m),sizeof(*(m))*(c))

#define MemoryMatch(a,b,z)     (MemoryCompare((a),(b),(z)) == 0)
#define MemoryMatchStruct(a,b)  MemoryMatch((a),(b),sizeof(*(a)))
#define MemoryMatchArray(a,b)   MemoryMatch((a),(b),sizeof(a))

#define MemoryRead(T,p,e)    ( ((p)+sizeof(T)<=(e))?(*(T*)(p)):(0) )
#define MemoryConsume(T,p,e) ( ((p)+sizeof(T)<=(e))?((p)+=sizeof(T),*(T*)((p)-sizeof(T))):((p)=(e),0) )

// TODO: platform_specific
#if COMPILER_CLANG || COMPILER_GCC
	#define Trap() __builtin_trap()
#elif COMPILER_MSVC
	#define Trap() __debugbreak()
#else
	#error Trap intrinsic not defined for this compiler
#endif
#define NotImplemented Assert(!(char *)"Not implemented!");

// REVIEW: remove the optimization thing?
#if BUILD_DEBUG
	#define Unreachable AssertAlways(!(char *)"Unreachable!");
#else
	#if COMPILER_MSVC
		#define Unreachable __assume(0);
	#elif COMPILER_CLANG || COMPILER_GCC
		#define Unreachable __builtin_unreachable();
	#else
		#error Unreachable not defined for this compiler
	#endif
#endif // BUILD_DEBUG

#define AssertAlways(x) do{if(!(x)) {Trap();}}while(0)
#if BUILD_DEBUG
	#define Assert(x) AssertAlways(x)
#else
	#define Assert(x) (void)(x)
#endif
#define StaticAssert(C, ID) global U8 Glue(ID, __LINE__)[C?1:-1]

#define CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define SetNil(nil,p) ((p) = nil)

#define DLLInsert_NPZ(nil,f,l,p,n,next,prev) (CheckNil(nil,f) ? \
((f) = (l) = (n), SetNil(nil,(n)->next), SetNil(nil,(n)->prev)) :\
CheckNil(nil,p) ? \
((n)->next = (f), (f)->prev = (n), (f) = (n), SetNil(nil,(n)->prev)) :\
((p)==(l)) ? \
((l)->next = (n), (n)->prev = (l), (l) = (n), SetNil(nil, (n)->next)) :\
(((!CheckNil(nil,p) && CheckNil(nil,(p)->next)) ? (0) : ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define DLLPushBack_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,f,l,l,n,next,prev)
#define DLLPushFront_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,l,f,f,n,prev,next)
#define DLLRemove_NPZ(nil,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)),\
((n) == (l) ? (l) = (l)->prev : (0)),\
(CheckNil(nil,(n)->prev) ? (0) :\
((n)->prev->next = (n)->next)),\
(CheckNil(nil,(n)->next) ? (0) :\
((n)->next->prev = (n)->prev)))

#define SLLQueuePush_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((l)->next=(n),(l)=(n),SetNil(nil,(n)->next)))
#define SLLQueuePushFront_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((n)->next=(f),(f)=(n)))
#define SLLQueuePop_NZ(nil,f,l,next) ((f)==(l)?\
(SetNil(nil,f),SetNil(nil,l)):\
((f)=(f)->next))

#define SLLStackPush_N(f,n,next) ((n)->next=(f), (f)=(n))
#define SLLStackPop_N(f,next) ((f)=(f)->next)

#define DLLInsert_NP(f,l,p,n,next,prev) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack_NP(f,l,n,next,prev) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront_NP(f,l,n,next,prev) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove_NP(f,l,n,next,prev) DLLRemove_NPZ(0,f,l,n,next,prev)
#define DLLInsert(f,l,p,n) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack(f,l,n) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront(f,l,n) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove(f,l,n) DLLRemove_NPZ(0,f,l,n,next,prev)

#define SLLQueuePush_N(f,l,n,next) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront_N(f,l,n,next) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop_N(f,l,next) SLLQueuePop_NZ(0,f,l,next)
#define SLLQueuePush(f,l,n) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront(f,l,n) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop(f,l) SLLQueuePop_NZ(0,f,l,next)

#define SLLStackPush(f,n) SLLStackPush_N(f,n,next)
#define SLLStackPop(f) SLLStackPop_N(f,next)

#if COMPILER_CLANG
	#if defined(__has_feature)
		#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
			#define ASAN_ENABLED 1
		#endif
	#endif
	#define NO_ASAN __attribute__((no_sanitize("address")))
#elif COMPILER_MSVC
	#if defined(__SANITIZE_ADDRESS__)
		#define ASAN_ENABLED 1
		#define NO_ASAN __declspec(no_sanitize_address)
	#else
		#define NO_ASAN
	#endif
#else
	#define NO_ASAN
#endif

#if ASAN_ENABLED
	#include <sanitizer/asan_interface.h>

	#define AsanPoisonMemoryRegion(addr, size) __asan_poison_memory_region((addr), (size))
	#define AsanUnpoisonMemoryRegion(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
	#define AsanPoisonMemoryRegion(addr, size)   ((void)(addr), (void)(size))
	#define AsanUnpoisonMemoryRegion(addr, size) ((void)(addr), (void)(size))
#endif

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)

#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#define Swap(T,a,b) do{T t__ = a; a = b; b = t__;}while(0)

#define IntFromPtr(ptr) ((U64)(ptr))
#define PtrFromInt(i) (void*)((U8*)0 + (i))

#define Compose64Bit(hi,lo) ((((U64)hi) << 32) | ((U64)lo))
#define AlignPow2(x,b)      (((x) + (b) - 1)&(~((b) - 1)))
#define AlignDownPow2(x,b)  ((x)&(~((b) - 1)))
#define AlignPadPow2(x,b)   ((0-(x)) & ((b) - 1))
#define IsPow2(x)           ((x)!=0 && ((x)&((x)-1))==0)
#define IsPow2OrZero(x)     ((((x) - 1)&(x)) == 0)

#define ExtractBit(word, idx) (((word) >> (idx)) & 1)

#if LANG_CPP
	#define zero_struct {}
#else
	#define zero_struct {0}
#endif

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;

typedef U8 Byte;

typedef enum
{
	Side_Min,
	Side_Left   = Side_Min,
	Side_Bottom = Side_Min,

	Side_Max,
	Side_Right  = Side_Max,
	Side_Top    = Side_Max,
	Side_COUNT,
} Side;
#define side_flip(s) ((Side)(!(s)))

typedef enum
{
	Axis2_X,
	Axis2_Y,
	Axis2_COUNT,
} Axis2;
#define axis2_flip(s) ((Axis2)(!(s)))

typedef enum
{
	Axis3_X,
	Axis3_Y,
	Axis3_Z,
	Axis3_COUNT,
} Axis3;


typedef enum
{
	Axis4_X,
	Axis4_Y,
	Axis4_Z,
	Axis4_W,
	Axis4_COUNT,
} Axis4;

typedef enum OperatingSystem
{
	OperatingSystem_Null,
	OperatingSystem_Windows,
	OperatingSystem_Linux,
	OperatingSystem_Mac,
	OperatingSystem_COUNT,
}
OperatingSystem;

typedef enum Arch
{
	Arch_Null,
	Arch_x64,
	Arch_arm64,
	Arch_COUNT,
}
Arch;

typedef enum Compiler
{
	Compiler_Null,
	Compiler_msvc,
	Compiler_clang,
	Compiler_gcc,
	Compiler_COUNT,
}
Compiler;

internal OperatingSystem operating_system_from_context(void);
internal Arch arch_from_context(void);
internal Compiler compiler_from_context(void);

#define Unused(name) (void)(name)

const global U64 max_U64 = UINT64_MAX;
const global U32 max_U32 = UINT32_MAX;
const global U16 max_U16 = UINT16_MAX;
const global U8  max_U8  = UINT8_MAX;

const global S64 max_S64 = INT64_MAX;
const global S32 max_S32 = INT32_MAX;
const global S16 max_S16 = INT16_MAX;
const global S8  max_S8  = INT8_MAX;

const global S64 min_S64 = INT64_MIN;
const global S32 min_S32 = INT32_MIN;
const global S16 min_S16 = INT16_MIN;
const global S8  min_S8  = INT8_MIN;

#define bitmask1  0x00000001u
#define bitmask2  0x00000003u
#define bitmask3  0x00000007u
#define bitmask4  0x0000000fu
#define bitmask5  0x0000001fu
#define bitmask6  0x0000003fu
#define bitmask7  0x0000007fu
#define bitmask8  0x000000ffu
#define bitmask9  0x000001ffu
#define bitmask10 0x000003ffu
#define bitmask11 0x000007ffu
#define bitmask12 0x00000fffu
#define bitmask13 0x00001fffu
#define bitmask14 0x00003fffu
#define bitmask15 0x00007fffu
#define bitmask16 0x0000ffffu
#define bitmask17 0x0001ffffu
#define bitmask18 0x0003ffffu
#define bitmask19 0x0007ffffu
#define bitmask20 0x000fffffu
#define bitmask21 0x001fffffu
#define bitmask22 0x003fffffu
#define bitmask23 0x007fffffu
#define bitmask24 0x00ffffffu
#define bitmask25 0x01ffffffu
#define bitmask26 0x03ffffffu
#define bitmask27 0x07ffffffu
#define bitmask28 0x0fffffffu
#define bitmask29 0x1fffffffu
#define bitmask30 0x3fffffffu
#define bitmask31 0x7fffffffu
#define bitmask32 0xffffffffu

#define bitmask33 0x00000001ffffffffull
#define bitmask34 0x00000003ffffffffull
#define bitmask35 0x00000007ffffffffull
#define bitmask36 0x0000000fffffffffull
#define bitmask37 0x0000001fffffffffull
#define bitmask38 0x0000003fffffffffull
#define bitmask39 0x0000007fffffffffull
#define bitmask40 0x000000ffffffffffull
#define bitmask41 0x000001ffffffffffull
#define bitmask42 0x000003ffffffffffull
#define bitmask43 0x000007ffffffffffull
#define bitmask44 0x00000fffffffffffull
#define bitmask45 0x00001fffffffffffull
#define bitmask46 0x00003fffffffffffull
#define bitmask47 0x00007fffffffffffull
#define bitmask48 0x0000ffffffffffffull
#define bitmask49 0x0001ffffffffffffull
#define bitmask50 0x0003ffffffffffffull
#define bitmask51 0x0007ffffffffffffull
#define bitmask52 0x000fffffffffffffull
#define bitmask53 0x001fffffffffffffull
#define bitmask54 0x003fffffffffffffull
#define bitmask55 0x007fffffffffffffull
#define bitmask56 0x00ffffffffffffffull
#define bitmask57 0x01ffffffffffffffull
#define bitmask58 0x03ffffffffffffffull
#define bitmask59 0x07ffffffffffffffull
#define bitmask60 0x0fffffffffffffffull
#define bitmask61 0x1fffffffffffffffull
#define bitmask62 0x3fffffffffffffffull
#define bitmask63 0x7fffffffffffffffull
#define bitmask64 0xffffffffffffffffull

#define bit1  (1u<<0u)
#define bit2  (1u<<1u)
#define bit3  (1u<<2u)
#define bit4  (1u<<3u)
#define bit5  (1u<<4u)
#define bit6  (1u<<5u)
#define bit7  (1u<<6u)
#define bit8  (1u<<7u)
#define bit9  (1u<<8u)
#define bit10 (1u<<9u)
#define bit11 (1u<<10u)
#define bit12 (1u<<11u)
#define bit13 (1u<<12u)
#define bit14 (1u<<13u)
#define bit15 (1u<<14u)
#define bit16 (1u<<15u)
#define bit17 (1u<<16u)
#define bit18 (1u<<17u)
#define bit19 (1u<<18u)
#define bit20 (1u<<19u)
#define bit21 (1u<<20u)
#define bit22 (1u<<21u)
#define bit23 (1u<<22u)
#define bit24 (1u<<23u)
#define bit25 (1u<<24u)
#define bit26 (1u<<25u)
#define bit27 (1u<<26u)
#define bit28 (1u<<27u)
#define bit29 (1u<<28u)
#define bit30 (1u<<29u)
#define bit31 (1u<<30u)
#define bit32 (1u<<31u)

#define bit33 (1ull<<32ull)
#define bit34 (1ull<<33ull)
#define bit35 (1ull<<34ull)
#define bit36 (1ull<<35ull)
#define bit37 (1ull<<36ull)
#define bit38 (1ull<<37ull)
#define bit39 (1ull<<38ull)
#define bit40 (1ull<<39ull)
#define bit41 (1ull<<40ull)
#define bit42 (1ull<<41ull)
#define bit43 (1ull<<42ull)
#define bit44 (1ull<<43ull)
#define bit45 (1ull<<44ull)
#define bit46 (1ull<<45ull)
#define bit47 (1ull<<46ull)
#define bit48 (1ull<<47ull)
#define bit49 (1ull<<48ull)
#define bit50 (1ull<<49ull)
#define bit51 (1ull<<50ull)
#define bit52 (1ull<<51ull)
#define bit53 (1ull<<52ull)
#define bit54 (1ull<<53ull)
#define bit55 (1ull<<54ull)
#define bit56 (1ull<<55ull)
#define bit57 (1ull<<56ull)
#define bit58 (1ull<<57ull)
#define bit59 (1ull<<58ull)
#define bit60 (1ull<<59ull)
#define bit61 (1ull<<60ull)
#define bit62 (1ull<<61ull)
#define bit63 (1ull<<62ull)
#define bit64 (1ull<<63ull)

internal U16 safe_cast_u16(U32 x);
internal U32 safe_cast_u32(U64 x);
internal S32 safe_cast_s32(S64 x);

internal U32 u32_from_u64_saturate(U64 x);

#endif // BASE_CORE_H
