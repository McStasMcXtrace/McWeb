#ifndef __INSTANTIO_STATEKEEPERXML_H
#define __INSTANTIO_STATEKEEPERXML_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: StateKeeperXML.h                                                //
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
#include <iosfwd>
namespace Hermes
{
	class URL;
}
namespace XMLWrap
{
	class XMLWriter;
}


namespace InstantIO
{


class Namespace;
class StateKeeperXMLHandler;


/**
 * @class StateKeeperXML StateKeeperXML.h InstantIO/StateKeeperXML.h
 *
 * Writes the current state of namespaces as XML to an output stream
 * or reads the state of namespaces as XML from an input stream.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING StateKeeperXML
{
public:

	/**
	 * Saves the state as XML.
	 * Writes the current state of a namespace (i.e. all subnamespaces,
	 * nodes and routes) as XML to an output stream.
	 * @param ns The namespace whose current state
	 * we want to write to the output stream.
	 * @param os The output stream we want to write the
	 * current state of the namespace to.
	 */
	static void save(Namespace &ns, std::ostream &os);

	/**
	 * Loads a XML-encoded state.
	 * Fills a namespace with a state (i.e. subnamespaces,
	 * nodes and routes) read as XML from an input stream.
	 * @param ns The namespace we fill with the stored state
	 * @param is The input stream we want to read the namespace
	 * from.
	 */
	static void load(Namespace &ns, std::istream &is)
	/*throw (ParseException)*/;

	/**
	 * Loads the state.
	 * Fills a namespace with a state (i.e. subnamespaces,
	 * nodes and routes) read from an URL.
	 * @param ns The namespace we fill with the stored state
	 * @param url The URL we want to read the namespace from.
	 */
	static void load(Namespace &ns, const Hermes::URL &url)
	/*throw (ParseException, Hermes::Exception)*/;

private:

	/** Prevents instances of the %StateKeeperXML class (not implemented!) */
	StateKeeperXML();

	/** Copy constructor (not implemented!) */
	StateKeeperXML(const StateKeeperXML &);

	/** Copy operator (not implemented!) */
	const StateKeeperXML &operator=(const StateKeeperXML &);

	/**
	 * Helper function used to save the state of namespaces into
	 * a XML file.
	 * @param ns The root of the namespace subtree we want to
	 * write into the XML file.
	 * @param writer The XML write object used to write the XML file.
	 */
	static void save(Namespace &ns, XMLWrap::XMLWriter &writer);

	/**
	 * Helper function used to load the state of namespaces from
	 * a XML file.
	 * @param handler The handler that handles events from the SAX parser
	 * @param is The input stream we want to read the namespace
	 * from.
	 */
	static void load(StateKeeperXMLHandler *handler, std::istream &is)
	/*throw (ParseException)*/;
};


} // namespace InstantIO


#endif // __INSTANTIO_STATEKEEPERXML_H
