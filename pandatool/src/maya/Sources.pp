#define DIRECTORY_IF_MAYA yes

#begin ss_lib_target
  #define USE_MAYA yes
  #define TARGET maya
  #define LOCAL_LIBS \
    converter pandatoolbase
  #define OTHER_LIBS \
    putil:c panda:m \
    express:c pandaexpress:m \
    dtoolutil:c dtoolbase:c dconfig:c dtoolconfig:m dtool:m pystub

  #define SOURCES \
    config_maya.cxx config_maya.h \
    mayaApi.cxx mayaApi.h \
    mayaShader.cxx mayaShader.h \
    mayaShaders.cxx mayaShaders.h \
    maya_funcs.I maya_funcs.cxx maya_funcs.h \
    post_maya_include.h pre_maya_include.h

#end ss_lib_target

