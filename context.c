#include "context.h"

#include <stdint.h>

#include "rpi-SmartStart.h"
#include "v3d.h"
#include "emb-stdio.h"

struct __attribute__((__packed__, aligned(1))) EMITDATA
{
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
    uint8_t byte4;
};

static void emit_uint8_t(uint8_t **list, uint8_t d)
{
    *((*list)++) = d;
}

static void emit_uint16_t(uint8_t **list, uint16_t d)
{
    struct EMITDATA *data = (struct EMITDATA *)&d;
    *((*list)++) = (*data).byte1;
    *((*list)++) = (*data).byte2;
}

static void emit_uint32_t(uint8_t **list, uint32_t d)
{
    struct EMITDATA *data = (struct EMITDATA *)&d;
    *((*list)++) = (*data).byte1;
    *((*list)++) = (*data).byte2;
    *((*list)++) = (*data).byte3;
    *((*list)++) = (*data).byte4;
}

static void emit_float(uint8_t **list, float f)
{
    struct EMITDATA *data = (struct EMITDATA *)&f;
    *((*list)++) = (*data).byte1;
    *((*list)++) = (*data).byte2;
    *((*list)++) = (*data).byte3;
    *((*list)++) = (*data).byte4;
}

static GPU_BUFFER alloc_gpu_buffer(uint32_t maxSize, uint32_t *err)
{
    GPU_HANDLE handle = V3D_mem_alloc(maxSize, 0x1000, MEM_FLAG_COHERENT | MEM_FLAG_ZERO);
    if (handle == 0)
    {
        printf("alloc_gpu_buffer: failed to allocate gpu buffer\n");
        *err = 1;
        return;
    }
    VC4_ADDR bufferGpuAddr = V3D_mem_lock(handle);
    if (bufferGpuAddr == 0)
    {
        printf("alloc_gpu_buffer: failed to lock gpu buffer\n");
        *err = 1;
        return;
    }
    GPU_BUFFER buffer = {
        .gpuBaseAddr = bufferGpuAddr,
        .gpuHandle = handle,
        .basePtr = (uint8_t *)GPUaddrToARMaddr(bufferGpuAddr),
        .maxSize = maxSize,
        .size = 0};
    return buffer;
}

static void free_gpu_buffer(GPU_BUFFER *buffer, uint32_t *err)
{
    if (!V3D_mem_unlock(buffer->gpuHandle))
    {
        printf("free_gpu_buffer: Failed to unlock gpu buffer\n");
        *err = 1;
        return;
    }
    if (!V3D_mem_free(buffer->gpuHandle))
    {
        printf("free_gpu_buffer: Failed to free gpu buffer\n");
        *err = 1;
        return;
    }
    buffer->basePtr = 0;
}

void gl_create_context(VC4_ADDR fbAddr, uint32_t fbWidth, uint32_t fbHeight, uint32_t *err, GL *gl)
{

    // Should check if V3D is already initialized
    if (!V3D_init())
    {
        printf("gl_create_context: Failed to initialize V3D\n");
        *err = 1;
        return gl;
    }
    gl->renderBufferAddr = fbAddr;
    gl->fbHeight = fbHeight;
    gl->fbWidth = fbWidth;

    gl->vertexBuffer = alloc_gpu_buffer(4 * 1024, err);
    if (*err != 0)
    {
        printf("gl_create_context: Failed to allocate vertex buffer\n");
        return gl;
    }
    gl->indexBuffer = alloc_gpu_buffer(4 * 1024, err);
    if (*err != 0)
    {
        printf("gl_create_context: Failed to allocate index buffer\n");
        return gl;
    }
    gl->shaderTextBuffer = alloc_gpu_buffer(4 * 1024, err);
    if (*err != 0)
    {
        printf("gl_create_context: Failed to allocate shader text buffer\n");
        return gl;
    }
    gl->shaderRecord = alloc_gpu_buffer(4 * 1024, err);
    if (*err != 0)
    {
        printf("gl_create_context: Failed to allocate shader record\n");
        return gl;
    }
    gl->renderCL = alloc_gpu_buffer(4 * 1024, err);
    if (*err != 0)
    {
        printf("gl_create_context: Failed to allocate rendering control list\n");
        return gl;
    }
    gl->binningCL = alloc_gpu_buffer(4 * 1024, err);
    if (*err != 0)
    {
        printf("gl_create_context: Failed to allocate binning control list\n");
        return gl;
    }
    gl->binWidth = (gl->fbWidth + 63) / 64;
    gl->binHeight = (gl->fbHeight + 63) / 64;
    gl->tileCL = alloc_gpu_buffer(32 * 1024, err);
    if (*err != 0)
    {
        printf("gl_create_context: Failed to allocate tile control list\n");
        return gl;
    }
    gl->tileState = alloc_gpu_buffer(32 * 1024, err);
    if (*err != 0)
    {
        printf("gl_create_context: Failed to allocate tile state buffer\n");
        return gl;
    }

    return gl;
}

