// Filename: lwoLayer.cxx
// Created by:  drose (24Apr01)
// 
////////////////////////////////////////////////////////////////////

#include "lwoLayer.h"
#include "lwoInputFile.h"

#include <indent.h>

TypeHandle LwoLayer::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: LwoLayer::read_iff
//       Access: Public, Virtual
//  Description: Reads the data of the chunk in from the given input
//               file, if possible.  The ID and length of the chunk
//               have already been read.  stop_at is the byte position
//               of the file to stop at (based on the current position
//               at in->get_bytes_read()).  Returns true on success,
//               false otherwise.
////////////////////////////////////////////////////////////////////
bool LwoLayer::
read_iff(IffInputFile *in, size_t stop_at) {
  LwoInputFile *lin = DCAST(LwoInputFile, in);

  _number = lin->get_be_uint16();
  _flags = lin->get_be_uint16();
  _pivot = lin->get_vec3();
  _name = lin->get_string();

  if (lin->get_bytes_read() >= stop_at) {
    _parent = -1;
  } else {
    _parent = lin->get_be_uint16();
    if (_parent == 0xffff) {
      _parent = -1;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: LwoLayer::write
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void LwoLayer::
write(ostream &out, int indent_level) const {
  indent(out, indent_level)
    << get_id() << " { number = " << _number << ", flags = 0x" 
    << hex << _flags << dec << ", pivot = " << _pivot
    << ", _name = \"" << _name << "\", _parent = " << _parent << " }\n";
}
