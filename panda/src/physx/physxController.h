// Filename: physxController.h
// Created by:  enn0x (24Sep09)
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

#ifndef PHYSXCONTROLLER_H
#define PHYSXCONTROLLER_H

#include "pandabase.h"
#include "pointerTo.h"
#include "lpoint3.h"
#include "lmatrix.h"
#include "lquaternion.h"

#include "physxObject.h"
#include "physxEnums.h"

#include "NxController.h"

class PhysxActor;

////////////////////////////////////////////////////////////////////
//       Class : PhysxController
// Description : Abstract base class for character controllers.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAPHYSX PhysxController : public PhysxObject, public PhysxEnums {

PUBLISHED:
  INLINE PhysxController();
  INLINE ~PhysxController();

  PT(PhysxActor) get_actor() const;

  void set_pos(const LPoint3f &pos);
  void set_sharpness(float sharpness);
  void set_collision(bool enable);

  LPoint3f get_pos() const;
  float get_sharpness() const;

/*
  void set_global_speed(const LVector2f &speed);
  void set_global_speed(const LVector3f &speed);
  void set_local_speed(const LVector2f &speed);
  void set_local_speed(const LVector3f &speed);

  void set_omega(float omega);
  void set_h(float heading);
  float get_h() const;
  void report_scene_changed();
  void start_jump(float v0);
  void stop_jump();
  void set_min_distance(float minDist);
  void set_step_offset(float offset);

public:
  void update(float dt);

protected:
  NxReal get_jump_height(float dt, float G);

private:
  NxControllerManager *_cm;

  LVector3f _speed;
  float _omega;
  float _heading;

  PT(PhysxActor) _actor;

  bool  _jumping;
  float _jumpTime;
  float _v0;

  float _minDist;
*/

private:
  float _sharpness;

////////////////////////////////////////////////////////////////////
PUBLISHED:
  void release();

public:
  static PT(PhysxController) factory(NxControllerType shapeType);

  virtual NxController *ptr() const = 0;

  virtual void link(NxController *controllerPtr) = 0;
  virtual void unlink() = 0;

////////////////////////////////////////////////////////////////////
public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    PhysxObject::init_type();
    register_type(_type_handle, "PhysxController", 
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

#include "physxController.I"

#endif // PHYSXCONTROLLER_H
