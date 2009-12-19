// Filename: physxTriangleMeshDesc.h
// Created by:  enn0x (11Oct09)
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

#ifndef PHYSXTRIANGLEMESHDESC_H
#define PHYSXTRIANGLEMESHDESC_H

#include "pandabase.h"
#include "lpoint3.h"

#include "NoMinMax.h"
#include "NxPhysics.h"

////////////////////////////////////////////////////////////////////
//       Class : PhysxTriangleMeshDesc
// Description :
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAPHYSX PhysxTriangleMeshDesc {

PUBLISHED:
  INLINE PhysxTriangleMeshDesc();
  INLINE ~PhysxTriangleMeshDesc();

  INLINE bool is_valid() const;

  void set_num_vertices(unsigned int n);
  void set_vertex(unsigned int idx, const LPoint3f &vert);

  void set_num_triangles(unsigned int n);
  void set_triangle(unsigned int idx, unsigned int i1, unsigned int i2, unsigned int i3);

public:
  const NxTriangleMeshDesc &get_desc() const;

private:
  NxVec3 *_vertices;
  NxU32 *_triangles;
  NxTriangleMeshDesc _desc;
};

#include "physxTriangleMeshDesc.I"

#endif // PHYSXTRIANGLEMESHDESC_H