#ifndef __INSTANTIO_BASICINSLOT_H
#define __INSTANTIO_BASICINSLOT_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BasicInSlot.h                                                   //
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
namespace Medusa
{
	class Semaphore;
}
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4251)
#endif


namespace InstantIO
{


class BasicOutSlot;


/**
 * @class BasicInSlot BasicInSlot.h InstantIO/BasicInSot.h
 *
 * Abstract ancestor of all InSlots. Applications do not use
 * this class directly, instead they use one of its descendants
 * (InSlot and BufferedInSlot).
 * @see InSlot
 * @see BufferedInSlot
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING BasicInSlot: public Slot
{
public:

	/**
	 * Allows to receive information about the status of
	 * an InSlot.
	 * <p>
	 * You can implement this interface by your own classes
	 * and add instances of these classes to {@link InSlot
	 * InSlots} by using the {@link InSlot#addListener addListener}
	 * method. In this case, the InSlot calls methods of this
	 * interface when special events occur. These events are:
	 * <ol>
	 * <li>The first OutSlot connects to the InSlot.
	 * <li>The last OutSlot disconnects from the InSlot.
	 * <li>New data is available for receival on the {@link InSlot
	 * InSlot}.
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
		 * Gets called when an InSlot gets connected to the
		 * first OutSlot.
		 * <p>
		 * You can use this callback to get a notification
		 * when you should start reading values from an {@link
		 * InSlot InSlot}.
		 * @param inSlot The InSlot that generated the event.
		 */
		virtual void startInSlot(BasicInSlot &inSlot);

		/**
		 * Gets called when an InSlot gets disconnected from
		 * the last OutSlot.
		 * <p>
		 * You can use this callback to get a notification
		 * when you can stop reading values from an {@link
		 * InSlot InSlot}.
		 * @param inSlot The InSlot that generated the event.
		 */
		virtual void stopInSlot(BasicInSlot &inSlot);

		/**
		 * Gets called when there is new data available on
		 * an InSlot.
		 * <p>
		 * You can use this callback to get a notification
		 * when you can read a data value from an {@link
		 * InSlot InSlot} using the #top or #pop methods.
		 * @param inSlot The InSlot that generated the event.
		 */
		virtual void newData(BasicInSlot &inSlot);
	};

	/**
	 * Destroys the %BasicInSlot object.
	 */
	virtual ~BasicInSlot();

	/**
	 * Returns the direction of the Slot (In or Out).
	 * @return The direction (always Slot::In for InSlots)
	 */
	virtual Direction getDirection() const;

	/**
	 * Adds a {@link Listener Listener} to the InSlot. After calling
	 * this methods, the listener receives status information from
	 * the InSlot.
	 * <p>
	 * Listeners can be used to receive notifications when the status
	 * of an InSlot changes, i.e. when the first {@link OutSlot
	 * OutSlot} connects to the InSlot, the last {@link OutSlot
	 * OutSlot} disconnects from the InSlot, or new data is
	 * available on the InSlot. Instead of implementing the
	 * {@link Listener Listener} interface, you can also create
	 * a descendant of the InSlot class and override the {@link
	 * #startInSlot startInSlot}, {@link #stopInSlot stopInSlot} and
	 * {@link #newData newData} methods.
	 * @param listener The listener.
	 * @see Listener Listener
	 * @see #removeListener removeListener
	 * @see #startInSlot startInSlot
	 * @see #stopInSlot stopInSlot
	 * @see #newData newData
	 */
	void addListener(Listener &listener);

	/**
	 * Removes a {@link Listener Listener} from the InSlot. After
	 * calling this method, the listener does not receive status
	 * notifications any more.
	 * @param listener The Listener.
	 * @see #addListener addListener
	 */
	void removeListener(Listener &listener);

	/**
	 * Returns if data is available on the InSlot.
	 * All method that receive data values (top,
	 * pop) block when no data is available until data is received
	 * from an OutSlot. By using this method, you can prevent this blocking
	 * by checking if data is actually available.
	 * @return false when data is available, true otherwise.
	 */
	virtual bool empty() const = 0;

	/**
	 * Interrupts one thread blocked at the pop or top methods.
	 */
	void interrupt();

	/**
	 * Connects an OutSlot to this InSlot. Usually, {@link OutSlot
	 * OutSlots} and InSlots are not connected manually. They are connected
	 * automatically by adding them to {@link Namespace Namespaces}. This
	 * method just exists for rare special circumstances where it might be
	 * helpful to connect slots manually.
	 * <p>
	 * It is only possible to connect {@link OutSlot OutSlots} and InSlots
	 * that have the same C++ data type.
	 * <p>
	 * It is possible to add the same OutSlot more than once to an InSlot.
	 * Nevertheless, just one connection is created. For each call of the
	 * <code>connect</code> method, there has to be a corresponding call to
	 * the <code>#disconnect</code> method. The slots do not get disconnected
	 * until the last call of the disconnect method.
	 * @param outSlot The OutSlot.
	 * @see #disconnect
	 */
	virtual void connect(BasicOutSlot *outSlot) = 0;

	/**
	 * Disconnects an OutSlot from this InSlot. Usually, {@link OutSlot
	 * OutSlots} and InSlots are not disconnected manually. The are disconnected
	 * automatically by removing them from {@link Namespace Namespaces}. This
	 * method just exists for rare special circumstances where it might be
	 * helpful to disconnect slots manually.
	 * <p>
	 * It is possible to add the same OutSlot more than once to an InSlot.
	 * Nevertheless, just one connection is created. For each call of the
	 * <code>#connect</code> method, there has to be a corresponding call to
	 * the <code>disconnect</code> method. The slots do not get disconnected
	 * until the last call of the disconnect method.
	 * @param outSlot The OutSlot.
	 * @see #connect
	 */
	virtual void disconnect(BasicOutSlot *outSlot) = 0;

