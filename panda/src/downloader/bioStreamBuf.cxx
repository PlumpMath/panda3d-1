// Filename: bioStreamBuf.cxx
// Created by:  drose (25Sep02)
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

#include "bioStreamBuf.h"

#ifdef HAVE_SSL

#ifndef HAVE_STREAMSIZE
// Some compilers (notably SGI) don't define this for us
typedef int streamsize;
#endif /* HAVE_STREAMSIZE */

////////////////////////////////////////////////////////////////////
//     Function: BioStreamBuf::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
BioStreamBuf::
BioStreamBuf() {
  _source = (BIO *)NULL;
  _owns_source = false;

#ifdef WIN32_VC
  // In spite of the claims of the MSDN Library to the contrary,
  // Windows doesn't seem to provide an allocate() function, so we'll
  // do it by hand.
  char *buf = new char[4096];
  char *ebuf = buf + 4096;
  setg(buf, ebuf, ebuf);
  setp(buf, ebuf);

#else
  allocate();
  setg(base(), ebuf(), ebuf());
  setp(base(), ebuf());
#endif
}

////////////////////////////////////////////////////////////////////
//     Function: BioStreamBuf::Destructor
//       Access: Public, Virtual
//  Description:
////////////////////////////////////////////////////////////////////
BioStreamBuf::
~BioStreamBuf() {
  close_read();
}

////////////////////////////////////////////////////////////////////
//     Function: BioStreamBuf::open_read
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
void BioStreamBuf::
open_read(BIO *source, bool owns_source) {
  _source = source;
  _owns_source = owns_source;
}

////////////////////////////////////////////////////////////////////
//     Function: BioStreamBuf::close_read
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
void BioStreamBuf::
close_read() {
  if (_source != (BIO *)NULL) {
    if (_owns_source) {
      BIO_free_all(_source);
      _owns_source = false;
    }
    _source = (BIO *)NULL;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: BioStreamBuf::underflow
//       Access: Protected, Virtual
//  Description: Called by the system istream implementation when its
//               internal buffer needs more characters.
////////////////////////////////////////////////////////////////////
int BioStreamBuf::
underflow() {
  // Sometimes underflow() is called even if the buffer is not empty.
  if (gptr() >= egptr()) {
    size_t buffer_size = egptr() - eback();
    gbump(-(int)buffer_size);

    size_t num_bytes = buffer_size;
    size_t read_count = BIO_read(_source, gptr(), buffer_size);

    if (read_count != num_bytes) {
      // Oops, we didn't read what we thought we would.
      if (read_count == 0) {
        return EOF;
      }

      // Slide what we did read to the top of the buffer.
      nassertr(read_count < num_bytes, EOF);
      size_t delta = num_bytes - read_count;
      memmove(gptr() + delta, gptr(), read_count);
      gbump(delta);
    }
  }

  return (unsigned char)*gptr();
}


#endif  // HAVE_SSL
