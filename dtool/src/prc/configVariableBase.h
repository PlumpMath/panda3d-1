// Filename: configVariableBase.h
// Created by:  drose (21Oct04)
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

#ifndef CONFIGVARIABLEBASE_H
#define CONFIGVARIABLEBASE_H

#include "dtoolbase.h"
#include "configFlags.h"
#include "configVariableCore.h"
#include "configDeclaration.h"
#include "configVariableManager.h"
#include "vector_string.h"

////////////////////////////////////////////////////////////////////
//       Class : ConfigVariableBase
// Description : This class is the base class for both
//               ConfigVariableList and ConfigVariable (and hence for
//               all of the ConfigVariableBool, ConfigVaribleString,
//               etc. classes).  It collects together the common
//               interface for all generic ConfigVariables.
//
//               Mostly, this class serves as a thin wrapper around
//               ConfigVariableCore and/or ConfigDeclaration, more or
//               less duplicating the interface presented there.
////////////////////////////////////////////////////////////////////
class EXPCL_DTOOLCONFIG ConfigVariableBase : public ConfigFlags {
protected:
  INLINE ConfigVariableBase(const string &name, ValueType type);
  ConfigVariableBase(const string &name, ValueType type,
                     int flags, const string &description);
  INLINE ~ConfigVariableBase();

PUBLISHED:
  INLINE const string &get_name() const;

  INLINE ValueType get_value_type() const;
  INLINE int get_flags() const;
  INLINE bool is_closed() const;
  INLINE int get_trust_level() const;
  INLINE bool is_dynamic() const;
  INLINE const string &get_description() const;

  INLINE bool clear_local_value();
  INLINE bool has_local_value() const;
  INLINE bool has_value() const;
  
  INLINE void output(ostream &out) const;
  INLINE void write(ostream &out) const;

protected:
  ConfigVariableCore *_core;
};

INLINE ostream &operator << (ostream &out, const ConfigVariableBase &variable);

#include "configVariableBase.I"

#endif
