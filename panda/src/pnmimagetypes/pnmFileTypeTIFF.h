// Filename: pnmFileTypeTIFF.h
// Created by:  drose (17Jun00)
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

#ifndef PNMFILETYPETIFF_H
#define PNMFILETYPETIFF_H

#include "pandabase.h"

#ifdef HAVE_TIFF

#include "pnmFileType.h"
#include "pnmReader.h"
#include "pnmWriter.h"

#include <stdarg.h>  // for va_list


#define TIFF_COLORMAP_MAXCOLORS 1024

////////////////////////////////////////////////////////////////////
//       Class : PNMFileTypeTIFF
// Description : For reading and writing TIFF files.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA_PNMIMAGETYPES PNMFileTypeTIFF : public PNMFileType {
public:
  PNMFileTypeTIFF();

  virtual string get_name() const;

  virtual int get_num_extensions() const;
  virtual string get_extension(int n) const;
  virtual string get_suggested_extension() const;

  virtual bool has_magic_number() const;
  virtual bool matches_magic_number(const string &magic_number) const;

  virtual PNMReader *make_reader(istream *file, bool owns_file = true,
                                 const string &magic_number = string());
  virtual PNMWriter *make_writer(ostream *file, bool owns_file = true);

public:
  class Reader : public PNMReader {
  public:
    Reader(PNMFileType *type, istream *file, bool owns_file, string magic_number);
    virtual ~Reader();

    virtual bool supports_read_row() const;
    virtual bool read_row(xel *array, xelval *alpha, int x_size, int y_size);

  private:
    xelval next_sample_lt_8(unsigned char *&buf_ptr, int &bits_left) const;
    xelval next_sample_8(unsigned char *&buf_ptr, int &bits_left) const;
    xelval next_sample_16(unsigned char *&buf_ptr, int &bits_left) const;
    xelval next_sample_32(unsigned char *&buf_ptr, int &bits_left) const;
    xelval next_sample_general(unsigned char *&buf_ptr, int &bits_left) const;

    unsigned short photomet;
    unsigned short bps, spp;
    unsigned short unassoc_alpha_sample, assoc_alpha_sample;
    xel colormap[TIFF_COLORMAP_MAXCOLORS];

    int current_row;
    struct tiff *tif;
  };

  class Writer : public PNMWriter {
  public:
    Writer(PNMFileType *type, ostream *file, bool owns_file);

    virtual int write_data(xel *array, xelval *alpha);
  };

private:
  static void install_error_handlers();

  static void tiff_warning(const char *module, const char *format, va_list ap);
  static void tiff_error(const char *module, const char *format, va_list ap);
  static bool _installed_error_handlers;

  // The TypedWritable interface follows.
public:
  static void register_with_read_factory();

protected:
  static TypedWritable *make_PNMFileTypeTIFF(const FactoryParams &params);

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    PNMFileType::init_type();
    register_type(_type_handle, "PNMFileTypeTIFF",
                  PNMFileType::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

#endif  // HAVE_TIFF

#endif
