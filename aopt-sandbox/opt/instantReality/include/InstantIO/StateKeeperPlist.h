#ifndef __INSTANTIO_STATEKEEPERPLIST_H
#define __INSTANTIO_STATEKEEPERPLIST_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: StateKeeperPlist.h                                              //
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

#ifdef __APPLE__

#include "InstantIODef.h"
#include <CoreFoundation/CoreFoundation.h>


namespace InstantIO
{


class Namespace;


/**
 * @class StateKeeperPlist StateKeeperPlist.h InstantIO/StateKeeperPlist.h
 *
 * Saves the current state of namespaces to or restores namespaces from
 * property lists.
 * @author Patrick D&auml;hne
 */
class StateKeeperPlist
{
public:

	/**
	 * Saves the state.
	 * Writes the current state of a namespace (i.e. all subnamespaces,
	 * nodes and routes) to a property list.
	 * @param ns The namespace whose current state
	 * we want to write to the property list.
	 * @return The property list that contains the state.
	 */
	static CFPropertyListRef save(Namespace &ns);

	/**
	 * Loads the state.
	 * Fills a namespace with a state (i.e. subnamespaces,
	 * nodes and routes) read from a property list.
	 * @param ns The namespace we fill with the stored state
	 * @param plist The property list we want to read the namespace
	 * from.
	 */
	static void load(Namespace &ns, CFPropertyListRef plist)
	/*throw (ParseException)*/;

private:

	/** Prevents instances of the %StateKeeperPlist class (not implemented!) */
	StateKeeperPlist();

	/** Copy constructor (not implemented!) */
	StateKeeperPlist(const StateKeeperPlist &);

	/** Copy operator (not implemented!) */
	const StateKeeperPlist &operator=(const StateKeeperPlist &);

	/**
	 * Saves the state.
	 */
	static void save(Namespace &ns, CFMutableDictionaryRef dict);

	/**
	 * Loads the state.
	 */
	static void load(Namespace &ns, CFDictionaryRef dict)
	/*throw (ParseException)*/;
};


} // namespace InstantIO


#endif // __APPLE__

#endif // __INSTANTIO_STATEKEEPERPLIST_H
