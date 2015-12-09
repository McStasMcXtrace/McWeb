#ifndef __INSTANTIO_SLOT_H
#define __INSTANTIO_SLOT_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Slot.h                                                          //
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
#include <string>
#include <typeinfo>
namespace Medusa
{
	class Mutex;
}


namespace InstantIO
{


/**
 * @class Slot Slot.h InstantIO/Slot.h
 *
 * Abstract ancestor of all In- and OutSlots. Applications do not use
 * this class directly, instead they use one of its descendants
 * (InSlot or OutSlot).
 * @see InSlot
 * @see OutSlot
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING Slot
{
public:

	/**
	 * This destroys the %Slot object.
	 */
	virtual ~Slot();

	/**
	 * Returns the description of the %Slot. The description is
	 * not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 * @return The description string.
	 */
	inline const std::string &getDescription() const
	{ return description_; }

	/**
	 * Returns a human-readable string that describes the type
	 * that can be sent to or received from this %Slot. This information is
	 * not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 * @return A string that describes the type that can be
	 * sent to or received from this %Slot.
	 */
	virtual const char *getTypeName() const = 0;

	/**
	 * Returns the RTTI type info object that describes the type
	 * that can be sent to or received from this %Slot.
	 * @return The RTTI type info object.
	 */
	virtual const std::type_info &getTypeID() const = 0;

	/** Enumeration of direction values. */
	typedef enum { In, Out } Direction;

	/**
	 * Returns the direction of the %Slot (In or Out).
	 * @return The direction
	 */
	virtual Direction getDirection() const = 0;

protected:

	/**
	 * Creates a new %Slot object.
	 * @param description The description of this %Slot. This information
	 * is not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 */
	explicit Slot(const std::string &description = std::string());

	/** Locks the set of Listeners */
	void listenersLock() const;

	/** Unlocks the set of Listeners */
	void listenersUnlock() const;

	/** Locks the data value and the vector of connected %Slots */
	void lock() const;

	/** Unlocks the data value and the vector of connected %Slots */
	void unlock() const;

	/** The description of this OutSlot */
	std::string description_;

	/** Mutex that locks the set of Listeners */
	Medusa::Mutex *listenerMutex_;

	/** Mutex that locks the data value and the vector of connected %Slots */
	Medusa::Mutex *mutex_;

private:

	/** Copy constructor (not implemented!) */
	Slot(const Slot &);

	/** Copy operator (not implemented!) */
	const Slot &operator=(const Slot &);
};


} // namespace InstantIO


#endif //__INSTANTIO_SLOT_H
