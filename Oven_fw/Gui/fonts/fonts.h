// This file is generated by build_fonts.sh

#define GDISP_INCLUDE_FONT_DEJAVUSANS16     1

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS10) && GDISP_INCLUDE_FONT_DEJAVUSANS10
#define GDISP_FONT_FOUND
#include "DejaVuSans10.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS12_AA) && GDISP_INCLUDE_FONT_DEJAVUSANS12_AA
#define GDISP_FONT_FOUND
#include "DejaVuSans12_aa.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS12) && GDISP_INCLUDE_FONT_DEJAVUSANS12
#define GDISP_FONT_FOUND
#include "DejaVuSans12.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS16_AA) && GDISP_INCLUDE_FONT_DEJAVUSANS16_AA
#define GDISP_FONT_FOUND
#include "DejaVuSans16_aa.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS16) && GDISP_INCLUDE_FONT_DEJAVUSANS16
#define GDISP_FONT_FOUND
//#include "DejaVuSans16.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS20_AA) && GDISP_INCLUDE_FONT_DEJAVUSANS20_AA
#define GDISP_FONT_FOUND
#include "DejaVuSans20_aa.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS20) && GDISP_INCLUDE_FONT_DEJAVUSANS20
#define GDISP_FONT_FOUND
#include "DejaVuSans20.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS24_AA) && GDISP_INCLUDE_FONT_DEJAVUSANS24_AA
#define GDISP_FONT_FOUND
#include "DejaVuSans24_aa.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS24) && GDISP_INCLUDE_FONT_DEJAVUSANS24
#define GDISP_FONT_FOUND
#include "DejaVuSans24.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS32_AA) && GDISP_INCLUDE_FONT_DEJAVUSANS32_AA
#define GDISP_FONT_FOUND
#include "DejaVuSans32_aa.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANS32) && GDISP_INCLUDE_FONT_DEJAVUSANS32
#define GDISP_FONT_FOUND
#include "DejaVuSans32.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12_AA) && GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12_AA
#define GDISP_FONT_FOUND
#include "DejaVuSansBold12_aa.c"
#endif

#if defined(GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12) && GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12
#define GDISP_FONT_FOUND
#include "DejaVuSansBold12.c"
#endif

#if defined(GDISP_INCLUDE_FONT_FIXED_10X20) && GDISP_INCLUDE_FONT_FIXED_10X20
#define GDISP_FONT_FOUND
#include "fixed_10x20.c"
#endif

#if defined(GDISP_INCLUDE_FONT_FIXED_5X8) && GDISP_INCLUDE_FONT_FIXED_5X8
#define GDISP_FONT_FOUND
#include "fixed_5x8.c"
#endif

#if defined(GDISP_INCLUDE_FONT_FIXED_7X14) && GDISP_INCLUDE_FONT_FIXED_7X14
#define GDISP_FONT_FOUND
#include "fixed_7x14.c"
#endif

#if defined(GDISP_INCLUDE_FONT_LARGENUMBERS) && GDISP_INCLUDE_FONT_LARGENUMBERS
#define GDISP_FONT_FOUND
#include "LargeNumbers.c"
#endif

#if defined(GDISP_INCLUDE_FONT_UI1) && GDISP_INCLUDE_FONT_UI1
#define GDISP_FONT_FOUND
#include "UI1.c"
#endif

#if defined(GDISP_INCLUDE_FONT_UI2) && GDISP_INCLUDE_FONT_UI2
#define GDISP_FONT_FOUND
#include "UI2.c"
#endif

#if defined(GDISP_INCLUDE_USER_FONTS) && GDISP_INCLUDE_USER_FONTS
#define GDISP_FONT_FOUND
#include "userfonts.h"
#endif

#ifndef GDISP_FONT_FOUND
#error "GDISP: No fonts have been included"
#endif
