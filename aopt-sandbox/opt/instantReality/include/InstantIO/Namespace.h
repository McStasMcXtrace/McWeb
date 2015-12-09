#ifndef __INSTANTIO_NAMESPACE_H
#define __INSTANTIO_NAMESPACE_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Namespace.h                                                     //
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
#include "VariableReplacer.h"
#include "Slot.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
namespace Medusa
{
	class Mutex;
}
namespace Hermes
{
	class URL;
}
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4251)
#endif


namespace InstantIO
{


class BasicOutSlot;
class BasicInSlot;
class Node;


/**
 * @class Namespace Namespace.h InstantIO/Namespace.h
 *
 * Container for InSlots and OutSlots.
 * <p>
 * %Namespace objects are used to automatically connect {@link
 * InSlot InSlots} and {@link OutSlot OutSlots}. InSlots and
 * OutSlots connect to each other when they
 * <ul>
 * <li> are members of the same %Namespace, and
 * <li> have the same C++ data type, and
 * <li> have the same label, or a route exists in the
 * %Namespace that maps the OutSlot label to the InSlot label.
 * </ul>
 * So, when you use the InstantIO system to exchange data values
 * between different software components, you usually create
 * a %Namespace, add your InSlots and OutSlots to that namespace,
 * and create routes between these InSlots and OutSlots. The
 * following piece of code demonstrates how to connect an
 * OutSlot "out" to an InSlot "in":
 * <code><pre>
 * #include <InstantIO/Namespace.h>
 * #include <InstantIO/OutSlot.h>
 * #include <InstantIO/InSlot.h>
 * ...
 * // Create the namespace
 * InstantIO::Namespace namespace;
 * 
 * // Create the outslot and add it to the namespace
 * InstantIO::OutSlot<int> outSlot;
 * namespace.addOutSlot("out", outSlot);
 * 
 * // Create the inslot and add it to the namespace
 * InstantIO::InSlot<int> inSlot;
 * namespace.addInSlot("in", inSlot);
 * 
 * // Create a route that maps "out" to "in"
 * namespace.addRoute("out", "in");
 * 
 * // Enable the namespace
 * namespace.enable();
 * ...
 * </pre></code>
 * In this example, the OutSlot and the InSlot get connected
 * when the namespace is enabled. A namespace is disabled
 * by default. A namespace connects OutSlots and InSlots only
 * when it is enabled. It is perfectly ok to enable the
 * namespace before you add any OutSlots, InSlots and routes.
 * In this case OutSlots and InSlots get connected as soon as
 * all preconditions to connect the are fulfilled.
 * <p>
 * In the example above, we created a new %Namespace object.
 * But when there are lots of different software components
 * that need to exchange data, it gets difficult to provide them
 * with the same namespace object. For this reason, a special
 * descendant of the %Namespace class exists: The {@link Root
 * Root} singleton. Exactly one instance of this class exists
 * in an application. So all software components can obtain
 * this single instance and add their OutSlots and InSlots to
 * it. The following piece of code demonstrates how to do that:
 * <code><pre>
 * #include <InstantIO/Root.h>
 * ...
 * // Get the root namespace
 * InstantIO::Namespace &root = InstantIO::Root::the();
 * 
 * // Add OutSlots and InSlots to the root namespace
 * root.addOutSlot("out", outSlot);
 * root.addInSlot("in", inSlot);
 * ...
 * </pre></code>
 * The Root singleton is enabled by default.
 * <p>
 * Besides of adding slots and routes to namespaces, you can
 * also add subnamespaces. Subnamespaces are normal %Namespace
 * objects, i.e. you can add slots and routes to them. By using
 * so-called "external routes", you can export OutSlots and
 * InSlots from subnamespaces to their parent namespace. The
 * following piece of code demonstrates how to create a
 * subnamespace, and how to export the slots in the subname
 * to the parent namespace.
 * <code><pre>
 * ...
 * // Get the root namespace
 * InstantIO::Namespace &root = InstantIO::Root::the();
 * 
 * // Create a subnamespace
 * std::auto_ptr<InstantIO::Namespace> subnamespace = new InstantIO::Namespace;
 * 
 * // Create the outslot and add it to the subnamespace
 * InstantIO::OutSlot<int> outSlot;
 * subnamespace->addOutSlot("subout", outSlot);
 * 
 * // Create the inslot and add it to the subnamespace
 * InstantIO::InSlot<int> inSlot;
 * subnamespace->addInSlot("subin", inSlot);
 * 
 * // Export the outslot as "out" to the root namespace
 * subnamespace->addExternalRoute("subout", "out");
 * 
 * // Export the inslot as "in" to the root namespace
 * subnamespace->addExternalRoute("subin", "in");
 * 
 * // Create a route that maps "out" to "in" in the root
 * // namespace
 * root.addRoute("out", "in");
 *
 * // Add the subnamespace to the root namespace
 * root.addNamespace("label", subnamespace);
 * ...
 * </pre></code>
 * In the example above, the outslot and the inslot get
 * connected, because there is a route from "out" to "in",
 * and the outslot and the inslot are exported under these
 * labels into the root namespace.
 * <p>
 * Subnamespaces inherit the enabled/disabled state of
 * their parent namespace. I.e. when you add a namespace
 * to an enabled namespace, it gets enabled, and when
 * you add a namespace to a disabled namespace, it gets
 * disabled. When you enabled or disable a parent namespace,
 * the subnamespaces get enabled or disabled likewise.
 *
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING Namespace
{
public:

	/**
	 * Allows to receive information about the status of a %Namespace.
	 * You can implement this interface when you need to get
	 * notifications when certain events happen to a %Namespace,
	 * i.e. when OutSlots or InSlots are added to or removed from the
	 * %Namespace.
	 * @author Patrick D&auml;hne
	 */
	class INSTANTIO_DLLMAPPING Listener
	{
	public:

		/** Destructor. */
		virtual ~Listener();

		/**
		 * Gets called when an OutSlot is added to the %Namespace.
		 * @param ns The namespace that calls the method.
		 * @param label The label of the OutSlot.
		 * @param outSlot The OutSlot.
		 */
		virtual void outSlotAdded(Namespace &ns, const std::string &label, BasicOutSlot &outSlot);

		/**
		 * Gets called when an OutSlot is removed from the %Namespace.
		 * @param ns The namespace that calls the method.
		 * @param label The label of the OutSlot.
		 * @param outSlot The OutSlot.
		 */
		virtual void outSlotRemoved(Namespace &ns, const std::string &label, BasicOutSlot &outSlot);

		/**
		 * Gets called when an InSlot is added to the %Namespace.
		 * @param ns The namespace that calls the method.
		 * @param label The label of the InSlot.
		 * @param inSlot The InSlot.
		 */
		virtual void inSlotAdded(Namespace &ns, const std::string &label, BasicInSlot &inSlot);

		/**
		 * Gets called when an InSlot is removed from the %Namespace.
		 * @param ns The namespace that calls the method.
		 * @param label The label of the InSlot.
		 * @param inSlot The InSlot.
		 */
		virtual void inSlotRemoved(Namespace &ns, const std::string &label, BasicInSlot &inSlot);

		/**
		 * Gets called when a route is added to the %Namespace.
		 * @param ns The namespace that calls the method.
		 * @param from The from part of the route.
		 * @param to The to part of the route.
		 */
		virtual void routeAdded(Namespace &ns, const std::string &from, const std::string &to);

		/**
		 * Gets called when a route is removed from the %Namespace.
		 * @param ns The namespace that calls the method.
		 * @param from The from part of the route.
		 * @param to The to part of the route.
		 */
		virtual void routeRemoved(Namespace &ns, const std::string &from, const std::string &to);
	};

