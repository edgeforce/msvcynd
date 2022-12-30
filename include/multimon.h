//=============================================================================
// 
// multimon.h -- Stub module that fakes multiple monitor apis on Win32 OSes
//               without them.
// 
// By using this header your code will get back default values from
// GetSystemMetrics() for new metrics, and the new multimonitor APIs
// will act like only one display is present on a Win32 OS without
// multimonitor APIs.
// 
// Exactly one source must include this with COMPILE_MULTIMON_STUBS defined.
// 
// Copyright (c) 1985-1996, Microsoft Corporation
// 
//=============================================================================

#ifdef __cplusplus
extern "C" {            // Assume C declarations for C++
#endif // __cplusplus

//
// If we are building with Win95/NT4 headers, we need to declare
// the multimonitor-related metrics and APIs ourselves.
//
#ifndef SM_CMONITORS

#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#define SM_SAMEDISPLAYFORMAT    81

// HMONITOR is already declared if _WIN32_WINNT >= 0x0500 in windef.h
// This is for components built with an older version number.
//
#if !defined(HMONITOR_DECLARED) && (_WIN32_WINNT < 0x0500)
DECLARE_HANDLE(HMONITOR);
#define HMONITOR_DECLARED
#endif

#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002

#define MONITORINFOF_PRIMARY        0x00000001

typedef struct tagMONITORINFO
{
    DWORD   cbSize;
    RECT    rcMonitor;
    RECT    rcWork;
    DWORD   dwFlags;
} MONITORINFO, *LPMONITORINFO;

#ifndef CCHDEVICENAME
#define CCHDEVICENAME 32
#endif

#ifdef __cplusplus
typedef struct tagMONITORINFOEX : public tagMONITORINFO
{
    TCHAR       szDevice[CCHDEVICENAME];
} MONITORINFOEX, *LPMONITORINFOEX;
#else
typedef struct
{
    MONITORINFO;
    TCHAR       szDevice[CCHDEVICENAME];
} MONITORINFOEX, *LPMONITORINFOEX;
#endif

typedef BOOL (CALLBACK* MONITORENUMPROC)(HMONITOR, HDC, LPRECT, LPARAM);

#endif  // SM_CMONITORS

#undef GetMonitorInfo
#undef GetSystemMetrics
#undef MonitorFromWindow
#undef MonitorFromRect
#undef MonitorFromPoint
#undef EnumDisplayMonitors

//
// Define COMPILE_MULTIMON_STUBS to compile the stubs;
// otherwise, you get the declarations.
//
#ifdef COMPILE_MULTIMON_STUBS

//-----------------------------------------------------------------------------
//
// Implement the API stubs.
//
//-----------------------------------------------------------------------------

#ifndef MULTIMON_FNS_DEFINED

int      (WINAPI* g_pfnGetSystemMetrics)(int) = NULL;
HMONITOR (WINAPI* g_pfnMonitorFromWindow)(HWND, DWORD) = NULL;
HMONITOR (WINAPI* g_pfnMonitorFromRect)(LPCRECT, DWORD) = NULL;
HMONITOR (WINAPI* g_pfnMonitorFromPoint)(POINT, DWORD) = NULL;
BOOL     (WINAPI* g_pfnGetMonitorInfo)(HMONITOR, LPMONITORINFO) = NULL;
BOOL     (WINAPI* g_pfnEnumDisplayMonitors)(HDC, LPCRECT, MONITORENUMPROC, LPARAM) = NULL;
BOOL     g_fMultiMonInitDone = FALSE;

#endif

BOOL InitMultipleMonitorStubs(void)
{
    HMODULE hUser32;

    if (g_fMultiMonInitDone)
    {
        return g_pfnGetMonitorInfo != NULL;
    }

    if ((hUser32 = GetModuleHandle(TEXT("USER32"))) &&
        (*(FARPROC*)&g_pfnGetSystemMetrics    = GetProcAddress(hUser32,"GetSystemMetrics")) &&
        (*(FARPROC*)&g_pfnMonitorFromWindow   = GetProcAddress(hUser32,"MonitorFromWindow")) &&
        (*(FARPROC*)&g_pfnMonitorFromRect     = GetProcAddress(hUser32,"MonitorFromRect")) &&
        (*(FARPROC*)&g_pfnMonitorFromPoint    = GetProcAddress(hUser32,"MonitorFromPoint")) &&
        (*(FARPROC*)&g_pfnEnumDisplayMonitors = GetProcAddress(hUser32,"EnumDisplayMonitors")) &&
#ifdef UNICODE
        (*(FARPROC*)&g_pfnGetMonitorInfo      = GetProcAddress(hUser32,"GetMonitorInfoW")) )
#else
        (*(FARPROC*)&g_pfnGetMonitorInfo      = GetProcAddress(hUser32,"GetMonitorInfoA")) )
#endif

    {
        g_fMultiMonInitDone = TRUE;
        return TRUE;
    }
    else
    {
        g_pfnGetSystemMetrics    = NULL;
        g_pfnMonitorFromWindow   = NULL;
        g_pfnMonitorFromRect     = NULL;
        g_pfnMonitorFromPoint    = NULL;
        g_pfnGetMonitorInfo      = NULL;
        g_pfnEnumDisplayMonitors = NULL;

        g_fMultiMonInitDone = TRUE;
        return FALSE;
    }
}

