/*==============================================================================
FMOD Example Framework
Copyright (c), Firelight Technologies Pty, Ltd 2012-2024.
==============================================================================*/
#include "common.h"
#include <vector>
#include <libsysmodule.h>
#include <video_out.h>
#include <sce_font.h>
#include <pad.h>
#include <agc.h>

unsigned int sceLibcHeapExtendedAlloc = 1;
size_t sceLibcHeapSize = SCE_LIBC_HEAP_SIZE_EXTENDED_ALLOC_NO_LIMIT;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int SURFACE_MEMORY_ALIGN = 16 * 1024;
const int SURFACE_MEMORY_SIZE = ((SCREEN_WIDTH * SCREEN_HEIGHT) + (SURFACE_MEMORY_ALIGN - 1)) & ~(SURFACE_MEMORY_ALIGN - 1);
const int FRAME_BUFFER_MEMORY_ALIGN = 64 * 1024;
const int FRAME_BUFFER_MEMORY_SIZE = 16 * 1024 * 1024;
const int GRAPHICS_MEMORY_SIZE = FRAME_BUFFER_MEMORY_SIZE * 2;
const float FONT_CHAR_WIDTH = 16.0f;
const float FONT_CHAR_HEIGHT = 16.0f;

int32_t gPadHandle;
int32_t gVideoOutHandle;
off_t gGraphicsMemOffset;
off_t gSurfaceMemOffset;
void *gFrameBuffer[2];
void *gSurfaceBuffer;
uint64_t gUntiledSizeForSurface;
sce::AgcGpuAddress::SurfaceSummary gSurfaceSummary;
SceKernelEqueue gFlipEventQue;
SceFontMemory gFontMemory;
SceFontLibrary gFontLib;
SceFontHandle gFontHandle;
SceFontRenderer gFontRenderer;
float gDrawIncY;
float gDrawOffsetY;
SceFontRenderSurface gRenderSurface;
uint32_t gPressedButtons;
uint32_t gDownButtons;
std::vector<char *> gStringList;
SceKernelModule gCorePRX;
SceKernelModule gStudioPRX;

static void *fontMalloc(void*, uint32_t size) { return malloc(size); }
static void *fontCalloc(void*, uint32_t n, uint32_t size) { return calloc(n, size); }
static void *fontRealloc(void*, void *p, uint32_t size) { return realloc(p, size); }
static void fontFree(void*, void *p) { free(p); }

static const SceFontMemoryInterface* getFontMemoryInterface()
{
    static SceFontMemoryInterface memInterface = {};
    memInterface.Malloc = fontMalloc;
    memInterface.Free = fontFree;
    memInterface.Realloc = fontRealloc;
    memInterface.Calloc = fontCalloc;
    return &memInterface;
}

static unsigned int convertButtonMask(uint32_t btn)
{
    unsigned int mask = 0;
    if (btn & SCE_PAD_BUTTON_CROSS)                     mask |= (1 << BTN_ACTION1);
    if (btn & SCE_PAD_BUTTON_CIRCLE)                    mask |= (1 << BTN_ACTION2);
    if (btn & SCE_PAD_BUTTON_SQUARE)                    mask |= (1 << BTN_ACTION3);
    if (btn & SCE_PAD_BUTTON_TRIANGLE)                  mask |= (1 << BTN_ACTION4);
    if (btn & SCE_PAD_BUTTON_LEFT)                      mask |= (1 << BTN_LEFT);
    if (btn & SCE_PAD_BUTTON_RIGHT)                     mask |= (1 << BTN_RIGHT);
    if (btn & SCE_PAD_BUTTON_UP)                        mask |= (1 << BTN_UP);
    if (btn & SCE_PAD_BUTTON_DOWN)                      mask |= (1 << BTN_DOWN);
    if (btn & SCE_PAD_BUTTON_TOUCH_PAD)                 mask |= (1 << BTN_MORE);
    if (btn & (SCE_PAD_BUTTON_L1 | SCE_PAD_BUTTON_R1))  mask |= (1 << BTN_QUIT);
    return mask;
}

