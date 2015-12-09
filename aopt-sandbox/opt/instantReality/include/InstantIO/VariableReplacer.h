#ifndef __INSTANTIO_VARIABLEREPLACER_H
#define __INSTANTIO_VARIABLEREPLACER_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: VariableReplacer.h                                              //
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
#include <vector>
#include <string>
namespace Config
{
	class ParamBag;
}
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4251)
#endif


namespace InstantIO
{


/**
 * @class VariableReplacer VariableReplacer.h InstantIO/VariableReplacer.h
 *
 * Internal helper class that replaces variables in a string
 * with their respective values.
 * <p>
 * When you create a %VariableReplacer object, you have to
 * specify a format string. This format string consists of
 * text and variables. Variables are text strings, surrounded
 * by curly braces.
 * <p>
 * Then, you can call the toString method of the %VariableReplacer
 * object with a set of name/value-pairs. This method returns the
 * format string where all variables are replaced by their respective
 * values, i.e. the method looks up the set for the name of the
 * variable and replaces the variable with its value.
 * <p>
 * The following piece of code demonstrates how to use the
 * %VariableReplacer class:
 * <p>
 * <code><pre>
 * VariableReplacer vr("Connecting to host {host}, port {port}");
 * ParamBag params;
 * params.setParam("host", "my_computer");
 * params.setParam("port", 80);
 * string result = vr.toString();
 * </pre></code>
 * <p>
 * The variable "result" contains the following string after
 * executing the code above: "Connecting to host my_computer,
 * port 80".
 *
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING VariableReplacer
{
public:

	/**
	 * Creates a new %VariableReplacer object with an empty
	 * format string.
	 */
	inline VariableReplacer(): format_() {}

	/**
	 * Initializes the Variable replacer object with a format string.
	 * The format string is described in the class documentation.
	 * @param format The format string.
	 */
	inline VariableReplacer(const std::string &format): format_()
	{ setFormat(format); }

	/** Destroys the %VariableReplacer object */
	~VariableReplacer();

	/**
	 * Sets a new format string.
	 * @param format The format string.
	 */
	void setFormat(const std::string &format);

	/** Clears the format string */
	void clear();

	/**
	 * Converts the %VariableReplacer object to a string.
	 * This method returns the format string specified in the
	 * constructor. It looks up the names of all variables
	 * contained in the format string in a set of name/value-pairs.
	 * When it finds a variable name in the set, it replaces it
	 * by the respective value, otherwise it simply removes the
	 * variable from the format string.
	 * @param params The set of name/value-pairs.
	 * @return The format string where all variables are replaced
	 * by their respective values.
	 */
	std::string toString(const Config::ParamBag &params) const;

private:
	
	/**
	 * Abstract superclass of all elements of a format string.
	 * @author Patrick D&auml;hne
	 */
	class Element
	{
	public:

		/** Destroys the %Element object */
		virtual ~Element();

		/**
		 * Converts the %Element object to a string.
		 * @param params The set of name/value-pairs.
		 * @return The string representation of the %Element object.
		 */
		virtual std::string toString(const Config::ParamBag &params) const = 0;
	};

	/**
	 * A variable element of a format string.
	 * @author Patrick D&auml;hne
	 */
	class Variable: public Element
	{
	public:

		/**
		 * Creates a new %Variable object.
		 * @param name The name of the variable.
		 */
		inline Variable(const std::string &name)
		: Element(), name_(name)
		{}

		/** Destroys the %Variable object */
		virtual ~Variable();

		/**
		 * Converts the variable element to a string.
		 * When the name of the %Variable object is contained
		 * in a set of name/value-pairs, this method returns
		 * the respective value. Otherwise, it returns an empty
		 * string.
		 * @param params The set of name/value-pairs.
		 * @return The string representation of the %Variable object.
		 */
		virtual std::string toString(const Config::ParamBag &params) const;

	private:

		/** The name of the variable */
		std::string name_;
	};

	/**
	 * A text element of a format string.
	 * @author Patrick D&auml;hne
	 */
	class Text: public Element
	{
	public:

		/**
		 * Creates a new %Text object.
		 * @param text The text of the text element.
		 */
		inline Text(const std::string &text)
		: Element(), text_(text)
		{}

		/** Destroys the %Text object */
		virtual ~Text();

		/**
		 * Converts the text element to a string.
		 * @param params A set of name/value-pairs. Not
		 * used by this class.
		 * @return The string representation of the %Text object.
		 */
		virtual std::string toString(const Config::ParamBag &params) const;

	private:

		/** The text of the text element */
		std::string text_;
	};

	/** Vector that contains the elements of the format string */
	std::vector<Element*> format_;

	/**
	 * Removes escape sequences from a string.
	 * That is, it replaces all occurences of "\{",
	 * "\}" and "\\" by "{", "}" and "\", respectively.
	 * @param s The string with escape sequences.
	 * @return The string without escape sequences.
	 */
	std::string stripEscapes(const std::string &s) const;
};


} // namespace InstantIO


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif // __INSTANTIO_VARIABLEREPLACER_H