	/// \name Constructors and destructor
	//@{

	/**
	 * Creates a new %Namespace object. The %Namespace is
	 * disabled by default and contains no slots, subnamespaces
	 * or routes.
	 */
	Namespace();

	/**
	 * Destroys a %Namespace object.
	 */
	virtual ~Namespace();

	//@}

	/**
	 * Adds a Listener to the %Namespace. You can use Listener
	 * objects when you need to get notifications when slots
	 * are added to or removed from the %Namespace.
	 * @param listener The listener.
	 */
	void addListener(Listener &listener);

	/**
	 * Removes a Listener from the %Namespace.
	 * @param listener The listener.
	 */
	void removeListener(Listener &listener);

	/**
	 * Enables the %Namespace. Only enabled namespaces connect
	 * their OutSlots and InSlots. Enabling a %Namespace also
	 * enables all subnamespaces. Calling this method on an
	 * already enabled namespace does not result in any action.
	 * @see #disable
	 * @see #enabled
	 */
	virtual void enable();

	/**
	 * Disables the %Namespace. Disabled namespaces disconnect
	 * their OutSlots and InSlots. Disabling a %Namespace also
	 * disables all subnamespaces. Calling this method on an
	 * already disabled namespace does not result in any action.
	 * @see #enable
	 * @see #enabled
	 */
	virtual void disable();

	/**
	 * Returns if the %Namespace is enabled or disabled. Namespaces
	 * only connect their OutSlots and InSlots when they are enabled.
	 * @return true when the %Namespace is enabled, false otherwise.
	 */
	bool enabled() const;

	/**
	 * Returns the label of the %Namespace.
	 * @return The label.
	 */
	const std::string &getLabel() const
	{ return label_; }

	/**
	 * Returns the type name of the %Namespace.
	 * @return The type name.
	 */
	virtual std::string typeName() const;

	/**
	 * Removes all routes, external routes and subnamespaces
	 * from this %Namespace.
	 */
	void clear();

	/**
	 * Returns if the contents of this %Namespace should be
	 * saved. The default implementation of this method always
	 * returns true. Overwrite this method when you implement
	 * a Node whoose contents should not be saved.
	 * @return true when the contents of this %Namespace should
	 * be saved, false otherwise
	 */
	virtual bool saveState() const;

	/**
	 * Returns if this %Namespaces has unsaved changes.
	 * @return true when the %Namespace needs to be saved.
	 */
	bool dirty() const;

	/**
	 * Sets the status of the dirty flag. The dirty flag is
	 * set (true) when the %Namespaces has unsaved changes.
	 * @param dirty true when the %Namespace needs to be saved,
	 * false otherwise. When you set the dirty flag to true,
	 * this method also sets the dirty flags of all parent
	 * namespaces to true. When you set the dirty flag to false,
	 * this method also sets the dirty flag of all children
	 * namespaces to false.
	 */
	void setDirty(bool dirty);

	/**
	 * Sets the base URL of the %Namespace. The base URL is
	 * used by Nodes to resolve relative URLs.
	 * @param url The base URL.
	 */
	void setBaseURL(const Hermes::URL &url);

	/**
	 * Returns the base URL of the %Namespace. The base URL is
	 * used by Nodes to resolve relative URLs.
	 * @return The base URL.
	 */
	const Hermes::URL &getBaseURL() const;

	/**
	 * Returns the mutex that protects the %Namespace.
	 * @return The mutex
	 */
	inline Medusa::Mutex *upMutex() const { return upMutex_; }

	// --------------------- Field interface --------------------

	/**
	 * Sets the values of fields from a line of parameters. The parameter
	 * line is a set of key-value parameters like this:
	 * "key1=value1 key2=value2". This method extracts the key-value pairs
	 * and calls setFieldValue(key, value).
	 * @param paramLine A string consisting of key-value pairs.
	 */
	void setFieldValues(const std::string &paramLine);