void Common_Init(void **extraDriverData)
{
    /* Init FMOD PRXs */
    gCorePRX = sceKernelLoadStartModule(FMOD_CORE_PRX, 0, nullptr, 0, nullptr, nullptr);
    assert(gCorePRX >= SCE_OK);

#ifdef FMOD_STUDIO_PRX
    gStudioPRX = sceKernelLoadStartModule(FMOD_STUDIO_PRX, 0, nullptr, 0, nullptr, nullptr);
    assert(gStudioPRX >= SCE_OK);
#endif

    /* Init Pad */
    int ret = sceUserServiceInitialize(nullptr);
    assert(ret == SCE_OK);

    SceUserServiceUserId userID;
    ret = sceUserServiceGetInitialUser(&userID);
    assert(ret == SCE_OK);

    ret = scePadInit();
    assert(ret == SCE_OK);

    gPadHandle = scePadOpen(userID, SCE_PAD_PORT_TYPE_STANDARD, 0, nullptr);
    assert(gPadHandle >= 0);

    /* Init VideoOut */
    gVideoOutHandle = sceVideoOutOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, nullptr);
    assert(gVideoOutHandle >= 0);

    ret = sce::Agc::init();
    assert(ret == SCE_OK);

    sce::AgcGpuAddress::SurfaceDescription surfaceDesc = {};
    surfaceDesc.m_tileMode = sce::AgcGpuAddress::TileMode::kRenderTarget;
    surfaceDesc.m_dimension = sce::AgcGpuAddress::SurfaceDimension::k2d;
    surfaceDesc.m_bytesPerElementLog2 = 2;
    surfaceDesc.m_multiElementMultiplier = 1;
    surfaceDesc.m_texelsPerElementWide = 1;
    surfaceDesc.m_texelsPerElementTall = 1;
    surfaceDesc.m_width = SCREEN_WIDTH;
    surfaceDesc.m_height = SCREEN_HEIGHT;
    surfaceDesc.m_depth = 1;
    surfaceDesc.m_numFragmentsLog2 = 0;
    surfaceDesc.m_numMips = 1;
    surfaceDesc.m_numSlices = 1;
    surfaceDesc.m_pipeAligned = true;

    ret = sce::AgcGpuAddress::computeSurfaceSummary(&gSurfaceSummary, &surfaceDesc);
    assert(ret == SCE_OK);

    ret = sce::AgcGpuAddress::computeUntiledSizeForSurface(&gUntiledSizeForSurface, &gSurfaceSummary, 0);
    assert(ret == SCE_OK);

    assert(FRAME_BUFFER_MEMORY_SIZE > gUntiledSizeForSurface);
    assert(FRAME_BUFFER_MEMORY_SIZE > gSurfaceSummary.m_totalSizeInBytes);
    assert(FRAME_BUFFER_MEMORY_ALIGN == gSurfaceSummary.m_baseAlign);

    ret = sceKernelAllocateDirectMemory(0, SCE_KERNEL_MAIN_DMEM_SIZE, GRAPHICS_MEMORY_SIZE, FRAME_BUFFER_MEMORY_ALIGN, SCE_KERNEL_MTYPE_C_SHARED, &gGraphicsMemOffset);
    assert(ret == SCE_OK);

    void *graphicsMemory = nullptr;
    ret = sceKernelMapDirectMemory(&graphicsMemory, GRAPHICS_MEMORY_SIZE, SCE_KERNEL_PROT_CPU_READ | SCE_KERNEL_PROT_CPU_WRITE | SCE_KERNEL_PROT_GPU_ALL, 0, gGraphicsMemOffset, FRAME_BUFFER_MEMORY_ALIGN);
    assert(ret == SCE_OK);

    memset(graphicsMemory, 0, GRAPHICS_MEMORY_SIZE);
    gFrameBuffer[0] = graphicsMemory;
    gFrameBuffer[1] = (uint8_t *)graphicsMemory + FRAME_BUFFER_MEMORY_SIZE;

    SceVideoOutBuffers videoBuffers[2] = { };
    videoBuffers[0].data = gFrameBuffer[0];
    videoBuffers[1].data = gFrameBuffer[1];

    SceVideoOutBufferAttribute2 videoOutBufferAttribute;
    sceVideoOutSetBufferAttribute2(&videoOutBufferAttribute, SCE_VIDEO_OUT_PIXEL_FORMAT2_B8_G8_R8_A8_SRGB, SCE_VIDEO_OUT_TILING_MODE_TILE, SCREEN_WIDTH, SCREEN_HEIGHT, SCE_VIDEO_OUT_BUFFER_ATTRIBUTE_OPTION_NONE, 0, 0UL);

    ret = sceVideoOutRegisterBuffers2(gVideoOutHandle, 0, 0, videoBuffers, 2, &videoOutBufferAttribute, SCE_VIDEO_OUT_BUFFER_ATTRIBUTE_CATEGORY_UNCOMPRESSED, nullptr);
    assert(ret == SCE_OK);
    
    ret = sceKernelCreateEqueue(&gFlipEventQue, "flipEventQue");
    assert(ret == SCE_OK);

    ret = sceVideoOutAddFlipEvent(gFlipEventQue, gVideoOutHandle, nullptr);
    assert(ret == SCE_OK);

    /* Init Surface */
    ret = sceKernelAllocateDirectMemory(0, SCE_KERNEL_MAIN_DMEM_SIZE, SURFACE_MEMORY_SIZE, SURFACE_MEMORY_ALIGN, SCE_KERNEL_MTYPE_C_SHARED, &gSurfaceMemOffset);
    assert(ret == SCE_OK);

    ret = sceKernelMapDirectMemory(&gSurfaceBuffer, SURFACE_MEMORY_SIZE, SCE_KERNEL_PROT_CPU_READ | SCE_KERNEL_PROT_CPU_WRITE | SCE_KERNEL_PROT_GPU_ALL, 0, gSurfaceMemOffset, SURFACE_MEMORY_ALIGN);
    assert(ret == SCE_OK);

    memset(gSurfaceBuffer, 0, SURFACE_MEMORY_SIZE);

    /* Init Font */
    ret = sceSysmoduleLoadModule(SCE_SYSMODULE_FONT);
    assert(ret == SCE_OK);

    ret = sceSysmoduleLoadModule(SCE_SYSMODULE_FONT_FT);
    assert(ret == SCE_OK);

    ret = sceSysmoduleLoadModule(SCE_SYSMODULE_FREETYPE_OT);
    assert(ret == SCE_OK);

    ret = sceFontMemoryInit(&gFontMemory, nullptr, 0, getFontMemoryInterface(), nullptr, nullptr, nullptr);
    assert(ret == SCE_FONT_OK);

    ret = sceFontCreateLibrary(&gFontMemory, sceFontSelectLibraryFt(0), &gFontLib);
    assert(ret == SCE_FONT_OK);

    ret = sceFontSupportSystemFonts(gFontLib);
    assert(ret == SCE_FONT_OK);

    ret = sceFontAttachDeviceCacheBuffer(gFontLib, nullptr, 1 * 1024 * 1024);
    assert(ret == SCE_FONT_OK);

    ret = sceFontCreateRenderer(&gFontMemory, sceFontSelectRendererFt(0), &gFontRenderer);
    assert(ret == SCE_FONT_OK);

    gFontHandle = SCE_FONT_HANDLE_INVALID;
    ret = sceFontOpenFontSet(gFontLib, SCE_FONT_SET_SST_TYPEWRITER_EUROPEAN_W1G, SCE_FONT_OPEN_FILE_STREAM, nullptr, &gFontHandle);
    assert(ret == SCE_FONT_OK);
    
    ret = sceFontSetScalePixel(gFontHandle, FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT);
    assert(ret == SCE_FONT_OK);

    ret = sceFontBindRenderer(gFontHandle, gFontRenderer);
    assert(ret == SCE_FONT_OK);

    ret = sceFontSetupRenderScalePixel(gFontHandle, FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT);
    assert(ret == SCE_FONT_OK);

    sceFontRenderSurfaceInit(&gRenderSurface, gSurfaceBuffer, SCREEN_WIDTH, 1, SCREEN_WIDTH, SCREEN_HEIGHT);

    SceFontHorizontalLayout layout;
    ret = sceFontGetHorizontalLayout(gFontHandle, &layout);
    assert(ret == SCE_FONT_OK);

    gDrawIncY = layout.lineHeight;
    gDrawOffsetY = 0.0f;
}

