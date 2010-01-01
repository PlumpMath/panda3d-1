// Filename: glxGraphicsWindow.cxx
// Created by:  mike (09Jan97)
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

#include "glxGraphicsWindow.h"
#include "glxGraphicsStateGuardian.h"
#include "config_glxdisplay.h"
#include "glxGraphicsPipe.h"

#include "graphicsPipe.h"
#include "keyboardButton.h"
#include "mouseButton.h"
#include "glgsg.h"
#include "clockObject.h"
#include "pStatTimer.h"
#include "textEncoder.h"
#include "throw_event.h"
#include "lightReMutexHolder.h"
#include "nativeWindowHandle.h"

#include <errno.h>
#include <sys/time.h>

TypeHandle glxGraphicsWindow::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: glxGraphicsWindow::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
glxGraphicsWindow::
glxGraphicsWindow(GraphicsEngine *engine, GraphicsPipe *pipe, 
                  const string &name,
                  const FrameBufferProperties &fb_prop,
                  const WindowProperties &win_prop,
                  int flags,
                  GraphicsStateGuardian *gsg,
                  GraphicsOutput *host) :
  x11GraphicsWindow(engine, pipe, name, fb_prop, win_prop, flags, gsg, host)
{
}

////////////////////////////////////////////////////////////////////
//     Function: glxGraphicsWindow::begin_frame
//       Access: Public, Virtual
//  Description: This function will be called within the draw thread
//               before beginning rendering for a given frame.  It
//               should do whatever setup is required, and return true
//               if the frame should be rendered, or false if it
//               should be skipped.
////////////////////////////////////////////////////////////////////
bool glxGraphicsWindow::
begin_frame(FrameMode mode, Thread *current_thread) {
  PStatTimer timer(_make_current_pcollector, current_thread);

  begin_frame_spam(mode);
  if (_gsg == (GraphicsStateGuardian *)NULL) {
    return false;
  }
  if (_awaiting_configure) {
    // Don't attempt to draw while we have just reconfigured the
    // window and we haven't got the notification back yet.
    return false;
  }

  glxGraphicsStateGuardian *glxgsg;
  DCAST_INTO_R(glxgsg, _gsg, false);
  {
    LightReMutexHolder holder(glxGraphicsPipe::_x_mutex);

    if (glXGetCurrentDisplay() == _display &&
        glXGetCurrentDrawable() == _xwindow &&
        glXGetCurrentContext() == glxgsg->_context) {
      // No need to make the context current again.  Short-circuit
      // this possibly-expensive call.
    } else {
      // Need to set the context.
      glXMakeCurrent(_display, _xwindow, glxgsg->_context);
    }
  }
  
  // Now that we have made the context current to a window, we can
  // reset the GSG state if this is the first time it has been used.
  // (We can't just call reset() when we construct the GSG, because
  // reset() requires having a current context.)
  glxgsg->reset_if_new();
  
  if (mode == FM_render) {
    // begin_render_texture();
    clear_cube_map_selection();
  }
  
  _gsg->set_current_properties(&get_fb_properties());
  return _gsg->begin_frame(current_thread);
}