	/**
	 * Sets the value of a field.
	 * @param name The name of the field.
	 * @param value The new value of the field.
	 */
	virtual void setFieldValue(const std::string &name, const std::string &value);

	/**
	 * Returns the value of a field.
	 * @param name The name of the field.
	 * @return The value of the field.
	 */
	virtual std::string getFieldValue(const std::string &name) const
	/* throw (InvalidFieldException)*/;

	/**
	 * Returns the names of all fields.
	 * @param names A vector that gets filled with the names.
	 */
	virtual void getFieldNames(std::vector<std::string> &names) const;

	// --------------------- Slot interface --------------------

	/// \name Slot interface
	//@{

	class LabeledOutSlot;
	class LabeledInSlot;

	/**
	 * Combines a Slot and a label.
	 * @author Patrick D&auml;hne
	 */
	class LabeledSlot
	{
	public:

		/**
		 * Creates a new %LabeledSlot object.
		 * @param l The label.
		 * @param s The slot.
		 */
		inline LabeledSlot(const std::string &l, Slot *s)
		: label(l), slot(s)
		{}

		/**
		 * Creates a new %LabeledSlot object.
		 * @param labeledOutSlot A LabeledOutSlot object we take the information from.
		 */
		LabeledSlot(const LabeledOutSlot &labeledOutSlot);

		/**
		 * Creates a new %LabeledSlot object.
		 * @param labeledInSlot A LabeledInSlot object we take the information from.
		 */
		LabeledSlot(const LabeledInSlot &labeledInSlot);

		/** The label */
		std::string label;

		/** The slot */
		Slot *slot;
	};

	/** Defines the vector of labeled Slots */
	typedef std::vector<LabeledSlot> LabeledSlotVector;

	/**
	 * Removes a Slot from the %Namespace. The Slot
	 * gets disconnected from all Slots contained in the
	 * %Namespace it is currently connected to.
	 * @param label The label of the Slot in this %Namespace.
	 * @param slot The Slot.
	 */
	virtual void removeSlot(const std::string &label, Slot *slot);

	/**
	 * Returns all slots that match a label and a direction.
	 * @param label The label used for filtering the slots.
	 * @param direction The direction of the slots you are interested in.
	 * @param slotVector A vector that gets filled with the matching
	 * slots.
	 */
	virtual void getSlots(const std::string &label, Slot::Direction direction, LabeledSlotVector &slotVector);

	//@}

	// --------------------- Outslot interface --------------------

	/// \name Outslot interface
	//@{

	/**
	 * Combines an OutSlot, a label, and a reference counter.
	 * @author Patrick D&auml;hne
	 */
	class LabeledOutSlot
	{
	public:

		/**
		 * Creates a new %LabeledOutSlot object.
		 * @param l The label.
		 * @param os The outslot.
		 */
		inline LabeledOutSlot(const std::string &l, BasicOutSlot *os)
		: label(l), outSlot(os), counter(0)
		{}

		/** The label */
		std::string label;

		/** The outslot */
		BasicOutSlot *outSlot;

		/** The reference counter */
		int counter;
	};

	/** Defines the vector of OutSlots */
	typedef std::vector<LabeledOutSlot> LabeledOutSlotVector;

	/**
	 * Adds an OutSlot to the %Namespace. When the %Namespace
	 * is enabled, the OutSlot automatically gets connected
	 * to all InSlots contained in the %Namespace that have
	 * <ul>
	 * <li> the same data type, and
	 * <li> the same label, or a route exists that maps the
	 * label of the OutSlot to the label of the InSlot.
	 * </ul>
	 * @param label The label of the OutSlot in this %Namespace.
	 * @param outSlot The OutSlot.
	 */
	virtual void addOutSlot(const std::string &label, BasicOutSlot *outSlot);