//-----------------------------------------------------------------------------
//
// fake implementations of Monitor APIs that work with the primary display
// no special parameter validation is made since these run in client code
//
//-----------------------------------------------------------------------------

int WINAPI
xGetSystemMetrics(int nIndex)
{
    if (InitMultipleMonitorStubs())
        return g_pfnGetSystemMetrics(nIndex);

    switch (nIndex)
    {
    case SM_CMONITORS:
    case SM_SAMEDISPLAYFORMAT:
        return 1;

    case SM_XVIRTUALSCREEN:
    case SM_YVIRTUALSCREEN:
        return 0;

    case SM_CXVIRTUALSCREEN:
        nIndex = SM_CXSCREEN;
        break;

    case SM_CYVIRTUALSCREEN:
        nIndex = SM_CYSCREEN;
        break;
    }

    return GetSystemMetrics(nIndex);
}

#define xPRIMARY_MONITOR ((HMONITOR)0x12340042)

HMONITOR WINAPI
xMonitorFromPoint(POINT ptScreenCoords, DWORD dwFlags)
{
    if (InitMultipleMonitorStubs())
        return g_pfnMonitorFromPoint(ptScreenCoords, dwFlags);

    if ((dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST)) ||
        ((ptScreenCoords.x >= 0) &&
        (ptScreenCoords.x < GetSystemMetrics(SM_CXSCREEN)) &&
        (ptScreenCoords.y >= 0) &&
        (ptScreenCoords.y < GetSystemMetrics(SM_CYSCREEN))))
    {
        return xPRIMARY_MONITOR;
    }

    return NULL;
}

HMONITOR WINAPI
xMonitorFromRect(LPCRECT lprcScreenCoords, DWORD dwFlags)
{
    if (InitMultipleMonitorStubs())
        return g_pfnMonitorFromRect(lprcScreenCoords, dwFlags);

    if ((dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST)) ||
        ((lprcScreenCoords->right > 0) &&
        (lprcScreenCoords->bottom > 0) &&
        (lprcScreenCoords->left < GetSystemMetrics(SM_CXSCREEN)) &&
        (lprcScreenCoords->top < GetSystemMetrics(SM_CYSCREEN))))
    {
        return xPRIMARY_MONITOR;
    }

    return NULL;
}

HMONITOR WINAPI
xMonitorFromWindow(HWND hWnd, DWORD dwFlags)
{
    WINDOWPLACEMENT wp;

    if (InitMultipleMonitorStubs())
        return g_pfnMonitorFromWindow(hWnd, dwFlags);

    if (dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST))
        return xPRIMARY_MONITOR;

    if (IsIconic(hWnd) ? 
            GetWindowPlacement(hWnd, &wp) : 
            GetWindowRect(hWnd, &wp.rcNormalPosition)) {

        return xMonitorFromRect(&wp.rcNormalPosition, dwFlags);
    }

    return NULL;
}

