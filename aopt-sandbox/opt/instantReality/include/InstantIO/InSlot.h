#ifndef __INSTANTIO_INSLOT_H
#define __INSTANTIO_INSLOT_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: InSlot.h                                                        //
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
#include "BasicInSlot.h"
#include "OutSlot.h"
#include "Data.h"
#include "TypeName.h"
#include "InterruptedException.h"
#include <string>
#include <vector>
#include <typeinfo>
#ifdef __sgi
# include <assert.h>
#else
# include <cassert>
#endif


namespace InstantIO
{


template <class T> class OutSlot;


/**
 * @class InSlot InSlot.h InstantIO/InSlot.h
 *
 * Allows to receive data values from other software components.
 * <p>
 * For each kind of information you want to receive from other
 * software components, you have to create an %InSlot. InSlots
 * have a type and an optional description that you
 * have to specify when creating an %InSlot object.
 * <p>
 * The type specifies the C++ type of the data you want to
 * receive on the %InSlot.
 * <p>
 * The description simply is a human-readable string that can be
 * used to describe more in detail the purpose of the %InSlot. It
 * is not used by the InstantIO system, but is displayed in user
 * interfaces used to configure the InstantIO system.
 * <p>
 * Before you can actually receive data values from the %InSlot,
 * you have to connect it to one or more OutSlots. You can do this
 * manually by calling the #connect method of either the %InSlot or
 * the OutSlots, but usually you add the %InSlot to a Namespace
 * object. When you add an %InSlot to a Namespace, you have to
 * specify a label. Namespaces automatically connect InSlots and
 * OutSlots contained in the Namespace that fulfill the following
 * constraints:
 * <ol>
 * <li>They have the same C++ type,</li>
 * <li>and they have the same label, or there is a route defined in
 * the Namespace that maps the OutSlot label to the %InSlot label.
 * </ol>
 * <p>
 * An %InSlot can be added to as many Namespaces as necessary.
 * <p>
 * When you do not need to receive values from the %InSlot anymore,
 * you simply disconnect it from the OutSlots by calling the
 * #disconnect method or by removing it from all Namespaces.
 * <p>
 * The following code snippet demonstrates how to receive button
 * press events from other software components. These events are
 * boolean values, so we create an %InSlot with the C++ type "bool".
 * Then we add it to the root namespace (the Root singleton), give
 * it the label "Button", and start receiving values by using the
 * #pop method:
 * <p>
 * <code><pre>
 * #include <InstantIO/InSlot.h>
 * #include <InstantIO/Root.h>
 * ...
 * InstantIO::%InSlot&lt;bool&gt; inSlot;
 * InstantIO::Root::the().addInSlot("Button", &inSlot);
 * ...
 * while (loop == true)
 * {
 *   ...
 *   bool value = inSlot.pop();
 *   ...
 * }
 * ...
 * InstantIO::Root::the().removeInSlot("Button", &inSlot);
 * ...
 * </pre></code>
 * <p>
 * Sometimes it is important not only to receive the data value,
 * but also the timestamp when the data value has been created.
 * The #pop method actually does not return the data value
 * directly, but a Data object. This Data object usually gets
 * automatically converted to the data value as demonstrated
 * above, but you can use this Data object to receive the
 * timestamp. The timestamp is an <code>unsigned long</code>
 * value that contains the number of milliseconds since midnight
 * January 1., 1970 UTC. The following code snippet demonstrates
 * how to use the Data object:
 * <p>
 * <code><pre>
 * #include <InstantIO/Data.h>
 * ...
 * InstantIO::Data<bool> data = inSlot.pop();
 * bool value = data.getValue();
 * unsigned long timeStamp = data.getTimeStamp();
 * ...
 * </pre></code>
 * <p>
 * Please keep in mind that the %InSlot does not buffer data values.
 * When a new data value is received before the current data value
 * has been read by the application, the current data value gets
 * overwritten by the new value. Under many circumstances, this
 * behaviour is ok, but when you need to receive all data values,
 * you have to use a BufferedInSlot object.
 * <p>
 * @see OutSlot
 * @see Namespace
 * @see Root
 * @see BufferedInSlot
 * @see Data
 * @author Patrick D&auml;hne
 */
template <class T> class InSlot: public BasicInSlot
{

/** Needs access to the addOutSlot and removeOutSlot methods */
friend class OutSlot<T>;

public:

	/**
	 * Creates a new %InSlot object.
	 * @param description The description of this %InSlot. This information
	 * is not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 */
	inline explicit InSlot(const std::string &description = std::string())
	: BasicInSlot(description), outSlots_(), currentValue_(), empty_(true)
	{}

	/**
	 * Destroys the %InSlot object.
	 */
	virtual ~InSlot() {};

	/**
	 * Returns a human-readable string that describes the type
	 * that can be received by this %InSlot. This information is
	 * not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 * @return A string that describes the type that can be
	 * received by this %InSlot.
	 */
	virtual const char *getTypeName() const
	{ return TypeName<T>::getName(); };

	/**
	 * Returns the RTTI type info object that describes the type
	 * that can be received by this InSlot.
	 * @return The RTTI type info object.
	 */
	virtual const std::type_info &getTypeID() const
	{ return typeid(T); };

	/**
	 * Returns if the %InSlot is connected to an OutSlot. When an
	 * %InSlot is not connected to an OutSlot, it is not necessary
	 * to receive values from it. This information can be used to
	 * improve performance of applications.
	 * <p>
	 * Instead of using this method, you can also implement a
	 * descendant of the BasicInSlot::Listener class and add an instance of this
	 * class to the %InSlot by using the BasicInSlot::addListener method.
	 * @return true when the %InSlot is connected to an OutSlot,
	 * false otherwise.
	 */
	inline bool isConnected() const
	{
		lock();
		bool result = outSlots_.empty() == false;
		unlock();
		return result;
	}

	/**
	 * Returns if data is available on the %InSlot.
	 * All method that receive data values ({@link #top top}, {@link
	 * #pop pop}) block when no data is available until data is received
	 * from an OutSlot. By using this method, you can prevent this blocking
	 * by checking if data is actually available.
	 * @return false when data is available, true otherwise.
	 */
	virtual bool empty() const
	{
		lock();
		bool result = empty_;
		unlock();
		return result;
	};

	/**
	 * Returns the current data value without removing it from
	 * the %InSlot.
	 * <p>
	 * To remove the data value from the %InSlot, you can call the
	 * {@link #pop pop} method.
	 * @return The current data value.
	 * @throw InterruptedException when the interrupt method has
	 * been called
	 * @see #pop
	 */
	virtual const Data<T> top()
	/*throw (InterruptedException)*/
	{
		lock();
		if (interrupted_ == true)
		{
			interrupted_ = false;
			unlock();
			throw InterruptedException();
		}
		Data<T> value = currentValue_;
		unlock();
		return value;
	};

	/**
	 * Removes the current data value from the %InSlot and
	 * returns it.
	 * <p>
	 * This method blocks when no data is available until data
	 * is received from an OutSlot. By using the
	 * {@link #empty empty} method you can check if data is
	 * available before calling this method.
	 * <p>
	 * To get the data value without removing it from the %InSlot,
	 * use the {@link #top top} method.
	 *
	 * @return The current data value.
	 * @throw InterruptedException when the interrupt method has
	 * been called
	 * @see #empty
	 * @see #top
	 */
	virtual const Data<T> pop()
	/*throw (InterruptedException)*/
	{
		down();
		lock();
		if (interrupted_ == true)
		{
			interrupted_ = false;
			unlock();
			throw InterruptedException();
		}
		empty_ = true;
		Data<T> value = currentValue_;
		unlock();
		return value;
	};

	/**
	 * Writes a new data value into the %InSlot. Usually, you
	 * do not write data values directly into an %InSlot. InSlots
	 * receive data values from {@link OutSlot OutSlots} they are
	 * connected to. This method is available for rare special
	 * circumstances where it might be helpful to write data values
	 * manually into the %InSlot.
	 * @param data A Data object that contains a new data value and
	 * the time when it has been created.
	 */
	virtual void push(const Data<T> &data)
	{
		lock();
		if (empty_ == true)
		{
			empty_ = false;
			up();
		}
		currentValue_ = data;
		unlock();
		newData();
	};

	/**
	 * Connects an OutSlot to this %InSlot. Usually, {@link OutSlot
	 * OutSlots} and InSlots are not connected manually. They are connected
	 * automatically by adding them to {@link Namespace Namespaces}. This
	 * method just exists for rare special circumstances where it might be
	 * helpful to connect slots manually.
	 * <p>
	 * It is only possible to connect {@link OutSlot OutSlots} and InSlots
	 * that have the same C++ data type.
	 * <p>
	 * It is possible to add the same OutSlot more than once to an %InSlot.
	 * Nevertheless, just one connection is created. For each call of the
	 * <code>connect</code> method, there has to be a corresponding call to
	 * the <code>#disconnect</code> method. The slots do not get disconnected
	 * until the last call of the disconnect method.
	 * @param outSlot The OutSlot.
	 * @see #disconnect
	 */
	virtual void connect(BasicOutSlot *outSlot)
	{
		OutSlot<T> *os = dynamic_cast<OutSlot<T>*>(outSlot);
		if (os != 0)
		{
			// Do not change the order of the following two calls!
			// We have to make sure that we call startInSlot before
			// we receive the first data value!
			addOutSlot(os);
			os->addInSlot(this);
		}
	};

	/**
	 * Disconnects an OutSlot from this %InSlot. Usually, {@link OutSlot
	 * OutSlots} and InSlots are not disconnected manually. The are disconnected
	 * automatically by removing them from {@link Namespace Namespaces}. This
	 * method just exists for rare special circumstances where it might be
	 * helpful to disconnect slots manually.
	 * <p>
	 * It is possible to add the same OutSlot more than once to an %InSlot.
	 * Nevertheless, just one connection is created. For each call of the
	 * <code>#connect</code> method, there has to be a corresponding call to
	 * the <code>disconnect</code> method. The slots do not get disconnected
	 * until the last call of the disconnect method.
	 * @param outSlot The OutSlot.
	 * @see #connect
	 */
	virtual void disconnect(BasicOutSlot *outSlot)
	{
		OutSlot<T> *os = dynamic_cast<OutSlot<T>*>(outSlot);
		if (os != 0)
		{
			// Do not change the order of the following two calls!
			// We have to make sure that we do not receive any new
			// data values before we call stopInSlot.
			os->removeInSlot(this);
			removeOutSlot(os);
		}
	};

private:

	/** Defines a pair of an OutSlot and a reference counter */
	typedef std::pair<OutSlot<T>*, unsigned long> RefCountedOutSlot;

	/** Defines a vector of reference-counted OutSlots */
	typedef std::vector<RefCountedOutSlot> OutSlotVector;

	/**
	 * Adds an OutSlot to the set of OutSlots that are
	 * connected to this %InSlot.
	 * @param outSlot The OutSlot.
	 */
	void addOutSlot(OutSlot<T> *outSlot)
	{
		assert(outSlot != 0);
		bool empty = false;
		// We have to lock the listenerMutex_ before we actually change
		// the vector of OutSlots to ensure that no stopInSlot call
		// can overtake the startInSlot call at the end of this method.
		// This should not make problems, because
		// - the code between lock() and unlock() cannot block
		// - we always call listenersLock() before we call lock()
		listenersLock();
		lock();
		typename OutSlotVector::iterator it = findOutSlot(outSlot);
		if (it == outSlots_.end())
		{
			empty = outSlots_.empty();
			outSlots_.push_back(RefCountedOutSlot(outSlot, 1));
		}
		else
			++(it->second);
		unlock();
		if (empty == true)
			startInSlot();
		listenersUnlock();
	};

	/**
	 * Removes an OutSlot from the set of OutSlots that are
	 * connected to this %InSlot.
	 * @param outSlot The OutSlot.
	 */
	void removeOutSlot(OutSlot<T> *outSlot)
	{
		assert(outSlot != 0);
		// We have to lock the listenerMutex_ before we actually change
		// the vector of OutSlots to ensure that no startInSlot call
		// can overtake the stopInSlot call at the end of this method.
		// This should not make problems, because
		// - the code between lock() and unlock() cannot block
		// - we always call listenersLock() before we call lock()
		listenersLock();
		lock();
		typename OutSlotVector::iterator it = findOutSlot(outSlot);
		if ((it != outSlots_.end()) && (--(it->second) == 0))
			outSlots_.erase(it);
		unlock();
		if (outSlots_.empty()) // protected by listenerMutex_
			stopInSlot();
		listenersUnlock();
	};

	/**
	 * Tries to find an OutSlot in the vector of OutSlots.
	 * @param outSlot The OutSlot.
	 * @return An iterator pointing to the position of the
	 * OutSlot in the vector of OutSlots.
	 */
	typename OutSlotVector::iterator findOutSlot(OutSlot<T> *outSlot)
	{
		// mutex_ must be locked before calling this method!
		assert(outSlot != 0);
		typename OutSlotVector::iterator it;
		for (it = outSlots_.begin(); it != outSlots_.end(); ++it)
			if (it->first == outSlot)
				break;
		return it;
	};

	/** The OutSlots this %InSlot is connected to */
	// protected by mutex_
	OutSlotVector outSlots_;

	/** The last data value received by this %InSlot */
	// protected by mutex_
	Data<T> currentValue_;

	/** True when this %InSlot contains unread data */
	// protected by mutex_
	bool empty_;

	/** Copy constructor (not implemented!) */
	InSlot(const InSlot<T> &);

	/** Copy operator (not implemented!) */
	const InSlot<T> &operator=(const InSlot<T> &);
};


} // namespace InstantIO


#endif //__INSTANTIO_INSLOT_H
