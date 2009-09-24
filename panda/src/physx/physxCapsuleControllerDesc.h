// Filename: physxCapsuleControllerDesc.h
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

#ifndef PHYSXCAPSULECONTROLLERDESC_H
#define PHYSXCAPSULECONTROLLERDESC_H

#include "pandabase.h"
#include "physxControllerDesc.h"

#include "NxCapsuleController.h"

////////////////////////////////////////////////////////////////////
//       Class : PhysxCapsuleControllerDesc
// Description : Descriptor class for PhysxCapsuleController.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAPHYSX PhysxCapsuleControllerDesc : public PhysxControllerDesc {

PUBLISHED:
  INLINE PhysxCapsuleControllerDesc();
  INLINE ~PhysxCapsuleControllerDesc();

  INLINE void set_to_default();
  INLINE bool is_valid() const;

  void set_radius(float radius);
  void set_height(float height);

  float get_radius() const;
  float get_height() const;

public:
  INLINE PhysxCapsuleControllerDesc(NxCapsuleControllerDesc &desc);

  virtual NxControllerDesc *ptr() const { return (NxControllerDesc *)&_desc; };

private:
  NxCapsuleControllerDesc _desc;

////////////////////////////////////////////////////////////////////
public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    PhysxControllerDesc::init_type();
    register_type(_type_handle, "PhysxCapsuleControllerDesc", 
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

#include "physxCapsuleControllerDesc.I"

#endif // PHYSXCAPSULECONTROLLERDESC_H
