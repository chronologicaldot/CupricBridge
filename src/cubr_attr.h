// Partly copyright 2018 Nicolaus Anderson
// Most of this was copied from IAttributes.h of Irrlicht with the exception of:
// - The removal of zeros and comments
// - The change of the wrapper class
// - The namespace wrapping

#ifndef _CUBR_ATTRIBUTE_SOURCE_H_
#define _CUBR_ATTRIBUTE_SOURCE_H_

#include <IAttributes.h> // from Irrlicht
#include <Strings.h> // from Copper
#include <Copper.h>
#include "cubr_defs.h"

namespace cubr {

using namespace irr;

//! Helper functions
//! Second parameter is the default value if the object is not numeric
bool getBoolValue( Cu::Object*, bool );
u32 getU32Value( Cu::Object*, u32 );
s32 getS32Value( Cu::Object*, s32 );
f32 getF32Value( Cu::Object*, f32 );

//! Attributes Interface
/*
	In order to comminicate with Irrlicht GUI elements and scene nodes fast and effectively,
	we use the irr::IAttributes interface.
	This implementation accepts a variable and uses it as the source of data. The data is extracted from the
	variable during one of the "get" calls to the interface.
*/
class AttributeSource : public irr::io::IAttributes, public Cu::AppendObjectInterface {

	typedef  util::List<util::String>  slist_t;

	video_driver_t*  videoDriver; // For loading textures
	Cu::FunctionObject&  infoSource;
	slist_t  infoNamesList;

public:

	AttributeSource( video_driver_t*, Cu::FunctionObject& );

	Cu::FunctionObject* getMemberByName(const c8*) const;
	Cu::FunctionObject* getSubMemberByName(Cu::FunctionObject&, const c8*) const;
	Cu::Object*  getMemberFunctionResult(const c8*) const;
	Cu::Object*  getSubMemberFunctionResult(Cu::FunctionObject&, const c8*) const;

	u32  getAttributeAsUInt(Cu::FunctionObject& source, const c8* attributeName, u32 defaultNotFound=0) const;
	s32  getAttributeAsInt(Cu::FunctionObject& source, const c8* attributeName, s32 defaultNotFound=0) const;
	f32  getAttributeAsFloat(Cu::FunctionObject& source, const c8* attributeName, f32 defaultNotFound=0.0) const;
	core::vector2df  getAttributeAsVector2d(Cu::FunctionObject& source, const c8* attributeName, core::vector2df defaultNotFound=core::vector2df(0)) const;
	core::vector3df  getAttributeAsVector3d(Cu::FunctionObject& source, const c8* attributeName, core::vector3df defaultNotFound=core::vector3df(0)) const;
	void  setSubMemberFunctionResult(Cu::FunctionObject&, const c8*, u32);
	void  setSubMemberFunctionResult(Cu::FunctionObject&, const c8*, s32);
	void  setSubMemberFunctionResult(Cu::FunctionObject&, const c8*, f32);
	void  setSubMemberFunctionResult(Cu::FunctionObject&, const c8*, core::vector2df);
	void  setSubMemberFunctionResult(Cu::FunctionObject&, const c8*, core::vector3df);

	// ***** From Copper *****

	// AppendObjectInterface
	virtual void append(Cu::Object*);


	// ***** From IAttributes *****

	//! Returns amount of attributes in this collection of attributes.
	virtual u32 getAttributeCount() const;

	//! Returns attribute name by index.
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual const c8* getAttributeName(s32 index) const;

	//! Returns the type of an attribute
	//! \param attributeName: Name for the attribute
	virtual io::E_ATTRIBUTE_TYPE getAttributeType(const c8* attributeName) const;

	//! Returns attribute type by index.
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual io::E_ATTRIBUTE_TYPE getAttributeType(s32 index) const;

	//! Returns the type string of the attribute
	//! \param attributeName: String for the attribute type
	//! \param defaultNotFound Value returned when attributeName was not found
	virtual const wchar_t* getAttributeTypeString(const c8* attributeName, const wchar_t* defaultNotFound = L"unknown") const;