	/**
	 * Removes an OutSlot from the %Namespace. The OutSlot
	 * gets disconnected from all InSlots contained in the
	 * %Namespace it is currently connected to.
	 * @param label The label of the OutSlot in this %Namespace.
	 * @param outSlot The OutSlot.
	 */
	virtual void removeOutSlot(const std::string &label, BasicOutSlot *outSlot);

	/**
	 * Returns all OutSlots that match a label.
	 * @param label The label used for filtering the OutSlots.
	 * @param outSlotVector A vector that gets filled with the matching
	 * OutSlots.
	 */
	virtual void getOutSlots(const std::string &label, LabeledOutSlotVector &outSlotVector);

	//@}

	// --------------------- Inslot interface --------------------

	/// \name Inslot interface
	//@{

	/**
	 * Combines an InSlot, a label, and a reference counter.
	 * @author Patrick D&auml;hne
	 */
	class LabeledInSlot
	{
	public:

		/**
		 * Creates a new %LabeledInSlot object.
		 * @param l The label.
		 * @param is The inslot.
		 */
		inline LabeledInSlot(const std::string &l, BasicInSlot *is)
		: label(l), inSlot(is), counter(0)
		{}

		/** The label */
		std::string label;

		/** The outslot */
		BasicInSlot *inSlot;

		/** The reference counter */
		int counter;
	};

	/** Defines the vector of InSlots */
	typedef std::vector<LabeledInSlot> LabeledInSlotVector;

	/**
	 * Adds an InSlot to the %Namespace. When the %Namespace
	 * is enabled, the InSlot automatically gets connected
	 * to all OutSlots contained in the %Namespace that have
	 * <ul>
	 * <li> the same data type, and
	 * <li> the same label, or a route exists that maps the
	 * label of the OutSlot to the label of the InSlot.
	 * </ul>
	 * @param label The label of the InSlot in this %Namespace.
	 * @param inSlot The InSlot.
	 */
	virtual void addInSlot(const std::string &label, BasicInSlot *inSlot);

	/**
	 * Removes an InSlot from the %Namespace. The InSlot
	 * gets disconnected from all OutSlots contained in the
	 * %Namespace it is currently connected to.
	 * @param label The label of the InSlot in this %Namespace.
	 * @param inSlot The InSlot.
	 */
	virtual void removeInSlot(const std::string &label, BasicInSlot *inSlot);

	/**
	 * Returns all InSlots that match a label.
	 * @param label The label used for filtering the InSlots.
	 * @param inSlotVector A vector that gets filled with the matching
	 * InSlots.
	 */
	virtual void getInSlots(const std::string &label, LabeledInSlotVector &inSlotVector);

	//@}

	// --------------------- External routes interface --------------------

	/// \name External routes interface
	//@{

	/**
	 * Represents an external route.
	 * @author Patrick D&auml;hne
	 */
	class ExternalRoute
	{
	public:

		/**
		 * Creates a new ExternalRoute object.
		 * @param ns The Namespace that contains this route.
		 * @param in The internal label.
		 * @param ex The external label.
		 */
		inline ExternalRoute(const Namespace &ns, const std::string &in, const std::string &ex)
		: internal(in), external(ex), namespace_(ns), variableReplacer_(ex)
		{}

		/**
		 * Returns the external label for a given internal label.
		 * @param in The internal label.
		 * @return The external label.
		 */
		std::string getExternalForInternal(const std::string &in) const;

		/** The internal label */
		std::string internal;

		/** The external label */
		std::string external;

	private:

		/** The Namespace that contains this route */
		const Namespace &namespace_;

		/** Replaces variables in the external label */
		VariableReplacer variableReplacer_;
	};

	/** Defines the vector of external routes */
	typedef std::vector<ExternalRoute*> ExternalRouteVector;

	/**
	 * Adds an external route to the %Namespace. External routes
	 * are used to export slots to the parent namespace. All
	 * slots whose label matches the internal label of the
	 * route are exported to the parent namespace under the
	 * external label of the route.
	 * @param internal The internal label.
	 * @param external The external label.
	 */
	void addExternalRoute(const std::string &internal, const std::string &external);