void Common_Close()
{
    /* Cleanup string */
    for (auto item = gStringList.begin(); item != gStringList.end(); ++item)
    {
        free(*item);
    }

    /* Cleanup Font */
    int ret = sceFontUnbindRenderer(gFontHandle);
    assert(ret == SCE_FONT_OK);

    ret = sceFontCloseFont(gFontHandle);
    assert(ret == SCE_FONT_OK);

    ret = sceFontDestroyRenderer(&gFontRenderer);
    assert(ret == SCE_FONT_OK);

    ret = sceFontDestroyLibrary(&gFontLib);
    assert(ret == SCE_FONT_OK);

    ret = sceFontMemoryTerm(&gFontMemory);
    assert(ret == SCE_FONT_OK);

    ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_FONT);
    assert(ret == SCE_OK);

    ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_FONT_FT);
    assert(ret == SCE_OK);

    ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_FREETYPE_OT);
    assert(ret == SCE_OK);

    /* Cleanup Surface */
    ret = sceKernelReleaseDirectMemory(gSurfaceMemOffset, SURFACE_MEMORY_SIZE);
    assert(ret == SCE_OK);

    /* Cleanup VideoOut */
    ret = sceVideoOutSubmitFlip(gVideoOutHandle, SCE_VIDEO_OUT_BUFFER_INDEX_BLANK, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, -1);
    assert(ret == SCE_OK);

    int arrivedEventCount;
    SceKernelEvent arrivedEvent;
    ret = sceKernelWaitEqueue(gFlipEventQue, &arrivedEvent, 1, &arrivedEventCount, nullptr);
    assert(ret == SCE_OK);

    ret = sceVideoOutDeleteFlipEvent(gFlipEventQue, gVideoOutHandle);
    assert(ret == SCE_OK);

    ret = sceKernelDeleteEqueue(gFlipEventQue);
    assert(ret == SCE_OK);

    ret = sceKernelReleaseDirectMemory(gGraphicsMemOffset, GRAPHICS_MEMORY_SIZE);
    assert(ret == SCE_OK);

    ret = sceVideoOutClose(gVideoOutHandle);
    assert(ret == SCE_OK);

    /* Cleanup FMOD PRXs */
    int err = sceKernelStopUnloadModule(gCorePRX, 0, nullptr, 0, nullptr, nullptr);
    assert(err == 0);

