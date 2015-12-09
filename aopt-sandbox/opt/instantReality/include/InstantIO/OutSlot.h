#ifndef __INSTANTIO_OUTSLOT_H
#define __INSTANTIO_OUTSLOT_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: OutSlot.h                                                       //
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
#include "BasicOutSlot.h"
#include "InSlot.h"
#include "Data.h"
#include "TypeName.h"
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


template <class T> class InSlot;


/**
 * @class OutSlot OutSlot.h InstantIO/OutSlot.h
 *
 * Allows to send data values to other software components.
 * <p>
 * For each kind of information you want to send to other
 * software components, you have to create an %OutSlot. OutSlots
 * have a label and an optional description that you
 * have to specify when creating an %OutSlot object.
 * <p>
 * The type specifies the C++ type of the data you want to
 * send on the %OutSlot.
 * <p>
 * The description simply is a human-readable string that can be
 * used to describe more in detail the purpose of the %OutSlot. It
 * is not used by the InstantIO system, but is displayed in user
 * interfaces used to configure the InstantIO system.
 * <p>
 * Before you can actually send data values via the %OutSlot,
 * you have to connect it to one or more InSlots. You can do this
 * manually by calling the #connect method of either the OutSlot or
 * the InSlots, but usually you add the %OutSlot to a Namespace
 * object. When you add an %OutSlot to a Namespace, you have to
 * specify a label. Namespaces automatically connect OutSlots and
 * InSlots contained in the Namespace that fulfill the following
 * constraints:
 * <ol>
 * <li>They have the same C++ type,</li>
 * <li>and they have the same label, or there is a route defined in
 * the Namespace that maps the %OutSlot label to the InSlot label.
 * </ol>
 * <p>
 * An %OutSlot can be added to as many Namespaces as necessary.
 * <p>
 * When you do not need to receive values from the %OutSlot anymore,
 * you simply disconnect it from the InSlots by calling the
 * #disconnect method or by removing it from all Namespaces.
 * <p>
 * The following code snippet demonstrates how to send button
 * press events to other software components. These events are
 * boolean values, so we create an %OutSlot with the C++ type "bool".
 * Then we add it to the root namespace (the Root singleton), give
 * it the label "Button", and start sending values by using the
 * {@link #push push} method:
 * <p>
 * <code><pre>
 * #include <InstantIO/OutSlot.h>
 * #include <InstantIO/Root.h>
 * ...
 * InstantIO::%OutSlot&lt;bool&gt; outSlot;
 * InstantIO::Root::the().addOutSlot(outSlot);
 * ...
 * while (loop == true)
 * {
 *   ...
 *   bool value = ...;
 *   outSlot.push(value);
 *   ...
 * }
 * ...
 * InstantIO::Root::the().removeOutSlot(outSlot);
 * ...
 * </pre></code>
 * <p>
 * Sometimes it is important not only to send the data value,
 * but also the timestamp when the data value has been created
 * (by default, the timestamp of each data value is set to the
 * current system time when it is pushed into the %OutSlot).
 * The #push method actually does not take a data value as parameter,
 * but a Data object. %Data values are usually converted automatically
 * into Data objects as demonstrated above, but you can use Data
 * objects to set the timestamp. The timestamp is an
 * <code>unsigned long</code> value that contains the number of milliseconds
 * since midnight January 1., 1970 UTC. The following code snippet
 * demonstrates how to use the {@link #push push} method with Data
 * objects:
 * <p>
 * <code><pre>
 * #include <InstantIO/Data.h>
 * ...
 * bool value = ...;
 * unsigned long timeStamp = ...;
 * Data data;
 * data.setValue(value);
 * data.setTimeStamp(timeStamp);
 * outSlot.push(data);
 * ...
 * </pre></code>
 * <p>
 * @see InSlot
 * @see Namespace
 * @see Root
 * @see Data
 * @author Patrick D&auml;hne
 */
