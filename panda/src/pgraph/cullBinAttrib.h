// Filename: cullBinAttrib.h
// Created by:  drose (01Mar02)
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

#ifndef CULLBINATTRIB_H
#define CULLBINATTRIB_H

#include "pandabase.h"

#include "renderAttrib.h"

class FactoryParams;

////////////////////////////////////////////////////////////////////
//       Class : CullBinAttrib
// Description : Assigns geometry to a particular bin by name.  The
//               bins must be created separately via the
//               CullBinManager interface.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA_PGRAPH CullBinAttrib : public RenderAttrib {
private:
  INLINE CullBinAttrib();

PUBLISHED:
  static CPT(RenderAttrib) make(const string &bin_name, int draw_order);
  static CPT(RenderAttrib) make_default();

  INLINE const string &get_bin_name() const;
  INLINE int get_draw_order() const;

public:
  virtual void output(ostream &out) const;

protected:
  virtual int compare_to_impl(const RenderAttrib *other) const;
  virtual size_t get_hash_impl() const;

private:
  string _bin_name;
  int _draw_order;

PUBLISHED:
  static int get_class_slot() {
    return _attrib_slot;
  }
  virtual int get_slot() const {
    return get_class_slot();
  }

public:
  static void register_with_read_factory();
  virtual void write_datagram(BamWriter *manager, Datagram &dg);

protected:
  static TypedWritable *make_from_bam(const FactoryParams &params);
  void fillin(DatagramIterator &scan, BamReader *manager);
  
public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    RenderAttrib::init_type();
    register_type(_type_handle, "CullBinAttrib",
                  RenderAttrib::get_class_type());
    _attrib_slot = register_slot(_type_handle, 100, make_default);
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
  static int _attrib_slot;
};

#include "cullBinAttrib.I"

#endif