void gl_clear_color(GL *gl, float red, float green, float blue, float alpha)
{
    gl->clearColor = ((uint32_t)(255.9f * red)) << 24 |
                     ((uint32_t)(255.9f * green)) << 16 |
                     ((uint32_t)(255.9f * blue)) << 8 |
                     ((uint32_t)(255.9f * alpha));
}

void gl_clear(GL *gl, uint32_t mask)
{
    uint8_t *p = gl->renderCL.basePtr + gl->renderCL.size;
    uint8_t *q = p; // Remember start address to compute new size

    emit_uint8_t(&p, GL_CLEAR_COLORS);
    // emit_uint32_t(&p, gl->clearColor);
    // emit_uint32_t(&p, gl->clearColor);
    emit_uint32_t(&p, 0);
    emit_uint8_t(&p, 0);

    gl->renderCL.size += p - q;
}

void gl_viewport(GL *gl, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    // TODO
}

void gl_load_identity(GL *gl)
{
    gl->matrix[0 + 0 * 4] = 1.0f;
    gl->matrix[1 + 1 * 4] = 1.0f;
    gl->matrix[2 + 2 * 4] = 1.0f;
    gl->matrix[3 + 3 * 4] = 1.0f;
}

void gl_orthof(GL *gl, float left, float right, float bottom, float top, float zNear, float zFar)
{
    // TODO: multiply instead of replacing
    // float orthoMatrix[16] = {0};
    float *orthoMatrix = gl->matrix;
    orthoMatrix[0 + 0 * 4] = 2.0f / (right - left);
    orthoMatrix[1 + 1 * 4] = 2.0f / (top - bottom);
    orthoMatrix[2 + 2 * 4] = -2.0f / (zFar - zNear);
    orthoMatrix[0 + 3 * 4] = -((right + left) / (right - left));
    orthoMatrix[1 + 3 * 4] = -((top + bottom) / (top - bottom));
    orthoMatrix[2 + 3 * 4] = -((zFar + zNear) / (zFar - zNear));
    orthoMatrix[3 + 3 * 4] = 1.0f;
    // gl_mult_matrix(gl, orthoMatrix);
}

void gl_enable_client_state(GL *gl, uint32_t array)
{
    // TODO
}

void gl_disable_client_state(GL *gl, uint32_t array)
{
    // TODO
}

void gl_color4f(GL *gl, float red, float green, float blue, float alpha)
{
    gl->currentColor[0] = red;
    gl->currentColor[1] = green;
    gl->currentColor[2] = blue;
    gl->currentColor[3] = alpha;
}

void gl_vertex_pointer(GL *gl, uint32_t size, uint32_t type, int32_t stride, const void *ptr)
{
    gl->vertexArray.type = type;
    gl->vertexArray.size = size;
    gl->vertexArray.stride = stride;
    gl->vertexArray.ptr = ptr;
}

static void write_vertex_buffers(GL *gl, uint32_t count)
{
    uint8_t *p = gl->vertexBuffer.basePtr + gl->vertexBuffer.size;
    uint8_t *q = p; // Remember start address to compute new size

    // TODO: make this work with other sizes, strides and types
    for (uint32_t i = 0; i < count; ++i)
    {
        float xf = *((float *)(gl->vertexArray.ptr) + i * gl->vertexArray.size + 0);
        uint16_t x = (uint32_t)(xf * (float)gl->fbWidth) << 4;
        float yf = *((float *)(gl->vertexArray.ptr) + i * gl->vertexArray.size + 1);
        uint16_t y = (uint32_t)(yf * (float)gl->fbHeight) << 4;
        emit_uint16_t(&p, x);                // X in 12.4 fixed point
        emit_uint16_t(&p, y);                // Y in 12.4 fixed point
        emit_float(&p, 1.0f);                // Z
        emit_float(&p, 1.0f);                // 1/W
        emit_float(&p, gl->currentColor[0]); // Varying 0 (Red)
        emit_float(&p, gl->currentColor[1]); // Varying 1 (Green)
        emit_float(&p, gl->currentColor[2]); // Varying 2 (Blue)
    }

    gl->vertexBuffer.size += p - q;
}