template <class T> class OutSlot: public BasicOutSlot
{

    /** Needs access to the addInSlot and removeInSlot methods */
friend class InSlot<T>;

public:

	/**
	 * Creates a new %OutSlot object.
	 */
	inline OutSlot()
	: BasicOutSlot(), inSlots_(), currentValue_()
	{}

	/**
	 * Creates a new %OutSlot object.
	 * @param description The description of this %OutSlot. This information
	 * is not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 */
	inline explicit OutSlot(const std::string &description)
	: BasicOutSlot(description), inSlots_(), currentValue_()
	{}

	/**
	 * Creates a new %OutSlot object.
	 * @param defaultValue The initial value of this %OutSlot.
	 */
	inline explicit OutSlot(const Data<T> &defaultValue)
	: BasicOutSlot(), inSlots_(), currentValue_(defaultValue)
	{}

	/**
	 * Creates a new %OutSlot object.
	 * @param description The description of this %OutSlot. This information
	 * is not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 * @param defaultValue The initial value of this %OutSlot.
	 */
	inline OutSlot(const std::string &description, const Data<T> &defaultValue)
	: BasicOutSlot(description), inSlots_(), currentValue_(defaultValue)
	{}

	/**
	 * Destroys the %OutSlot object.
	 */
	virtual ~OutSlot() {};

	/**
	 * Returns a human-readable string that describes the type
	 * that can be sent to this OutSlot. This information is
	 * not used by the InstantIO system directly, instead it is displayed
	 * for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 * @return A string that describes the type that can be
	 * sent to this OutSlot.
	 */
	virtual const char *getTypeName() const
	{ return TypeName<T>::getName(); };

	/**
	 * Returns the RTTI type info object that describes the type
	 * that can be sent to this OutSlot.
	 * @return The RTTI type info object.
	 */
	virtual const std::type_info &getTypeID() const
	{ return typeid(T); };

	/**
	 * Returns if the %OutSlot is connected to an InSlot. When an
	 * %OutSlot is not connected to an InSlot, it is not necessary
	 * to write values into it. This information can be used to
	 * improve performance of applications.
	 * <p>
	 * Instead of using this method, you can also implement a
	 * descendant of the BasicOutSlot::Listener class and add an instance of this
	 * class to the %OutSlot by using the BasicOutSlot::addListener method.
	 * @return true when the %OutSlot is connected to an InSlot,
	 * false otherwise.
	 */
	inline bool isConnected() const
	{
		lock();
		bool result = inSlots_.empty() == false;
		unlock();
		return result;
	}

	/**
	 * Returns the last data value written into this
	 * %OutSlot. By using this method, you can check if a new
	 * value you want to write into the %OutSlot actually
	 * differs from the one written previously. In this case,
	 * in many cases you can skip writing the new value into
	 * the %OutSlot. This allows to conserver resources (especially
	 * when sending data values via the network).
	 * @return The last data value written into the %OutSlot.
	 */
	inline const Data<T> getValue() const
	{
		lock();
		Data<T> result = currentValue_;
		unlock();
		return result;
	}

	/**
	 * Writes a new data value into the %OutSlot. The data value is send
	 * to all {@link InSlot InSlots} the %OutSlot is currently connected
	 * to.
	 * @param value The new data value.
	 */
	virtual void push(const Data<T> &value)
	{
		lock();
		currentValue_ = value;
		valueIsValid_ = true;
		typename InSlotVector::iterator it;
		for (it = inSlots_.begin(); it != inSlots_.end(); ++it)
		{
			assert(it->first != 0);
			it->first->push(value);
		}
		unlock();
	};

	/**
	 * Connects an InSlot to this %OutSlot. Usually, {@link InSlot
	 * InSlots} and %OutSlots are not connected manually. They are connected
	 * automatically by adding them to {@link Namespace Namespaces}. This
	 * method just exists for rare special circumstances where it might be
	 * helpful to connect slots manually.
	 * <p>
	 * It is only possible to connect {@link InSlot InSlots} to this
	 * %OutSlot that have the same data type.
	 * <p>
	 * It is possible to add the same {@link InSlot InSlot} more than
	 * once to an %OutSlot. Nevertheless, just one connection is created.
	 * For each call of the <code>connect</code> method, there has to be
	 * a corresponding call to the <code>#disconnect</code> method. The
	 * slots do not get disconnected until the last call of the disconnect
	 * method.
	 * @param inSlot The InSlot.
	 * @see #disconnect
	 */
	virtual void connect(BasicInSlot *inSlot)
	{
		InSlot<T> *is = dynamic_cast<InSlot<T>*>(inSlot);
		if (is != 0)
		{
			// Do not change the order of the following two calls!
			// We have to make sure that we call startInSlot before
			// we send the first data value to the InSlot!
			is->addOutSlot(this);
			addInSlot(is);
		}
	};

	/**
	 * Disconnects an InSlot from this %OutSlot. Usually, {@link InSlot
	 * InSlots} and %OutSlots are not disconnected manually. The are disconnected
	 * automatically by removing them from {@link Namespace Namespaces}. This
	 * method just exists for rare special circumstances where it might be
	 * helpful to disconnect slots manually.
	 * <p>
	 * It is possible to add the same {@link InSlot InSlot} more than
	 * once to an %OutSlot. Nevertheless, just one connection is created.
	 * For each call of the <code>#connect</code> method, there has to be
	 * a corresponding call of the <code>disconnect</code> method. The slots
	 * do not get disconnected until the last call of the disconnect method.
	 * @param inSlot The InSlot.
	 * @see #connect connect
	 */
	virtual void disconnect(BasicInSlot *inSlot)
	{
		InSlot<T> *is = dynamic_cast<InSlot<T>*>(inSlot);
		if (is != 0)
		{
			// Do not change the order of the following two calls!
			// We have to make sure that we do not send new data
			// to the InSlot after we called stopInSlot
			removeInSlot(is);
			is->removeOutSlot(this);
		}
	};

    virtual BasicInSlot *createMatchingInSlot()
    {
        return new InSlot<T>();
    }

private:

	/** Defines a pair of an InSlot and a reference counter */
	typedef std::pair<InSlot<T>*, unsigned long> RefCountedInSlot;

	/** Defines a vector of reference-counted OutSlots */
	typedef std::vector<RefCountedInSlot> InSlotVector;

	/**
	 * Adds an InSlot to the set of InSlots that are
	 * connected to this %OutSlot.
	 * @param inSlot The InSlot.
	 */
	void addInSlot(InSlot<T> *inSlot)
	{
		assert(inSlot != 0);
		bool empty = false;
		// We have to lock the listenerMutex_ before we actually change
		// the vector of InSlots to ensure that no stopOutSlot call
		// can overtake the startOutSlot call at the end of this method.
		// This should not make problems, because
		// - the code between lock() and unlock() cannot block
		// - we always call listenersLock() before we call lock()
		listenersLock();
		lock();
		typename InSlotVector::iterator it = findInSlot(inSlot);
		if (it == inSlots_.end())
		{
			empty = inSlots_.empty();
			inSlots_.push_back(RefCountedInSlot(inSlot, 1));
			if (valueIsValid_ == true)
				inSlot->push(currentValue_);
		}
		else
			++(it->second);
		unlock();
		if (empty == true)
			startOutSlot();
		listenersUnlock();
	};

	/**
	 * Removes an InSlot from the set of InSlots that are
	 * connected to this %OutSlot.
	 * @param inSlot The InSlot.
	 */
	void removeInSlot(InSlot<T> *inSlot)
	{
		assert(inSlot != 0);
		// We have to lock the listenerMutex_ before we actually change
		// the vector of InSlots to ensure that no startOutSlot call
		// can overtake the stopOutSlot call at the end of this method.
		// This should not make problems, because
		// - the code between lock() and unlock() cannot block
		// - we always call listenersLock() before we call lock()
		listenersLock();
		lock();
		typename InSlotVector::iterator it = findInSlot(inSlot);
		if ((it != inSlots_.end()) && (--(it->second) == 0))
			inSlots_.erase(it);
		unlock();
		if (inSlots_.empty()) // protected by listenerMutex_
			stopOutSlot();
		listenersUnlock();
	};

	/**
	 * Tries to find an InSlot in the vector of InSlots.
	 * @param inSlot The InSlot.
	 * @return An iterator pointing to the position of the
	 * InSlot in the vector of InSlots.
	 */
	typename InSlotVector::iterator findInSlot(InSlot<T> *inSlot)
	{
		// mutex_ must be locked before calling this method!
		assert(inSlot != 0);
		typename InSlotVector::iterator it;
		for (it = inSlots_.begin(); it != inSlots_.end(); ++it)
			if (it->first == inSlot)
				break;
		return it;
	};

	/** The InSlots this %OutSlot is connected to */
	// protected by mutex_
	InSlotVector inSlots_;

	/** The last data value sent to this %OutSlot */
	// protected by mutex_
	Data<T> currentValue_;

	/** Copy constructor (not implemented!) */
	OutSlot(const OutSlot<T> &);

	/** Copy operator (not implemented!) */
	const OutSlot<T> &operator=(const OutSlot<T> &);
};


} // namespace InstantIO


#endif //__INSTANTIO_OUTSLOT_H
