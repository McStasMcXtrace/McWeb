#ifndef __INSTANTIO_STATEKEEPERREGISTRY_H
#define __INSTANTIO_STATEKEEPERREGISTRY_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: StateKeeperRegistry.h                                           //
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

#ifdef _WIN32

#include "InstantIODef.h"
#include <windows.h>


namespace InstantIO
{


class Namespace;


/**
 * @class StateKeeperRegistry StateKeeperRegistry.h InstantIO/StateKeeperRegistry.h
 *
 * Saves the current state of namespaces to or restores namespaces from
 * the registry.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING StateKeeperRegistry
{
public:

	/**
	 * Saves the state.
	 * Writes the current state of a namespace (i.e. all subnamespaces,
	 * nodes and routes) to the registry.
	 * @param ns The namespace whose current state
	 * we want to write to the property list.
	 * @param hKey The registry key we write the state to
	 */
	static void save(Namespace &ns, HKEY hKey);

	/**
	 * Loads the state.
	 * Fills a namespace with a state (i.e. subnamespaces,
	 * nodes and routes) read from the registry.
	 * @param ns The namespace we fill with the stored state
	 * @param hKey The registry key we read the state from
	 * from.
	 */
	static void load(Namespace &ns, HKEY hKey)
	/*throw (ParseException)*/;

private:

	/** Prevents instances of the %StateKeeperRegistry class (not implemented!) */
	StateKeeperRegistry();

	/** Copy constructor (not implemented!) */
	StateKeeperRegistry(const StateKeeperRegistry &);

	/** Copy operator (not implemented!) */
	const StateKeeperRegistry &operator=(const StateKeeperRegistry &);

	/**
	 * Saves the state.
	 */
	static void internalSave(Namespace &ns, HKEY hKey);

	/**
	 * Loads the state.
	 */
	static void internalLoad(Namespace &ns, HKEY hKey)
	/*throw (ParseException)*/;
};


} // namespace InstantIO


#endif // _WIN32

#endif // __INSTANTIO_STATEKEEPERREGISTRY_H
