// Filename: multiplexStream.h
// Created by:  drose (27Nov00)
// 
////////////////////////////////////////////////////////////////////

#ifndef MULTIPLEXSTREAM_H
#define MULTIPLEXSTREAM_H

#include <pandabase.h>

#include "multiplexStreamBuf.h"

#include <filename.h>

////////////////////////////////////////////////////////////////////
//       Class : MultiplexStream
// Description : This is a special ostream that forwards the data that
//               is written to it to any number of other sources, for
//               instance other ostreams, or explicitly to a disk file
//               or to system logging utilities.  It's a very handy
//               thing to set Notify to refer to when running in batch
//               mode.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAEXPRESS MultiplexStream : public ostream {
PUBLISHED:
  INLINE MultiplexStream();

  INLINE void add_ostream(ostream *out, bool delete_later = false);
  INLINE void add_standard_output();
  INLINE bool add_file(Filename file);
  INLINE void add_system_debug();

  INLINE void flush();

private:
  MultiplexStreamBuf _msb;
};

#include "multiplexStream.I"

#endif
