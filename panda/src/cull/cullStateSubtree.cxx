// Filename: cullStateSubtree.cxx
// Created by:  drose (09Apr00)
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

#if defined(WIN32_VC) && !defined(NO_PCH)
#include "cull_headers.h"
#endif

#pragma hdrstop

#if !defined(WIN32_VC) || defined(NO_PCH)
#include "cullStateSubtree.h"
#include "config_cull.h"
#endif


////////////////////////////////////////////////////////////////////
//     Function: CullStateSubtree::Constructor
//       Access: Public, Virtual
//  Description:
////////////////////////////////////////////////////////////////////
CullStateSubtree::
~CullStateSubtree() {
}

////////////////////////////////////////////////////////////////////
//     Function: CullStateSubtree::check_currency
//       Access: Public
//  Description: Returns true if the CullStateSubtree is fresh enough to
//               still apply to the indicated cached transition, false
//               if it should be recomputed.
////////////////////////////////////////////////////////////////////
bool CullStateSubtree::
check_currency(const AllTransitionsWrapper &, Node *top_subtree,
               UpdateSeq as_of) {
  if (cull_cat.is_spam()) {
    cull_cat.spam()
      << "Checking currency for subtree " << (void *)this
      << ", _verified = " << _verified << " as_of = " << as_of << "\n";
  }

  // Make sure we've still got the same top_subtree node.
  if (_top_subtree != top_subtree) {
    return false;
  }

  // First, check the verified time stamp.
  if (as_of <= _verified && !_verified.is_fresh()) {
    return true;
  }

  // Now we should verify the set of transitions individually.  Skip
  // that for now.

  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: CullStateLookup::get_top_subtree
//       Access: Public, Virtual
//  Description: Returns the node that represents the top of the
//               subtree for this particular CullStateSubtree.
////////////////////////////////////////////////////////////////////
Node *CullStateSubtree::
get_top_subtree() const {
  return _top_subtree;
}

////////////////////////////////////////////////////////////////////
//     Function: CullStateSubtree::compose_trans
//       Access: Public, Virtual
//  Description: Composes the transitions given in "from" with the net
//               transitions that appear above the top node of this
//               subtree, and returns the result in "to".  This is a
//               pass-thru for CullStateLookup; it has effect only for
//               CullStateSubtree.
////////////////////////////////////////////////////////////////////
void CullStateSubtree::
compose_trans(const AllTransitionsWrapper &from,
              AllTransitionsWrapper &to) const {
  to = _trans_from_root;
  to.compose_in_place(from);
}