static void write_shader_record(GL *gl)
{
    uint8_t *p = gl->shaderRecord.basePtr + gl->shaderRecord.size;
    uint8_t *q = p; // Remember start address to compute new size

    // Okay now we need Shader Record to buffer
    emit_uint8_t(&p, 0x01);                              // flags
    emit_uint8_t(&p, 6 * 4);                             // stride
    emit_uint8_t(&p, 0xcc);                              // num uniforms (not used)
    emit_uint8_t(&p, 3);                                 // num varyings
    emit_uint32_t(&p, gl->shaderTextBuffer.gpuBaseAddr); // Shader code address
    emit_uint32_t(&p, 0);                                // Fragment shader uniforms (not in use)
    emit_uint32_t(&p, gl->vertexBuffer.gpuBaseAddr);     // Vertex Data

    gl->shaderRecord.size += p - q;
}

static void load_NV_shader(GL *gl)
{
    uint8_t *p = gl->shaderTextBuffer.basePtr + gl->shaderTextBuffer.size;
    uint8_t *q = p; // Remember start address to compute new size
    uint32_t nvShader[18] = {
        0x958e0dbf, 0xd1724823, /* mov r0, vary; mov r3.8d, 1.0 */
        0x818e7176, 0x40024821, /* fadd r0, r0, r5; mov r1, vary */
        0x818e7376, 0x10024862, /* fadd r1, r1, r5; mov r2, vary */
        0x819e7540, 0x114248a3, /* fadd r2, r2, r5; mov r3.8a, r0 */
        0x809e7009, 0x115049e3, /* nop; mov r3.8b, r1 */
        0x809e7012, 0x116049e3, /* nop; mov r3.8c, r2 */
        0x159e76c0, 0x30020ba7, /* mov tlbc, r3; nop; thrend */
        0x009e7000, 0x100009e7, /* nop; nop; nop */
        0x009e7000, 0x500009e7, /* nop; nop; sbdone */
    };
    for (uint32_t i = 0; i < 18; ++i)
    {
        emit_uint32_t(&p, nvShader[i]);
    }
    gl->shaderTextBuffer.size += p - q;

    write_shader_record(gl);
}

static void finish_render_cl(GL *gl)
{

    uint8_t *p = gl->renderCL.basePtr + gl->renderCL.size;
    uint8_t *q = p; // Remember start address to compute new size

    emit_uint8_t(&p, GL_TILE_RENDER_CONFIG);
    emit_uint32_t(&p, gl->renderBufferAddr);

    emit_uint16_t(&p, gl->fbWidth);  // render width
    emit_uint16_t(&p, gl->fbHeight); // render height
    emit_uint8_t(&p, 0x04);          // framebuffer mode (linear rgba8888)
    emit_uint8_t(&p, 0x00);

    // Do a store of the first tile to force the tile buffer to be cleared
    // Tile Coordinates
    emit_uint8_t(&p, GL_TILE_COORDINATES);
    emit_uint8_t(&p, 0);
    emit_uint8_t(&p, 0);

    // Store Tile Buffer General
    emit_uint8_t(&p, GL_STORE_TILE_BUFFER);
    emit_uint16_t(&p, 0); // Store nothing (just clear)
    emit_uint32_t(&p, 0); // no address is needed

    // Link all binned lists together
    for (int x = 0; x < gl->binWidth; x++)
    {
        for (int y = 0; y < gl->binHeight; y++)
        {

            // Tile Coordinates
            emit_uint8_t(&p, GL_TILE_COORDINATES);
            emit_uint8_t(&p, x);
            emit_uint8_t(&p, y);

            // Call Tile sublist
            emit_uint8_t(&p, GL_BRANCH_TO_SUBLIST);
            emit_uint32_t(&p, gl->tileCL.gpuBaseAddr + (y * gl->binWidth + x) * 32);

            // Last tile needs a special store instruction
            if (x == (gl->binWidth - 1) && (y == gl->binWidth - 1))
            {
                // Store resolved tile color buffer and signal end of frame
                emit_uint8_t(&p, GL_STORE_MULTISAMPLE_END);
            }
            else
            {
                // Store resolved tile color buffer
                emit_uint8_t(&p, GL_STORE_MULTISAMPLE);
            }
        }
    }

    gl->renderCL.size += p - q;
}

