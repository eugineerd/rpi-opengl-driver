#ifndef V3D_H
#define V3D_H
#include <stdint.h>
#include <stdbool.h>

#define v3d ((volatile __attribute__((aligned(4))) uint32_t *)(uintptr_t)(RPi_IO_Base_Addr + 0xc00000))

/* Registers shamelessly copied from Eric AnHolt */

// Defines for v3d register offsets
#define V3D_IDENT0 (0x000 >> 2) // V3D Identification 0 (V3D block identity)
#define V3D_IDENT1 (0x004 >> 2) // V3D Identification 1 (V3D Configuration A)
#define V3D_IDENT2 (0x008 >> 2) // V3D Identification 1 (V3D Configuration B)

#define V3D_SCRATCH (0x010 >> 2) // Scratch Register

#define V3D_L2CACTL (0x020 >> 2) // 2 Cache Control
#define V3D_SLCACTL (0x024 >> 2) // Slices Cache Control

#define V3D_INTCTL (0x030 >> 2) // Interrupt Control
#define V3D_INTENA (0x034 >> 2) // Interrupt Enables
#define V3D_INTDIS (0x038 >> 2) // Interrupt Disables

#define V3D_CT0CS (0x100 >> 2)   // Control List Executor Thread 0 Control and Status.
#define V3D_CT1CS (0x104 >> 2)   // Control List Executor Thread 1 Control and Status.
#define V3D_CT0EA (0x108 >> 2)   // Control List Executor Thread 0 End Address.
#define V3D_CT1EA (0x10c >> 2)   // Control List Executor Thread 1 End Address.
#define V3D_CT0CA (0x110 >> 2)   // Control List Executor Thread 0 Current Address.
#define V3D_CT1CA (0x114 >> 2)   // Control List Executor Thread 1 Current Address.
#define V3D_CT00RA0 (0x118 >> 2) // Control List Executor Thread 0 Return Address.
#define V3D_CT01RA0 (0x11c >> 2) // Control List Executor Thread 1 Return Address.
#define V3D_CT0LC (0x120 >> 2)   // Control List Executor Thread 0 List Counter
#define V3D_CT1LC (0x124 >> 2)   // Control List Executor Thread 1 List Counter
#define V3D_CT0PC (0x128 >> 2)   // Control List Executor Thread 0 Primitive List Counter
#define V3D_CT1PC (0x12c >> 2)   // Control List Executor Thread 1 Primitive List Counter

#define V3D_PCS (0x130 >> 2) // V3D Pipeline Control and Status
#define V3D_BFC (0x134 >> 2) // Binning Mode Flush Count
#define V3D_RFC (0x138 >> 2) // Rendering Mode Frame Count

#define V3D_BPCA (0x300 >> 2) // Current Address of Binning Memory Pool
#define V3D_BPCS (0x304 >> 2) // Remaining Size of Binning Memory Pool
#define V3D_BPOA (0x308 >> 2) // Address of Overspill Binning Memory Block
#define V3D_BPOS (0x30c >> 2) // Size of Overspill Binning Memory Block
#define V3D_BXCF (0x310 >> 2) // Binner Debug

#define V3D_SQRSV0 (0x410 >> 2) // Reserve QPUs 0-7
#define V3D_SQRSV1 (0x414 >> 2) // Reserve QPUs 8-15
#define V3D_SQCNTL (0x418 >> 2) // QPU Scheduler Control

#define V3D_SRQPC (0x430 >> 2) // QPU User Program Request Program Address
#define V3D_SRQUA (0x434 >> 2) // QPU User Program Request Uniforms Address
#define V3D_SRQUL (0x438 >> 2) // QPU User Program Request Uniforms Length
#define V3D_SRQCS (0x43c >> 2) // QPU User Program Request Control and Status

#define V3D_VPACNTL (0x500 >> 2) // VPM Allocator Control
#define V3D_VPMBASE (0x504 >> 2) // VPM base (user) memory reservation

#define V3D_PCTRC (0x670 >> 2) // Performance Counter Clear
#define V3D_PCTRE (0x674 >> 2) // Performance Counter Enables