	//! Returns the type string of the attribute by index.
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	//! \param defaultNotFound Value returned for an invalid index
	virtual const wchar_t* getAttributeTypeString(s32 index, const wchar_t* defaultNotFound = L"unknown") const;

	//! Returns if an attribute with a name exists
	virtual bool existsAttribute(const c8* attributeName) const;

	//! Returns attribute index from name, -1 if not found
	virtual s32 findAttribute(const c8* attributeName) const;

	//! Removes all attributes
	virtual void clear();

	//! Reads attributes from a xml file.
	//! This function is never used for scripts interfacing with GUI or scene elements.
	virtual bool read(io::IXMLReader* reader, bool readCurrentElementOnly=false, const wchar_t* elementName=0) {
		return false;
	}

	//! Write these attributes into a xml file
	//! This function is never used for scripts interfacing with GUI or scene elements.
	virtual bool write(io::IXMLWriter* writer, bool writeXMLHeader=false, const wchar_t* elementName=0) {
		return false;
	}


	/*

		Integer Attribute

	*/

	//! Adds an attribute as integer
	virtual void addInt(const c8* attributeName, s32 value);

	//! Sets an attribute as integer value
	virtual void setAttribute(const c8* attributeName, s32 value);

	//! Gets an attribute as integer value
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual s32 getAttributeAsInt(const c8* attributeName, irr::s32 defaultNotFound=0) const;

	//! Gets an attribute as integer value
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual s32 getAttributeAsInt(s32 index) const;

	//! Sets an attribute as integer value
	virtual void setAttribute(s32 index, s32 value);

	/*

		Float Attribute

	*/

	//! Adds an attribute as float
	virtual void addFloat(const c8* attributeName, f32 value);

	//! Sets a attribute as float value
	virtual void setAttribute(const c8* attributeName, f32 value);

	//! Gets an attribute as float value
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual f32 getAttributeAsFloat(const c8* attributeName, irr::f32 defaultNotFound=0.f) const;

	//! Gets an attribute as float value
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual f32 getAttributeAsFloat(s32 index) const;

	//! Sets an attribute as float value
	virtual void setAttribute(s32 index, f32 value);

	/*

		String Attribute

	*/

	//! Adds an attribute as string
	virtual void addString(const c8* attributeName, const c8* value);

	//! Sets an attribute value as string.
	//! \param attributeName: Name for the attribute
	//! \param value: Value for the attribute. Set this to 0 to delete the attribute
	virtual void setAttribute(const c8* attributeName, const c8* value);

	//! Gets an attribute as string.
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	//! or defaultNotFound if attribute is not set.
	virtual core::stringc getAttributeAsString(const c8* attributeName, const core::stringc& defaultNotFound=core::stringc()) const;

	//! Gets an attribute as string.
	//! \param attributeName Name of the attribute to get.
	//! \param target Buffer where the string is copied to.
	virtual void getAttributeAsString(const c8* attributeName, c8* target) const;

	//! Returns attribute value as string by index.
	//! \param index Index value, must be between 0 and getAttributeCount()-1.
	virtual core::stringc getAttributeAsString(s32 index) const;

	//! Sets an attribute value as string.
	//! \param index Index value, must be between 0 and getAttributeCount()-1.
	//! \param value String to which the attribute is set.
	virtual void setAttribute(s32 index, const c8* value);

	// wide strings

	//! Adds an attribute as string
	virtual void addString(const c8* attributeName, const wchar_t* value);

	//! Sets an attribute value as string.
	//! \param attributeName: Name for the attribute
	//! \param value: Value for the attribute. Set this to 0 to delete the attribute
	virtual void setAttribute(const c8* attributeName, const wchar_t* value);

	//! Gets an attribute as string.
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	//! or defaultNotFound if attribute is not set.
	virtual core::stringw getAttributeAsStringW(const c8* attributeName, const core::stringw& defaultNotFound = core::stringw()) const;