static void write_binnig_config(GL *gl)
{

    uint8_t *p = gl->binningCL.basePtr + gl->binningCL.size;
    uint8_t *q = p; // Remember start address to compute new size

    emit_uint8_t(&p, GL_TILE_BINNING_CONFIG);     // tile binning config control
    emit_uint32_t(&p, gl->tileCL.gpuBaseAddr);    // tile allocation memory address
    emit_uint32_t(&p, gl->tileCL.maxSize);        // tile allocation memory size
    emit_uint32_t(&p, gl->tileState.gpuBaseAddr); // Tile state data address
    emit_uint8_t(&p, gl->binWidth);               // renderWidth/64
    emit_uint8_t(&p, gl->binHeight);              // renderHt/64
    emit_uint8_t(&p, 0x04);                       // config

    // Start tile binning.
    emit_uint8_t(&p, GL_START_TILE_BINNING); // Start binning command

    // Primitive type
    emit_uint8_t(&p, GL_PRIMITIVE_LIST_FORMAT);
    emit_uint8_t(&p, 0x32); // 16 bit triangle

    // Clip Window
    emit_uint8_t(&p, GL_CLIP_WINDOW); // Clip window
    emit_uint16_t(&p, 0);             // 0
    emit_uint16_t(&p, 0);             // 0
    emit_uint16_t(&p, gl->fbWidth);   // width
    emit_uint16_t(&p, gl->fbWidth);   // height

    // GL State
    emit_uint8_t(&p, GL_CONFIG_STATE);
    emit_uint8_t(&p, 0x03); // enable both foward and back facing polygons
    emit_uint8_t(&p, 0x00); // depth testing disabled
    emit_uint8_t(&p, 0x02); // enable early depth write

    // Viewport offset
    emit_uint8_t(&p, GL_VIEWPORT_OFFSET); // Viewport offset
    emit_uint16_t(&p, 0);                 // 0
    emit_uint16_t(&p, 0);                 // 0

    // The triangle
    // No Vertex Shader state (takes pre-transformed vertexes so we don't have to supply a working coordinate shader.)
    emit_uint8_t(&p, GL_NV_SHADER_STATE);
    emit_uint32_t(&p, gl->shaderRecord.gpuBaseAddr); // Shader Record

    // primitive index list
    emit_uint8_t(&p, GL_INDEXED_PRIMITIVE_LIST);    // Indexed primitive list command
    emit_uint8_t(&p, PRIM_TRIANGLE);                // 8bit index, triangles
    emit_uint32_t(&p, gl->indexBuffer.size);        // Number of index vertex
    emit_uint32_t(&p, gl->indexBuffer.gpuBaseAddr); // Address of index vertex data
    emit_uint32_t(&p, gl->maxIndex);                // Maximum index

    // End of bin list
    // So Flush
    emit_uint8_t(&p, GL_FLUSH_ALL_STATE);
    // Nop
    emit_uint8_t(&p, GL_NOP);
    // Halt
    emit_uint8_t(&p, GL_HALT);

    gl->binningCL.size += p - q;
}

void gl_draw_elements(GL *gl, uint32_t mode, int32_t count, uint32_t type, const void *indices)
{
    uint8_t *p = gl->indexBuffer.basePtr + gl->indexBuffer.size;
    uint8_t *q = p; // Remember start address to compute new size

    for (uint32_t i = 0; i < count; ++i)
    {
        uint8_t index = ((uint8_t *)indices)[i];
        if (index > gl->maxIndex)
        {
            gl->maxIndex = index;
        }
        emit_uint8_t(&p, index);
    }

    gl->indexBuffer.size += p - q;
}

void gl_flush(GL *gl)
{
    finish_render_cl(gl);
    printf("finish_render_cl\n");
    load_NV_shader(gl);
    printf("load_NV_shader\n");
    write_vertex_buffers(gl, gl->maxIndex + 1);
    printf("write_vertex_buffers\n");
    write_binnig_config(gl);
    printf("write_binnig_config\n");

    // clear caches
    v3d[V3D_L2CACTL] = 4;
    v3d[V3D_SLCACTL] = 0x0F0F0F0F;

    // stop the thread
    v3d[V3D_CT0CS] = 0x20;
    // wait for it to stop
    while (v3d[V3D_CT0CS] & 0x20)
        ;

    printf("before starting binning\n");
    // Run our control list
    v3d[V3D_BFC] = 1;                                                // reset binning frame count
    v3d[V3D_CT0CA] = gl->binningCL.gpuBaseAddr;                      // Start binning config address
    v3d[V3D_CT0EA] = gl->binningCL.gpuBaseAddr + gl->binningCL.size; // End binning config address is at render control start
    printf("after starting binning\n");

    // wait for binning to finish
    while (v3d[V3D_BFC] == 0)
        ;
    printf("binning finished\n");

    // stop the thread
    v3d[V3D_CT1CS] = 0x20;

    // Wait for thread to stop
    while (v3d[V3D_CT1CS] & 0x20)
        ;

    // Run our render
    v3d[V3D_RFC] = 1;                                              // reset rendering frame count
    v3d[V3D_CT1CA] = gl->renderCL.gpuBaseAddr;                     // Start address for render control
    v3d[V3D_CT1EA] = gl->renderCL.gpuBaseAddr + gl->renderCL.size; // End address for render control

    // wait for render to finish
    while (v3d[V3D_RFC] == 0)
        ;
}