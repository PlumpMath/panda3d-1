// Filename: alphaTransformTransition.cxx
// Created by:  jason (01Aug00)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://www.panda3d.org/license.txt .
//
// To contact the maintainers of this program write to
// panda3d@yahoogroups.com .
//
////////////////////////////////////////////////////////////////////

#include "alphaTransformTransition.h"
#include "alphaTransformAttribute.h"

#include "graphicsStateGuardianBase.h"
#include "indent.h"

TypeHandle AlphaTransformTransition::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: AlphaTransformTransition::make_copy
//       Access: Public, Virtual
//  Description: Returns a newly allocated AlphaTransformTransition just like
//               this one.
////////////////////////////////////////////////////////////////////
NodeTransition *AlphaTransformTransition::
make_copy() const {
  return new AlphaTransformTransition(*this);
}

////////////////////////////////////////////////////////////////////
//     Function: AlphaTransformTransition::make_attrib
//       Access: Public, Virtual
//  Description: Returns a newly allocated AlphaTransformAttribute.
////////////////////////////////////////////////////////////////////
NodeAttribute *AlphaTransformTransition::
make_attrib() const {
  return new AlphaTransformAttribute;
}

////////////////////////////////////////////////////////////////////
//     Function: AlphaTransformTransition::make_initial
//       Access: Public, Virtual
//  Description: Returns a newly allocated AlphaTransformTransition
//               corresponding to the default initial state.
////////////////////////////////////////////////////////////////////
NodeTransition *AlphaTransformTransition::
make_initial() const {
  return new AlphaTransformTransition;
}

////////////////////////////////////////////////////////////////////
//     Function: AlphaTransformTransition::issue
//       Access: Public, Virtual
//  Description: This is called on scene graph rendering attributes
//               when it is time to issue the particular attribute to
//               the graphics engine.  It should call the appropriate
//               method on GraphicsStateGuardianBase.
////////////////////////////////////////////////////////////////////
void AlphaTransformTransition::
issue(GraphicsStateGuardianBase *gsgbase) {
  gsgbase->issue_alpha_transform(this);
}

////////////////////////////////////////////////////////////////////
//     Function: AlphaTransformTransition::set_value_from
//       Access: Protected, Virtual
//  Description: Copies the value from the other transition pointer,
//               which is guaranteed to be another AlphaTransformTransition.
////////////////////////////////////////////////////////////////////
void AlphaTransformTransition::
set_value_from(const OnTransition *other) {
  const AlphaTransformTransition *ot;
  DCAST_INTO_V(ot, other);
  _state = ot->_state;
}

////////////////////////////////////////////////////////////////////
//     Function: AlphaTransformTransition::compare_values
//       Access: Protected, Virtual
//  Description:
////////////////////////////////////////////////////////////////////
int AlphaTransformTransition::
compare_values(const OnTransition *other) const {
  const AlphaTransformTransition *ot;
  DCAST_INTO_R(ot, other, false);
  return _state.compare_to(ot->_state);
}

////////////////////////////////////////////////////////////////////
//     Function: AlphaTransformTransition::output_value
//       Access: Protected, Virtual
//  Description: Formats the value for human consumption on one line.
////////////////////////////////////////////////////////////////////
void AlphaTransformTransition::
output_value(ostream &out) const {
  out << _state;
}

////////////////////////////////////////////////////////////////////
//     Function: AlphaTransformTransition::write_value
//       Access: Protected, Virtual
//  Description: Formats the value for human consumption on multiple
//               lines if necessary.
////////////////////////////////////////////////////////////////////
void AlphaTransformTransition::
write_value(ostream &out, int indent_level) const {
  indent(out, indent_level) << _state << "\n";
}
