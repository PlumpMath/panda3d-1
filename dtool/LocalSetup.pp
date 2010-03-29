//
// LocalSetup.pp
//
// This file contains further instructions to set up the DTOOL package
// when using ppremake.  In particular, it creates the dtool_config.h
// file based on the user's selected configure variables.  This script
// need not execute when BUILD_TYPE is "autoconf"; in this case, the
// dtool_config.h file will automatically be correctly generated by
// configure.
//

#print
#print Configuring support for the following optional third-party packages:
#if $[HAVE_OPENSSL]
#print + OpenSSL
#else
#print - Did not find OpenSSL
#endif
#if $[HAVE_JPEG]
#print + libjpeg
#else
#print - Did not find libjpeg
#endif
#if $[HAVE_PNG]
#print + libpng
#else
#print - Did not find libpng
#endif
#if $[HAVE_TIFF]
#print + libtiff
#else
#print - Did not find libtiff
#endif
#if $[HAVE_TAR]
#print + libtar
#else
#print - Did not find libtar
#endif
#if $[HAVE_FFTW]
#print + fftw
#else
#print - Did not find fftw
#endif
#if $[HAVE_SQUISH]
#print + squish
#else
#print - Did not find squish
#endif
#if $[HAVE_CG]
#print + Nvidia Cg High Level Shading Language
#else
#print - Did not find Nvidia Cg High Level Shading Language
#endif
#if $[HAVE_CGGL]
#print + Cg OpenGL API
#else
#print - Did not find Cg OpenGL API
#endif
#if $[HAVE_CGDX8]
#print + Cg DX8 API
#else
#print - Did not find Cg DX8 API
#endif
#if $[HAVE_CGDX9]
#print + Cg DX9 API
#else
#print - Did not find Cg DX9 API
#endif
#if $[HAVE_CGDX10]
#print + Cg DX10 API
#else
#print - Did not find Cg DX10 API
#endif
#if $[HAVE_VRPN]
#print + VRPN
#else
#print - Did not find VRPN
#endif
#if $[HAVE_ZLIB]
#print + zlib
#else
#print - Did not find zlib
#endif
#if $[HAVE_RAD_MSS]
#print + Miles Sound System
#else
#print - Did not find Miles Sound System
#endif
#if $[HAVE_FMODEX]
#print + FMOD Ex sound library
#else
#print - Did not find FMOD Ex sound library
#endif
#if $[HAVE_OPENAL]
#print + OpenAL sound library
#else
#print - Did not find OpenAL sound library
#endif
#if $[HAVE_PHYSX]
#print + Ageia PhysX
#else
#print - Did not find Ageia PhysX
#endif
#if $[HAVE_TINYXML]
#print + TinyXML
#else
#print - Did not find TinyXML
#endif
#if $[HAVE_GTK]
#print + gtk+-2
#else
#print - Did not find gtk+-2
#endif
#if $[HAVE_FREETYPE]
#print + Freetype
#else
#print - Did not find Freetype
#endif
#if $[HAVE_WX]
#print + WxWidgets
#else
#print - Did not find WxWidgets
#endif
#if $[HAVE_GL]
#print + OpenGL
#elif $[HAVE_GLES2]
#print + OpenGL ES 2
#elif $[HAVE_GLES]
#print + OpenGL ES
#else
#print - Did not find OpenGL
#endif
#if $[HAVE_DX8]
#print + DirectX8
#else
#print - Did not find DirectX8
#endif
#if $[HAVE_DX9]
#print + DirectX9
#else
#print - Did not find DirectX9
#endif
#if $[HAVE_TINYDISPLAY]
#print + Tinydisplay
#else
#print - Not building Tinydisplay
#endif
//#if $[HAVE_SDL]
//#print + SDL
//#else
//#print - Did not find SDL
//#endif
#if $[HAVE_X11]
#print + X11
#else
#print - Did not find X11
#endif
#if $[HAVE_MESA]
#print + Mesa
#else
#print - Did not find Mesa
#endif
#if $[HAVE_OPENCV]
#print + OpenCV
#else
#print - Did not find OpenCV
#endif
#if $[HAVE_FFMPEG]
#if $[HAVE_SWSCALE]
#print + FFMPEG, with libswscale
#else
#print + FFMPEG
#endif
#else
#print - Did not find FFMPEG
#endif
#if $[HAVE_ODE]
#print + ODE
#else
#print - Did not find ODE
#endif
#if $[HAVE_AWESOMIUM]
#print + AWESOMIUM
#else
#print - Did not find AWESOMIUM
#endif
#if $[HAVE_MAYA]
#print + OpenMaya
#else
#print - Did not find OpenMaya
#endif
#if $[HAVE_FCOLLADA]
#print + FCollada
#else
#print - Did not find FCollada
#endif
#if $[HAVE_ARTOOLKIT]
#print + ARToolKit
#else
#print - Did not find ARToolKit
#endif

