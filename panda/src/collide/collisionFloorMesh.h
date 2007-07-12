// Filename: collisionFloorMesh.h
// Created by:  zpavlov (28Jun07)
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

#ifndef COLLISIONFLOORMESH_H
#define COLLISIONFLOORMESH_H

#include "pandabase.h"

#include "collisionPlane.h"
#include "clipPlaneAttrib.h"
#include "look_at.h"

#include "vector_LPoint2f.h"

class GeomNode;

////////////////////////////////////////////////////////////////////
//       Class : CollisionFloorMesh
// Description : This object represents a solid made entirely of triangles, which
//               will only be tested again z axis aligned rays
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA CollisionFloorMesh : public CollisionSolid {
public:
  typedef struct {
    unsigned int p1;
    unsigned int p2;
    unsigned int p3;
    float min_x;
    float max_x;
    float min_y;
    float max_y;
  } TriangleIndices;


PUBLISHED:

  INLINE CollisionFloorMesh();
  
  INLINE void add_vertex(const LPoint3f &vert);
  void add_triangle(unsigned int pointA, unsigned int pointB, unsigned int pointC);

  INLINE const unsigned int get_num_vertices() const;
  INLINE const unsigned int get_num_triangles() const;
  INLINE const LPoint3d get_triangle(unsigned int index) const;
  INLINE const LPoint3f get_vertex(unsigned int index) const;
  
  virtual LPoint3f get_collision_origin() const;

public:
  CollisionFloorMesh(const CollisionFloorMesh &copy);
  virtual CollisionSolid *make_copy();

public:
  
  virtual void xform(const LMatrix4f &mat);

  virtual PStatCollector &get_volume_pcollector();
  virtual PStatCollector &get_test_pcollector();

  virtual void output(ostream &out) const;
  virtual void write(ostream &out, int indent_level = 0) const;

  INLINE static void flush_level();

protected:
  virtual PT(BoundingVolume) compute_internal_bounds() const;

  virtual PT(CollisionEntry)
    test_intersection_from_ray(const CollisionEntry &entry) const;
  virtual void fill_viz_geom();

private:
  typedef pvector< LPoint3f > Vertices;

  typedef pvector< TriangleIndices > Triangles;

  Vertices _vertices;
  Triangles _triangles;

  
  static PStatCollector _volume_pcollector;
  static PStatCollector _test_pcollector;

protected:
  void fillin(DatagramIterator& scan, BamReader* manager);

public:
  static void register_with_read_factory();
  virtual void write_datagram(BamWriter* manager, Datagram &me);

  static TypedWritable *make_CollisionFloorMesh(const FactoryParams &params);  
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    CollisionSolid::init_type();
    register_type(_type_handle, "CollisionFloorMesh",
                  CollisionSolid::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

#include "collisionFloorMesh.I"

#endif


