// Filename: eggPolysetMaker.h
// Created by:  drose (19Jun01)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://www.panda3d.org/license.txt .
//
// To contact the maintainers of this program write to
// panda3d@yahoogroups.com .
//
////////////////////////////////////////////////////////////////////

#ifndef EGGPOLYSETMAKER_H
#define EGGPOLYSETMAKER_H

#include "pandabase.h"

#include "eggBinMaker.h"

#include "dcast.h"

///////////////////////////////////////////////////////////////////
//       Class : EggPolysetMaker
// Description : A specialization on EggBinMaker for making polysets
//               that share the same basic rendering characteristic.
//               This really just defines the example functions
//               described in the leading comment to EggBinMaker.
//
//               It makes some common assumptions about how polysets
//               should be grouped; if these are not sufficient, you
//               can always rederive your own further specialization
//               of this class.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAEGG EggPolysetMaker : public EggBinMaker {
PUBLISHED:
  // The BinNumber serves to identify why a particular EggBin was
  // created.
  enum BinNumber {
    BN_none = 0,
    BN_polyset,
  };

  enum Properties {
    P_has_texture        = 0x001,
    P_texture            = 0x002,
    P_has_material       = 0x004,
    P_material           = 0x008,
    P_has_poly_color     = 0x010,
    P_poly_color         = 0x020,
    P_has_poly_normal    = 0x040,
    P_has_vertex_normal  = 0x080,
    P_has_vertex_color   = 0x100,
    P_bface              = 0x200,
  };

  EggPolysetMaker();
  void set_properties(int properties);

public:
  virtual int
  get_bin_number(const EggNode *node);

  virtual bool
  sorts_less(int bin_number, const EggNode *a, const EggNode *b);

private:
  int _properties;
};


#endif
