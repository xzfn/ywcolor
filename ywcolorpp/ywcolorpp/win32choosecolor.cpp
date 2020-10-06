#include "win32choosecolor.h"

COLORREF choose_color(COLORREF default_color) {
    CHOOSECOLOR cc;                 // common dialog box structure 
    static COLORREF acrCustClr[16]; // array of custom colors 
    DWORD rgbCurrent = default_color;        // initial color selection

    // Initialize CHOOSECOLOR 
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = 0;
    cc.lpCustColors = (LPDWORD)acrCustClr;
    cc.rgbResult = rgbCurrent;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    if (ChooseColor(&cc) == TRUE) {
        return cc.rgbResult;
    }
    return default_color;
}