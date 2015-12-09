#ifndef __INSTANTIO_ROOT_H
#define __INSTANTIO_ROOT_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Root.h                                                          //
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

#include "InstantIODef.h"
#include "Namespace.h"
#include <iostream>
#include <vector>
#include <cstdlib>
namespace Medusa
{
	//class RWLock;
	class Mutex;
	class DynamicLibrary;
}
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4251)
#endif


namespace InstantIO
{


/**
 * @class Root Root.h InstantIO/Root.h
 *
 * The root namespace. {@link InSlot InSlots} and {@link OutSlot
 * OutSlots} have to be added to {@link Namespace Namespaces}
 * to connect them to each other. But there is one major problem:
 * How do two software components that do not know anything about
 * each other add their InSlots and OutSlots to the same Namespace?
 * <p>
 * The %Root class is the answer to this question. It serves as
 * a root namespace for all namespaces used by the software system.
 * %Root is a singleton, i.e. there exists exactly one instance of
 * the root. Therefore, it is not possible to create new instances
 * of the %Root class (the constructor is private). Instead, software
 * components that need to access the single instance have to call
 * the static method {@link #the the}, as demonstrated in the following
 * example:
 * <code><pre>
 * Root &root = Root::the();
 * </pre></code>
 *
 * @author Patrick D&auml;hne
 * @see Namespace Namespace
 */
class INSTANTIO_DLLMAPPING Root: public Namespace
{
friend class NodeType;

public:

	/**
	 * Helper class for exception-safe read-locking.
	 * @author Patrick D&auml;hne
	 */
	class INSTANTIO_DLLMAPPING ReadLock
	{
	public:

		/**
		 * Locks the InstantIO system for read access.
		 */
		inline ReadLock()
		{ Root::the().readLock(); }

		/**
		 * Unlocks the InstantIO system.
		 */
		inline ~ReadLock()
		{ Root::the().unlock(); }

	private:

		/** Copy constructor (not implemented!) */
		ReadLock(const ReadLock &);

		/** Copy operator (not implemented!) */
		const ReadLock &operator=(const ReadLock &);
	};

	/**
	 * Helper class for exception-safe write-locking.
	 * @author Patrick D&auml;hne
	 */
	class INSTANTIO_DLLMAPPING WriteLock
	{
	public:

		/**
		 * Locks the InstantIO system for write access.
		 */
		inline WriteLock()
		{ Root::the().writeLock(); }

		/**
		 * Unlocks the InstantIO system.
		 */
		inline ~WriteLock()
		{ Root::the().unlock(); }

	private:

		/** Copy constructor (not implemented!) */
		WriteLock(const WriteLock &);

		/** Copy operator (not implemented!) */
		const WriteLock &operator=(const WriteLock &);
	};

	/**
	 * Destroys the %Root object.
	 */
	virtual ~Root();

	/**
	 * Returns the single instance of the %Root class.
	 * @return The single instance.
	 */
	static inline Root &the()
	{
		if (initialized_ == false) {
			std::cerr << "FATAL: Accessed InstantIO::Root that is not initialized!" << std::endl;
			abort();
		}
		return the_;
	}

	/** Locks the whole InstantIO system for read access */
	void readLock();

	/** Locks the whole InstantIO system for write access */
	void writeLock();

	/** Unlocks the whole InstantIO system */
	void unlock();

	/**
	 * Tries to find a Namespace in a subtree of Namespaces.
	 * @param ns The path to the Namespace. The path consists
	 * of the names of the Namespaces, starting from the root,
	 * down to the searched Namespace, divided by slashes ("/").
	 * @return The Namespace object
	 * @throw InvalidNamespaceException when the method
	 * cannot find the Namespace in the subtree.
	 */
	Namespace *getNamespaceFromPath(const std::string &ns)
	/*throw (InvalidNamespaceException)*/;

protected:

	/** Creates a new Root object. */
	Root();

private:

	/** Copy constructor (not implemented!) */
	Root(const Root &);

	/** Copy operator (not implemented!) */
	const Root &operator=(const Root &);

	/** True when the Root singleton is initialized */
	static bool initialized_;

	/** The single instance of the Root class. */
	static Root the_;

	/** Read-write-lock used to lock the whole InstantIO system */
	//Medusa::RWLock *rwLock_;
	Medusa::Mutex *mutex_;

	/** Defines the vector containing plugins */
	typedef std::vector<Medusa::DynamicLibrary*> PluginVector;

	/** A vector containing all plugins currently loaded */
	PluginVector plugins_;
};


} // namespace InstantIO


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif // __INSTANTIO_ROOT_H
