/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#ifndef PRECOMPILED_CORE_H_
#define PRECOMPILED_CORE_H_

#if defined __cplusplus
// Add C++ includes here

// Put here includes which appear in header files. This will also be visible to other plug-in which depend on this one
// and will be included in their precompiled headers
#include "global.h"

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>

#include <QtCore/QCoreApplication>
#include <QtCore/QtGlobal>
#include <QtCore/QtPlugin>
#include <QtCore/QObject>

#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtCore/QPair>
#include <QtCore/QMap>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QList>

#include <QtCore/QMutexLocker>
#include <QtCore/QMutex>

#include <QtCore/QDebug>

#include <functional>
#include <typeinfo>

#ifdef Q_OS_LINUX
	#include <google/profiler.h>
#endif
#include <QtCore/QElapsedTimer>

#if defined(CORE_LIBRARY)
// Put here includes which only appear in compilation units and do not appear in headers. Precompiled headers of
// plug-ins which depend on this one will not include these headers.

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include <QtCore/QPluginLoader>

#include <QtCore/QDir>
#include <QtCore/QEvent>

#endif
#endif

#endif /* PRECOMPILED_CORE_H_ */