////////////////////////////////////////////////////////////////////
//     Function: glxGraphicsWindow::begin_flip
//       Access: Public, Virtual
//  Description: This function will be called within the draw thread
//               after end_frame() has been called on all windows, to
//               initiate the exchange of the front and back buffers.
//
//               This should instruct the window to prepare for the
//               flip at the next video sync, but it should not wait.
//
//               We have the two separate functions, begin_flip() and
//               end_flip(), to make it easier to flip all of the
//               windows at the same time.
////////////////////////////////////////////////////////////////////
void glxGraphicsWindow::
begin_flip() {
  if (_gsg != (GraphicsStateGuardian *)NULL) {

    // It doesn't appear to be necessary to ensure the graphics
    // context is current before flipping the windows, and insisting
    // on doing so can be a significant performance hit.

    //make_current();

    LightReMutexHolder holder(glxGraphicsPipe::_x_mutex);
    glXSwapBuffers(_display, _xwindow);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: glxGraphicsWindow::close_window
//       Access: Protected, Virtual
//  Description: Closes the window right now.  Called from the window
//               thread.
////////////////////////////////////////////////////////////////////
void glxGraphicsWindow::
close_window() {
  if (_gsg != (GraphicsStateGuardian *)NULL) {
    glXMakeCurrent(_display, None, NULL);
    _gsg.clear();
    _active = false;
  }
  
  x11GraphicsWindow::close_window();
}

////////////////////////////////////////////////////////////////////
//     Function: glxGraphicsWindow::open_window
//       Access: Protected, Virtual
//  Description: Opens the window right now.  Called from the window
//               thread.  Returns true if the window is successfully
//               opened, or false if there was a problem.
////////////////////////////////////////////////////////////////////
bool glxGraphicsWindow::
open_window() {
  glxGraphicsPipe *glx_pipe;
  DCAST_INTO_R(glx_pipe, _pipe, false);

  // GSG Creation/Initialization
  glxGraphicsStateGuardian *glxgsg;
  if (_gsg == 0) {
    // There is no old gsg.  Create a new one.
    glxgsg = new glxGraphicsStateGuardian(_engine, _pipe, NULL);
    glxgsg->choose_pixel_format(_fb_properties, glx_pipe->get_display(), glx_pipe->get_screen(), false, false);
    _gsg = glxgsg;
  } else {
    // If the old gsg has the wrong pixel format, create a
    // new one that shares with the old gsg.
    DCAST_INTO_R(glxgsg, _gsg, false);
    if (!glxgsg->get_fb_properties().subsumes(_fb_properties)) {
      glxgsg = new glxGraphicsStateGuardian(_engine, _pipe, glxgsg);
      glxgsg->choose_pixel_format(_fb_properties, glx_pipe->get_display(), glx_pipe->get_screen(), false, false);
      _gsg = glxgsg;
    }
  }
  
  XVisualInfo *visual_info = glxgsg->_visual;
  if (visual_info == NULL) {
    // No X visual for this fbconfig; how can we open the window?
    glxdisplay_cat.error()
      << "No X visual: cannot open window.\n";
    return false;
  }
  Visual *visual = visual_info->visual;
  int depth = visual_info->depth;

  if (!_properties.has_origin()) {
    _properties.set_origin(0, 0);
  }
  if (!_properties.has_size()) {
    _properties.set_size(100, 100);
  }
  
  Window parent_window = glx_pipe->get_root();
  WindowHandle *window_handle = _properties.get_parent_window();
  if (window_handle != NULL) {
    glxdisplay_cat.info()
      << "Got parent_window " << *window_handle << "\n";
    WindowHandle::OSHandle *os_handle = window_handle->get_os_handle();
    if (os_handle != NULL) {
      glxdisplay_cat.info()
        << "os_handle type " << os_handle->get_type() << "\n";
      
      if (os_handle->is_of_type(NativeWindowHandle::X11Handle::get_class_type())) {
        NativeWindowHandle::X11Handle *x11_handle = DCAST(NativeWindowHandle::X11Handle, os_handle);
        parent_window = x11_handle->get_handle();
      } else if (os_handle->is_of_type(NativeWindowHandle::IntHandle::get_class_type())) {
        NativeWindowHandle::IntHandle *int_handle = DCAST(NativeWindowHandle::IntHandle, os_handle);
        parent_window = (Window)int_handle->get_handle();
      }
    }
  }
  _parent_window_handle = window_handle;
  
#ifdef HAVE_GLXFBCONFIG
  if (glxgsg->_fbconfig != None) {
    setup_colormap(glxgsg->_fbconfig);
  } else {
    setup_colormap(visual_info);
  }
#else
  setup_colormap(visual_info);
#endif  // HAVE_GLXFBCONFIG

  _event_mask =
    ButtonPressMask | ButtonReleaseMask |
    KeyPressMask | KeyReleaseMask |
    EnterWindowMask | LeaveWindowMask |
    PointerMotionMask |
    FocusChangeMask |
    StructureNotifyMask;

  // Initialize window attributes
  XSetWindowAttributes wa;
  wa.background_pixel = XBlackPixel(_display, _screen);
  wa.border_pixel = 0;
  wa.colormap = _colormap;
  wa.event_mask = _event_mask;

  unsigned long attrib_mask = 
    CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

  _xwindow = XCreateWindow
    (_display, parent_window,
     _properties.get_x_origin(), _properties.get_y_origin(),
     _properties.get_x_size(), _properties.get_y_size(),
     0, depth, InputOutput, visual, attrib_mask, &wa);

  if (_xwindow == (Window)0) {
    glxdisplay_cat.error()
      << "failed to create X window.\n";
    return false;
  }
  set_wm_properties(_properties, false);

  // We don't specify any fancy properties of the XIC.  It would be
  // nicer if we could support fancy IM's that want preedit callbacks,
  // etc., but that can wait until we have an X server that actually
  // supports these to test it on.
  XIM im = glx_pipe->get_im();
  _ic = NULL;
  if (im) {
    _ic = XCreateIC
      (im,
       XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
       (void*)NULL);
    if (_ic == (XIC)NULL) {
      glxdisplay_cat.warning()
        << "Couldn't create input context.\n";
    }
  }

  if (_properties.get_cursor_hidden()) {
    XDefineCursor(_display, _xwindow, glx_pipe->get_hidden_cursor());
  }

  glXMakeCurrent(_display, _xwindow, glxgsg->_context);
  glxgsg->reset_if_new();
  if (!glxgsg->is_valid()) {
    close_window();
    return false;
  }
  if (!glxgsg->get_fb_properties().verify_hardware_software
      (_fb_properties, glxgsg->get_gl_renderer())) {
    close_window();
    return false;
  }
  _fb_properties = glxgsg->get_fb_properties();
  
  XMapWindow(_display, _xwindow);

  if (_properties.get_raw_mice()) {
    open_raw_mice();
  } else {
    if (glxdisplay_cat.is_debug()) {
      glxdisplay_cat.debug()
        << "Raw mice not requested.\n";
    }
  }

  // Create a WindowHandle for ourselves
  _window_handle = NativeWindowHandle::make_x11(_xwindow);

  // And tell our parent window that we're now its child.
  if (_parent_window_handle != (WindowHandle *)NULL) {
    _parent_window_handle->attach_child(_window_handle);
  }

  return true;
}

#ifdef HAVE_GLXFBCONFIG
////////////////////////////////////////////////////////////////////
//     Function: glxGraphicsWindow::setup_colormap
//       Access: Private
//  Description: Allocates a colormap appropriate to the fbconfig and
//               stores in in the _colormap method.
////////////////////////////////////////////////////////////////////
void glxGraphicsWindow::
setup_colormap(GLXFBConfig fbconfig) {
  XVisualInfo *visual_info = glXGetVisualFromFBConfig(_display, fbconfig);
  if (visual_info == NULL) {
    // No X visual; no need to set up a colormap.
    return;
  }
  int visual_class = visual_info->c_class;
  Visual *visual = visual_info->visual;
  XFree(visual_info);

  glxGraphicsPipe *glx_pipe;
  DCAST_INTO_V(glx_pipe, _pipe);
  Window root_window = glx_pipe->get_root();

  int rc, is_rgb;

  switch (visual_class) {
    case PseudoColor:
      rc = glXGetFBConfigAttrib(_display, fbconfig, GLX_RGBA, &is_rgb);
      if (rc == 0 && is_rgb) {
        glxdisplay_cat.warning()
          << "mesa pseudocolor not supported.\n";
        // this is a terrible terrible hack, but it seems to work
        _colormap = (Colormap)0;

      } else {
        _colormap = XCreateColormap(_display, root_window,
                                    visual, AllocAll);
      }
      break;
    case TrueColor:
    case DirectColor:
      _colormap = XCreateColormap(_display, root_window,
                                  visual, AllocNone);
      break;
    case StaticColor:
    case StaticGray:
    case GrayScale:
      _colormap = XCreateColormap(_display, root_window,
                                  visual, AllocNone);
      break;
    default:
      glxdisplay_cat.error()
        << "Could not allocate a colormap for visual class "
        << visual_class << ".\n";
      break;
  }
}
#endif  // HAVE_GLXFBCONFIG

////////////////////////////////////////////////////////////////////
//     Function: glxGraphicsWindow::setup_colormap
//       Access: Private, Virtual
//  Description: Allocates a colormap appropriate to the visual and
//               stores in in the _colormap method.
////////////////////////////////////////////////////////////////////
void glxGraphicsWindow::
setup_colormap(XVisualInfo *visual) {
  glxGraphicsPipe *glx_pipe;
  DCAST_INTO_V(glx_pipe, _pipe);
  Window root_window = glx_pipe->get_root();

  int visual_class = visual->c_class;
  int rc, is_rgb;

  switch (visual_class) {
    case PseudoColor:
      rc = glXGetConfig(_display, visual, GLX_RGBA, &is_rgb);
      if (rc == 0 && is_rgb) {
        glxdisplay_cat.warning()
          << "mesa pseudocolor not supported.\n";
        // this is a terrible terrible hack, but it seems to work
        _colormap = (Colormap)0;

      } else {
        _colormap = XCreateColormap(_display, root_window,
                                    visual->visual, AllocAll);
      }
      break;
    case TrueColor:
    case DirectColor:
      _colormap = XCreateColormap(_display, root_window,
                                  visual->visual, AllocNone);
      break;
    case StaticColor:
    case StaticGray:
    case GrayScale:
      _colormap = XCreateColormap(_display, root_window,
                                  visual->visual, AllocNone);
      break;
    default:
      glxdisplay_cat.error()
        << "Could not allocate a colormap for visual class "
        << visual_class << ".\n";
      break;
  }
}