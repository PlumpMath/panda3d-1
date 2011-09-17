// Filename: stTerrain.cxx
// Created by:  drose (11Oct10)
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

#include "stTerrain.h"
#include "indent.h"

TypeHandle STTerrain::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::Constructor
//       Access: Protected
//  Description: 
////////////////////////////////////////////////////////////////////
STTerrain::
STTerrain() {
  _is_valid = false;
  _min_height = 0.0f;
  _max_height = 1.0f;
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::Copy Constructor
//       Access: Protected
//  Description: 
////////////////////////////////////////////////////////////////////
STTerrain::
STTerrain(const STTerrain &copy) :
  TypedReferenceCount(copy),
  Namable(copy),
  _is_valid(copy._is_valid),
  _normal_map(copy._normal_map),
  _splat_layers(copy._splat_layers),
  _min_height(copy._min_height),
  _max_height(copy._max_height)
{
  set_vertex_format(copy._vertex_format);
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::Destructor
//       Access: Published, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
STTerrain::
~STTerrain() {
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::clear
//       Access: Published, Virtual
//  Description: Resets the terrain to its initial, unloaded state.
////////////////////////////////////////////////////////////////////
void STTerrain::
clear() {
  _is_valid = false;
  _min_height = 0.0f;
  _max_height = 1.0f;

  _normal_map = "";
  _splat_map = "";
  _splat_layers.clear();

  set_vertex_format(NULL);
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::load_data
//       Access: Published, Virtual
//  Description: This will be called at some point after
//               initialization.  It should be overridden by a derived
//               class to load up the terrain data from its source and
//               fill in the data members of this class appropriately,
//               especially _is_valid.  After this call, if _is_valid
//               is true, then get_height() etc. will be called to
//               query the terrain's data.
////////////////////////////////////////////////////////////////////
void STTerrain::
load_data() {
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::get_height
//       Access: Published, Virtual
//  Description: After load_data() has been called, this should return
//               the computed height value at point (x, y) of the
//               terrain, where x and y are unbounded and may refer to
//               any 2-d point in space.
////////////////////////////////////////////////////////////////////
float STTerrain::
get_height(float x, float y) const {
  return 0.0f;
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::get_smooth_height
//       Access: Published, Virtual
//  Description: After load_data() has been called, this should return
//               the approximate average height value over a circle of
//               the specified radius, centered at point (x, y) of the
//               terrain.
////////////////////////////////////////////////////////////////////
float STTerrain::
get_smooth_height(float x, float y, float radius) const {
  return get_height(x, y);
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::get_slope
//       Access: Published, Virtual
//  Description: After load_data() has been called, this should return
//               the directionless slope at point (x, y) of the
//               terrain, where 0.0 is flat and 1.0 is vertical.  This
//               is used for determining the legal points to place
//               trees and grass.
////////////////////////////////////////////////////////////////////
float STTerrain::
get_slope(float x, float y) const {
  return 0.0f;
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::placement_is_acceptable
//       Access: Published
//  Description: Returns true if the elevation and slope of point (x,
//               y) fall within the requested limits, false otherwise.
////////////////////////////////////////////////////////////////////
bool STTerrain::
placement_is_acceptable(float x, float y,
                        float height_min, float height_max, 
                        float slope_min, float slope_max) {
  float height = get_height(x, y);
  if (height < height_min || height > height_max) {
    return false;
  }

  float slope = get_slope(x, y);
  if (slope < slope_min || slope > slope_max) {
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::fill_vertices
//       Access: Published, Virtual
//  Description: After load_data() has been called, this will be
//               called occasionally to populate the vertices for a
//               terrain cell.
//
//               It will be passed a GeomVertexData whose format will
//               match get_vertex_format(), and already allocated with
//               num_xy * num_xy rows.  This method should fill the
//               rows of the data with the appropriate vertex data for
//               the terrain, over the grid described by the corners
//               (start_x, start_y) up to and including (start_x +
//               size_x, start_y + size_xy)--a square of the terrain
//               with num_xy vertices on a side, arranged in row-major
//               order.
////////////////////////////////////////////////////////////////////
void STTerrain::
fill_vertices(GeomVertexData *data,
              float start_x, float start_y,
              float size_xy, int num_xy) const {
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::output
//       Access: Published, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void STTerrain::
output(ostream &out) const {
  Namable::output(out);
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::write
//       Access: Published, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void STTerrain::
write(ostream &out, int indent_level) const {
  indent(out, indent_level)
    << *this << "\n";
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::get_st_vertex_format
//       Access: Public
//  Description: Returns a pointer to the SpeedTree array of vertex
//               attribs that defines the vertex format for SpeedTree.
////////////////////////////////////////////////////////////////////
const SpeedTree::SVertexAttribDesc *STTerrain::
get_st_vertex_format() const {
  //  return SpeedTree::std_vertex_format;
  nassertr(!_st_vertex_attribs.empty(), NULL);

  return &_st_vertex_attribs[0];
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::set_vertex_format
//       Access: Protected
//  Description: Should be called in load_data() by a derived class to
//               fill in the _vertex_format member.  This will also
//               compute and store the appropriate value for
//               _st_vertex_attribs.
////////////////////////////////////////////////////////////////////
bool STTerrain::
set_vertex_format(const GeomVertexFormat *format) {
  if (format == NULL) {
    _vertex_format = NULL;
    _st_vertex_attribs.clear();
    _is_valid = false;
    return true;
  }

  _vertex_format = GeomVertexFormat::register_format(format);
  if (!convert_vertex_format(_st_vertex_attribs, _vertex_format)) {
    _is_valid = false;
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::convert_vertex_format
//       Access: Protected, Static
//  Description: Populates the indicated st_vertex_attribs vector with
//               an array of SpeedTree vertex attribute entries that
//               corresponds to the requested format.  Returns true on
//               success, or false if the format cannot be represented
//               in SpeedTree.
////////////////////////////////////////////////////////////////////
bool STTerrain::
convert_vertex_format(STTerrain::VertexAttribs &st_vertex_attribs,
                      const GeomVertexFormat *format) {
  st_vertex_attribs.clear();

  if (format->get_num_arrays() != 1) {
    speedtree_cat.error()
      << "Cannot represent multi-array vertex format in SpeedTree.\n";
    return false;
  }

  const GeomVertexArrayFormat *array = format->get_array(0);

  int num_columns = array->get_num_columns();
  for (int ci = 0; ci < num_columns; ++ci) {
    const GeomVertexColumn *column = array->get_column(ci);
    st_vertex_attribs.push_back(SpeedTree::SVertexAttribDesc());
    SpeedTree::SVertexAttribDesc &attrib = st_vertex_attribs.back();
    if (!convert_vertex_column(attrib, column)) {
      st_vertex_attribs.clear();
      return false;
    }
  }

  st_vertex_attribs.push_back(SpeedTree::st_attrib_end);
  
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: STTerrain::convert_vertex_column
//       Access: Protected, Static
//  Description: Converts the indicated vertex column definition to
//               the corresponding SpeedTree::SVertexAttribDesc
//               format.  Returns true on success, false on failure.
////////////////////////////////////////////////////////////////////
bool STTerrain::
convert_vertex_column(SpeedTree::SVertexAttribDesc &st_attrib,
                      const GeomVertexColumn *column) {
  switch (column->get_numeric_type()) {
  case GeomEnums::NT_float32:
    st_attrib.m_eDataType = SpeedTree::VERTEX_ATTRIB_TYPE_FLOAT;
    break;

  default:
    speedtree_cat.error()
      << "Unsupported vertex numeric type for " << *column << "\n";
    return false;
  }

  st_attrib.m_uiNumElements = column->get_num_components();

  if (column->get_name() == InternalName::get_vertex()) {
    st_attrib.m_eSemantic = SpeedTree::VERTEX_ATTRIB_SEMANTIC_POS;

  } else if (column->get_name() == InternalName::get_texcoord()) {
    st_attrib.m_eSemantic = SpeedTree::VERTEX_ATTRIB_SEMANTIC_TEXCOORD0;

  } else {
    speedtree_cat.error()
      << "Unsupported vertex semantic name for " << *column << "\n";
    return false;
  }

  nassertr(st_attrib.SizeOfAttrib() == column->get_total_bytes(), false);

  return true;
}
