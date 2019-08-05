// Copyright 2018 Nicolaus Anderson

#ifndef _CUBR_BASE_H_
#define _CUBR_BASE_H_

#include <irrptr.h>
#include <IGUIElement.h>
#include <Copper.h>
#include "cubr_defs.h"

namespace cubr {

using util::String;
using irr::tools::irrptr;

// Set however you wish.
// It will be incremented by ObjectType::UnknownData + 1 in the object type return.
#define CUBR_TYPE_START_INDEX 0

struct CubrObjectType {
enum Value {
	GUIElement = CUBR_TYPE_START_INDEX,
	GUIWatcher,
	Image,
	Texture,

	LAST_INDEX, // Total number of types + starting index
	FORCE_32BIT = 0x7fffffff, // NOT A TYPE. Forces enumeration to compile to 32 bits
};};

Cu::ObjectType::Value
getCubrTypeAsCuType( CubrObjectType::Value );

bool
isGUIElementObject( Cu::Object& );

bool
isImageObject( Cu::Object& );

bool
isTextureObject( Cu::Object& );

//! Wrapper for a GUI element
class GUIElement : public Cu::Object {

	irrptr<gui_element_t>  data;
	gui_environment_t*  environment;

public:
	GUIElement( gui_element_t*, gui_environment_t* );

	gui_element_t*
	getElement();

	// ** Cu::Object virtual methods **

	virtual Cu::Object*
	copy();

	virtual void
	writeToString(String& out) const;

	static const char*
	StaticTypeName() {
		return "cubrguie";
	}

	virtual const char*
	typeName() const;

	virtual bool
	supportsInterface( Cu::ObjectType::Value ) const;

	// Helper
	static Cu::ObjectType::Value
	getTypeAsCuType() {
		return getCubrTypeAsCuType( CubrObjectType::GUIElement );
	}

	//! Initialize the GUI element by container
	/*
		Uses the values stored in the given function container and passes them along to the GUI element via
		a class inheriting the Irrlicht serialize interface: IOAttributes.
	*/
	bool
	initialize( Cu::FunctionObject& ); // infoSource

	//void
	//getAttribute( const util::String&, AttributeSource& );

	//bool
	//setAttributeByString( const util::String&, const util::String& ); // name, value

		// WANTED!
	//! Set the attribute of the given name with the given value
	// Defers to setExtraAttributeByFunctionObject() if no attribute found.
	// gui_set_attribute(element: "attribute" value:)
	//bool
	//setAttribute( const util::String&, Cu::FunctionObject* ); // name, value storage

	// Only necessary for explicit calls (faster than setting by attribute name)
	// (i.e. gui_set_enabled(element:) )

	// gui_enabled(element: state:)
	void
	setEnabled( Cu::BoolObject& );

	// gui_visible(element: state:)
	void
	setVisible( Cu::BoolObject& );

	// gui_position(element: pos:)
	void
	setRelativePosition( Cu::FunctionObject& );

	// gui_id
	void
	setID( Cu::NumericObject& );

	// gui_text(element: "text")
	void
	setText( Cu::StringObject& );

	void
	setText( const util::String& );

/*
protected:
	//virtual bool
	//setExtraAttributeByString( const util::String&, const util::String& );

	virtual bool
	setExtraAttributeByFunctionObject( const util::String&, Cu::FunctionObject* ) { // name, value storage
		return false;
	}
*/

	// gui_to_front(elem:)
	bool
	bringToFrontOnParent();

	// v = gui_enabled( element: )
	bool
	isEnabled();

	// v = gui_visible( element: )
	bool
	isVisible();

	// v = gui_position( element: ) # v == [ x, y, x2, y2 ] #
	void
	getRelativePosition( Cu::FunctionObject& ); // The container is for storage of the result

	// v = gui_id( element: )
	irr::u32
	getID();

	// v = gui_text( element: )
	util::String
	getText();

	// v = gui_child_with_id( element: id: [search_children:] )
	gui_element_t*
	getChildWithId( Cu::UInteger, bool );
/*
	// Irrlicht doesn't yet provide a way to access a child mutably by index
	// v = gui_child_at_index( element: index: )
	gui_element_t*
	getChildByIndex( Cu::UInteger );
*/
	// gui_add_child( element: child: )
	void
	addChild( gui_element_t* );

	// gui_remove_child( element: child: )
	void
	removeChild( gui_element_t* );

	// gui_remove_children( element: )
	void
	removeChildren();

	// gui_expand( element: )
	bool
	expandToParentBounds();

protected:
	static Cu::Object*
	pullResultObject( Cu::FunctionObject* );
};

/*
//! Wrapper for a button
// Contains button-specific wrapper functions.
class Button : public Element {

public:
	Button();

	~Button();

	void setImage( irr::video::ITexture* );

	void setImageSprite( ... );

protected:
	virtual bool
	setExtraAttributeByVariable( const util::String&, Cu::Variable* ); // name, value storage
};
*/

class Image : public Cu::Object {

	irrptr<image_t>  data;
	video_driver_t*  videoDriver;

public:
	Image( image_t*, video_driver_t* );

	image_t*
	getImage();

	// ** Cu::Object virtual methods **

	virtual Cu::Object*
	copy();

	virtual void
	writeToString(String& out) const;

	static const char*
	StaticTypeName() {
		return "cubrimage";
	}

	virtual const char*
	typeName() const;

	// Helper
	static Cu::ObjectType::Value
	getTypeAsCuType() {
		return getCubrTypeAsCuType( CubrObjectType::Image );
	}
};

class Texture : public Cu::Object {

	irrptr<texture_t>  data;

public:
	Texture( texture_t* );

	texture_t*
	getTexture();

	// ** Cu::Object virtual methods **

	virtual Cu::Object*
	copy();

	virtual void
	writeToString(String& out) const;

	static const char*
	StaticTypeName() {
		return "cubrtex";
	}

	virtual const char*
	typeName() const;

	// Helper
	static Cu::ObjectType::Value
	getTypeAsCuType() {
		return getCubrTypeAsCuType( CubrObjectType::Texture );
	}
};

}

#endif