#print
#if $[and $[HAVE_INTERROGATE],$[HAVE_PYTHON]]
#print Compilation will generate Python interfaces.
#else
#print Configuring Panda WITHOUT Python interfaces.
#endif
#if $[HAVE_THREADS]
#if $[SIMPLE_THREADS]
#print Compilation will include simulated threading support.
#else
#if $[DO_PIPELINING]
#print Compilation will include full, pipelined threading support.
#else
#print Compilation will include nonpipelined threading support.
#endif
#endif
#else
#print Configuring Panda without threading support.
#endif

#print
#print See dtool_config.h for more details about the specified configuration.

#print

// We don't include the ppremake version in the first comment line of
// the output to dtool_config.h, below, to minimize unnecessary
// complete rebuilds due to an updated ppremake version.
#output dtool_config.h notouch
#format straight
/* dtool_config.h.  Generated automatically by $[PPREMAKE] from $[SOURCEFILE]. */

/* Debug / non-debug symbols.  OPTIMIZE = $[OPTIMIZE] */
#if $[<= $[OPTIMIZE],2]
#define _DEBUG 1
#elif $[= $[OPTIMIZE],4]
#define NDEBUG 1
#endif
$[cdefine _DEBUG]
$[cdefine NDEBUG]

/* Define if we have Python installed.  */
$[cdefine HAVE_PYTHON]
$[cdefine USE_DEBUG_PYTHON]
/* Define if we have Python as a framework (Mac OS X).  */
$[cdefine PYTHON_FRAMEWORK]

/* Define if we have RAD game tools, Miles Sound System installed.  */
$[cdefine HAVE_RAD_MSS]

/* Define if we have FMODex installed. */
$[cdefine HAVE_FMODEX]

/* Define if we have OpenAL installed. */
$[cdefine HAVE_OPENAL]

/* Define if we have Ageia PhysX SDK installed. */
$[cdefine HAVE_PHYSX]

/* Define if we have Freetype 2.0 or better available. */
$[cdefine HAVE_FREETYPE]

/* Define if we want to compile in a default font. */
$[cdefine COMPILE_IN_DEFAULT_FONT]

/* Define if we have Maya available. */
$[cdefine HAVE_MAYA]
$[cdefine MAYA_PRE_5_0]

/* Define if we have SoftImage available. */
$[cdefine HAVE_SOFTIMAGE]

/* Define if we have FCollada available. */
$[cdefine HAVE_FCOLLADA]

/* Define if we have ARToolKit available. */
$[cdefine HAVE_ARTOOLKIT]

/* Define if we have OpenSSL installed.  */
$[cdefine HAVE_OPENSSL]
$[cdefine OPENSSL_097]
$[cdefine REPORT_OPENSSL_ERRORS]

/* Define if we have libjpeg installed.  */
$[cdefine HAVE_JPEG]

