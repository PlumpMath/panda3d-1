// Filename: nurbsSurfaceResult.cxx
// Created by:  drose (10Oct03)
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

#include "nurbsSurfaceResult.h"


////////////////////////////////////////////////////////////////////
//     Function: NurbsSurfaceResult::Constructor
//       Access: Public
//  Description: The constructor automatically builds up the result as
//               the product of the indicated set of basis matrices
//               and the indicated table of control vertex positions.
////////////////////////////////////////////////////////////////////
NurbsSurfaceResult::
NurbsSurfaceResult(const NurbsBasisVector &u_basis, 
                   const NurbsBasisVector &v_basis, 
                   const LVecBase4f vecs[], const NurbsVertex *verts,
                   int num_u_vertices, int num_v_vertices) :
  _u_basis(u_basis),
  _v_basis(v_basis),
  _verts(verts),
  _num_u_vertices(num_u_vertices),
  _num_v_vertices(num_v_vertices)
{
  _last_u_segment = -1;
  _last_v_segment = -1;
  int u_order = _u_basis.get_order();
  int v_order = _v_basis.get_order();
  int num_u_segments = _u_basis.get_num_segments();
  int num_v_segments = _v_basis.get_num_segments();
  int num_segments = num_u_segments * num_v_segments;

  _composed.reserve(num_segments);
  for (int ui = 0; ui < num_u_segments; ui++) {
    const LMatrix4f &u_basis_mat = _u_basis.get_basis(ui);

    int un = _u_basis.get_vertex_index(ui);
    nassertv(un >= 0 && un + u_order - 1 < _num_u_vertices);

    for (int vi = 0; vi < num_v_segments; vi++) {
      LMatrix4f v_basis_transpose = transpose(_v_basis.get_basis(vi));

      int vn = _v_basis.get_vertex_index(vi);
      nassertv(vn >= 0 && vn + v_order - 1 < _num_v_vertices);

      // Create four geometry matrices from our (up to) sixteen
      // involved vertices.
      LVecBase4f c[4][4];
      for (int uni = 0; uni < 4; uni++) {
        for (int vni = 0; vni < 4; vni++) {
          c[uni][vni] = (uni < u_order && vni < v_order) ? 
            vecs[verti(un + uni, vn + vni)] :
            LVecBase4f::zero();
        }
      }

      LMatrix4f geom_x(c[0][0][0], c[0][1][0], c[0][2][0], c[0][3][0],
                       c[1][0][0], c[1][1][0], c[1][2][0], c[1][3][0],
                       c[2][0][0], c[2][1][0], c[2][2][0], c[2][3][0],
                       c[3][0][0], c[3][1][0], c[3][2][0], c[3][3][0]);

      LMatrix4f geom_y(c[0][0][1], c[0][1][1], c[0][2][1], c[0][3][1],
                       c[1][0][1], c[1][1][1], c[1][2][1], c[1][3][1],
                       c[2][0][1], c[2][1][1], c[2][2][1], c[2][3][1],
                       c[3][0][1], c[3][1][1], c[3][2][1], c[3][3][1]);

      LMatrix4f geom_z(c[0][0][2], c[0][1][2], c[0][2][2], c[0][3][2],
                       c[1][0][2], c[1][1][2], c[1][2][2], c[1][3][2],
                       c[2][0][2], c[2][1][2], c[2][2][2], c[2][3][2],
                       c[3][0][2], c[3][1][2], c[3][2][2], c[3][3][2]);
      
      LMatrix4f geom_w(c[0][0][3], c[0][1][3], c[0][2][3], c[0][3][3],
                       c[1][0][3], c[1][1][3], c[1][2][3], c[1][3][3],
                       c[2][0][3], c[2][1][3], c[2][2][3], c[2][3][3],
                       c[3][0][3], c[3][1][3], c[3][2][3], c[3][3][3]);

      // And compose these geometry matrices with the basis matrices
      // to produce a new set of matrices, which will be used to
      // evaluate the surface.
      ComposedMats result;
      result._x = u_basis_mat * geom_x * v_basis_transpose;
      result._y = u_basis_mat * geom_y * v_basis_transpose;
      result._z = u_basis_mat * geom_z * v_basis_transpose;
      result._w = u_basis_mat * geom_w * v_basis_transpose;

      _composed.push_back(result);
    }
  }

  nassertv((int)_composed.size() == num_segments);
}