	//! Gets an attribute as string.
	//! \param attributeName: Name of the attribute to get.
	//! \param target: Buffer where the string is copied to.
	virtual void getAttributeAsStringW(const c8* attributeName, wchar_t* target) const;

	//! Returns attribute value as string by index.
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::stringw getAttributeAsStringW(s32 index) const;

	//! Sets an attribute value as string.
	//! \param index Index value, must be between 0 and getAttributeCount()-1.
	//! \param value String to which the attribute is set.
	virtual void setAttribute(s32 index, const wchar_t* value);

	/*

		Binary Data Attribute

	*/

	//! Adds an attribute as binary data
	virtual void addBinary(const c8* attributeName, void* data, s32 dataSizeInBytes);

	//! Sets an attribute as binary data
	virtual void setAttribute(const c8* attributeName, void* data, s32 dataSizeInBytes );

	//! Gets an attribute as binary data
	/** \param attributeName: Name of the attribute to get.
	\param outData Pointer to buffer where data shall be stored.
	\param maxSizeInBytes Maximum number of bytes to write into outData.
	*/
	virtual void getAttributeAsBinaryData(const c8* attributeName, void* outData, s32 maxSizeInBytes) const;

	//! Gets an attribute as binary data
	/** \param index: Index value, must be between 0 and getAttributeCount()-1.
	\param outData Pointer to buffer where data shall be stored.
	\param maxSizeInBytes Maximum number of bytes to write into outData.
	*/
	virtual void getAttributeAsBinaryData(s32 index, void* outData, s32 maxSizeInBytes) const;

	//! Sets an attribute as binary data
	virtual void setAttribute(s32 index, void* data, s32 dataSizeInBytes );


	/*
		Array Attribute
	*/

	//! Adds an attribute as wide string array
	virtual void addArray(const c8* attributeName, const core::array<core::stringw>& value);

	//! Sets an attribute value as a wide string array.
	//! \param attributeName: Name for the attribute
	//! \param value: Value for the attribute. Set this to 0 to delete the attribute
	virtual void setAttribute(const c8* attributeName, const core::array<core::stringw>& value);

	//! Gets an attribute as an array of wide strings.
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	//! or defaultNotFound if attribute is not set.
	virtual core::array<core::stringw> getAttributeAsArray(const c8* attributeName, const core::array<core::stringw>& defaultNotFound = core::array<core::stringw>()) const;

	//! Returns attribute value as an array of wide strings by index.
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::array<core::stringw> getAttributeAsArray(s32 index) const;

	//! Sets an attribute as an array of wide strings
	virtual void setAttribute(s32 index, const core::array<core::stringw>& value);


	/*

		Bool Attribute

	*/

	//! Adds an attribute as bool
	virtual void addBool(const c8* attributeName, bool value);

	//! Sets an attribute as boolean value
	virtual void setAttribute(const c8* attributeName, bool value);

	//! Gets an attribute as boolean value
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual bool getAttributeAsBool(const c8* attributeName, bool defaultNotFound=false) const;

	//! Gets an attribute as boolean value
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual bool getAttributeAsBool(s32 index) const;

	//! Sets an attribute as boolean value
	virtual void setAttribute(s32 index, bool value);

	/*

		Enumeration Attribute

	*/

	//! Adds an attribute as enum
	virtual void addEnum(const c8* attributeName, const c8* enumValue, const c8* const* enumerationLiterals);

	//! Adds an attribute as enum
	virtual void addEnum(const c8* attributeName, s32 enumValue, const c8* const* enumerationLiterals);

	//! Sets an attribute as enumeration
	virtual void setAttribute(const c8* attributeName, const c8* enumValue, const c8* const* enumerationLiterals);

	//! Gets an attribute as enumeration
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual const c8* getAttributeAsEnumeration(const c8* attributeName, const c8* defaultNotFound = 0) const;

