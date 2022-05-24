#ifndef QTBLE_GLOBAL_H
#define QTBLE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTBLE_LIBRARY)
#define QTBLE_EXPORT Q_DECL_EXPORT
#else
#define QTBLE_EXPORT Q_DECL_IMPORT
#endif

#endif // QTBLE_GLOBAL_H
