// Filename: colladaElement.h
// Created by: Xidram (15Apr10)
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

#ifndef COLLADAELEMENT_H
#define COLLADAELEMENT_H

#include "config_collada.h"
#include "namable.h"

////////////////////////////////////////////////////////////////////
//       Class : ColladaElement
// Description : Object that represents the <element> COLLADA tag.
////////////////////////////////////////////////////////////////////
class EXPCL_COLLADA ColladaElement : public Namable {
PUBLISHED:
  INLINE virtual void clear();
  INLINE virtual bool load_xml(const TiXmlElement *element);
  INLINE virtual TiXmlElement *make_xml() const;
};

#include "colladaElement.I"

#endif