/* Define if we have libpng installed.  */
$[cdefine HAVE_PNG]

/* Define if we have libtiff installed.  */
$[cdefine HAVE_TIFF]

/* Define if we want to build these other image file formats. */
$[cdefine HAVE_SGI_RGB]
$[cdefine HAVE_TGA]
$[cdefine HAVE_IMG]
$[cdefine HAVE_SOFTIMAGE_PIC]
$[cdefine HAVE_BMP]
$[cdefine HAVE_PNM]

/* Define if we have libtar installed.  */
$[cdefine HAVE_TAR]

/* Define if we have libfftw installed.  */
$[cdefine HAVE_FFTW]

/* Define if we have libsquish installed.  */
$[cdefine HAVE_SQUISH]

/* Define if we have Berkeley DB installed.  */
$[cdefine HAVE_BDB]

/* Define if we have VRPN installed.  */
$[cdefine HAVE_VRPN]

/* Define if we have HELIX installed.  */
$[cdefine HAVE_HELIX]

/* Define if we have CG installed.  */
$[cdefine HAVE_CG]

/* Define if we have CGGL installed.  */
$[cdefine HAVE_CGGL]

/* Define if we have CGDX8 installed.  */
$[cdefine HAVE_CGDX8]

/* Define if we have CGDX9 installed.  */
$[cdefine HAVE_CGDX9]

/* Define if we have CGDX10 installed.  */
$[cdefine HAVE_CGDX10]

/* Define if we have zlib installed.  */
$[cdefine HAVE_ZLIB]

/* Define if we have OpenGL installed and want to build for GL.  */
$[cdefine HAVE_GL]
$[cdefine HAVE_GLU]
#if HAVE_GL
# define MIN_GL_VERSION_MAJOR $[word 1,$[MIN_GL_VERSION]]
# define MIN_GL_VERSION_MINOR $[word 2,$[MIN_GL_VERSION]]
#endif

/* Define if we have OpenGL ES installed and want to build for GLES. */
$[cdefine HAVE_GLES]

/* Define if we have OpenGL ES installed and want to build for GLES2. */
$[cdefine HAVE_GLES2]

/* Define if we have OpenCV installed and want to build for OpenCV.  */
$[cdefine HAVE_OPENCV]

/* Define if we have FFMPEG installed and want to build for FFMPEG.  */
$[cdefine HAVE_FFMPEG]
$[cdefine HAVE_SWSCALE]

/* Define if we have ODE installed and want to build for ODE.  */
$[cdefine HAVE_ODE]

/* Define if we have AWESOMIUM installed and want to build for AWESOMIUM.  */
$[cdefine HAVE_AWESOMIUM]

/* Define if we have Mesa installed and want to build mesadisplay.  */
$[cdefine HAVE_MESA]
$[cdefine MESA_MGL]
#if HAVE_MESA
# define MIN_MESA_VERSION_MAJOR $[word 1,$[MIN_MESA_VERSION]]
# define MIN_MESA_VERSION_MINOR $[word 2,$[MIN_MESA_VERSION]]
#endif

/* Define if we have GLX installed and want to build for GLX.  */
$[cdefine HAVE_GLX]

/* Define if we have EGL installed and want to build for EGL.  */
$[cdefine HAVE_EGL]

/* Define if we have Windows-GL installed and want to build for Wgl.  */
$[cdefine HAVE_WGL]

/* Define if we have DirectX installed and want to build for DX.  */
$[cdefine HAVE_DX8]

/* Define if we have DirectX installed and want to build for DX.  */
$[cdefine HAVE_DX9]

/* Define if we want to build tinydisplay. */
$[cdefine HAVE_TINYDISPLAY]

/* Define if we have the SDL library. */
$[cdefine HAVE_SDL]

/* Define if we have X11. */
$[cdefine HAVE_X11]

/* Define if we have the XFree86-DGA extension. */
$[cdefine HAVE_XF86DGA]