#define V3D_PCTR0 (0x680 >> 2)   // Performance Counter Count 0
#define V3D_PCTRS0 (0x684 >> 2)  // Performance Counter Mapping 0
#define V3D_PCTR1 (0x688 >> 2)   // Performance Counter Count 1
#define V3D_PCTRS1 (0x68c >> 2)  // Performance Counter Mapping 1
#define V3D_PCTR2 (0x690 >> 2)   // Performance Counter Count 2
#define V3D_PCTRS2 (0x694 >> 2)  // Performance Counter Mapping 2
#define V3D_PCTR3 (0x698 >> 2)   // Performance Counter Count 3
#define V3D_PCTRS3 (0x69c >> 2)  // Performance Counter Mapping 3
#define V3D_PCTR4 (0x6a0 >> 2)   // Performance Counter Count 4
#define V3D_PCTRS4 (0x6a4 >> 2)  // Performance Counter Mapping 4
#define V3D_PCTR5 (0x6a8 >> 2)   // Performance Counter Count 5
#define V3D_PCTRS5 (0x6ac >> 2)  // Performance Counter Mapping 5
#define V3D_PCTR6 (0x6b0 >> 2)   // Performance Counter Count 6
#define V3D_PCTRS6 (0x6b4 >> 2)  // Performance Counter Mapping 6
#define V3D_PCTR7 (0x6b8 >> 2)   // Performance Counter Count 7
#define V3D_PCTRS7 (0x6bc >> 2)  // Performance Counter Mapping 7
#define V3D_PCTR8 (0x6c0 >> 2)   // Performance Counter Count 8
#define V3D_PCTRS8 (0x6c4 >> 2)  // Performance Counter Mapping 8
#define V3D_PCTR9 (0x6c8 >> 2)   // Performance Counter Count 9
#define V3D_PCTRS9 (0x6cc >> 2)  // Performance Counter Mapping 9
#define V3D_PCTR10 (0x6d0 >> 2)  // Performance Counter Count 10
#define V3D_PCTRS10 (0x6d4 >> 2) // Performance Counter Mapping 10
#define V3D_PCTR11 (0x6d8 >> 2)  // Performance Counter Count 11
#define V3D_PCTRS11 (0x6dc >> 2) // Performance Counter Mapping 11
#define V3D_PCTR12 (0x6e0 >> 2)  // Performance Counter Count 12
#define V3D_PCTRS12 (0x6e4 >> 2) // Performance Counter Mapping 12
#define V3D_PCTR13 (0x6e8 >> 2)  // Performance Counter Count 13
#define V3D_PCTRS13 (0x6ec >> 2) // Performance Counter Mapping 13
#define V3D_PCTR14 (0x6f0 >> 2)  // Performance Counter Count 14
#define V3D_PCTRS14 (0x6f4 >> 2) // Performance Counter Mapping 14
#define V3D_PCTR15 (0x6f8 >> 2)  // Performance Counter Count 15
#define V3D_PCTRS15 (0x6fc >> 2) // Performance Counter Mapping 15

#define V3D_DBGE (0xf00 >> 2)  // PSE Error Signals
#define V3D_FDBGO (0xf04 >> 2) // FEP Overrun Error Signals
#define V3D_FDBGB (0xf08 >> 2) // FEP Interface Ready and Stall Signals, FEP Busy Signals
#define V3D_FDBGR (0xf0c >> 2) // FEP Internal Ready Signals
#define V3D_FDBGS (0xf10 >> 2) // FEP Internal Stall Input Signals

#define V3D_ERRSTAT (0xf20 >> 2) // Miscellaneous Error Signals (VPM, VDW, VCD, VCM, L2C)

/* primitive typoe in the GL pipline */
typedef enum
{
    PRIM_POINT = 0,
    PRIM_LINE = 1,
    PRIM_LINE_LOOP = 2,
    PRIM_LINE_STRIP = 3,
    PRIM_TRIANGLE = 4,
    PRIM_TRIANGLE_STRIP = 5,
    PRIM_TRIANGLE_FAN = 6,
} PRIMITIVE;