BOOL WINAPI
xGetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo)
{
    RECT rcWork;

    if (InitMultipleMonitorStubs())
        return g_pfnGetMonitorInfo(hMonitor, lpMonitorInfo);

    if ((hMonitor == xPRIMARY_MONITOR) &&
        lpMonitorInfo &&
        (lpMonitorInfo->cbSize >= sizeof(MONITORINFO)) &&
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0))
    {
        lpMonitorInfo->rcMonitor.left = 0;
        lpMonitorInfo->rcMonitor.top  = 0;
        lpMonitorInfo->rcMonitor.right  = GetSystemMetrics(SM_CXSCREEN);
        lpMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
        lpMonitorInfo->rcWork = rcWork;
        lpMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;

        if (lpMonitorInfo->cbSize >= sizeof(MONITORINFOEX))
            lstrcpy(((MONITORINFOEX*)lpMonitorInfo)->szDevice, TEXT("DISPLAY"));

        return TRUE;
    }

    return FALSE;
}

BOOL WINAPI
xEnumDisplayMonitors(
        HDC             hdcOptionalForPainting,
        LPCRECT         lprcEnumMonitorsThatIntersect,
        MONITORENUMPROC lpfnEnumProc,
        LPARAM          dwData)
{
    RECT rcLimit;

    if (InitMultipleMonitorStubs()) {
        return g_pfnEnumDisplayMonitors(
                hdcOptionalForPainting,
                lprcEnumMonitorsThatIntersect,
                lpfnEnumProc,
                dwData);
    }

    if (!lpfnEnumProc)
        return FALSE;

    rcLimit.left   = 0;
    rcLimit.top    = 0;
    rcLimit.right  = GetSystemMetrics(SM_CXSCREEN);
    rcLimit.bottom = GetSystemMetrics(SM_CYSCREEN);

    if (hdcOptionalForPainting)
    {
        RECT    rcClip;
        POINT   ptOrg;

        switch (GetClipBox(hdcOptionalForPainting, &rcClip))
        {
        default:
            if (!GetDCOrgEx(hdcOptionalForPainting, &ptOrg))
                return FALSE;

            OffsetRect(&rcLimit, -ptOrg.x, -ptOrg.y);
            if (IntersectRect(&rcLimit, &rcLimit, &rcClip) &&
                (!lprcEnumMonitorsThatIntersect ||
                     IntersectRect(&rcLimit, &rcLimit, lprcEnumMonitorsThatIntersect))) {

                break;
            }
            //fall thru
        case NULLREGION:
             return TRUE;
        case ERROR:
             return FALSE;
        }
    } else {
        if (    lprcEnumMonitorsThatIntersect &&
                !IntersectRect(&rcLimit, &rcLimit, lprcEnumMonitorsThatIntersect)) {

            return TRUE;
        }
    }

    return lpfnEnumProc(
            xPRIMARY_MONITOR,
            hdcOptionalForPainting,
            &rcLimit,
            dwData);
}

#undef xPRIMARY_MONITOR
#undef COMPILE_MULTIMON_STUBS

#else   // COMPILE_MULTIMON_STUBS

extern int  WINAPI xGetSystemMetrics(int);
extern HMONITOR WINAPI xMonitorFromWindow(HWND, DWORD);
extern HMONITOR WINAPI xMonitorFromRect(LPCRECT, DWORD);
extern HMONITOR WINAPI xMonitorFromPoint(POINT, DWORD);
extern BOOL WINAPI xGetMonitorInfo(HMONITOR, LPMONITORINFO);
extern BOOL WINAPI xEnumDisplayMonitors(HDC, LPCRECT, MONITORENUMPROC, LPARAM);

#endif  // COMPILE_MULTIMON_STUBS

//
// build defines that replace the regular APIs with our versions
//
#define GetSystemMetrics    xGetSystemMetrics
#define MonitorFromWindow   xMonitorFromWindow
#define MonitorFromRect     xMonitorFromRect
#define MonitorFromPoint    xMonitorFromPoint
#define GetMonitorInfo      xGetMonitorInfo
#define EnumDisplayMonitors xEnumDisplayMonitors

#ifdef __cplusplus
}
#endif  // __cplusplus
