#ifndef __INSTANTIO_BASICOUTSLOT_H
#define __INSTANTIO_BASICOUTSLOT_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BasicOutSlot.h                                                  //
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
#include "Slot.h"
#include <string>
#include <set>
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4251)
#endif


namespace InstantIO
{


class BasicInSlot;


/**
 * @class BasicOutSlot BasicOutSlot.h InstantIO/BasicOutSlot.h
 *
 * Abstract ancestor of all OutSlots. Applications do not use
 * this class directly, instead they use one of its descendants
 * (OutSlot).
 * @see OutSlot
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING BasicOutSlot: public Slot
{
public:

	/**
	 * Allows to receive information about the status of
	 * an OutSlot.
	 * <p>
	 * You can implement this interface by your own classes
	 * and add instances of these classes to {@link OutSlot
	 * OutSlots} by using the {@link OutSlot#addListener addListener}
	 * method. In this case, the OutSlot calls methods of this
	 * interface when special events occur. These events are:
	 * <ol>
	 * <li>The first {@link InSlot InSlot} connects to the
	 * {@link OutSlot OutSlot}.
	 * <li>The last {@link InSlot InSlot} disconnects from the
	 * {@link OutSlot OutSlot}.
	 * </ol>
	 * Please keep in mind that you should not block under any
	 * circumstance in the methods, and you should not perform
	 * time-consuming tasks. These methods are just meant to do
	 * short notifications about the events.
	 * <p>
	 * @author Patrick D&auml;hne
	 */
	class INSTANTIO_DLLMAPPING Listener
	{
	public:

		/** Destructor. */
		virtual ~Listener();

		/**
		 * Gets called when an OutSlot gets connected to the
		 * first InSlot.
		 * <p>
		 * You can use this callback to get a notification
		 * when you should start writing values to an {@link
		 * OutSlot OutSlot}.
		 * @param outSlot The OutSlot that generated the event.
		 */
		virtual void startOutSlot(BasicOutSlot &outSlot);

		/**
		 * Gets called when an OutSlot gets disconnected from
		 * the last InSlot.
		 * <p>
		 * You can use this callback to get a notification
		 * when you can stop writing values to an {@link
		 * OutSlot OutSlot}.
		 * @param outSlot The OutSlot that generated the event.
		 */
		virtual void stopOutSlot(BasicOutSlot &outSlot);
	};

	/**
	 * This destroys the %BasicOutSlot object.
	 */
	virtual ~BasicOutSlot();

	/**
	 * Returns the direction of the Slot (In or Out).
	 * @return The direction (always Slot::Out for OutSlots)
	 */
	virtual Direction getDirection() const;

	/**
	 * Adds a {@link Listener Listener} to the OutSlot. After calling
	 * this methods, the listener receives status information from
	 * the OutSlot.
	 * <p>
	 * Listeners can be used to receive notifications when the status
	 * of an OutSlot changes, i.e. when the first {@link InSlot
	 * InSlot} connects to the OutSlot, the last {@link InSlot
	 * InSlot} disconnects from the OutSlot. Instead of implementing
	 * the {@link Listener Listener} interface, you can also create
	 * a descendant of the OutSlot class and override the {@link
	 * #startOutSlot startOutSlot} and {@link #stopOutSlot stopOutSlot}
	 * methods.
	 * @param listener The listener.
	 * @see Listener Listener
	 * @see #removeListener removeListener
	 * @see #startOutSlot startOutSlot
	 * @see #stopOutSlot stopOutSlot
	 */
	void addListener(Listener &listener);

	/**
	 * Removes a {@link Listener Listener} from the OutSlot. After
	 * calling this method, the listener does not receive status
	 * notifications any more.
	 * @param listener The Listener.
	 * @see #addListener addListener
	 */
	void removeListener(Listener &listener);

	/**
	 * Makes the current value of the OutSlot invalid. The current
	 * value is the last data value written into the OutSlot. By default,
	 * the current value is invalid until the first data value is written
	 * into the OutSlot.
	 */
	void invalidateValue();

	/**
	 * Connects an InSlot to this OutSlot. Usually, {@link InSlot
	 * InSlots} and OutSlots are not connected manually. They are connected
	 * automatically by adding them to {@link Namespace Namespaces}. This
	 * method just exists for rare special circumstances where it might be
	 * helpful to connect slots manually.
	 * <p>
	 * It is only possible to connect {@link InSlot InSlots} to this
	 * OutSlot that have the same data type.
	 * <p>
	 * It is possible to add the same {@link InSlot InSlot} more than
	 * once to an OutSlot. Nevertheless, just one connection is created.
	 * For each call of the <code>connect</code> method, there has to be
	 * a corresponding call to the <code>#disconnect</code> method. The
	 * slots do not get disconnected until the last call of the disconnect
	 * method.
	 * @param inSlot The InSlot.
	 * @see #disconnect
	 */
	virtual void connect(BasicInSlot *inSlot) = 0;

	/**
	 * Disconnects an InSlot from this OutSlot. Usually, {@link InSlot
	 * InSlots} and OutSlots are not disconnected manually. The are disconnected
	 * automatically by removing them from {@link Namespace Namespaces}. This
	 * method just exists for rare special circumstances where it might be
	 * helpful to disconnect slots manually.
	 * <p>
	 * It is possible to add the same {@link InSlot InSlot} more than
	 * once to an OutSlot. Nevertheless, just one connection is created.
	 * For each call of the <code>#connect</code> method, there has to be
	 * a corresponding call of the <code>disconnect</code> method. The slots
	 * do not get disconnected until the last call of the disconnect method.
	 * @param inSlot The InSlot.
	 * @see #connect connect
	 */
	virtual void disconnect(BasicInSlot *inSlot) = 0;

    /**
     * Creates a matching InSlot<T> for this OutSlot if the specific
     * InSlot type T is unknown.
     * @return The newly created BasicOutSlot pointer
     */
    virtual BasicInSlot *createMatchingInSlot() = 0;

protected:

	/**
	 * Creates a new %BasicOutSlot object.
	 * @param description The description of this OutSlot. This information
	 * is not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 */
	explicit BasicOutSlot(const std::string &description = std::string());

	/**
	 * Gets called when an OutSlot gets connected to the
	 * first InSlot.
	 * You can override this method to get a notification
	 * when you should start writing values to an {@link
	 * OutSlot OutSlot}. The default implementation of this method
	 * calls the startOutSlot method of all listeners.
	 */
	virtual void startOutSlot();

	/**
	 * Gets called when an OutSlot gets disconnected from
	 * the last InSlot.
	 * You can override this method to get a notification
	 * when you can stop writing values to an {@link
	 * OutSlot OutSlot}. The default implementation of this method
	 * calls the stopOutSlot method of all listeners.
	 */
	virtual void stopOutSlot();

	/** Flag that is true when the OutSlot has a valid data value */
	// protected by mutex_
	bool valueIsValid_;

private:

	/** Defines the set of listeners */
	typedef std::set<Listener*> ListenerSet;

	/** The listeners that listen on this OutSlot */
	// protected by listenerMutex_
	ListenerSet listeners_;

	/** Copy constructor (not implemented!) */
	BasicOutSlot(const BasicOutSlot &);

	/** Copy operator (not implemented!) */
	const BasicOutSlot &operator=(const BasicOutSlot &);
};


} // namespace InstantIO


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif //__INSTANTIO_BASICOUTSLOT_H