protected:

	/**
	 * Creates a new %BasicInSlot object.
	 * @param description The description of this InSlot. This information
	 * is not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 */
	explicit BasicInSlot(const std::string &description = std::string());

	/**
	 * Gets called when an InSlot gets connected to the
	 * first OutSlot.
	 * You can override this method to get a notification
	 * when you should start reading values from an {@link
	 * InSlot InSlot}. The default implementation of this method
	 * calls the startInSlot method of all listeners.
	 */
	virtual void startInSlot();

	/**
	 * Gets called when an InSlot gets disconnected from
	 * the last OutSlot.
	 * You can override this method to get a notification
	 * when you can stop reading values from an {@link
	 * InSlot InSlot}. The default implementation of this method
	 * calls the stopInSlot method of all listeners.
	 */
	virtual void stopInSlot();

	/**
	 * Gets called when there is new data available on
	 * the InSlot.
	 * You can override this method to get a notification
	 * when you can read a data value from an {@link
	 * InSlot InSlot} using the #top or #pop methods.
	 * The default implementation of this method calls the
	 * newData method of all listeners.
	 */
	virtual void newData();

	/**
	 * Increments the semaphore used to block threads in the
	 * top and pop methods.
	 */
	void up() const;

	/**
	 * Decrements the semaphore used to block threads in the
	 * top and pop methods.
	 */
	void down() const;

	/** Flag that is set to true when the interrupt method is called */
	// protected by mutex_
	bool interrupted_;

private:

	/** Defines the set of listeners */
	typedef std::set<Listener*> ListenerSet;

	/** The listeners that listen on this InSlot */
	// protected by listenerMutex_
	ListenerSet listeners_;

	/** Semaphore that blocks threads in the top and pop methods */
	Medusa::Semaphore *semaphore_;

	/** Copy constructor (not implemented!) */
	BasicInSlot(const BasicInSlot &);

	/** Copy operator (not implemented!) */
	const BasicInSlot &operator=(const BasicInSlot &);
};


} // namespace InstantIO


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif //__INSTANTIO_BASICINSLOT_H
