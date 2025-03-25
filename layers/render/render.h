#ifndef RENDER_H
#define RENDER_H

typedef enum
{
	R_BufferKind_TODO,
} R_BufferKind;

typedef enum
{
	R_ResourceKind_TODO,
} R_ResourceKind;

typedef enum
{
	R_UsageKind_TODO,
} R_UsageKind;

typedef enum
{
	R_ImageKind_TODO,
} R_ImageKind;

typedef enum
{
	R_PixelFormat_TODO,
} R_PixelFormat;


typedef struct R_Buffer      { U32 id; } R_Buffer;
typedef struct R_Texture     { U32 id; } R_Texture;
typedef struct R_Sampler     { U32 id; } R_Sampler;
typedef struct R_Shader      { U32 id; } R_Shader;
typedef struct R_Pipeline    { U32 id; } R_Pipeline;
typedef struct R_Attachments { U32 id; } R_Attachments;

typedef struct R_BufferDesc
{
	S64 length;
	R_BufferKind kind;
	R_UsageKind usage;
	String8 initial_bytes, label;
} R_BufferDesc;
typedef struct R_TextureDesc
{
	R_ImageKind kind;
	B8 is_render_target;
	Vec3S32 dim;
	S32 mipmaps;
	R_UsageKind usage;
	R_PixelFormat pixel_format;
	S32 sample_count;
	// TODO: data
	String8 label;
} R_TextureDesc;
typedef struct R_SamplerDesc
{
} R_SamplerDesc;
typedef struct R_ShaderDesc
{
} R_ShaderDesc;
typedef struct R_PipelineDesc
{
} R_PipelineDesc;
typedef struct R_AttachmentsDesc
{
} R_AttachmentsDesc;

typedef struct { Byte ignoreme; } R_InitReceipt;

internal R_InitReceipt r_init(OS_GFX_InitReceipt gfx_init);

internal void r_window_equip(OS_Window os_window);
internal void r_window_unequip(OS_Window os_window);

internal void r_window_close(OS_Window os_window);

#endif // RENDER_H
