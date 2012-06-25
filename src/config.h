#ifndef CONFIG_H
#define CONFIG_H

#if HAVE_FLTK
    #include "flgui.h"
    #include "fl_funcs.h"
    #include "FL/fl_ask.H"
#elif HAVE_QT
    #include "qt_funcs.h"
#endif

#if HAVE_LIBVORBIS
    #include "vorbis_encode.h"
#endif


#endif