	//! Gets an attribute as enumeration
	/** \param attributeName: Name of the attribute to get.
	\param enumerationLiteralsToUse: Use these enumeration literals to get
	the index value instead of the set ones. This is useful when the
	attribute list maybe was read from an xml file, and only contains the
	enumeration string, but no information about its index.
	\return Returns value of the attribute previously set by setAttribute()
	*/
	virtual s32 getAttributeAsEnumeration(const c8* attributeName, const c8* const* enumerationLiteralsToUse, s32 defaultNotFound) const;

	//! Gets an attribute as enumeration
	/** \param index: Index value, must be between 0 and getAttributeCount()-1.
	\param enumerationLiteralsToUse: Use these enumeration literals to get
	the index value instead of the set ones. This is useful when the
	attribute list maybe was read from an xml file, and only contains the
	enumeration string, but no information about its index.
	\return Returns value of the attribute previously set by setAttribute()
	*/
	virtual s32 getAttributeAsEnumeration(s32 index, const c8* const* enumerationLiteralsToUse, s32 defaultNotFound) const;

	//! Gets an attribute as enumeration
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual const c8* getAttributeAsEnumeration(s32 index) const;

	//! Gets the list of enumeration literals of an enumeration attribute
	//! \param attributeName Name of the attribute to get.
	//! \param outLiterals Set of strings to choose the enum name from.
	virtual void getAttributeEnumerationLiteralsOfEnumeration(const c8* attributeName, core::array<core::stringc>& outLiterals) const;

	//! Gets the list of enumeration literals of an enumeration attribute
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	//! \param outLiterals Set of strings to choose the enum name from.
	virtual void getAttributeEnumerationLiteralsOfEnumeration(s32 index, core::array<core::stringc>& outLiterals) const;

	//! Sets an attribute as enumeration
	virtual void setAttribute(s32 index, const c8* enumValue, const c8* const* enumerationLiterals);


	/*

		SColor Attribute

	*/

	//! Adds an attribute as color
	virtual void addColor(const c8* attributeName, video::SColor value);


	//! Sets a attribute as color
	virtual void setAttribute(const c8* attributeName, video::SColor color);

	//! Gets an attribute as color
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual video::SColor getAttributeAsColor(const c8* attributeName, const video::SColor& defaultNotFound = video::SColor(0)) const;

	//! Gets an attribute as color
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual video::SColor getAttributeAsColor(s32 index) const;

	//! Sets an attribute as color
	virtual void setAttribute(s32 index, video::SColor color);

	/*

		SColorf Attribute

	*/

	//! Adds an attribute as floating point color
	virtual void addColorf(const c8*, video::SColorf);

	//! Sets a attribute as floating point color
	virtual void setAttribute(const c8*, video::SColorf);

	//! Gets an attribute as floating point color
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual video::SColorf getAttributeAsColorf(const c8*, const video::SColorf&) const;

	//! Gets an attribute as floating point color
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual video::SColorf getAttributeAsColorf(s32) const;

	//! Sets an attribute as floating point color
	virtual void setAttribute(s32 index, video::SColorf);


	/*

		Vector3d Attribute

	*/

	//! Adds an attribute as 3d vector
	virtual void addVector3d(const c8*, const core::vector3df&);

	//! Sets a attribute as 3d vector
	virtual void setAttribute(const c8*, const core::vector3df&);

	//! Gets an attribute as 3d vector
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::vector3df getAttributeAsVector3d(const c8*, const core::vector3df&) const;

	//! Gets an attribute as 3d vector
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::vector3df getAttributeAsVector3d(s32) const;

	//! Sets an attribute as vector
	virtual void setAttribute(s32, const core::vector3df&);

	/*

		Vector2d Attribute

	*/

	//! Adds an attribute as 2d vector
	virtual void addVector2d(const c8*, const core::vector2df&);

	//! Sets a attribute as 2d vector
	virtual void setAttribute(const c8*, const core::vector2df&);