	/**
	 * Removes an external route from the %Namespace. All slots that
	 * are exported to the parent namespace by this route are removed
	 * from the parent namespace.
	 * @param internal The internal label.
	 * @param external The external label.
	 */
	void removeExternalRoute(const std::string &internal, const std::string &external);

	/**
	 * Returns all external routes of the %Namespace.
	 * @param externalRoutes A vector that gets filled with all external
	 * routes of the %Namespace.
	 */
	void getExternalRoutes(ExternalRouteVector &externalRoutes) const;

	/**
	 * Returns all external routes that match a given internal label.
	 * @param internal The internal label.
	 * @return A vector of ExternalRoute objects that contains all
	 * external routes that match the given internal label.
	 */
	ExternalRouteVector getExternalRoutesForInternal(const std::string &internal) const;

	/** Removes all external routes from the %Namespace */
	void clearExternalRoutes();

	//@}

	// --------------------- Route interface --------------------

	/// \name route interface
	//@{

	/**
	 * Represents a route.
	 * @author Patrick D&auml;hne
	 */
	class Route
	{
	public:

		/**
		 * Creates a new Route object.
		 * @param f The "from" label.
		 * @param t The "to" label.
		 */
		inline Route(const std::string &f, const std::string &t)
		: from(f), to(t)
		{}

		/** The "from" label */
		std::string from;

		/** The "to" label */
		std::string to;
	};

	/** Defines the vector of routes */
	typedef std::vector<Route> RouteVector;

	/**
	 * Adds a route to the %Namespace. All OutSlots in the %Namespace
	 * that match the "from" label of the route are connected to all
	 * InSlots in the %Namespace that match the "to" label and have
	 * the same data type.
	 * @param from The "from" label.
	 * @param to The "to" label.
	 */
	void addRoute(const std::string &from, const std::string &to);

	/**
	 * Removes a route from the %Namespace. All OutSlots and InSlots
	 * that are connected by this route are disconnected.
	 * @param from The "from" label.
	 * @param to The "to" label.
	 */
	void removeRoute(const std::string &from, const std::string &to);

	/**
	 * Returns all routes of the %Namespace.
	 * @param routes A vector that gets filled with all routes contained
	 * in the %Namespace.
	 */
	void getRoutes(RouteVector &routes) const;

	/**
	 * Returns all routes that match a given "from" label.
	 * @param from The "from" label.
	 * @param routes A Vector of Route objects that gets filled with all Routes that
	 * match the given "from" label.
	 */
	void getRoutesFrom(const std::string &from, RouteVector &routes);

	/**
	 * Returns all routes that match a given "to" label.
	 * @param to The "to" label.
	 * @param routes A Vector of Route objects that gets filled with all Routes that
	 * match the given "to" label.
	 */
	void getRoutesTo(const std::string &to, RouteVector &routes);

	/** Removes all routes from the %Namespace */
	void clearRoutes();

	//@}

	// --------------------- Namespace interface --------------------

	/// \name namespace interface
	//@{

	/**
	 * Adds a subnamespace to this %Namespace.
	 * @param ns The child %Namespace.
	 * @return The unique label the subnamespace actually has
	 * in this %Namespace.
	 */
	inline const std::string &addNamespace(std::auto_ptr<Namespace> ns)
	{
		std::string typeName = ns->typeName(); // Do not try to optimize this!
		return addNamespace(typeName, ns);
	}

	/**
	 * Adds a subnamespace to this %Namespace.
	 * @param ns The child %Namespace.
	 * @return The unique label the subnamespace actually has
	 * in this %Namespace.
	 */
	const std::string &addNamespace(std::auto_ptr<Node> ns);

