#include "context.h"
#include "rpi-SmartStart.h"
#include "emb-stdio.h"

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_VERTEX_ARRAY 0x8074
#define GL_FLOAT 0x1406
#define GL_TRIANGLES 0x0004
#define GL_UNSIGNED_BYTE 0x1401

GL gl = {0};

void run_example()
{
    Init_EmbStdio(Embedded_Console_WriteChar); // Initialize embedded stdio
    PiConsole_Init(640, 480, 32, printf);      // Auto resolution console, message to screen
    displaySmartStart(printf);                 // Display smart start details
    ARM_setmaxspeed(printf);                   // ARM CPU to max speed no message to screen

    printf("Test Before\n");
    printf("Test After\n");
    uint32_t err = 0;
    uint32_t width = GetConsole_Width();
    uint32_t height = GetConsole_Height();
    VC4_ADDR fbAddr = GetConsole_FrameBuffer();
    gl_create_context(fbAddr, width, height, &err, &gl);
    if (err != 0)
    {
        printf("gl_create_context failed!");
        return;
    }

    float x1 = 50.0f;
    float y1 = 50.0f;
    uint32_t rsize = 50;
    uint8_t indices[] = {0, 3, 1, 1, 3, 2};
    float vertices[] = {x1, y1, 0.0f, x1, y1 + rsize, 0.0f, x1 + rsize, y1 + rsize, 0.0f, x1 + rsize, y1, 0.0f};
    gl_viewport(&gl, 0, 0, GetConsole_Width(), GetConsole_Height());
    printf("gl_viewport completed\n");
    gl_load_identity(&gl);
    printf("load_identity completed\n");
    gl_orthof(&gl, 0.0f, 250.0f * width / height, 0.0f, 250.0f, 1.0f, -1.0f);
    printf("gl_orthof completed\n");

    gl_clear_color(&gl, 1.0f, 0.0f, 0.0f, 1.0f);
    printf("gl_clear_color completed\n");
    gl_clear(&gl, GL_COLOR_BUFFER_BIT);
    printf("gl_clear completed\n");
    gl_color4f(&gl, 0.0f, 0.0f, 1.0f, 0.0f);
    printf("gl_color4f completed\n");

    gl_enable_client_state(&gl, GL_VERTEX_ARRAY);
    printf("gl_enable_client_state completed\n");

    gl_vertex_pointer(&gl, 3, GL_FLOAT, 0, vertices);
    printf("gl_vertex_pointer completed\n");

    gl_draw_elements(&gl, GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
    printf("gl_draw_elements completed\n");
    gl_disable_client_state(&gl, GL_VERTEX_ARRAY);
    printf("gl_disable_client_state completed\n");

    gl_flush(&gl);
    printf("gl_flush completed\n");
}

int main()
{
    run_example();

    while (1)
    {
        set_Activity_LED(1); // Turn LED on
        timer_wait(500000);  // 0.5 sec delay
        set_Activity_LED(0); // Turn Led off
        timer_wait(500000);  // 0.5 sec delay
    }

    return 0;
}