// Filename: multiplexStreamBuf.cxx
// Created by:  drose (27Nov00)
// 
////////////////////////////////////////////////////////////////////

#include "multiplexStreamBuf.h"

// We use real assert() instead of nassert(), because we're likely
// to be invoked directly by notify here, and we don't want to
// risk infinite recursion.
#include <assert.h>

#ifndef HAVE_STREAMSIZE
// Some compilers--notably SGI--don't define this for us.
typedef int streamsize;
#endif

////////////////////////////////////////////////////////////////////
//     Function: MultiplexStreamBuf::Output::wrinte_string
//       Access: Public
//  Description: Dumps the indicated string to the appropriate place.
////////////////////////////////////////////////////////////////////
void MultiplexStreamBuf::Output::
write_string(const string &str) {
  switch (_output_type) {
  case OT_ostream:
    assert(_out != (ostream *)NULL);
    _out->write(str.data(), str.length());
    break;

  case OT_system_debug:
    break;
  }
}

static char test[1024];

////////////////////////////////////////////////////////////////////
//     Function: MultiplexStreamBuf::Constructor
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
MultiplexStreamBuf::
MultiplexStreamBuf() {
  allocate();
  setp(base(), ebuf());
}

////////////////////////////////////////////////////////////////////
//     Function: MultiplexStreamBuf::Destructor
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
MultiplexStreamBuf::
~MultiplexStreamBuf() {
  sync();
}

////////////////////////////////////////////////////////////////////
//     Function: MultiplexStreamBuf::sync
//       Access: Public, Virtual
//  Description: Called by the system ostream implementation when the
//               buffer should be flushed to output (for instance, on
//               destruction).
////////////////////////////////////////////////////////////////////
int MultiplexStreamBuf::
sync() { 
  streamsize n = pptr() - pbase();

  // We pass in false for the flush value, even though our
  // transmitting ostream said to sync.  This allows us to get better
  // line buffering, since our transmitting ostream is often set
  // unitbuf, and might call sync multiple times in one line.  We
  // still have an explicit flush() call to force the issue.
  write_chars(pbase(), n, false);
  pbump(-n);

  return 0;  // Return 0 for success, EOF to indicate write full.
}

////////////////////////////////////////////////////////////////////
//     Function: MultiplexStreamBuf::overflow
//       Access: Public, Virtual
//  Description: Called by the system ostream implementation when its
//               internal buffer is filled, plus one character.
////////////////////////////////////////////////////////////////////
int MultiplexStreamBuf::
overflow(int ch) { 
  streamsize n = pptr() - pbase();

  if (n != 0) {
    write_chars(pbase(), n, false);
    pbump(-n);  // Reset pptr().
  }

  if (ch != EOF) {
    // Write one more character.
    char c = ch;
    write_chars(&c, 1, false);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: MultiplexStreamBuf::write_chars
//       Access: Private
//  Description: An internal function called by sync() and overflow()
//               to store one or more characters written to the stream
//               into the memory buffer.
////////////////////////////////////////////////////////////////////
void MultiplexStreamBuf::
write_chars(const char *start, int length, bool flush) {
  size_t orig = _line_buffer.length();
  string latest(start, length);
  string line;

  if (flush) {
    // If we're to flush the stream now, we dump the whole thing
    // regardless of whether we have reached end-of-line.
    line = _line_buffer + latest;
    _line_buffer = "";

  } else {
    // Otherwise, we check for the end-of-line character, for our
    // ostreams that only want a complete line at a time.
    _line_buffer += latest;
    size_t eol = _line_buffer.rfind('\n', orig);
    if (eol != string::npos) {
      line = _line_buffer.substr(0, eol + 1);
      _line_buffer = _line_buffer.substr(eol + 1);
    }
  }

  Outputs::iterator oi;
  for (oi = _outputs.begin(); oi != _outputs.end(); ++oi) {
    Output &out = (*oi);
    switch (out._buffer_type) {
    case BT_none:
      // No buffering: send all new characters directly to the ostream.
      if (!latest.empty()) {
	out.write_string(latest);
      }
      break;

    case BT_line:
      // Line buffering: send only when a complete line has been
      // received.
      if (!line.empty()) {
	out.write_string(line);
      }
      break;
    }
  }
  
}
