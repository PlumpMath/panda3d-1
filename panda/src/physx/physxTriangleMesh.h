// Filename: physxTriangleMesh.h
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

#ifndef PHYSXTRIANGLEMESH_H
#define PHYSXTRIANGLEMESH_H

#include "pandabase.h"

#include "physxObject.h"

#include "NoMinMax.h"
#include "NxPhysics.h"

////////////////////////////////////////////////////////////////////
//       Class : PhysxTriangleMesh
// Description : 
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAPHYSX PhysxTriangleMesh : public PhysxObject {

PUBLISHED:
  INLINE PhysxTriangleMesh();
  INLINE ~PhysxTriangleMesh();

  unsigned int get_reference_count() const;

////////////////////////////////////////////////////////////////////
PUBLISHED:
  void release();

public:
  INLINE NxTriangleMesh *ptr() const { return _ptr; };

  void link(NxTriangleMesh *meshPtr);
  void unlink();

private:
  NxTriangleMesh *_ptr;

////////////////////////////////////////////////////////////////////
public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    PhysxObject::init_type();
    register_type(_type_handle, "PhysxTriangleMesh", 
                  PhysxObject::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {
    init_type();
    return get_class_type();
  }

private:
  static TypeHandle _type_handle;
};

#include "physxTriangleMesh.I"

#endif // PHYSXTRIANGLEMESH_H
