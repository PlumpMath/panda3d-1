#define BUILD_DIRECTORY $[HAVE_DX]

// dont try build dxgsg7 until its moved to new window stuff
#define BUILD_DIRECTORY

#define OTHER_LIBS interrogatedb:c dconfig:c dtoolconfig:m \
                   dtoolutil:c dtoolbase:c dtool:m
#define USE_PACKAGES dx

#begin lib_target

  #define TARGET dxgsg7
  #define LOCAL_LIBS \
    gsgmisc gsgbase gobj display windisplay \
    putil linmath mathutil pnmimage event
    
  #define COMBINED_SOURCES $[TARGET]_composite1.cxx     

  // need to install these due to external projects that link directly with libpandadx (bartop)  
  #define INSTALL_HEADERS \
    config_dxgsg7.h dxGraphicsStateGuardian7.I dxGraphicsStateGuardian7.h \
    dxTextureContext7.h dxGeomNodeContext7.h dxGeomNodeContext7.I dxgsg7base.h

  // build dxGraphicsStateGuardian separately since its so big
  
  #define SOURCES \
    dxGraphicsStateGuardian7.cxx dxSavedFrameBuffer7.I dxSavedFrameBuffer7.h \
    wdxGraphicsPipe7.I wdxGraphicsPipe7.h \
    wdxGraphicsWindow7.I wdxGraphicsWindow7.h \
    $[INSTALL_HEADERS]
    
  #define INCLUDED_SOURCES \
    config_dxgsg7.cxx \
    dxgsg7base.cxx \
    dxSavedFrameBuffer7.cxx dxTextureContext7.cxx \
    dxGeomNodeContext7.cxx \
    wdxGraphicsPipe7.cxx wdxGraphicsWindow7.cxx

#end lib_target
