// Filename: physxBoxControllerDesc.h
// Created by:  enn0x (22Sep09)
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

#ifndef PHYSXBOXCONTROLLERDESC_H
#define PHYSXBOXCONTROLLERDESC_H

#include "pandabase.h"
#include "lvector3.h"

#include "physxControllerDesc.h"

#include "NoMinMax.h"
#include "NxPhysics.h"
#include "NxBoxController.h"

////////////////////////////////////////////////////////////////////
//       Class : PhysxBoxControllerDesc
// Description : Descriptor class for PhysxBoxController.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAPHYSX PhysxBoxControllerDesc : public PhysxControllerDesc {

PUBLISHED:
  INLINE PhysxBoxControllerDesc();
  INLINE ~PhysxBoxControllerDesc();

  INLINE void set_to_default();
  INLINE bool is_valid() const;

  void set_extents(const LVector3f &extents);

  LVector3f get_extents() const;

public:
  INLINE PhysxBoxControllerDesc(NxBoxControllerDesc &desc);

  virtual NxControllerDesc *ptr() const { return (NxControllerDesc *)&_desc; };

private:
  NxBoxControllerDesc _desc;

////////////////////////////////////////////////////////////////////
public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    PhysxControllerDesc::init_type();
    register_type(_type_handle, "PhysxBoxControllerDesc", 
                  PhysxControllerDesc::get_class_type());
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

#include "physxBoxControllerDesc.I"

#endif // PHYSXBOXCONTROLLERDESC_H