	/**
	 * Adds a Subnamespace to this %Namespace.
	 * @param label The label of the child %Namespace.
	 * @param ns The child %Namespace.
	 * @return The unique label the subnamespace actually has
	 * in this %Namespace.
	 */
	const std::string &addNamespace(const std::string &label, std::auto_ptr<Namespace> ns);

	/**
	 * Adds a Subnamespace to this %Namespace.
	 * @param label The label of the child %Namespace.
	 * @param ns The child %Namespace.
	 * @return The unique label the subnamespace actually has
	 * in this %Namespace.
	 */
	const std::string &addNamespace(const std::string &label, std::auto_ptr<Node> ns);

	/**
	 * Removes a Subnamespace from this %Namespace.
	 * @param label The label of the child %Namespace.
	 * @return The subnamespace removed by this method.
	 * @throw InvalidNamespaceException when a namespace of
	 * this label does not exist in the %Namespace.
	 */
	std::auto_ptr<Namespace> removeNamespace(const std::string &label)
	/*throw (InvalidNamespaceException)*/;

	/**
	 * Returns a subnamespace that matches a given label.
	 * @param label The label of the subnamespace.
	 * @return The subnamespace.
	 * @throw InvalidNamespaceException when a subnamespace with
	 * the given label does not exist.
	 */
	Namespace *getNamespace(const std::string &label) const
	/*throw (InvalidNamespaceException)*/;

	/** Defines the map that holds all subnamespaces */
	typedef std::map<std::string, Namespace*> NamespaceMap;

	/**
	 * Returns all subnamespaces contained in this %Namespace.
	 * @return The map of subnamespaces.
	 */
	inline NamespaceMap &getNamespaces()
	{ return namespaces_; }

	/**
	 * Returns all subnamespaces contained in this %Namespace.
	 * @return The map of subnamespaces.
	 */
	inline const NamespaceMap &getNamespaces() const
	{ return namespaces_; }

	/** Removes all subnamespaces from this %Namespace */
	void clearNamespaces();

	//@}

protected:

	/** Up-Locks the Namespace object. */
	Medusa::Mutex *upMutex_;

	/** Down-Locks the Namespace object. */
	Medusa::Mutex *downMutex_;

	/** The parent Namespace of the Namespace */
	Namespace *parentNamespace_;

	/** The "enabled" flag - true when the Namespace is enabled. */
	bool enabled_;

	/** The label of the Namespace */
	std::string label_;

	/**
	 * Gets called when the Namespace is enabled.
	 * Override this method when you need to do your
	 * own initializations when the Namespace is enabled.
	 * The default implementation does nothing.
	 */
	virtual void initialize();

	/**
	 * Gets called when the Namespace is disabled.
	 * Override this method when you need to do your
	 * own deinitializations when the Namespace is disabled.
	 * The default implementation does nothing.
	 */
	virtual void shutdown();

	// --------------------- Outslot members --------------------

	/** The Vector that contains all OutSlots of the %Namespace */
	LabeledOutSlotVector outSlots_;

	/**
	 * Tries to find an OutSlot in the vector of OutSlots contained
	 * in this %Namespace.
	 * @param label The label of the OutSlot in this %Namespace.
	 * @param outSlot The OutSlot.
	 * @return An iterator pointing to the position of the OutSlot
	 * in the vector of OutSlots.
	 */
	LabeledOutSlotVector::iterator findOutSlot(const std::string &label, BasicOutSlot *outSlot);

	/**
	 * Returns all OutSlots that have routes to a given
	 * InSlot label.
	 * @param label The InSlot label.
	 * @param outSlotVector The vector that gets filled with the OutSlots
	 * that have routes to the given InSlot label.
	 */
	void getRoutedOutSlots(const std::string &label, LabeledOutSlotVector &outSlotVector);

	/**
	 * Returns all OutSlots that match a given external label.
	 * @param label The external label.
	 * @param outSlotVector A Vector of LabeledOutSlot objects that
	 * contains the OutSlots that match the given external label.
	 */
	void getExportedOutSlots(const std::string &label, LabeledOutSlotVector &outSlotVector);