/* Define if we have the XRandR extension. */
$[cdefine HAVE_XRANDR]

/* Define if we want to compile the threading code.  */
$[cdefine HAVE_THREADS]

/* Define if we want to use fast, user-space simulated threads.  */
$[cdefine SIMPLE_THREADS]

/* Define to enable deadlock detection, mutex recursion checks, etc. */
$[cdefine DEBUG_THREADS]

/* Define to implement mutexes and condition variables via a user-space spinlock. */
$[cdefine MUTEX_SPINLOCK]

/* Define to enable the PandaFileStream implementation of pfstream etc. */
$[cdefine USE_PANDAFILESTREAM]

/* Define if we want to compile the net code.  */
$[cdefine HAVE_NET]

/* Define if we want to compile the egg code.  */
$[cdefine HAVE_EGG]

/* Define if we want to compile the audio code.  */
$[cdefine HAVE_AUDIO]

/* Define if we have bison and flex available. */
$[cdefine HAVE_BISON]

/* Define if we want to use PStats.  */
$[cdefine DO_PSTATS]

/* Define if we want to type-check downcasts.  */
$[cdefine DO_DCAST]

/* Define if we want to provide collision system recording and
   visualization tools. */
$[cdefine DO_COLLISION_RECORDING]

/* Define if we want to enable track-memory-usage.  */
$[cdefine DO_MEMORY_USAGE]

/* Define if we want to enable min-lag and max-lag.  */
$[cdefine SIMULATE_NETWORK_DELAY]

/* Define if we want to allow immediate mode OpenGL rendering.  */
$[cdefine SUPPORT_IMMEDIATE_MODE]

/* Define if we want to compile in support for pipelining.  */
$[cdefine DO_PIPELINING]

/* Define if we want to keep Notify debug messages around, or undefine
   to compile them out.  */
$[cdefine NOTIFY_DEBUG]

/* Define if we want to export template classes from the DLL.  Only
   makes sense to MSVC++. */
$[cdefine EXPORT_TEMPLATES]

/* Define if we are linking PANDAPHYSX in with PANDA. */
$[cdefine LINK_IN_PHYSX]

/* The compiled-in character(s) to expect to separate different
   components of a path list (e.g. $PRC_PATH). */
# define DEFAULT_PATHSEP "$[DEFAULT_PATHSEP]"

/* Many of the prc variables are exported by
   dtool/src/prc/prc_parameters.h.pp, instead of here.  Only those prc
   variables that must be visible outside of the prc directory are
   exported here. */

/* The filename that specifies the public keys to import into
   config. */
# define PRC_PUBLIC_KEYS_FILENAME "$[unixfilename $[PRC_PUBLIC_KEYS_FILENAME]]"
#if $[PRC_PUBLIC_KEYS_FILENAME]
# define PRC_PUBLIC_KEYS_INCLUDE "$[osfilename $[PRC_PUBLIC_KEYS_FILENAME]]"
#endif

/* Define if you want to save the descriptions for ConfigVariables. */
$[cdefine PRC_SAVE_DESCRIPTIONS]


/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
$[cdefine WORDS_BIGENDIAN]

/* Define if the C++ compiler uses namespaces.  */
$[cdefine HAVE_NAMESPACE]

/* Define if fstream::open() accepts a third parameter for umask. */
$[cdefine HAVE_OPEN_MASK]

/* Define if some header file defines wchar_t. */
$[cdefine HAVE_WCHAR_T]

/* Define if the <string> header file defines wstring. */
$[cdefine HAVE_WSTRING]

/* Define if the C++ compiler supports the typename keyword.  */
$[cdefine HAVE_TYPENAME]

/* Define if we can trust the compiler not to insert extra bytes in
   structs between base structs and derived structs. */
$[cdefine SIMPLE_STRUCT_POINTERS]

/* Define if we have Dinkumware STL installed.  */
$[cdefine HAVE_DINKUM]