////////////////////////////////////////////////////////////////////
//     Function: NurbsSurfaceResult::eval_segment_point
//       Access: Published
//  Description: Evaluates the point on the surface corresponding to the
//               indicated value in parametric time within the
//               indicated surface segment.  u and v should be in the
//               range [0, 1].
//
//               The surface is internally represented as a number of
//               connected (or possibly unconnected) piecewise
//               continuous segments.  The exact number of segments
//               for a particular surface depends on the knot vector,
//               and is returned by get_num_segments().  Normally,
//               eval_point() is used to evaluate a point along the
//               continuous surface, but when you care more about local
//               continuity, you can use eval_segment_point() to
//               evaluate the points along each segment.
////////////////////////////////////////////////////////////////////
void NurbsSurfaceResult::
eval_segment_point(int ui, int vi, float u, float v, LVecBase3f &point) const {
  float u2 = u*u;
  LVecBase4f uvec(u*u2, u2, u, 1.0f);
  float v2 = v*v;
  LVecBase4f vvec(v*v2, v2, v, 1.0f);
  int i = segi(ui, vi);
  nassertv(i >= 0 && i < (int)_composed.size());

  float weight = vvec.dot(uvec * _composed[i]._w);

  point.set(vvec.dot(uvec * _composed[i]._x) / weight,
            vvec.dot(uvec * _composed[i]._y) / weight,
            vvec.dot(uvec * _composed[i]._z) / weight);
}

////////////////////////////////////////////////////////////////////
//     Function: NurbsSurfaceResult::eval_segment_normal
//       Access: Published
//  Description: As eval_segment_point, but computes the normal to
//               the surface at the indicated point.  The normal vector
//               will not necessarily be normalized, and could be
//               zero.
////////////////////////////////////////////////////////////////////
void NurbsSurfaceResult::
eval_segment_normal(int ui, int vi, float u, float v, LVecBase3f &normal) const {
  /*
  float t2 = t*t;
  LVecBase4f tvec(t2, t, 1.0f, 0.0f);

  normal.set(tvec.dot(_composed[segment].get_col(0)),
              tvec.dot(_composed[segment].get_col(1)),
              tvec.dot(_composed[segment].get_col(2)));
  */
}

////////////////////////////////////////////////////////////////////
//     Function: NurbsSurfaceResult::eval_segment_extended_point
//       Access: Published
//  Description: Evaluates the surface in n-dimensional space according
//               to the extended vertices associated with the surface in
//               the indicated dimension.
////////////////////////////////////////////////////////////////////
float NurbsSurfaceResult::
eval_segment_extended_point(int ui, int vi, float u, float v, int d) const {
  /*
  nassertr(segment >= 0 && segment < _basis.get_num_segments(), 0.0f);

  int order = _basis.get_order();
  int vi = _basis.get_vertex_index(segment);

  LVecBase4f geom;
  int ci = 0;
  while (ci < order) {
    geom[ci] = _verts[vi + ci].get_extended_vertex(d);
    ci++;
  }
  while (ci < 4) {
    geom[ci] = 0.0f;
    ci++;
  }

  const LMatrix4f &basis = _basis.get_basis(segment);

  // Compute matrix * column vector.
  LVecBase4f composed_geom(basis.get_row(0).dot(geom),
                           basis.get_row(1).dot(geom),
                           basis.get_row(2).dot(geom),
                           basis.get_row(3).dot(geom));

  float t2 = t*t;
  LVecBase4f tvec(t*t2, t2, t, 1.0f);

  float weight = tvec.dot(_composed[segment].get_col(3));

  float result = tvec.dot(composed_geom) / weight;
  return result;
  */
  return 0.0f;
}

