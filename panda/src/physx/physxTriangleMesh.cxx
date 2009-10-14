// Filename: physxTriangleMesh.cxx
// Created by:  enn0x (14Oct09)
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

#include "physxTriangleMesh.h"

TypeHandle PhysxTriangleMesh::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: PhysxTriangleMesh::link
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
void PhysxTriangleMesh::
link(NxTriangleMesh *meshPtr) {

  // Link self
  //ref();
  _ptr = meshPtr;
  //_ptr->userData = this;
  _error_type = ET_ok;
}

////////////////////////////////////////////////////////////////////
//     Function: PhysxTriangleMesh::unlink
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
void PhysxTriangleMesh::
unlink() {

  // Unlink self
  //_ptr->userData = NULL;
  _error_type = ET_released;
  //unref();
}

////////////////////////////////////////////////////////////////////
//     Function: PhysxTriangleMesh::release
//       Access: Published
//  Description: 
////////////////////////////////////////////////////////////////////
void PhysxTriangleMesh::
release() {

  nassertv(_error_type == ET_ok);

  unlink();
  //_ptr->getScene().releaseTriangleMesh(*_ptr);
  //_ptr = NULL;

  NxGetPhysicsSDK()->releaseTriangleMesh(*_ptr);
  _ptr = NULL;
}

////////////////////////////////////////////////////////////////////
//     Function: PhysxTriangleMesh::get_reference_count
//       Access: Published
//  Description: Returns the reference count for shared meshes.
////////////////////////////////////////////////////////////////////
unsigned int PhysxTriangleMesh::
get_reference_count() const {

  nassertr(_error_type == ET_ok, 0);

  return _ptr->getReferenceCount();
}