/* Define if we have STL hash_map etc. available  */
$[cdefine HAVE_STL_HASH]

/* Define if we have a gettimeofday() function. */
$[cdefine HAVE_GETTIMEOFDAY]

/* Define if gettimeofday() takes only one parameter. */
$[cdefine GETTIMEOFDAY_ONE_PARAM]

/* Define if you have the getopt function.  */
$[cdefine HAVE_GETOPT]

/* Define if you have the getopt_long_only function.  */
$[cdefine HAVE_GETOPT_LONG_ONLY]

/* Define if getopt appears in getopt.h.  */
$[cdefine PHAVE_GETOPT_H]

/* Define if you have ioctl(TIOCGWINSZ) to determine terminal width. */
$[cdefine IOCTL_TERMINAL_WIDTH]

/* Do the system headers define a "streamsize" typedef? */
$[cdefine HAVE_STREAMSIZE]

/* Do the system headers define key ios typedefs like ios::openmode
   and ios::fmtflags? */
$[cdefine HAVE_IOS_TYPEDEFS]

/* Define if the C++ iostream library defines ios::binary.  */
$[cdefine HAVE_IOS_BINARY]

/* Can we safely call getenv() at static init time? */
$[cdefine STATIC_INIT_GETENV]

/* Can we read the file /proc/self/[*] to determine our
   environment variables at static init time? */
$[cdefine HAVE_PROC_SELF_EXE]
$[cdefine HAVE_PROC_SELF_MAPS]
$[cdefine HAVE_PROC_SELF_ENVIRON]
$[cdefine HAVE_PROC_SELF_CMDLINE]
$[cdefine HAVE_PROC_CURPROC_FILE]
$[cdefine HAVE_PROC_CURPROC_MAP]
$[cdefine HAVE_PROC_CURPROC_CMDLINE]

/* Do we have a global pair of argc/argv variables that we can read at
   static init time?  Should we prototype them?  What are they called? */
$[cdefine HAVE_GLOBAL_ARGV]
$[cdefine PROTOTYPE_GLOBAL_ARGV]
$[cdefine GLOBAL_ARGV]
$[cdefine GLOBAL_ARGC]

/* Define if you have the <io.h> header file.  */
$[cdefine PHAVE_IO_H]

/* Define if you have the <iostream> header file.  */
$[cdefine PHAVE_IOSTREAM]

/* Define if you have the <malloc.h> header file.  */
$[cdefine PHAVE_MALLOC_H]

/* Define if you have the <sys/malloc.h> header file.  */
$[cdefine PHAVE_SYS_MALLOC_H]

/* Define if you have the <alloca.h> header file.  */
$[cdefine PHAVE_ALLOCA_H]

/* Define if you have the <locale.h> header file.  */
$[cdefine PHAVE_LOCALE_H]

/* Define if you have the <string.h> header file.  */
$[cdefine PHAVE_STRING_H]

/* Define if you have the <stdlib.h> header file.  */
$[cdefine PHAVE_STDLIB_H]

/* Define if you have the <limits.h> header file.  */
$[cdefine PHAVE_LIMITS_H]

/* Define if you have the <minmax.h> header file.  */
$[cdefine PHAVE_MINMAX_H]

/* Define if you have the <sstream> header file.  */
$[cdefine PHAVE_SSTREAM]

/* Define if you have the <new> header file.  */
$[cdefine PHAVE_NEW]

/* Define if you have the <sys/types.h> header file.  */
$[cdefine PHAVE_SYS_TYPES_H]

/* Define if you have the <sys/time.h> header file.  */
$[cdefine PHAVE_SYS_TIME_H]

/* Define if you have the <unistd.h> header file.  */
$[cdefine PHAVE_UNISTD_H]

/* Define if you have the <utime.h> header file.  */
$[cdefine PHAVE_UTIME_H]

