/* TODO(beau):
 *  msvc stuff
 *  c++ stuff
 */

#ifndef BASE_CORE_H
#define BASE_CORE_H

#include <stdint.h>
#include <string.h>

#define function      static
#define global        static
#define local_persist static

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

#define KB(n)  (((U64)(n)) << 10)
#define MB(n)  (((U64)(n)) << 20)
#define Gigabytes(n)  (((U64)(n)) << 30)
#define Terabytes(n)  (((U64)(n)) << 40)
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

#define Compose64Bit(a,b)  ((((U64)a) << 32) | ((U64)b));
#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define AlignPadPow2(x,b)  ((0-(x)) & ((b) - 1))
#define IsPow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define IsPow2OrZero(x)    ((((x) - 1)&(x)) == 0)

#define ExtractBit(word, idx) (((word) >> (idx)) & 1)

#define zero_struct {0}

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

#define Unused(name) (void)(name)

#endif // BASE_CORE_H