	// --------------------- Inslot members --------------------

	/** The Vector that contains all InSlots of the %Namespace */
	LabeledInSlotVector inSlots_;

	/**
	 * Tries to find an InSlot in the vector of InSlots contained
	 * in this %Namespace.
	 * @param label The label of the InSlot in this %Namespace.
	 * @param inSlot The InSlot.
	 * @return An iterator pointing to the position of the InSlot
	 * in the vector of InSlots.
	 */
	LabeledInSlotVector::iterator findInSlot(const std::string &label, BasicInSlot *inSlot);

	/**
	 * Returns all InSlots that have routes to a given
	 * OutSlot label.
	 * @param label The OutSlot label.
	 * @param inSlotVector The vector that gets filled with the InSlots
	 * that have routes to the given OutSlot label.
	 */
	void getRoutedInSlots(const std::string &label, LabeledInSlotVector &inSlotVector);

	/**
	 * Returns all InSlots that match a given external label.
	 * @param label The external label.
	 * @param inSlotVector A Vector of LabeledInSlot objects that
	 * contains the InSlots that match the given external label.
	 */
	void getExportedInSlots(const std::string &label, LabeledInSlotVector &inSlotVector);

	// --------------------- External route members --------------------

	/** Contains all external routes of the Namespace */
	ExternalRouteVector externalRoutes_;

	/**
	 * Tries to find an external route in the vector of external routes contained
	 * in this %Namespace.
	 * @param internal The internal label of the route.
	 * @param external The external label of the route.
	 * @return An iterator pointing to the position of the external route
	 * in the vector of external routes.
	 */
	ExternalRouteVector::iterator findExternalRoute(const std::string &internal, const std::string &external);

	/**
	 * Removes an external route from the %Namespace. All slots that
	 * are exported to the parent namespace by this route are removed
	 * from the parent namespace.
	 * @param it Iterator pointing to the route in the vector of
	 * external routes.
	 */
	void removeExternalRoute(const ExternalRouteVector::const_iterator &it);

	// --------------------- Route members --------------------

	/** Contains all routes of the %Namespace */
	RouteVector routes_;

	/**
	 * Tries to find a route in the vector of routes contained
	 * in this %Namespace.
	 * @param from The "from" label of the route.
	 * @param to The "to" label of the route.
	 * @return An iterator pointing to the position of the route
	 * in the vector of routes.
	 */
	RouteVector::iterator findRoute(const std::string &from, const std::string &to);

	/**
	 * Removes a route from the %Namespace. All OutSlots and InSlots
	 * that are connected by this route are disconnected.
	 * @param it Iterator pointing to the route in the route vector.
	 */
	void removeRoute(const RouteVector::const_iterator &it);

	// --------------------- Namespace members --------------------

	/** Contains all child %Namespaces of the %Namespace */
	NamespaceMap namespaces_;

	/**
	 * Creates a unique name for a child %Namespace.
	 * @param label The label of the child %Namespace.
	 * @return The unique name for the child %Namespace.
	 */
	std::string createUniqueLabel(const std::string &label) const;

private:

	/** Copy constructor (not implemented!) */
	Namespace(const Namespace &);

	/** Copy operator (not implemented!) */
	const Namespace &operator=(const Namespace &);

	/** Flags that is set when the %Namespace contains unsaved changes */
	bool dirty_;

	/** The base URL of this %Namespace */
	Hermes::URL *url_;

	/** Defines the map of fields */
	typedef std::map<std::string, std::pair<std::string, std::string> > FieldMap;

	/** The map of field values */
	FieldMap fields_;

	/** Defines the set of listeners */
	typedef std::set<Listener*> ListenerSet;

	/** The listeners that listen on this %Namespace */
	ListenerSet listeners_;
};


} // namespace InstantIO


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif // __INSTANTIO_NAMESPACE_H
