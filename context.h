#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H
#include <stdint.h>
#include "v3d.h"

typedef struct __attribute__((packed)) vertex_array_t
{
    uint32_t size;
    uint32_t stride;
    uint32_t type;
    const void *ptr;
} VERTEX_ARRAY;

// Allocated buffer on GPU
typedef struct __attribute__((packed)) gpu_buffer_t
{
    GPU_HANDLE gpuHandle;
    VC4_ADDR gpuBaseAddr;

    uint8_t *basePtr;
    uint32_t size;
    uint32_t maxSize;
} GPU_BUFFER;

// GL context
typedef struct __attribute__((packed)) gl_t
{
    // Framebuffer
    VC4_ADDR renderBufferAddr;
    uint16_t fbWidth;
    uint16_t fbHeight;

    // GPU buffers
    GPU_BUFFER indexBuffer;
    uint32_t maxIndex;

    GPU_BUFFER vertexBuffer;
    GPU_BUFFER shaderRecord;
    GPU_BUFFER shaderTextBuffer;
    GPU_BUFFER renderCL;
    GPU_BUFFER binningCL;
    GPU_BUFFER tileCL;
    GPU_BUFFER tileState;
    uint32_t binWidth;
    uint32_t binHeight;

    // OpenGL state
    uint32_t clearColor;
    float currentColor[4];
    float matrix[16]; // Column-major

    VERTEX_ARRAY vertexArray;
} GL;
void gl_create_context(VC4_ADDR fbAddr, uint32_t fbWidth, uint32_t fbHeight, uint32_t *err, GL *gl);

void gl_clear_color(GL *gl, float red, float green, float blue, float alpha);
void gl_clear(GL *gl, uint32_t mask);
void gl_viewport(GL *gl, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void gl_load_identity(GL *gl);
void gl_orthof(GL *gl, float left, float right, float bottom, float top, float zNear, float zFar);
void gl_enable_client_state(GL *gl, uint32_t array);
void gl_color4f(GL *gl, float red, float green, float blue, float alpha);
void gl_vertex_pointer(GL *gl, uint32_t size, uint32_t type, int32_t stride, const void *ptr);
void gl_draw_elements(GL *gl, uint32_t mode, int32_t count, uint32_t type, const void *indices);
void gl_disable_client_state(GL *gl, uint32_t array);
void gl_flush(GL *gl);

#endif