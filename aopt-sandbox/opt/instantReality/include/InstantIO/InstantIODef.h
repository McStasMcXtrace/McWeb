#ifndef __INSTANTIO_InstantIODEF_H
#define __INSTANTIO_InstantIODEF_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: InstantIODef.h                                                  //
//                                                                           //
//                                                                           //
// Author: Fraunhofer Institut fuer Graphische Datenverarbeitung (IGD)       //
//         Department Virtual and Augmented Reality                          //
//         Fraunhoferstrasse 5                                               //
//         64283 Darmstadt, Germany                                          //
//                                                                           //
// Rights: Copyright (c) 2009 by Fraunhofer IGD.                             //
//         All rights reserved.                                              //
//         Fraunhofer IGD provides this product without warranty of any kind //
//         and shall not be liable for any damages caused by the use         //
//         of this product.                                                  //
//                                                                           //
// ========================================================================= //

//#define DEBUG_DEADLOCK

#ifdef DEBUG_DEADLOCK
# ifdef _WIN32
#  include <winsock2.h>
# endif
# include <Medusa/Thread.h>
# ifdef __sgi
#  include <stdio.h>
# else
#  include <cstdio>
# endif
# define DEBUG_LOCK(method) printf("Debug_Lock %s (threadID = %li)\n", method, Medusa::Thread::self())
# define DEBUG_UNLOCK(method) printf("Debug_Unlock %s (threadID = %li)", method, Medusa::Thread::self())
#else
# define DEBUG_LOCK(method)
# define DEBUG_UNLOCK(method)
#endif

#if defined(_WIN32)
# ifdef InstantIO_EXPORTS
#  define INSTANTIO_DLLMAPPING __declspec(dllexport)
# else
#  define INSTANTIO_DLLMAPPING __declspec(dllimport)
# endif
#else
# define INSTANTIO_DLLMAPPING
#endif

#endif // __INSTANTIO_InstantIODEF_H
