// Filename: wdxGraphicsPipe8.cxx
// Created by:  drose (20Dec02)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "wdxGraphicsPipe8.h"
#include "dxGraphicsDevice8.h"
#include "wdxGraphicsWindow8.h"
#include "wdxGraphicsBuffer8.h"
#include "config_dxgsg8.h"

TypeHandle wdxGraphicsPipe8::_type_handle;

#define LOWVIDMEMTHRESHOLD 5700000  // 4MB cards should fall below this
#define CRAPPY_DRIVER_IS_LYING_VIDMEMTHRESHOLD 1000000  // if # is > 1MB, card is lying and I cant tell what it is
#define UNKNOWN_VIDMEM_SIZE 0xFFFFFFFF

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsPipe8::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
wdxGraphicsPipe8::
wdxGraphicsPipe8() {
  _hDDrawDLL = NULL;
  _hD3D8_DLL = NULL;
  __d3d8 = NULL;
  _is_valid = init();
}

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsPipe8::Destructor
//       Access: Public, Virtual
//  Description:
////////////////////////////////////////////////////////////////////
wdxGraphicsPipe8::
~wdxGraphicsPipe8() {
  RELEASE(__d3d8, wdxdisplay8, "ID3D8", RELEASE_DOWN_TO_ZERO);
  SAFE_FREELIB(_hD3D8_DLL);
  SAFE_FREELIB(_hDDrawDLL);
}

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsPipe8::get_interface_name
//       Access: Published, Virtual
//  Description: Returns the name of the rendering interface
//               associated with this GraphicsPipe.  This is used to
//               present to the user to allow him/her to choose
//               between several possible GraphicsPipes available on a
//               particular platform, so the name should be meaningful
//               and unique for a given platform.
////////////////////////////////////////////////////////////////////
string wdxGraphicsPipe8::
get_interface_name() const {
  return "DirectX8";
}

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsPipe8::pipe_constructor
//       Access: Public, Static
//  Description: This function is passed to the GraphicsPipeSelection
//               object to allow the user to make a default
//               wdxGraphicsPipe8.
////////////////////////////////////////////////////////////////////
PT(GraphicsPipe) wdxGraphicsPipe8::
pipe_constructor() {
  return new wdxGraphicsPipe8;
}

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsPipe8::make_output
//       Access: Protected, Virtual
//  Description: Creates a new window on the pipe, if possible.
////////////////////////////////////////////////////////////////////
PT(GraphicsOutput) wdxGraphicsPipe8::
make_output(const string &name,
            const FrameBufferProperties &fb_prop,
            const WindowProperties &win_prop,
            int flags,
            GraphicsEngine *engine,
            GraphicsStateGuardian *gsg,
            GraphicsOutput *host,
            int retry,
            bool &precertify) {

  if (!_is_valid) {
    return NULL;
  }

  DXGraphicsStateGuardian8 *wdxgsg = 0;
  if (gsg != 0) {
    DCAST_INTO_R(wdxgsg, gsg, NULL);
  }

  // First thing to try: a visible window.

  if (retry == 0) {
    if (((flags&BF_require_parasite)!=0)||
        ((flags&BF_refuse_window)!=0)||
        ((flags&BF_resizeable)!=0)||
        ((flags&BF_size_track_host)!=0)||
        ((flags&BF_rtt_cumulative)!=0)||
        ((flags&BF_can_bind_color)!=0)||
        ((flags&BF_can_bind_every)!=0)) {
      return NULL;
    }
    // Early failure - if we are sure that this buffer WONT
    // meet specs, we can bail out early.
    if ((flags & BF_fb_props_optional) == 0) {
      if ((fb_prop.get_aux_rgba() > 0)||
          (fb_prop.get_aux_rgba() > 0)||
          (fb_prop.get_aux_float() > 0)) {
        return NULL;
      }
    }
    return new wdxGraphicsWindow8(engine, this, name, fb_prop, win_prop,
                                  flags, gsg, host);
  }

  // Second thing to try: a wdxGraphicsBuffer8

  if (retry == 1) {
    if ((!support_render_texture)||
        ((flags&BF_require_parasite)!=0)||
        ((flags&BF_require_window)!=0)||
        ((flags&BF_resizeable)!=0)||
        ((flags&BF_size_track_host)!=0)||
        ((flags&BF_rtt_cumulative)!=0)||
        ((flags&BF_can_bind_every)!=0)) {
      return NULL;
    }
    // Early failure - if we are sure that this buffer WONT
    // meet specs, we can bail out early.
    if ((flags & BF_fb_props_optional) == 0) {
      if ((fb_prop.get_aux_rgba() > 0)||
          (fb_prop.get_aux_rgba() > 0)||
          (fb_prop.get_aux_float() > 0)||
          (fb_prop.get_indexed_color() > 0)||
          (fb_prop.get_back_buffers() > 0)||
          (fb_prop.get_accum_bits() > 0)||
          (fb_prop.get_multisamples() > 0)) {
        return NULL;
      }
    }
    // Early success - if we are sure that this buffer WILL
    // meet specs, we can precertify it.
    // This looks rather overly optimistic -- ie, buggy.
    if ((gsg != 0)&&
        (gsg->is_valid())&&
        (!gsg->needs_reset())&&
        (gsg->get_supports_render_texture())) {
      precertify = true;
    }
    return new wdxGraphicsBuffer8(engine, this, name, fb_prop, win_prop,
                                  flags, gsg, host);
  }

  // Nothing else left to try.
  return NULL;
}

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsPipe8::init
//       Access: Private
//  Description: Performs some initialization steps to load up
//               function pointers from the relevant DLL's, and
//               determine the number and type of available graphics
//               adapters, etc.  Returns true on success, false on
//               failure.
////////////////////////////////////////////////////////////////////
bool wdxGraphicsPipe8::
init() {
  if (!MyLoadLib(_hDDrawDLL, "ddraw.dll")) {
    goto error;
  }

  if (!MyGetProcAddr(_hDDrawDLL, (FARPROC*)&_DirectDrawCreateEx, "DirectDrawCreateEx")) {
    goto error;
  }

  if (!MyGetProcAddr(_hDDrawDLL, (FARPROC*)&_DirectDrawEnumerateExA, "DirectDrawEnumerateExA")) {
    goto error;
  }

  if (!MyLoadLib(_hD3D8_DLL, "d3d8.dll")) {
    goto error;
  }

  if (!MyGetProcAddr(_hD3D8_DLL, (FARPROC*)&_Direct3DCreate8, "Direct3DCreate8")) {
    goto error;
  }

  // Create a Direct3D object.

  // these were taken from the 8.0 and 8.1 d3d8.h SDK headers
#define D3D_SDK_VERSION_8_0  120
#define D3D_SDK_VERSION_8_1  220

  // are we using 8.0 or 8.1?
  WIN32_FIND_DATA TempFindData;
  HANDLE hFind;
  char tmppath[_MAX_PATH + 128];
  GetSystemDirectory(tmppath, MAX_PATH);
  strcat(tmppath, "\\dpnhpast.dll");
  hFind = FindFirstFile (tmppath, &TempFindData);
  if (hFind != INVALID_HANDLE_VALUE) {
    FindClose(hFind);
    __is_dx8_1 = true;
    __d3d8 = (*_Direct3DCreate8)(D3D_SDK_VERSION_8_1);
  } else {
    __is_dx8_1 = false;
    __d3d8 = (*_Direct3DCreate8)(D3D_SDK_VERSION_8_0);
  }

  if (__d3d8 == NULL) {
    wdxdisplay8_cat.error() << "Direct3DCreate8(8." << (__is_dx8_1 ? "1" : "0") << ") failed!, error = " << GetLastError() << endl;
    //release_gsg();
    goto error;
  }

  Init_D3DFORMAT_map();

  return find_all_card_memavails();

 error:
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsPipe8::find_all_card_memavails
//       Access: Private
//  Description: Uses DX7 calls to determine how much video memory is
//               available for each video adapter in the system.
//               Returns true on success, false on failure.
////////////////////////////////////////////////////////////////////
bool wdxGraphicsPipe8::
find_all_card_memavails() {
  HRESULT hr;

  hr = (*_DirectDrawEnumerateExA)(dx7_driver_enum_callback, this,
                                  DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES);
  if (FAILED(hr)) {
    wdxdisplay8_cat.fatal()
      << "DirectDrawEnumerateEx failed" << D3DERRORSTRING(hr);
    return false;
  }

  if (_card_ids.empty()) {
    wdxdisplay8_cat.error()
      << "DirectDrawEnumerateEx enum'ed no devices!\n";
    return false;
  }

  GUID ZeroGUID;
  ZeroMemory(&ZeroGUID, sizeof(GUID));

  if (_card_ids.size() > 1) {
    nassertr(IsEqualGUID(ZeroGUID, _card_ids[0].DX7_DeviceGUID), false);
    // delete enum of primary display (always the first), since it is
    // duplicated by explicit entry
    _card_ids.erase(_card_ids.begin());
  }

  for (UINT i = 0; i < _card_ids.size(); i++) {
    LPDIRECTDRAW7 pDD;
    BYTE ddd_space[sizeof(DDDEVICEIDENTIFIER2)+4];  //bug in DX7 requires 4 extra bytes for GetDeviceID
    DDDEVICEIDENTIFIER2 *pDX7DeviceID = (DDDEVICEIDENTIFIER2 *)&ddd_space[0];
    GUID *pGUID = &(_card_ids[i].DX7_DeviceGUID);

    if (IsEqualGUID(*pGUID, ZeroGUID)) {
      pGUID = NULL;
    }

    // Create the Direct Draw Object
    hr = (*_DirectDrawCreateEx)(pGUID, (void **)&pDD, IID_IDirectDraw7, NULL);
    if (FAILED(hr)) {
      wdxdisplay8_cat.error()
        << "DirectDrawCreateEx failed for device (" << i
        << ")" << D3DERRORSTRING(hr);
      continue;
    }

    ZeroMemory(ddd_space, sizeof(DDDEVICEIDENTIFIER2));

    hr = pDD->GetDeviceIdentifier(pDX7DeviceID, 0x0);
    if (FAILED(hr)) {
      wdxdisplay8_cat.error()
        << "GetDeviceID failed for device (" << i << ")" << D3DERRORSTRING(hr);
      continue;
    }

    _card_ids[i].DeviceID = pDX7DeviceID->dwDeviceId;
    _card_ids[i].VendorID = pDX7DeviceID->dwVendorId;

    // Get Current VidMem avail.  Note this is only an estimate, when
    // we switch to fullscreen mode from desktop, more vidmem will be
    // available (typically 1.2 meg).  I dont want to switch to
    // fullscreen more than once due to the annoying monitor flicker,
    // so try to figure out optimal mode using this estimate
    DDSCAPS2 ddsGAVMCaps;
    DWORD dwVidMemTotal, dwVidMemFree;
    dwVidMemTotal = dwVidMemFree = 0;
    {
      // print out total INCLUDING AGP just for information purposes
      // and future use.  The real value I'm interested in for
      // purposes of measuring possible valid screen sizes shouldnt
      // include AGP.
      ZeroMemory(&ddsGAVMCaps, sizeof(DDSCAPS2));
      ddsGAVMCaps.dwCaps = DDSCAPS_VIDEOMEMORY;

      hr = pDD->GetAvailableVidMem(&ddsGAVMCaps, &dwVidMemTotal, &dwVidMemFree);
      if (FAILED(hr)) {
        wdxdisplay8_cat.error()
          << "GetAvailableVidMem failed for device #" << i
          << D3DERRORSTRING(hr);
        //goto skip_device;
        //exit(1);  // probably want to exit, since it may be my fault
      }
    }

    wdxdisplay8_cat.info()
      << "GetAvailableVidMem (including AGP) returns Total: "
      << dwVidMemTotal <<", Free: " << dwVidMemFree
      << " for device #" << i << endl;

    ZeroMemory(&ddsGAVMCaps, sizeof(DDSCAPS2));

    // just want to measure localvidmem, not AGP texmem
    ddsGAVMCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;

    hr = pDD->GetAvailableVidMem(&ddsGAVMCaps, &dwVidMemTotal, &dwVidMemFree);
    if (FAILED(hr)) {
      wdxdisplay8_cat.error() << "GetAvailableVidMem failed for device #" << i<< D3DERRORSTRING(hr);
      // sometimes GetAvailableVidMem fails with hr = DDERR_NODIRECTDRAWHW for some unknown reason (bad drivers?)
      // see bugs: 15327, 18122, others.  is it because D3D8 object has already been created?
      if (hr == DDERR_NODIRECTDRAWHW)
        continue;
      exit(1);  // probably want to exit, since it may be my fault
    }

    wdxdisplay8_cat.info()
      << "GetAvailableVidMem (no AGP) returns Total: " << dwVidMemTotal
      << ", Free: " << dwVidMemFree << " for device #" << i<< endl;

    pDD->Release();  // release DD obj, since this is all we needed it for

    if (!dx_do_vidmemsize_check) {
      // still calling the DD stuff to get deviceID, etc.  is this necessary?
      _card_ids[i]._max_available_video_memory = UNKNOWN_VIDMEM_SIZE;
      _card_ids[i]._is_low_memory_card = false;
      continue;
    }

    if (dwVidMemTotal == 0) {  // unreliable driver
      dwVidMemTotal = UNKNOWN_VIDMEM_SIZE;
    } else {
      if (!ISPOW2(dwVidMemTotal)) {
        // assume they wont return a proper max value, so
        // round up to next pow of 2
        UINT count = 0;
        while ((dwVidMemTotal >> count) != 0x0) {
          count++;
        }
        dwVidMemTotal = (1 << count);
      }
    }

    // after Set_display_mode, GetAvailVidMem totalmem seems to go down
    // by 1.2 meg (contradicting above comment and what I think would
    // be correct behavior (shouldnt FS mode release the desktop
    // vidmem?), so this is the true value
    _card_ids[i]._max_available_video_memory = dwVidMemTotal;

    // I can never get this stuff to work reliably, so I'm just
    // rounding up to nearest pow2.  Could try to get
    // HardwareInformation.Memory_size MB number from registry like
    // video control panel, but its not clear how to find the proper
    // registry location for a given card

    // assume buggy drivers (this means you, FireGL2) may return zero
    // (or small amts) for dwVidMemTotal, so ignore value if its < CRAPPY_DRIVER_IS_LYING_VIDMEMTHRESHOLD
    bool bLowVidMemFlag =
      ((dwVidMemTotal > CRAPPY_DRIVER_IS_LYING_VIDMEMTHRESHOLD) &&
       (dwVidMemTotal< LOWVIDMEMTHRESHOLD));

    _card_ids[i]._is_low_memory_card = bLowVidMemFlag;
    wdxdisplay8_cat.info()
      << "SetLowVidMem flag to " << bLowVidMemFlag
      << " based on adjusted VidMemTotal: " << dwVidMemTotal << endl;
  }
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsPipe8::dx7_driver_enum_callback
//       Access: Private, Static
//  Description:
////////////////////////////////////////////////////////////////////
BOOL WINAPI wdxGraphicsPipe8::
dx7_driver_enum_callback(GUID *pGUID, TCHAR *strDesc, TCHAR *strName,
                         VOID *argptr, HMONITOR hm) {
  wdxGraphicsPipe8 *self = (wdxGraphicsPipe8 *)argptr;

  CardID card_id;
  ZeroMemory(&card_id, sizeof(CardID));

  if (hm == NULL) {
    card_id._monitor = MonitorFromWindow(GetDesktopWindow(),
                                     MONITOR_DEFAULTTOPRIMARY);
  } else {
    card_id._monitor = hm;
  }

  if (pGUID != NULL) {
    memcpy(&card_id.DX7_DeviceGUID, pGUID, sizeof(GUID));
  }

  card_id._max_available_video_memory = UNKNOWN_VIDMEM_SIZE;

  self->_card_ids.push_back(card_id);

  return DDENUMRET_OK;
}

//////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsWindow8::find_best_depth_format
//       Access: Private
//  Description:
////////////////////////////////////////////////////////////////////
bool wdxGraphicsPipe8::
find_best_depth_format(DXScreenData &Display, D3DDISPLAYMODE &Test_display_mode,
                       D3DFORMAT *pBestFmt, bool bWantStencil,
                       bool bForce16bpp, bool bVerboseMode) const {
  if (dxgsg8_cat.is_debug()) {
    bVerboseMode = true;
  }

  // list formats to try in order of preference.

#define NUM_TEST_ZFMTS 6
#define FIRST_NON_STENCIL_ZFMT 3
  static D3DFORMAT FormatPrefList[NUM_TEST_ZFMTS] = {
    D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D15S1,  // with stencil
    D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D16         // without stencil
  };

  // do not use Display._display_mode since that is probably not set yet, use Test_display_mode instead

  *pBestFmt = D3DFMT_UNKNOWN;
  HRESULT hr;

  // nvidia likes zbuf depth to match rendertarget depth
  bool bOnlySelect16bpp = (bForce16bpp ||
                           (IS_NVIDIA(Display._dx_device_id) && IS_16BPP_DISPLAY_FORMAT(Test_display_mode.Format)));

  if (bVerboseMode) {
    wdxdisplay8_cat.info()
      << "FindBestDepthFmt: bSelectOnly16bpp: " << bOnlySelect16bpp << endl;
  }

  int first_format = (bWantStencil ? 0 : FIRST_NON_STENCIL_ZFMT);
  for (int i = first_format; i < NUM_TEST_ZFMTS; i++) {
    D3DFORMAT TestDepthFmt = FormatPrefList[i];

    if (bOnlySelect16bpp && !IS_16BPP_ZBUFFER(TestDepthFmt)) {
      continue;
    }

    hr = Display._d3d8->CheckDeviceFormat(Display._card_id,
                                          D3DDEVTYPE_HAL,
                                          Test_display_mode.Format,
                                          D3DUSAGE_DEPTHSTENCIL,
                                          D3DRTYPE_SURFACE, TestDepthFmt);

    if (FAILED(hr)) {
      if (hr == D3DERR_NOTAVAILABLE) {
        if (bVerboseMode)
          wdxdisplay8_cat.info()
            << "FindBestDepthFmt: ChkDevFmt returns NotAvail for "
            << D3DFormatStr(TestDepthFmt) << endl;
        continue;
      }

      wdxdisplay8_cat.error()
        << "unexpected CheckDeviceFormat failure" << D3DERRORSTRING(hr)
        << endl;
      exit(1);
    }

    hr = Display._d3d8->CheckDepthStencilMatch(Display._card_id,
                                               D3DDEVTYPE_HAL,
                                               Test_display_mode.Format,   // adapter format
                                               Test_display_mode.Format,   // backbuffer fmt  (should be the same in my apps)
                                               TestDepthFmt);
    if (SUCCEEDED(hr)) {
      *pBestFmt = TestDepthFmt;
      break;
    } else {
      if (hr == D3DERR_NOTAVAILABLE) {
        if (bVerboseMode) {
          wdxdisplay8_cat.info()
            << "FindBestDepthFmt: ChkDepMatch returns NotAvail for "
            << D3DFormatStr(Test_display_mode.Format) << ", "
            << D3DFormatStr(TestDepthFmt) << endl;
        }
      } else {
        wdxdisplay8_cat.error()
          << "unexpected CheckDepthStencilMatch failure for "
          << D3DFormatStr(Test_display_mode.Format) << ", "
          << D3DFormatStr(TestDepthFmt) << endl;
      }
    }
  }

  if (bVerboseMode) {
    wdxdisplay8_cat.info()
      << "FindBestDepthFmt returns fmt " << D3DFormatStr(*pBestFmt) << endl;
  }

  return (*pBestFmt != D3DFMT_UNKNOWN);
}


////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsWindow8::special_check_fullscreen_resolution
//       Access: Private
//  Description: overrides of the general estimator for known working
//               cases
////////////////////////////////////////////////////////////////////
bool wdxGraphicsPipe8::
special_check_fullscreen_resolution(DXScreenData &scrn, UINT x_size, UINT y_size) {
  DWORD VendorId = scrn._dx_device_id.VendorId;
  DWORD DeviceId = scrn._dx_device_id.DeviceId;

  switch (VendorId) {
  case 0x8086:  // Intel
    if ((x_size == 640) && (y_size == 480)) {
      return true;
    }
    if ((x_size == 800) && (y_size == 600)) {
      return true;
    }
    if ((x_size == 1024) && (y_size == 768)) {
      return true;
    }
    break;
  }

  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsWindow8::search_for_valid_displaymode
//       Access: Private
//  Description: All ptr args are output parameters.  If no valid mode
//               found, returns *pSuggestedPixFmt = D3DFMT_UNKNOWN;
////////////////////////////////////////////////////////////////////
void wdxGraphicsPipe8::
search_for_valid_displaymode(DXScreenData &scrn,
                             UINT RequestedX_Size, UINT RequestedY_Size,
                             bool bWantZBuffer, bool bWantStencil,
                             UINT *p_supported_screen_depths_mask,
                             bool *pCouldntFindAnyValidZBuf,
                             D3DFORMAT *pSuggestedPixFmt,
                             bool bForce16bppZBuffer,
                             bool bVerboseMode) {

  nassertv(IS_VALID_PTR(scrn._d3d8));
  HRESULT hr;

  *pSuggestedPixFmt = D3DFMT_UNKNOWN;
  *p_supported_screen_depths_mask = 0x0;
  *pCouldntFindAnyValidZBuf = false;

  int cNumModes = scrn._d3d8->GetAdapterModeCount(scrn._card_id);
  D3DDISPLAYMODE BestDispMode;
  ZeroMemory(&BestDispMode, sizeof(BestDispMode));

  if (bVerboseMode) {
    wdxdisplay8_cat.info()
      << "searching for valid display modes at res: ("
      << RequestedX_Size << ", " << RequestedY_Size
      << "), TotalModes: " << cNumModes << endl;
  }

  // ignore memory based checks for min res 640x480.  some cards just
  // dont give accurate memavails.  (should I do the check anyway for
  // 640x480 32bpp?)
  bool bDoMemBasedChecks =
    ((!((RequestedX_Size == 640)&&(RequestedY_Size == 480))) &&
     (scrn._max_available_video_memory != UNKNOWN_VIDMEM_SIZE) &&
     (!special_check_fullscreen_resolution(scrn, RequestedX_Size, RequestedY_Size)));

  if (bVerboseMode || wdxdisplay8_cat.is_spam()) {
    wdxdisplay8_cat.info()
      << "DoMemBasedChecks = " << bDoMemBasedChecks << endl;
  }

  for (int i = 0; i < cNumModes; i++) {
    D3DDISPLAYMODE dispmode;
    hr = scrn._d3d8->EnumAdapterModes(scrn._card_id, i, &dispmode);
    if (FAILED(hr)) {
      wdxdisplay8_cat.error()
        << "EnumAdapter_display_mode failed for device #"
        << scrn._card_id << D3DERRORSTRING(hr);
      continue;
    }

    if ((dispmode.Width != RequestedX_Size) ||
        (dispmode.Height != RequestedY_Size)) {
      if (bVerboseMode) {
        wdxdisplay8_cat.info()
          << "Mode dimension " << dispmode.Width << "x" << dispmode.Height
          << "; format " << D3DFormatStr(dispmode.Format)
          << ": onto next mode\n";
      }
      continue;
    }

    // disable refresh rate checking since SLI video cards may use 
    // refresh rates less than 60
    if (0) {
      if ((dispmode.RefreshRate<60) && (dispmode.RefreshRate>1)) {
        // dont want refresh rates under 60Hz, but 0 or 1 might indicate
        // a default refresh rate, which is usually > = 60
        if (bVerboseMode) {
          wdxdisplay8_cat.info()
            << "skipping mode[" << i << "], bad refresh rate: "
            << dispmode.RefreshRate << endl;
        }
        continue;
      }
    }
    
    // Note no attempt is made to verify if format will work at
    // requested size, so even if this call succeeds, could still get
    // an out-of-video-mem error

    hr = scrn._d3d8->CheckDeviceFormat(scrn._card_id, D3DDEVTYPE_HAL, dispmode.Format,
                                       D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE,
                                       dispmode.Format);
    if (FAILED(hr)) {
      if (hr == D3DERR_NOTAVAILABLE) {
        if (bVerboseMode) {
          wdxdisplay8_cat.info()
            << "skipping mode[" << i
            << "], CheckDevFmt returns NotAvail for fmt: "
            << D3DFormatStr(dispmode.Format) << endl;
        }
        continue;
      } else {
        wdxdisplay8_cat.error()
          << "CheckDeviceFormat failed for device #"
          << scrn._card_id << D3DERRORSTRING(hr);
        continue;
      }
    }

    bool bIs16bppRenderTgt = IS_16BPP_DISPLAY_FORMAT(dispmode.Format);
    float RendTgtMinMemReqmt = 0.0f;

    // if we have a valid memavail value, try to determine if we have
    // enough space
    if (bDoMemBasedChecks) {
      // assume user is testing fullscreen, not windowed, so use the
      // dwTotal value see if 3 scrnbufs (front/back/z)at 16bpp at
      // x_size*y_size will fit with a few extra megs for texmem

      // 8MB Rage Pro says it has 6.8 megs Total free and will run at
      // 1024x768, so formula makes it so that is OK

#define REQD_TEXMEM 1800000

      float bytes_per_pixel = (bIs16bppRenderTgt ? 2 : 4);

      // *2 for double buffer

      RendTgtMinMemReqmt =
        ((float)RequestedX_Size) * ((float)RequestedY_Size) *
        bytes_per_pixel * 2 + REQD_TEXMEM;

      if (bVerboseMode || wdxdisplay8_cat.is_spam())
        wdxdisplay8_cat.info()
          << "Testing Mode (" <<RequestedX_Size<<"x" << RequestedY_Size
          << ", " << D3DFormatStr(dispmode.Format) << ")\nReqdVidMem: "
          << (int)RendTgtMinMemReqmt << " AvailVidMem: "
          << scrn._max_available_video_memory << endl;

      if (RendTgtMinMemReqmt > scrn._max_available_video_memory) {
        if (bVerboseMode || wdxdisplay8_cat.is_debug())
          wdxdisplay8_cat.info()
            << "not enough VidMem for render tgt, skipping display fmt "
            << D3DFormatStr(dispmode.Format) << " ("
            << (int)RendTgtMinMemReqmt << " > "
            << scrn._max_available_video_memory << ")\n";
        continue;
      }
    }

    if (bWantZBuffer) {
      D3DFORMAT zformat;
      if (!find_best_depth_format(scrn, dispmode, &zformat,
                                  bWantStencil, bForce16bppZBuffer)) {
        *pCouldntFindAnyValidZBuf = true;
        continue;
      }

      float MinMemReqmt = 0.0f;

      if (bDoMemBasedChecks) {
        // test memory again, this time including zbuf size
        float zbytes_per_pixel = (IS_16BPP_ZBUFFER(zformat) ? 2 : 4);
        float MinMemReqmt = RendTgtMinMemReqmt + ((float)RequestedX_Size)*((float)RequestedY_Size)*zbytes_per_pixel;

        if (bVerboseMode || wdxdisplay8_cat.is_spam())
          wdxdisplay8_cat.info()
            << "Testing Mode w/Z (" << RequestedX_Size << "x"
            << RequestedY_Size << ", " << D3DFormatStr(dispmode.Format)
            << ")\nReqdVidMem: " << (int)MinMemReqmt << " AvailVidMem: "
            << scrn._max_available_video_memory << endl;

        if (MinMemReqmt > scrn._max_available_video_memory) {
          if (bVerboseMode || wdxdisplay8_cat.is_debug())
            wdxdisplay8_cat.info()
              << "not enough VidMem for RendTgt+zbuf, skipping display fmt "
              << D3DFormatStr(dispmode.Format) << " (" << (int)MinMemReqmt
              << " > " << scrn._max_available_video_memory << ")\n";
          continue;
        }
      }

//      Optimizing for 16-bit depth does not work in all cases so turn it off.
      if (false) {
        if ((!bDoMemBasedChecks) || (MinMemReqmt<scrn._max_available_video_memory)) {
          if (!IS_16BPP_ZBUFFER(zformat)) {
            // see if things fit with a 16bpp zbuffer

            if (!find_best_depth_format(scrn, dispmode, &zformat,
                                        bWantStencil, true, bVerboseMode)) {
              if (bVerboseMode)
                wdxdisplay8_cat.info()
                  << "FindBestDepthFmt rejected Mode[" << i << "] ("
                  << RequestedX_Size << "x" << RequestedY_Size
                  << ", " << D3DFormatStr(dispmode.Format) << endl;
              *pCouldntFindAnyValidZBuf = true;
              continue;
            }

            // right now I'm not going to use these flags, just let the
            // create fail out-of-mem and retry at 16bpp
            *p_supported_screen_depths_mask |=
              (IS_16BPP_DISPLAY_FORMAT(dispmode.Format) ? DISPLAY_16BPP_REQUIRES_16BPP_ZBUFFER_FLAG : DISPLAY_32BPP_REQUIRES_16BPP_ZBUFFER_FLAG);
          }
        }
      }
    }

    if (bVerboseMode || wdxdisplay8_cat.is_spam())
      wdxdisplay8_cat.info()
        << "Validated Mode (" << RequestedX_Size << "x"
        << RequestedY_Size << ", " << D3DFormatStr(dispmode.Format) << endl;

    /*
    // dx8 valid display modes for render targets.
    D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, D3DFMT_X8R8G8B8, and D3DFMT_A8R8G8B8
    */

    switch (dispmode.Format) {
    case D3DFMT_X1R5G5B5:
      *p_supported_screen_depths_mask |= X1R5G5B5_FLAG;
      break;
    case D3DFMT_X8R8G8B8:
      *p_supported_screen_depths_mask |= X8R8G8B8_FLAG;
      break;
    case D3DFMT_A8R8G8B8:
      *p_supported_screen_depths_mask |= A8R8G8B8_FLAG;
      break;
    case D3DFMT_R5G6B5:
      *p_supported_screen_depths_mask |= R5G6B5_FLAG;
      break;
    default:
      // Render target formats should be only D3DFMT_X1R5G5B5,
      // D3DFMT_R5G6B5, D3DFMT_X8R8G8B8 and D3DFMT_A8R8G8B8
      wdxdisplay8_cat.error()
        << "unrecognized supported fmt " << D3DFormatStr(dispmode.Format)
        << " returned by EnumAdapter_display_modes!\n";
    }
  }

  // note: this chooses 32bpp, which may not be preferred over 16 for
  // memory & speed reasons on some older cards in particular
  if (*p_supported_screen_depths_mask & X8R8G8B8_FLAG) {
    *pSuggestedPixFmt = D3DFMT_X8R8G8B8;
  } else if (*p_supported_screen_depths_mask & A8R8G8B8_FLAG) {
    *pSuggestedPixFmt = D3DFMT_A8R8G8B8;
  } else if (*p_supported_screen_depths_mask & R5G6B5_FLAG) {
    *pSuggestedPixFmt = D3DFMT_R5G6B5;
  } else if (*p_supported_screen_depths_mask & X1R5G5B5_FLAG) {
    *pSuggestedPixFmt = D3DFMT_X1R5G5B5;
  }

  if (bVerboseMode || wdxdisplay8_cat.is_spam()) {
    wdxdisplay8_cat.info()
      << "search_for_valid_device returns fmt: "
      << D3DFormatStr(*pSuggestedPixFmt) << endl;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: wdxGraphicsPipew8::make_device
//       Access: Public, Virtual
//  Description: Creates a new reference to a particular hardware
//               device and associates it with the pipe.
////////////////////////////////////////////////////////////////////
PT(GraphicsDevice) wdxGraphicsPipe8::
make_device(void *scrn) {
  PT(DXGraphicsDevice8) device = new DXGraphicsDevice8(this);
  memcpy(&device->_Scrn, scrn, sizeof(device->_Scrn));
  device->_d3d_device = device->_Scrn._d3d_device;

  _device = device;
  wdxdisplay8_cat.info() << "walla: device" << device << "\n";

  return device.p();
}

pmap<D3DFORMAT_FLAG, D3DFORMAT> g_D3DFORMATmap;

void Init_D3DFORMAT_map() {
  if (g_D3DFORMATmap.size() != 0)
    return;

#define INSERT_ELEM(XX)  g_D3DFORMATmap[XX##_FLAG] = D3DFMT_##XX;

  INSERT_ELEM(R8G8B8);
  INSERT_ELEM(A8R8G8B8);
  INSERT_ELEM(X8R8G8B8);
  INSERT_ELEM(R5G6B5);
  INSERT_ELEM(X1R5G5B5);
  INSERT_ELEM(A1R5G5B5);
  INSERT_ELEM(A4R4G4B4);
  INSERT_ELEM(R3G3B2);
  INSERT_ELEM(A8);
  INSERT_ELEM(A8R3G3B2);
  INSERT_ELEM(X4R4G4B4);
  INSERT_ELEM(A2B10G10R10);
  INSERT_ELEM(G16R16);
  INSERT_ELEM(A8P8);
  INSERT_ELEM(P8);
  INSERT_ELEM(L8);
  INSERT_ELEM(A8L8);
  INSERT_ELEM(A4L4);
  INSERT_ELEM(V8U8);
  INSERT_ELEM(L6V5U5);
  INSERT_ELEM(X8L8V8U8);
  INSERT_ELEM(Q8W8V8U8);
  INSERT_ELEM(V16U16);
  INSERT_ELEM(W11V11U10);
  INSERT_ELEM(A2W10V10U10);
  INSERT_ELEM(UYVY);
  INSERT_ELEM(YUY2);
  INSERT_ELEM(DXT1);
  INSERT_ELEM(DXT2);
  INSERT_ELEM(DXT3);
  INSERT_ELEM(DXT4);
  INSERT_ELEM(DXT5);
}



const char *D3DFormatStr(D3DFORMAT fmt) {

#define CASESTR(XX) case XX: return #XX;
  switch(fmt) {
    CASESTR(D3DFMT_UNKNOWN);
    CASESTR(D3DFMT_R8G8B8);
    CASESTR(D3DFMT_A8R8G8B8);
    CASESTR(D3DFMT_X8R8G8B8);
    CASESTR(D3DFMT_R5G6B5);
    CASESTR(D3DFMT_X1R5G5B5);
    CASESTR(D3DFMT_A1R5G5B5);
    CASESTR(D3DFMT_A4R4G4B4);
    CASESTR(D3DFMT_R3G3B2);
    CASESTR(D3DFMT_A8);
    CASESTR(D3DFMT_A8R3G3B2);
    CASESTR(D3DFMT_X4R4G4B4);
    CASESTR(D3DFMT_A2B10G10R10);
    CASESTR(D3DFMT_G16R16);
    CASESTR(D3DFMT_A8P8);
    CASESTR(D3DFMT_P8);
    CASESTR(D3DFMT_L8);
    CASESTR(D3DFMT_A8L8);
    CASESTR(D3DFMT_A4L4);
    CASESTR(D3DFMT_V8U8);
    CASESTR(D3DFMT_L6V5U5);
    CASESTR(D3DFMT_X8L8V8U8);
    CASESTR(D3DFMT_Q8W8V8U8);
    CASESTR(D3DFMT_V16U16);
    CASESTR(D3DFMT_W11V11U10);
    CASESTR(D3DFMT_A2W10V10U10);
    CASESTR(D3DFMT_UYVY);
    CASESTR(D3DFMT_YUY2);
    CASESTR(D3DFMT_DXT1);
    CASESTR(D3DFMT_DXT2);
    CASESTR(D3DFMT_DXT3);
    CASESTR(D3DFMT_DXT4);
    CASESTR(D3DFMT_DXT5);
    CASESTR(D3DFMT_D16_LOCKABLE);
    CASESTR(D3DFMT_D32);
    CASESTR(D3DFMT_D15S1);
    CASESTR(D3DFMT_D24S8);
    CASESTR(D3DFMT_D16);
    CASESTR(D3DFMT_D24X8);
    CASESTR(D3DFMT_D24X4S4);
    CASESTR(D3DFMT_VERTEXDATA);
    CASESTR(D3DFMT_INDEX16);
    CASESTR(D3DFMT_INDEX32);
  }

  return "Invalid D3DFORMAT";
}
