#include "ElfUtils.h"
#include "ios_exploit.h"
#include "utils/logger.h"
#include <coreinit/cache.h>
#include <coreinit/foreground.h>
#include <coreinit/memdefaultheap.h>
#include <coreinit/screen.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <cstdio>
#include <cstring>
#include <nn/act/client_cpp.h>
#include <proc_ui/procui.h>
#include <string>
#include <sysapp/launch.h>
#include <sysapp/title.h>
#include <vpad/input.h>
#include <whb/log.h>
#include <whb/log_udp.h>

#include "gx2sploit.h"

void SplashScreen(int32_t durationInMs);

bool CheckRunning() {

    switch (ProcUIProcessMessages(true)) {
        case PROCUI_STATUS_EXITING: {
            return false;
        }
        case PROCUI_STATUS_RELEASE_FOREGROUND: {
            ProcUIDrawDoneRelease();
            break;
        }
        case PROCUI_STATUS_IN_FOREGROUND: {
            break;
        }
        case PROCUI_STATUS_IN_BACKGROUND:
        default:
            break;
    }
    return true;
}

extern "C" void __init_wut_malloc();

// Override __preinit_user to use the Cafe OS heap
void __preinit_user(MEMHeapHandle *outMem1,
                    MEMHeapHandle *outFG,
                    MEMHeapHandle *outMem2) {
    __init_wut_malloc();
}

int main(int argc, char **argv) {
    WHBLogUdpInit();

    DEBUG_FUNCTION_LINE("Welcome..");

    WHBLogPrintf("Press A to load your content.\n");

    VPADReadError err;
    VPADStatus vpad_data;
    VPADRead(VPAD_CHAN_0, &vpad_data, 1, &err);

    uint32_t btn = 0;
    if (err == VPAD_READ_SUCCESS) {
        btn = vpad_data.hold | vpad_data.trigger;
    }

    if ((btn & VPAD_BUTTON_A) == VPAD_BUTTON_A) {
        WHBLogPrintf("test");
    }
    if ((btn & VPAD_BUTTON_HOME) == VPAD_BUTTON_HOME) {
        SYSLaunchMenu();
    }

    while (CheckRunning()) {
        // wait.
        OSSleepTicks(OSMillisecondsToTicks(100));
    }
    ProcUIShutdown();

    DEBUG_FUNCTION_LINE("Exiting..");
    WHBLogUdpDeinit();

    return 0;
}

void SplashScreen(int32_t durationInMs) {
    uint32_t screen_buf0_size;

    // Init screen and screen buffers
    OSScreenInit();
    screen_buf0_size = OSScreenGetBufferSizeEx(SCREEN_TV);
    OSScreenSetBufferEx(SCREEN_TV, (void *) 0xF4000000);
    OSScreenSetBufferEx(SCREEN_DRC, (void *) (0xF4000000 + screen_buf0_size));

    OSScreenEnableEx(SCREEN_TV, 1);
    OSScreenEnableEx(SCREEN_DRC, 1);

    // Clear screens
    OSScreenClearBufferEx(SCREEN_TV, 0);
    OSScreenClearBufferEx(SCREEN_DRC, 0);

    std::string message1 = "What?";
    std::string message2 = "Sorry again..";

    OSScreenPutFontEx(SCREEN_TV, 0, 0, message1.c_str());
    OSScreenPutFontEx(SCREEN_DRC, 0, 0, message1.c_str());

    OSScreenPutFontEx(SCREEN_TV, 0, 1, message2.c_str());
    OSScreenPutFontEx(SCREEN_DRC, 0, 1, message2.c_str());

    OSScreenFlipBuffersEx(SCREEN_TV);
    OSScreenFlipBuffersEx(SCREEN_DRC);

    OSSleepTicks(OSMillisecondsToTicks(durationInMs));
}
