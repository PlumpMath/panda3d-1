// Filename: config_glxdisplay.cxx
// Created by:  cary (07Oct99)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#include "config_glxdisplay.h"
#include "glxGraphicsBuffer.h"
#include "glxGraphicsPipe.h"
#include "glxGraphicsWindow.h"
#include "glxGraphicsStateGuardian.h"
#include "graphicsPipeSelection.h"
#include "dconfig.h"
#include "pandaSystem.h"

Configure(config_glxdisplay);
NotifyCategoryDef(glxdisplay, "display");

ConfigureFn(config_glxdisplay) {
  init_libglxdisplay();
}

ConfigVariableString display_cfg
("display", "");

ConfigVariableBool glx_error_abort
("glx-error-abort", false);

ConfigVariableBool glx_get_proc_address
("glx-get-proc-address", true,
 PRC_DESC("Set this to true to allow the use of glxGetProcAddress(), if "
	  "it is available, to query the OpenGL extension functions.  This "
	  "is the standard way to query extension functions."));


ConfigVariableBool glx_get_os_address
("glx-get-os-address", true,
 PRC_DESC("Set this to true to allow Panda to query the OpenGL library "
	  "directly using standard operating system calls to locate "
	  "addresses of extension functions.  This will be done only "
	  "if glxGetProcAddress() cannot be used for some reason."));

ConfigVariableInt glx_wheel_up_button
("glx-wheel-up-button", 4,
 PRC_DESC("This is the mouse button index of the wheel_up event: which "
          "mouse button number does the system report when the mouse wheel "
          "is rolled one notch up?"));

ConfigVariableInt glx_wheel_down_button
("glx-wheel-down-button", 5,
 PRC_DESC("This is the mouse button index of the wheel_down event: which "
          "mouse button number does the system report when the mouse wheel "
          "is rolled one notch down?"));

////////////////////////////////////////////////////////////////////
//     Function: init_libglxdisplay
//  Description: Initializes the library.  This must be called at
//               least once before any of the functions or classes in
//               this library can be used.  Normally it will be
//               called by the static initializers and need not be
//               called explicitly, but special cases exist.
////////////////////////////////////////////////////////////////////
void
init_libglxdisplay() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

#ifdef HAVE_GLXFBCONFIG
  glxGraphicsBuffer::init_type();
#endif  // HAVE_GLXFBCONFIG
  glxGraphicsPipe::init_type();
  glxGraphicsWindow::init_type();
  glxGraphicsStateGuardian::init_type();

  GraphicsPipeSelection *selection = GraphicsPipeSelection::get_global_ptr();
  selection->add_pipe_type(glxGraphicsPipe::get_class_type(),
                           glxGraphicsPipe::pipe_constructor);

  PandaSystem *ps = PandaSystem::get_global_ptr();
  ps->set_system_tag("OpenGL", "window_system", "GLX");
}