/* Define if you have the <glob.h> header file.  */
$[cdefine PHAVE_GLOB_H]

/* Define if you have the <dirent.h> header file.  */
$[cdefine PHAVE_DIRENT_H]

/* Define if you have the <drfftw.h> header file.  */
$[cdefine PHAVE_DRFFTW_H]

/* Do we have <sys/soundcard.h> (and presumably a Linux-style audio
   interface)? */
$[cdefine PHAVE_SYS_SOUNDCARD_H]

/* Do we have <ucontext.h> (and therefore makecontext() /
   swapcontext())? */
$[cdefine PHAVE_UCONTEXT_H]

/* Do we have <linux/input.h> ? This enables us to use raw mouse input. */
$[cdefine PHAVE_LINUX_INPUT_H]

/* Do we have <stdint.h>? */
$[cdefine PHAVE_STDINT_H]

/* Do we have RTTI (and <typeinfo>)? */
$[cdefine HAVE_RTTI]

/* Do we have Posix threads? */
$[cdefine HAVE_POSIX_THREADS]

/* Is the code being compiled with the Tau profiler's instrumentor? */
$[cdefine USE_TAU]

/* Define if needed to have 64-bit file i/o */
$[cdefine __USE_LARGEFILE64]

/* Which memory allocation scheme should we use? */
#define USE_MEMORY_DLMALLOC
#define USE_MEMORY_PTMALLOC2
#define USE_MEMORY_MALLOC
#define USE_MEMORY_NOWRAPPERS
#if $[ALTERNATIVE_MALLOC]
  #if $[and $[WIN32_PLATFORM], $[HAVE_THREADS], $[not $[SIMPLE_THREADS]]]
    // A fast thread-safe alternative implementation, but which only
    // seems to be a good choice on Windows.  (It crashes on Linux and
    // isn't thread-safe on OSX).
    #set USE_MEMORY_PTMALLOC2 1
  #else
    // A faster, but non-thread-safe, alternative implementation.
    // When threading support is compiled in, we use a global mutex to
    // protect it.
    #set USE_MEMORY_DLMALLOC 1
  #endif
#else
  #if $[DO_MEMORY_USAGE]
    // Redefine new and delete to malloc(), and also provide hooks for
    // the benefit of the MemoryUsage class.
    #set USE_MEMORY_MALLOC 1
  #else
    // Don't redefine new and delete at all.
    #set USE_MEMORY_NOWRAPPERS 1
  #endif
#endif
$[cdefine USE_MEMORY_DLMALLOC]
$[cdefine USE_MEMORY_PTMALLOC2]
$[cdefine USE_MEMORY_MALLOC]
$[cdefine USE_MEMORY_NOWRAPPERS]

// To activate the DELETED_CHAIN macros.
$[cdefine USE_DELETED_CHAIN]

// If we are to build the native net interfaces.
$[cdefine WANT_NATIVE_NET]

/* Turn off warnings for using scanf and such */
#if $[eq $[USE_COMPILER],MSVC9]
        #print Will ignore CRT_SECURE warnings for MSVC9
        $[cdefine _CRT_SECURE_NO_WARNINGS]
        # pragma warning( disable : 4996 4275 )
#endif


/* Can we define a modern-style STL allocator? */
$[cdefine USE_STL_ALLOCATOR]

/* Static linkage instead of the normal dynamic linkage? */
$[cdefine LINK_ALL_STATIC]

/* Define to compile the plugin code. */
$[cdefine HAVE_P3D_PLUGIN]

/* Platform-identifying defines. */
$[cdefine IS_OSX]
$[cdefine IS_LINUX]
$[cdefine IS_FREEBSD]
$[cdefine BUILD_IPHONE]
$[cdefine UNIVERSAL_BINARIES]

#if $[DTOOL_PLATFORM]
# define DTOOL_PLATFORM "$[DTOOL_PLATFORM]"
#endif

#end dtool_config.h