#ifdef FMOD_STUDIO_PRX
    err = sceKernelStopUnloadModule(gStudioPRX, 0, nullptr, 0, nullptr, nullptr);
    assert(err == 0);
#endif
}

void Common_Update()
{
    /* Update Pad */
    ScePadData data = {0};
    int ret = scePadReadState(gPadHandle, &data);
    assert(ret == SCE_OK);

    unsigned int newButtons = convertButtonMask(data.buttons);
    gPressedButtons = newButtons & ~gDownButtons;
    gDownButtons = newButtons;

    /* Update Surface */
    gDrawOffsetY = 0.0f;

    // Convert source (0-255) to destination (0RGB) ~ gray scale
    const uint8_t *src = (const uint8_t *)gSurfaceBuffer;
    uint32_t *dst = (uint32_t *)gFrameBuffer[1];
    for (int i = 0; i < (SCREEN_WIDTH * SCREEN_HEIGHT); i++)
    {
        dst[i] = (src[i] << 16) | (src[i] << 8) | (src[i] << 0);
    }
    memset(gSurfaceBuffer, 0, SURFACE_MEMORY_SIZE);

    /* Update VideoOut */
    ret = sce::AgcGpuAddress::tileSurface(gFrameBuffer[0], gSurfaceSummary.m_totalSizeInBytes, gFrameBuffer[1], gUntiledSizeForSurface, &gSurfaceSummary, 0, 0);
    assert(ret == SCE_OK);

    ret = sce::Agc::suspendPoint();
    assert(ret == SCE_OK);
    
    ret = sceVideoOutSubmitFlip(gVideoOutHandle, 0, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0);
    assert(ret == SCE_OK);

    int arrivedEventCount;
    SceKernelEvent arrivedEvent;
    ret = sceKernelWaitEqueue(gFlipEventQue, &arrivedEvent, 1, &arrivedEventCount, nullptr);
    assert(ret == SCE_OK);
}

void Common_Exit(int returnCode)
{
    exit(returnCode);
}

void Common_DrawText(const char *text)
{
    float drawOffsetX = 0.0f;

    for (const char *textPtr = text; *textPtr; textPtr++)
    {
        if (gDrawOffsetY < SCREEN_HEIGHT)
        {
            SceFontGlyphMetrics metrics;
            SceFontRenderResult result;
            int ret = sceFontRenderCharGlyphImage(gFontHandle, *textPtr, &gRenderSurface, drawOffsetX, gDrawOffsetY, &metrics, &result);
            assert(ret == SCE_FONT_OK);

            drawOffsetX += metrics.Horizontal.advance;
        }
    }

    gDrawOffsetY += gDrawIncY;
}

bool Common_BtnPress(Common_Button btn)
{
    return ((gPressedButtons & (1 << btn)) != 0);
}

bool Common_BtnDown(Common_Button btn)
{
    return ((gDownButtons & (1 << btn)) != 0);
}

const char *Common_BtnStr(Common_Button btn)
{
    switch (btn)
    {
        case BTN_ACTION1:   return "Cross";
        case BTN_ACTION2:   return "Circle";
        case BTN_ACTION3:   return "Square";
        case BTN_ACTION4:   return "Triangle";
        case BTN_LEFT:      return "Left";
        case BTN_RIGHT:     return "Right";
        case BTN_UP:        return "Up";
        case BTN_DOWN:      return "Down";
        case BTN_MORE:      return "Pad";
        case BTN_QUIT:      return "Both Shoulders";
        default:            return "Unknown";
    }
}

const char *Common_MediaPath(const char *fileName)
{
    char *filePath = (char *)calloc(256, sizeof(char));

    strcat(filePath, "/app0/");
    strcat(filePath, fileName);
    gStringList.push_back(filePath);

    return filePath;
}

const char *Common_WritePath(const char *fileName)
{
    char *filePath = (char *)calloc(256, sizeof(char));

    strcat(filePath, "/host/");
    strcat(filePath, fileName);
    gStringList.push_back(filePath);

    return filePath;
}