////////////////////////////////////////////////////////////////////
//     Function: NurbsSurfaceResult::find_u_segment
//       Access: Private
//  Description: Returns the index of the segment that contains the
//               indicated value of t, or -1 if no segment contains
//               this value.
////////////////////////////////////////////////////////////////////
int NurbsSurfaceResult::
find_u_segment(float u) {
  // Trivially check the endpoints of the surface.
  if (u >= get_end_u()) {
    return _u_basis.get_num_segments() - 1;
  } else if (u <= get_start_u()) {
    return 0;
  }

  // Check the last segment we searched for.  Often, two consecutive
  // requests are for the same segment.
  if (_last_u_segment != -1 && (u >= _last_u_from && u < _last_u_to)) {
    return _last_u_segment;
  }

  // Look for the segment the hard way.
  int segment = r_find_u_segment(u, 0, _u_basis.get_num_segments() - 1);
  if (segment != -1) {
    _last_u_segment = segment;
    _last_u_from = _u_basis.get_from(segment);
    _last_u_to = _u_basis.get_to(segment);
  }
  return segment;
}

////////////////////////////////////////////////////////////////////
//     Function: NurbsSurfaceResult::r_find_u_segment
//       Access: Private
//  Description: Recursively searches for the segment that contains
//               the indicated value of t by performing a binary
//               search.  This assumes the segments are stored in
//               increasing order of t, and they don't overlap.
////////////////////////////////////////////////////////////////////
int NurbsSurfaceResult::
r_find_u_segment(float u, int top, int bot) const {
  if (bot < top) {
    // Not found.
    return -1;
  }
  int mid = (top + bot) / 2;
  nassertr(mid >= 0 && mid < _u_basis.get_num_segments(), -1);

  float from = _u_basis.get_from(mid);
  float to = _u_basis.get_to(mid);
  if (from > u) {
    // Too high, try lower.
    return r_find_u_segment(u, top, mid - 1);

  } else if (to <= u) {
    // Too low, try higher.
    return r_find_u_segment(u, mid + 1, bot);

  } else {
    // Here we are!
    return mid;
  }
}


////////////////////////////////////////////////////////////////////
//     Function: NurbsSurfaceResult::find_v_segment
//       Access: Private
//  Description: Returns the index of the segment that contains the
//               indicated value of t, or -1 if no segment contains
//               this value.
////////////////////////////////////////////////////////////////////
int NurbsSurfaceResult::
find_v_segment(float v) {
  // Trivially check the endpoints of the surface.
  if (v >= get_end_v()) {
    return _v_basis.get_num_segments() - 1;
  } else if (v <= get_start_v()) {
    return 0;
  }

  // Check the last segment we searched for.  Often, two consecutive
  // requests are for the same segment.
  if (_last_v_segment != -1 && (v >= _last_v_from && v < _last_v_to)) {
    return _last_v_segment;
  }

  // Look for the segment the hard way.
  int segment = r_find_v_segment(v, 0, _v_basis.get_num_segments() - 1);
  if (segment != -1) {
    _last_v_segment = segment;
    _last_v_from = _v_basis.get_from(segment);
    _last_v_to = _v_basis.get_to(segment);
  }
  return segment;
}

////////////////////////////////////////////////////////////////////
//     Function: NurbsSurfaceResult::r_find_v_segment
//       Access: Private
//  Description: Recursively searches for the segment that contains
//               the indicated value of t by performing a binary
//               search.  This assumes the segments are stored in
//               increasing order of t, and they don't overlap.
////////////////////////////////////////////////////////////////////
int NurbsSurfaceResult::
r_find_v_segment(float v, int top, int bot) const {
  if (bot < top) {
    // Not found.
    return -1;
  }
  int mid = (top + bot) / 2;
  nassertr(mid >= 0 && mid < _v_basis.get_num_segments(), -1);

  float from = _v_basis.get_from(mid);
  float to = _v_basis.get_to(mid);
  if (from > v) {
    // Too high, try lower.
    return r_find_v_segment(v, top, mid - 1);

  } else if (to <= v) {
    // Too low, try higher.
    return r_find_v_segment(v, mid + 1, bot);

  } else {
    // Here we are!
    return mid;
  }
}