	//! Gets an attribute as vector
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::vector2df getAttributeAsVector2d(const c8*, const core::vector2df&) const;

	//! Gets an attribute as position
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::vector2df getAttributeAsVector2d(s32) const;

	//! Sets an attribute as 2d vector
	virtual void setAttribute(s32, const core::vector2df&);

	/*

		Position2d Attribute

	*/

	//! Adds an attribute as 2d position
	virtual void addPosition2d(const c8*, const core::position2di&);

	//! Sets a attribute as 2d position
	virtual void setAttribute(const c8*, const core::position2di&);

	//! Gets an attribute as position
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::position2di getAttributeAsPosition2d(const c8*, const core::position2di&) const;

	//! Gets an attribute as position
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::position2di getAttributeAsPosition2d(s32) const;

	//! Sets an attribute as 2d position
	virtual void setAttribute(s32, const core::position2di&);

	/*

		Rectangle Attribute

	*/

	//! Adds an attribute as rectangle
	virtual void addRect(const c8*, const core::rect<s32>&);

	//! Sets an attribute as rectangle
	virtual void setAttribute(const c8*, const core::rect<s32>&);

	//! Gets an attribute as rectangle
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::rect<s32> getAttributeAsRect(const c8*, const core::rect<s32>&) const;

	//! Gets an attribute as rectangle
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::rect<s32> getAttributeAsRect(s32) const;

	//! Sets an attribute as rectangle
	virtual void setAttribute(s32, const core::rect<s32>&);


	/*

		Dimension2d Attribute

	*/

	//! Adds an attribute as dimension2d
	virtual void addDimension2d(const c8*, const core::dimension2d<u32>&);

	//! Sets an attribute as dimension2d
	virtual void setAttribute(const c8*, const core::dimension2d<u32>&);

	//! Gets an attribute as dimension2d
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::dimension2d<u32> getAttributeAsDimension2d(const c8*, const core::dimension2d<u32>&) const;

	//! Gets an attribute as dimension2d
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::dimension2d<u32> getAttributeAsDimension2d(s32) const;

	//! Sets an attribute as dimension2d
	virtual void setAttribute(s32, const core::dimension2d<u32>&);


	/*
		matrix attribute
	*/

	//! Adds an attribute as matrix
	virtual void addMatrix(const c8*, const core::matrix4&);

	//! Sets an attribute as matrix
	virtual void setAttribute(const c8*, const core::matrix4&);

	//! Gets an attribute as a matrix4
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::matrix4 getAttributeAsMatrix(const c8*, const core::matrix4&) const;

	//! Gets an attribute as matrix
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::matrix4 getAttributeAsMatrix(s32) const;

	//! Sets an attribute as matrix
	virtual void setAttribute(s32, const core::matrix4&);

	/*
		quaternion attribute

	*/

	//! Adds an attribute as quaternion
	virtual void addQuaternion(const c8*, const core::quaternion&);

	//! Sets an attribute as quaternion
	virtual void setAttribute(const c8*, const core::quaternion&);

	//! Gets an attribute as a quaternion
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::quaternion getAttributeAsQuaternion(const c8*, const core::quaternion&) const;

	//! Gets an attribute as quaternion
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::quaternion getAttributeAsQuaternion(s32) const;

	//! Sets an attribute as quaternion
	virtual void setAttribute(s32, const core::quaternion&);

	/*

		3d bounding box

	*/

	//! Adds an attribute as axis aligned bounding box
	virtual void addBox3d(const c8*, const core::aabbox3df&);

	//! Sets an attribute as axis aligned bounding box
	virtual void setAttribute(const c8*, const core::aabbox3df&);

	//! Gets an attribute as a axis aligned bounding box
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::aabbox3df getAttributeAsBox3d(const c8*, const core::aabbox3df&) const;

	//! Gets an attribute as axis aligned bounding box
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::aabbox3df getAttributeAsBox3d(s32) const;

