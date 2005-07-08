// Filename: geomVertexAnimationSpec.h
// Created by:  drose (29Mar05)
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

#ifndef GEOMVERTEXANIMATIONSPEC_H
#define GEOMVERTEXANIMATIONSPEC_H

#include "pandabase.h"
#include "geomEnums.h"

class BamWriter;
class BamReader;
class Datagram;
class DatagramIterator;

////////////////////////////////////////////////////////////////////
//       Class : GeomVertexAnimationSpec
// Description : This object describes how the vertex animation, if
//               any, represented in a GeomVertexData is encoded.
//
//               Vertex animation includes soft-skinned skeleton
//               animation and morphs (blend shapes), and might be
//               performed on the CPU by Panda, or passed down to the
//               graphics backed to be performed on the hardware
//               (depending on the hardware's advertised
//               capabilities).
//
//               Changing this setting doesn't by itself change the
//               way the animation is actually performed; this just
//               specifies how the vertices are set up to be animated.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA GeomVertexAnimationSpec : public GeomEnums {
PUBLISHED:
  INLINE GeomVertexAnimationSpec();
  INLINE GeomVertexAnimationSpec(const GeomVertexAnimationSpec &other);
  INLINE void operator = (const GeomVertexAnimationSpec &other);

  INLINE AnimationType get_animation_type() const;

  INLINE int get_num_transforms() const;
  INLINE bool get_indexed_transforms() const;

  INLINE void set_none();
  INLINE void set_panda();
  INLINE void set_hardware(int num_transforms, bool indexed_transforms);

  void output(ostream &out) const;

public:
  INLINE bool operator < (const GeomVertexAnimationSpec &other) const;
  INLINE bool operator == (const GeomVertexAnimationSpec &other) const;
  INLINE bool operator != (const GeomVertexAnimationSpec &other) const;
  INLINE int compare_to(const GeomVertexAnimationSpec &other) const;

public:
  void write_datagram(BamWriter *manager, Datagram &dg);
  void fillin(DatagramIterator &scan, BamReader *manager);

private:  
  AnimationType _animation_type;

  int _num_transforms;
  bool _indexed_transforms;
};

INLINE ostream &
operator << (ostream &out, const GeomVertexAnimationSpec &animation);

#include "geomVertexAnimationSpec.I"

#endif