/* GL pipe control commands */
typedef enum
{
    GL_HALT = 0,
    GL_NOP = 1,
    GL_FLUSH = 4,
    GL_FLUSH_ALL_STATE = 5,
    GL_START_TILE_BINNING = 6,
    GL_INCREMENT_SEMAPHORE = 7,
    GL_WAIT_ON_SEMAPHORE = 8,
    GL_BRANCH = 16,
    GL_BRANCH_TO_SUBLIST = 17,
    GL_RETURN_FROM_SUBLIST = 18,
    GL_STORE_MULTISAMPLE = 24,
    GL_STORE_MULTISAMPLE_END = 25,
    GL_STORE_FULL_TILE_BUFFER = 26,
    GL_RELOAD_FULL_TILE_BUFFER = 27,
    GL_STORE_TILE_BUFFER = 28,
    GL_LOAD_TILE_BUFFER = 29,
    GL_INDEXED_PRIMITIVE_LIST = 32,
    GL_VERTEX_ARRAY_PRIMITIVES = 33,
    GL_VG_COORDINATE_ARRAY_PRIMITIVES = 41,
    GL_COMPRESSED_PRIMITIVE_LIST = 48,
    GL_CLIP_COMPRESSD_PRIMITIVE_LIST = 49,
    GL_PRIMITIVE_LIST_FORMAT = 56,
    GL_SHADER_STATE = 64,
    GL_NV_SHADER_STATE = 65,
    GL_VG_SHADER_STATE = 66,
    GL_VG_INLINE_SHADER_RECORD = 67,
    GL_CONFIG_STATE = 96,
    GL_FLAT_SHADE_FLAGS = 97,
    GL_POINTS_SIZE = 98,
    GL_LINE_WIDTH = 99,
    GL_RHT_X_BOUNDARY = 100,
    GL_DEPTH_OFFSET = 101,
    GL_CLIP_WINDOW = 102,
    GL_VIEWPORT_OFFSET = 103,
    GL_Z_CLIPPING_PLANES = 104,
    GL_CLIPPER_XY_SCALING = 105,
    GL_CLIPPER_Z_ZSCALE_OFFSET = 106,
    GL_TILE_BINNING_CONFIG = 112,
    GL_TILE_RENDER_CONFIG = 113,
    GL_CLEAR_COLORS = 114,
    GL_TILE_COORDINATES = 115
} GL_CONTROL;

typedef enum
{
    MEM_FLAG_DISCARDABLE = 1 << 0,                                     /* can be resized to 0 at any time. Use for cached data */
    MEM_FLAG_NORMAL = 0 << 2,                                          /* normal allocating alias. Don't use from ARM			*/
    MEM_FLAG_DIRECT = 1 << 2,                                          /* 0xC alias uncached									*/
    MEM_FLAG_COHERENT = 2 << 2,                                        /* 0x8 alias. Non-allocating in L2 but coherent			*/
    MEM_FLAG_L1_NONALLOCATING = (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT), /* Allocating in L2									*/
    MEM_FLAG_ZERO = 1 << 4,                                            /* initialise buffer to all zeros						*/
    MEM_FLAG_NO_INIT = 1 << 5,                                         /* don't initialise (default is initialise to all ones	*/
    MEM_FLAG_HINT_PERMALOCK = 1 << 6,                                  /* Likely to be locked for long periods of time.		*/
} V3D_MEMALLOC_FLAGS;

typedef uint32_t GPU_HANDLE;
typedef uint32_t VC4_ADDR;

/*-[ V3D_mem_alloc ]--------------------------------------------------------}
. Allocates contiguous memory on the GPU with size and alignment in bytes
. and with the properties of the flags.
. RETURN : GPU handle on successs, 0 if allocation fails
.--------------------------------------------------------------------------*/
GPU_HANDLE V3D_mem_alloc(uint32_t size, uint32_t align, V3D_MEMALLOC_FLAGS flags);

/*-[ V3D_mem_free ]---------------------------------------------------------}
. All memory associated with the GPU handle is released.
. RETURN : TRUE on successs, FALSE if release fails
.--------------------------------------------------------------------------*/
bool V3D_mem_free(GPU_HANDLE handle);

/*-[ V3D_mem_lock ]---------------------------------------------------------}
. Locks the memory associated to the GPU handle to a GPU bus address.
. RETURN : locked gpu address, 0 if lock fails
.--------------------------------------------------------------------------*/
uint32_t V3D_mem_lock(GPU_HANDLE handle);

/*-[ V3D_mem_unlock ]-------------------------------------------------------}
. Unlocks the memory associated to the GPU handle from the GPU bus address.
. RETURN : TRUE if sucessful, FALSE if it fails
.--------------------------------------------------------------------------*/
bool V3D_mem_unlock(GPU_HANDLE handle);

bool V3D_init();

#endif