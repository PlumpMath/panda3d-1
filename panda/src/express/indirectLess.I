// Filename: indirectLess.I
// Created by:  drose (27Jun01)
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


////////////////////////////////////////////////////////////////////
//     Function: IndirectLess::operator ()
//       Access: Public
//  Description: Returns true if a sorts before b, false otherwise.
////////////////////////////////////////////////////////////////////
template<class ObjectType>
INLINE bool IndirectLess<ObjectType>::
operator () (const ObjectType *a, const ObjectType *b) const {
  return (a != b && (*a) < (*b));
}
