// Filename: pfmVizzer.cxx
// Created by:  drose (30Sep12)
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

#include "pfmVizzer.h"
#include "geomNode.h"
#include "geom.h"
#include "geomVertexData.h"
#include "geomVertexFormat.h"
#include "geomPoints.h"
#include "geomTriangles.h"
#include "geomVertexWriter.h"
#include "lens.h"
#include "config_grutil.h"

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::Constructor
//       Access: Published
//  Description: The PfmVizzer constructor receives a reference to a
//               PfmFile which it will operate on.  It does not keep
//               ownership of this reference; it is your
//               responsibility to ensure the PfmFile does not
//               destruct during the lifetime of the PfmVizzer.
////////////////////////////////////////////////////////////////////
PfmVizzer::
PfmVizzer(PfmFile &pfm) : _pfm(pfm) {
  _vis_inverse = false;
  _vis_2d = false;
  _vis_blend = NULL;
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::project
//       Access: Published
//  Description: Adjusts each (x, y, z) point of the Pfm file by
//               projecting it through the indicated lens, converting
//               each point to a (u, v, w) texture coordinate.  The
//               resulting file can be generated to a mesh (with
//               set_vis_inverse(true) and generate_vis_mesh())
//               that will apply the lens distortion to an arbitrary
//               texture image.
////////////////////////////////////////////////////////////////////
void PfmVizzer::
project(const Lens *lens) {
  nassertv(_pfm.is_valid());

  static LMatrix4f to_uv(0.5f, 0.0f, 0.0f, 0.0f,
                         0.0f, 0.5f, 0.0f, 0.0f, 
                         0.0f, 0.0f, 0.5f, 0.0f, 
                         0.5f, 0.5f, 0.5f, 1.0f);
  
  for (int yi = 0; yi < _pfm.get_y_size(); ++yi) {
    for (int xi = 0; xi < _pfm.get_x_size(); ++xi) {
      if (!_pfm.has_point(xi, yi)) {
        continue;
      }
      LPoint3f &p = _pfm.modify_point(xi, yi);

      LPoint3 film;
      lens->project(LCAST(PN_stdfloat, p), film);
      p = to_uv.xform_point(LCAST(PN_float32, film));
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::extrude
//       Access: Published
//  Description: Converts each (u, v, depth) point of the Pfm file to
//               an (x, y, z) point, by reversing project().  If the
//               original file is only a 1-d file, assumes that it is
//               a depth map with implicit (u, v) coordinates.
//
//               This method is only valid for a linear lens (e.g. a
//               PerspectiveLens or OrthographicLens).  Non-linear
//               lenses don't necessarily compute a sensible depth
//               coordinate.
////////////////////////////////////////////////////////////////////
void PfmVizzer::
extrude(const Lens *lens) {
  nassertv(_pfm.is_valid());
  nassertv(lens->is_linear());

  static LMatrix4 from_uv(2.0, 0.0, 0.0, 0.0,
                          0.0, 2.0, 0.0, 0.0,
                          0.0, 0.0, 2.0, 0.0,
                          -1.0, -1.0, -1.0, 1.0);
  const LMatrix4 &proj_mat_inv = lens->get_projection_mat_inv();

  PfmFile result;
  result.clear(_pfm.get_x_size(), _pfm.get_y_size(), 3);
  result.set_zero_special(true);

  if (_pfm.get_num_channels() == 1) {
    // Create an implicit UV coordinate for each point.
    LPoint2 uv_scale(1.0, 1.0);
    if (_pfm.get_x_size() > 1) {
      uv_scale[0] = 1.0 / PN_stdfloat(_pfm.get_x_size() - 1);
    }
    if (_pfm.get_y_size() > 1) {
      uv_scale[1] = 1.0 / PN_stdfloat(_pfm.get_y_size() - 1);
    }
    for (int yi = 0; yi < _pfm.get_y_size(); ++yi) {
      for (int xi = 0; xi < _pfm.get_x_size(); ++xi) {
        if (!_pfm.has_point(xi, yi)) {
          continue;
        }
        LPoint3 p;
        p.set((PN_stdfloat)xi * uv_scale[0],
              (PN_stdfloat)yi * uv_scale[1],
              (PN_stdfloat)_pfm.get_point1(xi, yi));
        
        from_uv.xform_point_in_place(p);
        proj_mat_inv.xform_point_general_in_place(p);
        result.set_point(xi, yi, p);
      }
    }
  } else {
    // Use the existing UV coordinate for each point.
    for (int yi = 0; yi < _pfm.get_y_size(); ++yi) {
      for (int xi = 0; xi < _pfm.get_x_size(); ++xi) {
        if (!_pfm.has_point(xi, yi)) {
          continue;
        }
        LPoint3 p;
        p = LCAST(PN_stdfloat, _pfm.get_point(xi, yi));
        
        from_uv.xform_point_in_place(p);
        proj_mat_inv.xform_point_general_in_place(p);
        result.set_point(xi, yi, p);
      }
    }
  }

  _pfm = result;
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::clear_vis_columns
//       Access: Published
//  Description: Removes all of the previously-added vis columns in
//               preparation for building a new list.  See
//               add_vis_column().
////////////////////////////////////////////////////////////////////
void PfmVizzer::
clear_vis_columns() {
  _vis_columns.clear();
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::add_vis_column
//       Access: Published
//  Description: Adds a new vis column specification to the list of
//               vertex data columns that will be generated at the
//               next call to generate_vis_points() or
//               generate_vis_mesh().  This advanced interface
//               supercedes the higher-level set_vis_inverse(),
//               set_flat_texcoord_name(), and set_vis_2d().
//
//               If you use this advanced interface, you must specify
//               explicitly the complete list of data columns to be
//               created in the resulting GeomVertexData, by calling
//               add_vis_column() each time.  For each column, you
//               specify the source of the column in the PFMFile, the
//               target column and name in the GeomVertexData, and an
//               optional transform matrix and/or lens to transform
//               and project the point before generating it.
////////////////////////////////////////////////////////////////////
void PfmVizzer::
add_vis_column(ColumnType source, ColumnType target,
               InternalName *name, const TransformState *transform,
               const Lens *lens) {
  add_vis_column(_vis_columns, source, target, name, transform, lens);
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::generate_vis_points
//       Access: Published
//  Description: Creates a point cloud with the points of the pfm as
//               3-d coordinates in space, and texture coordinates
//               ranging from 0 .. 1 based on the position within the
//               pfm grid.
////////////////////////////////////////////////////////////////////
NodePath PfmVizzer::
generate_vis_points() const {
  nassertr(_pfm.is_valid(), NodePath());

  CPT(GeomVertexFormat) format;
  if (_vis_inverse) {
    if (_vis_2d) {
      format = GeomVertexFormat::get_v3t2();
    } else {
      // We need a 3-d texture coordinate if we're inverting the vis
      // and it's 3-d.
      GeomVertexArrayFormat *v3t3 = new GeomVertexArrayFormat
        (InternalName::get_vertex(), 3, 
         Geom::NT_stdfloat, Geom::C_point,
         InternalName::get_texcoord(), 3, 
         Geom::NT_stdfloat, Geom::C_texcoord);
      format = GeomVertexFormat::register_format(v3t3);
    }
  } else {
    format = GeomVertexFormat::get_v3t2();
  }

  PT(GeomVertexData) vdata = new GeomVertexData
    ("points", format, Geom::UH_static);
  vdata->set_num_rows(_pfm.get_x_size() * _pfm.get_y_size());
  GeomVertexWriter vertex(vdata, InternalName::get_vertex());
  GeomVertexWriter texcoord(vdata, InternalName::get_texcoord());

  LPoint2f uv_scale(1.0, 1.0);
  if (_pfm.get_x_size() > 1) {
    uv_scale[0] = 1.0f / PN_float32(_pfm.get_x_size() - 1);
  }
  if (_pfm.get_y_size() > 1) {
    uv_scale[1] = 1.0f / PN_float32(_pfm.get_y_size() - 1);
  }

  int num_points = 0;
  for (int yi = 0; yi < _pfm.get_y_size(); ++yi) {
    for (int xi = 0; xi < _pfm.get_x_size(); ++xi) {
      if (!_pfm.has_point(xi, yi)) {
        continue;
      }

      const LPoint3f &point = _pfm.get_point(xi, yi);
      LPoint2f uv(PN_float32(xi) * uv_scale[0],
                  PN_float32(yi) * uv_scale[1]);
      if (_vis_inverse) {
        vertex.add_data2f(uv);
        texcoord.add_data3f(point);
      } else if (_vis_2d) {
        vertex.add_data2f(point[0], point[1]);
        texcoord.add_data2f(uv);
      } else {
        vertex.add_data3f(point);
        texcoord.add_data2f(uv);
      }
      ++num_points;
    }
  }
  
  PT(Geom) geom = new Geom(vdata);
  PT(GeomPoints) points = new GeomPoints(Geom::UH_static);
  points->add_next_vertices(num_points);
  geom->add_primitive(points);
  
  PT(GeomNode) gnode = new GeomNode("");
  gnode->add_geom(geom);
  return NodePath(gnode);
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::generate_vis_mesh
//       Access: Published
//  Description: Creates a triangle mesh with the points of the pfm as
//               3-d coordinates in space, and texture coordinates
//               ranging from 0 .. 1 based on the position within the
//               pfm grid.
////////////////////////////////////////////////////////////////////
NodePath PfmVizzer::
generate_vis_mesh(MeshFace face) const {
  nassertr(_pfm.is_valid(), NodePath());
  nassertr(face != 0, NodePath());

  if (_pfm.get_num_channels() == 1 && _vis_columns.empty()) {
    // If we're generating a default mesh from a one-channel pfm file,
    // expand it to a three-channel pfm file to make the visualization
    // useful.
    PfmFile expanded;
    expanded.clear_to_texcoords(_pfm.get_x_size(), _pfm.get_y_size());
    expanded.copy_channel(2, _pfm, 0);
    PfmVizzer exvizzer(expanded);
    return exvizzer.generate_vis_mesh(face);
  }
  
  if (_pfm.get_x_size() == 1 || _pfm.get_y_size() == 1) {
    // Can't generate a 1-d mesh, so generate points in this case.
    return generate_vis_points();
  }
  
  PT(GeomNode) gnode = new GeomNode("");

  if (face & MF_front) {
    make_vis_mesh_geom(gnode, false);
  }

  if (face & MF_back) {
    make_vis_mesh_geom(gnode, true);
  }

  return NodePath(gnode);
}


////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::calc_max_u_displacement
//       Access: Private
//  Description: Computes the maximum amount of shift, in pixels
//               either left or right, of any pixel in the distortion
//               map.  This can be passed to make_displacement(); see
//               that function for more information.
////////////////////////////////////////////////////////////////////
double PfmVizzer::
calc_max_u_displacement() const {
  int x_size = _pfm.get_x_size();
  int y_size = _pfm.get_y_size();

  double max_u = 0;

  for (int yi = 0; yi < y_size; ++yi) {
    for (int xi = 0; xi < x_size; ++xi) {
      if (!_pfm.has_point(xi, yi)) {
        continue;
      }

      const LPoint3f &point = _pfm.get_point(xi, yi);
      double nxi = point[0] * (double)(x_size - 1);

      max_u = max(max_u, cabs(nxi - (double)xi));
    }
  }

  return max_u;
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::calc_max_v_displacement
//       Access: Private
//  Description: Computes the maximum amount of shift, in pixels
//               either up or down, of any pixel in the distortion
//               map.  This can be passed to make_displacement(); see
//               that function for more information.
////////////////////////////////////////////////////////////////////
double PfmVizzer::
calc_max_v_displacement() const {
  int x_size = _pfm.get_x_size();
  int y_size = _pfm.get_y_size();

  double max_v = 0;

  for (int yi = 0; yi < y_size; ++yi) {
    for (int xi = 0; xi < x_size; ++xi) {
      if (!_pfm.has_point(xi, yi)) {
        continue;
      }

      const LPoint3f &point = _pfm.get_point(xi, yi);
      double nyi = point[1] * (double)(y_size - 1);

      max_v = max(max_v, cabs(nyi - (double)yi));
    }
  }

  return max_v;
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::make_displacement
//       Access: Private
//  Description: Assuming the underlying PfmFile is a 2-d distortion
//               mesh, with the U and V in the first two components
//               and the third component unused, this computes an
//               AfterEffects-style displacement map that represents
//               the same distortion.  The indicated PNMImage will be
//               filled in with a displacement map image, with
//               horizontal shift in the red channel and vertical
//               shift in the green channel, where a fully bright (or
//               fully black) pixel indicates a shift of max_u or
//               max_v pixels.
//
//               Use calc_max_u_displacement() and
//               calc_max_v_displacement() to compute suitable values
//               for max_u and max_v.
////////////////////////////////////////////////////////////////////
void PfmVizzer::
make_displacement(PNMImage &result, double max_u, double max_v) const {
  int x_size = _pfm.get_x_size();
  int y_size = _pfm.get_y_size();
  result.clear(x_size, y_size, 3, PNM_MAXMAXVAL);
  result.fill(0.5);

  for (int yi = 0; yi < y_size; ++yi) {
    for (int xi = 0; xi < x_size; ++xi) {
      if (!_pfm.has_point(xi, yi)) {
        continue;
      }

      const LPoint3f &point = _pfm.get_point(xi, yi);
      double nxi = point[0] * (double)(x_size - 1);
      double nyi = point[1] * (double)(y_size - 1);

      double u_shift = (nxi - (double)xi) / max_u;
      double v_shift = (nyi - (double)yi) / max_v;

      result.set_red(xi, yi, u_shift * 0.5 + 0.5);
      result.set_green(xi, yi, v_shift * 0.5 + 0.5);
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::make_vis_mesh_geom
//       Access: Private
//  Description: Returns a triangle mesh for the pfm.  If inverted is
//               true, the mesh is facing the opposite direction.
////////////////////////////////////////////////////////////////////
void PfmVizzer::
make_vis_mesh_geom(GeomNode *gnode, bool inverted) const {
  int num_x_cells = 1;
  int num_y_cells = 1;

  int x_size = _pfm.get_x_size();
  int y_size = _pfm.get_y_size();

  // This is the number of independent vertices we will require.
  int num_vertices = x_size * y_size;
  if (num_vertices == 0) {
    // Trivial no-op.
    return;
  }

  bool reverse_normals = inverted;
  bool reverse_faces = inverted;
  if (!is_right_handed(get_default_coordinate_system())) {
    reverse_faces = !reverse_faces;
  }

  // This is the max number of vertex indices we might add to the
  // GeomTriangles.  (We might actually add fewer than this due to
  // omitting the occasional missing data point.)
  int max_indices = (x_size - 1) * (y_size - 1) * 6;

  while (num_vertices > pfm_vis_max_vertices || max_indices > pfm_vis_max_indices) {
    // Too many vertices in one mesh.  Subdivide the mesh into smaller
    // pieces.
    if (num_x_cells > num_y_cells) {
      ++num_y_cells;
    } else {
      ++num_x_cells;
    }

    x_size = (_pfm.get_x_size() + num_x_cells - 1) / num_x_cells + 1;
    y_size = (_pfm.get_y_size() + num_y_cells - 1) / num_y_cells + 1;

    num_vertices = x_size * y_size;
    max_indices = (x_size - 1) * (y_size - 1) * 6;
  }

  // OK, now we know how many cells we need.
  if (grutil_cat.is_debug()) {
    grutil_cat.debug()
      << "Generating mesh with " << num_x_cells << " x " << num_y_cells
      << " pieces.\n";
  }

  VisColumns vis_columns = _vis_columns;
  if (vis_columns.empty()) {
    build_auto_vis_columns(vis_columns, true);
  }

  CPT(GeomVertexFormat) format = make_array_format(vis_columns);

  for (int yci = 0; yci < num_y_cells; ++yci) {
    int y_begin = (yci * _pfm.get_y_size()) / num_y_cells;
    int y_end = ((yci + 1) * _pfm.get_y_size()) / num_y_cells;

    // Include the first vertex from the next strip in this strip's
    // vertices, so we are connected.
    y_end = min(y_end + 1, _pfm.get_y_size());

    y_size = y_end - y_begin;
    if (y_size == 0) {
      continue;
    }

    for (int xci = 0; xci < num_x_cells; ++xci) {
      int x_begin = (xci * _pfm.get_x_size()) / num_x_cells;
      int x_end = ((xci + 1) * _pfm.get_x_size()) / num_x_cells;
      x_end = min(x_end + 1, _pfm.get_x_size());
      x_size = x_end - x_begin;
      if (x_size == 0) {
        continue;
      }

      num_vertices = x_size * y_size;
      max_indices = (x_size - 1) * (y_size - 1) * 6;

      ostringstream mesh_name;
      mesh_name << "mesh_" << xci << "_" << yci;
      PT(GeomVertexData) vdata = new GeomVertexData
        (mesh_name.str(), format, Geom::UH_static);

      vdata->set_num_rows(num_vertices);

      // Fill in all of the vertices.
      for (VisColumns::const_iterator vci = vis_columns.begin();
           vci != vis_columns.end();
           ++vci) {
        const VisColumn &column = *vci;
        GeomVertexWriter vwriter(vdata, column._name);
        vwriter.set_row(0);

        for (int yi = y_begin; yi < y_end; ++yi) {
          for (int xi = x_begin; xi < x_end; ++xi) {
            column.add_data(*this, vwriter, xi, yi, reverse_normals);
          }
        }
      }
  
      PT(Geom) geom = new Geom(vdata);
      PT(GeomTriangles) tris = new GeomTriangles(Geom::UH_static);

      tris->reserve_num_vertices(max_indices);

      for (int yi = y_begin; yi < y_end - 1; ++yi) {
        for (int xi = x_begin; xi < x_end - 1; ++xi) {

          if (_pfm.has_no_data_value()) {
            if (!_pfm.has_point(xi, yi) ||
                !_pfm.has_point(xi, yi + 1) ||
                !_pfm.has_point(xi + 1, yi + 1) ||
                !_pfm.has_point(xi + 1, yi)) {
              continue;
            }
          }

          int xi0 = xi - x_begin;
          int yi0 = yi - y_begin;

          int vi0 = ((xi0) + (yi0) * x_size);
          int vi1 = ((xi0) + (yi0 + 1) * x_size);
          int vi2 = ((xi0 + 1) + (yi0 + 1) * x_size);
          int vi3 = ((xi0 + 1) + (yi0) * x_size);
          
          if (reverse_faces) {
            tris->add_vertices(vi2, vi0, vi1);
            tris->close_primitive();
            
            tris->add_vertices(vi3, vi0, vi2);
            tris->close_primitive();
          } else {
            tris->add_vertices(vi2, vi1, vi0);
            tris->close_primitive();
            
            tris->add_vertices(vi3, vi2, vi0);
            tris->close_primitive();
          }
        }
      }
      geom->add_primitive(tris);
      gnode->add_geom(geom);
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::add_vis_column
//       Access: Private, Static
//  Description: The private implementation of the public
//               add_vis_column(), this adds the column to the
//               indicated specific vector.
////////////////////////////////////////////////////////////////////
void PfmVizzer::
add_vis_column(VisColumns &vis_columns, ColumnType source, ColumnType target,
               InternalName *name, const TransformState *transform,
               const Lens *lens) {
  VisColumn column;
  column._source = source;
  column._target = target;
  column._name = name;
  column._transform = transform;
  if (transform == NULL) {
    column._transform = TransformState::make_identity();
  }
  column._lens = lens;
  vis_columns.push_back(column);
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::build_auto_vis_columns
//       Access: Private
//  Description: This function is called internally to construct the
//               list of vis_columns automatically from the high-level
//               interfaces such as set_vis_inverse(),
//               set_flat_texcoord_name(), and set_vis_2d().  It's not
//               called if the list has been build explicitly.
////////////////////////////////////////////////////////////////////
void PfmVizzer::
build_auto_vis_columns(VisColumns &vis_columns, bool for_points) const {
  vis_columns.clear();

  if (_vis_2d) {
    // No normals needed if we're just generating a 2-d mesh.
    if (_vis_inverse) {
      add_vis_column(vis_columns, CT_texcoord2, CT_vertex2, InternalName::get_vertex());
      add_vis_column(vis_columns, CT_vertex2, CT_texcoord2, InternalName::get_texcoord());
    } else {
      add_vis_column(vis_columns, CT_vertex2, CT_vertex2, InternalName::get_vertex());
      add_vis_column(vis_columns, CT_texcoord2, CT_texcoord2, InternalName::get_texcoord());
    }

  } else {
    if (_vis_inverse) {
      // We need a 3-d texture coordinate if we're inverting the vis
      // and it's 3-d.  But we still don't need normals in that case.
      add_vis_column(vis_columns, CT_texcoord3, CT_vertex3, InternalName::get_vertex());
      add_vis_column(vis_columns, CT_vertex3, CT_texcoord3, InternalName::get_texcoord());
    } else {
      // Otherwise, we only need a 2-d texture coordinate, and we do
      // want normals.
      add_vis_column(vis_columns, CT_vertex3, CT_vertex3, InternalName::get_vertex());
      add_vis_column(vis_columns, CT_normal3, CT_normal3, InternalName::get_normal());
      add_vis_column(vis_columns, CT_texcoord2, CT_texcoord2, InternalName::get_texcoord());
    }
  }

  if (_flat_texcoord_name != (InternalName *)NULL) {
    // We need an additional texcoord column for the flat texcoords.
    add_vis_column(vis_columns, CT_texcoord2, CT_texcoord2, _flat_texcoord_name);
  }

  if (_vis_blend != (PNMImage *)NULL) {
    // The blend map, if specified, also gets applied to the vertices.
    add_vis_column(vis_columns, CT_blend1, CT_blend1, InternalName::get_color());
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::make_array_format
//       Access: Private
//  Description: Constructs a GeomVertexFormat that corresponds to the
//               vis_columns list.
////////////////////////////////////////////////////////////////////
CPT(GeomVertexFormat) PfmVizzer::
make_array_format(const VisColumns &vis_columns) const {
  PT(GeomVertexArrayFormat) array_format = new GeomVertexArrayFormat;

  for (VisColumns::const_iterator vci = vis_columns.begin();
       vci != vis_columns.end();
       ++vci) {
    const VisColumn &column = *vci;
    InternalName *name = column._name;

    int num_components = 0;
    GeomEnums::NumericType numeric_type = GeomEnums::NT_float32;
    GeomEnums::Contents contents = GeomEnums::C_point;
    switch (column._target) {
    case CT_texcoord2:
      num_components = 2;
      numeric_type = GeomEnums::NT_float32;
      contents = GeomEnums::C_texcoord;
      break;

    case CT_texcoord3:
      num_components = 3;
      numeric_type = GeomEnums::NT_float32;
      contents = GeomEnums::C_texcoord;
      break;

    case CT_vertex1:
    case CT_vertex2:
    case CT_vertex3:
      num_components = 3;
      numeric_type = GeomEnums::NT_float32;
      contents = GeomEnums::C_point;
      break;

    case CT_normal3:
      num_components = 3;
      numeric_type = GeomEnums::NT_float32;
      contents = GeomEnums::C_vector;
      break;

    case CT_blend1:
      num_components = 4;
      numeric_type = GeomEnums::NT_uint8;
      contents = GeomEnums::C_color;
      break;
    }
    nassertr(num_components != 0, NULL);
    
    array_format->add_column(name, num_components, numeric_type, contents);
  }

  return GeomVertexFormat::register_format(array_format);
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::VisColumn::add_data
//       Access: Public
//  Description: Adds the data for this column to the appropriate
//               column of the GeomVertexWriter.
////////////////////////////////////////////////////////////////////
void PfmVizzer::VisColumn::
add_data(const PfmVizzer &vizzer, GeomVertexWriter &vwriter, int xi, int yi, bool reverse_normals) const {
  const PfmFile &pfm = vizzer.get_pfm();

  switch (_source) {
  case CT_texcoord2:
    { 
      LPoint2f uv(PN_float32(xi) / PN_float32(pfm.get_x_size() - 1),
                  PN_float32(yi) / PN_float32(pfm.get_y_size() - 1));
      transform_point(uv);
      vwriter.set_data2f(uv);
    }
    break;

  case CT_texcoord3:
    {
      LPoint3f uv(PN_float32(xi) / PN_float32(pfm.get_x_size() - 1),
                  PN_float32(yi) / PN_float32(pfm.get_y_size() - 1), 
                  0.0f);
      transform_point(uv);
      vwriter.set_data3f(uv);
    }
    break;

  case CT_vertex1:
    {
      PN_float32 p = pfm.get_point1(xi, yi);
      LPoint2f point(p, 0.0);
      transform_point(point);
      vwriter.set_data2f(point);
    }
    break;

  case CT_vertex2:
    {
      LPoint2f point = pfm.get_point2(xi, yi);
      transform_point(point);
      vwriter.set_data2f(point);
    }
    break;

  case CT_vertex3:
    {
      LPoint3f point = pfm.get_point(xi, yi);
      transform_point(point);
      vwriter.set_data3f(point);
    }
    break;

  case CT_normal3:
    {
      // Calculate the normal based on two neighboring vertices.
      LPoint3f v[3];
      v[0] = pfm.get_point(xi, yi);
      if (xi + 1 < pfm.get_x_size()) {
        v[1] = pfm.get_point(xi + 1, yi);
      } else {
        v[1] = v[0];
        v[0] = pfm.get_point(xi - 1, yi);
      }
                
      if (yi + 1 < pfm.get_y_size()) {
        v[2] = pfm.get_point(xi, yi + 1);
      } else {
        v[2] = v[0];
        v[0] = pfm.get_point(xi, yi - 1);
      }
                
      LVector3f n = LVector3f::zero();
      for (int i = 0; i < 3; ++i) {
        const LPoint3f &v0 = v[i];
        const LPoint3f &v1 = v[(i + 1) % 3];
        n[0] += v0[1] * v1[2] - v0[2] * v1[1];
        n[1] += v0[2] * v1[0] - v0[0] * v1[2];
        n[2] += v0[0] * v1[1] - v0[1] * v1[0];
      }
      n.normalize();
      nassertv(!n.is_nan());
      if (reverse_normals) {
        n = -n;
      }
      transform_vector(n);
      vwriter.set_data3f(n);
    }
    break;

  case CT_blend1:
    {
      const PNMImage *vis_blend = vizzer.get_vis_blend();
      if (vis_blend != NULL) {
        double gray = vis_blend->get_gray(xi, yi);
        vwriter.set_data3d(gray, gray, gray);
      }
    }
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::VisColumn::transform_point
//       Access: Public
//  Description: Transforms the indicated point as specified by the
//               VisColumn.
////////////////////////////////////////////////////////////////////
void PfmVizzer::VisColumn::
transform_point(LPoint2f &point) const {
  if (!_transform->is_identity()) {
    LCAST(PN_float32, _transform->get_mat3()).xform_point_in_place(point);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::VisColumn::transform_point
//       Access: Public
//  Description: Transforms the indicated point as specified by the
//               VisColumn.
////////////////////////////////////////////////////////////////////
void PfmVizzer::VisColumn::
transform_point(LPoint3f &point) const {
  if (!_transform->is_identity()) {
    LCAST(PN_float32, _transform->get_mat()).xform_point_in_place(point);
  }
  if (_lens != (Lens *)NULL) {
    static LMatrix4f to_uv(0.5, 0.0, 0.0, 0.0,
                           0.0, 0.5, 0.0, 0.0, 
                           0.0, 0.0, 1.0, 0.0, 
                           0.5, 0.5, 0.0, 1.0);
    LPoint3 film;
    _lens->project(LCAST(PN_stdfloat, point), film);
    point = to_uv.xform_point(LCAST(PN_float32, film));
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PfmVizzer::VisColumn::transform_vector
//       Access: Public
//  Description: Transforms the indicated vector as specified by the
//               VisColumn.
////////////////////////////////////////////////////////////////////
void PfmVizzer::VisColumn::
transform_vector(LVector3f &vec) const {
  if (!_transform->is_identity()) {
    LCAST(PN_float32, _transform->get_mat()).xform_vec_in_place(vec);
  }
}