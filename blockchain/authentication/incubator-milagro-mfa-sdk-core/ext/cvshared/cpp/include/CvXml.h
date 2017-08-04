/*
 * File:   CvXml.h
 * Author: mony
 *
 * Created on September 27, 2011, 5:35 PM
 */
////////////////////////////////////////////////////////////////////
///	\file XmlParser.h
/// @brief	This is a wrapper for the RapidXML library.
///	It is provided for convenience purposes and also can serve in the future
///	as an abstraction layer for a XML library.
////////////////////////////////////////////////////////////////////

#ifndef CVXML_H
#define	CVXML_H

#include "xml/rapidxml.hpp"
#include "xml/rapidxml_print.hpp"

typedef rapidxml::xml_document<>	CvXmlDoc;
typedef rapidxml::xml_node<>		CvXmlNode;
typedef rapidxml::xml_attribute<>	CvXmlAttr;

typedef rapidxml::parse_error		CvXmlParseException;

#endif	/* CVXML_H */

