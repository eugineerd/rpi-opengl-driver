#include "v3d.h"
#include "rpi-SmartStart.h" // Need for mailbox

/*==========================================================================}
{						  PUBLIC GPU MEMORY FUNCTIONS						}
{==========================================================================*/

/*-[ V3D_mem_alloc ]--------------------------------------------------------}
. Allocates contiguous memory on the GPU with size and alignment in bytes
. and with the properties of the flags.
. RETURN : GPU handle on successs, 0 if allocation fails
.--------------------------------------------------------------------------*/

GPU_HANDLE V3D_mem_alloc(uint32_t size, uint32_t align, V3D_MEMALLOC_FLAGS flags)
{
    uint32_t buffer[6];
    if (mailbox_tag_message(&buffer[0], 6,
                            MAILBOX_TAG_ALLOCATE_MEMORY, 12, 12, size, align, flags)) // Allocate memory tag
    {                                                                                 // Message was successful
        return buffer[3];                                                             // GPU handle returned
    }
    return 0; // Memory allocate failed
}

/*-[ V3D_mem_free ]---------------------------------------------------------}
. All memory associated with the GPU handle is released.
. RETURN : TRUE on successs, FALSE if release fails
.--------------------------------------------------------------------------*/
bool V3D_mem_free(GPU_HANDLE handle)
{
    uint32_t buffer[4] = {0};
    if (mailbox_tag_message(0, 4,
                            MAILBOX_TAG_RELEASE_MEMORY, 4, 4, handle)) // Release memory tag
    {                                                                  // Release was successful
        if (buffer[3] == 0)
            return true; // Return true
    }
    return false; // Return false
}

/*-[ V3D_mem_lock ]---------------------------------------------------------}
. Locks the memory associated to the GPU handle to a GPU bus address.
. RETURN : locked gpu address, 0 if lock fails
.--------------------------------------------------------------------------*/
uint32_t V3D_mem_lock(GPU_HANDLE handle)
{
    uint32_t buffer[4] = {0};
    if (mailbox_tag_message(&buffer[0], 4,
                            MAILBOX_TAG_LOCK_MEMORY, 4, 4, handle)) // Lock memory tag
    {                                                               // message was successful
        return buffer[3];                                           // Return the bus address
    }
    return 0; // Return failure
}

/*-[ V3D_mem_unlock ]-------------------------------------------------------}
. Unlocks the memory associated to the GPU handle from the GPU bus address.
. RETURN : TRUE if sucessful, FALSE if it fails
.--------------------------------------------------------------------------*/
bool V3D_mem_unlock(GPU_HANDLE handle)
{
    uint32_t buffer[4] = {0};
    if (mailbox_tag_message(0, 4,
                            MAILBOX_TAG_UNLOCK_MEMORY, 4, 4, handle)) // Memory unlock tag
    {                                                                 // Message was successful
        if (buffer[3] == 0)
            return true; // Return true
    }
    return false; // Return false
}

bool V3D_init()
{
    if (mailbox_tag_message(0, 9,
                            MAILBOX_TAG_SET_CLOCK_RATE, 8, 8, CLK_V3D_ID, 250000000, // Set V3D clock to 250Mhz
                            MAILBOX_TAG_ENABLE_QPU, 4, 4, 1))                        // Enable the QPU untis
    {                                                                                // Message was successful
        if (v3d[V3D_IDENT0] == 0x02443356)
        {
            return true;
        }
        printf("V3D_init: QPU init failed\n");
    }
    printf("V3D_init: send tag message failed\n");
    return false;
}