	//! Sets an attribute as axis aligned bounding box
	virtual void setAttribute(s32, const core::aabbox3df&);

	/*

		plane

	*/

	//! Adds an attribute as 3d plane
	virtual void addPlane3d(const c8*, const core::plane3df&);

	//! Sets an attribute as 3d plane
	virtual void setAttribute(const c8*, const core::plane3df&);

	//! Gets an attribute as a 3d plane
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::plane3df getAttributeAsPlane3d(const c8*, const core::plane3df&) const;

	//! Gets an attribute as 3d plane
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::plane3df getAttributeAsPlane3d(s32) const;

	//! Sets an attribute as 3d plane
	virtual void setAttribute(s32, const core::plane3df&);


	/*

		3d triangle

	*/

	//! Adds an attribute as 3d triangle
	virtual void addTriangle3d(const c8*, const core::triangle3df&);

	//! Sets an attribute as 3d trianle
	virtual void setAttribute(const c8*, const core::triangle3df&);

	//! Gets an attribute as a 3d triangle
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::triangle3df getAttributeAsTriangle3d(const c8*, const core::triangle3df&) const;

	//! Gets an attribute as 3d triangle
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::triangle3df getAttributeAsTriangle3d(s32) const;

	//! Sets an attribute as 3d triangle
	virtual void setAttribute(s32, const core::triangle3df&);


	/*

		line 2d

	*/

	//! Adds an attribute as a 2d line
	virtual void addLine2d(const c8*, const core::line2df&);

	//! Sets an attribute as a 2d line
	virtual void setAttribute(const c8*, const core::line2df&);

	//! Gets an attribute as a 2d line
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::line2df getAttributeAsLine2d(const c8*, const core::line2df&) const;

	//! Gets an attribute as a 2d line
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::line2df getAttributeAsLine2d(s32) const;

	//! Sets an attribute as a 2d line
	virtual void setAttribute(s32, const core::line2df&);


	/*

		line 3d

	*/

	//! Adds an attribute as a 3d line
	virtual void addLine3d(const c8*, const core::line3df&);

	//! Sets an attribute as a 3d line
	virtual void setAttribute(const c8*, const core::line3df&);

	//! Gets an attribute as a 3d line
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	//! \return Returns value of the attribute previously set by setAttribute()
	virtual core::line3df getAttributeAsLine3d(const c8*, const core::line3df&) const;

	//! Gets an attribute as a 3d line
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual core::line3df getAttributeAsLine3d(s32) const;

	//! Sets an attribute as a 3d line
	virtual void setAttribute(s32, const core::line3df&);


	/*

		Texture Attribute

	*/

	//! Adds an attribute as texture reference
	virtual void addTexture(const c8*, video::ITexture*, const io::path&);

	//! Sets an attribute as texture reference
	virtual void setAttribute(const c8*, video::ITexture*, const io::path&);

	//! Gets an attribute as texture reference
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	virtual video::ITexture* getAttributeAsTexture(const c8*, video::ITexture*) const;

	//! Gets an attribute as texture reference
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual video::ITexture* getAttributeAsTexture(s32) const;

	//! Sets an attribute as texture reference
	virtual void setAttribute(s32 index, video::ITexture*, const io::path&);


	/*

		User Pointer Attribute

	*/

	//! Adds an attribute as user pointer
	virtual void addUserPointer(const c8*, void*);

	//! Sets an attribute as user pointer
	virtual void setAttribute(const c8*, void*);

	//! Gets an attribute as user pointer
	//! \param attributeName: Name of the attribute to get.
	//! \param defaultNotFound Value returned when attributeName was not found
	virtual void* getAttributeAsUserPointer(const c8*, void*) const;

	//! Gets an attribute as user pointer
	//! \param index: Index value, must be between 0 and getAttributeCount()-1.
	virtual void* getAttributeAsUserPointer(s32) const;

	//! Sets an attribute as user pointer
	virtual void setAttribute(s32, void*);


};

}

#endif
