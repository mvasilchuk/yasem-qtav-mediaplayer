#ifndef LIBAVMEDIAPLAYER_GLOBAL_H
#define LIBAVMEDIAPLAYER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBAVMEDIAPLAYER_LIBRARY)
#  define LIBAVMEDIAPLAYERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBAVMEDIAPLAYERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBAVMEDIAPLAYER_GLOBAL_H