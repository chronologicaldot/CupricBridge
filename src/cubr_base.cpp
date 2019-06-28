// Copyright 2018 Nicolaus Anderson

#include "cubr_base.h"
#include "cubr_str.h"
#include "cubr_attr.h"
#include <irrList.h>

namespace cubr {

Cu::ObjectType::Value  getCubrTypeAsCuType( CubrObjectType::Value  type ) {
	return static_cast<Cu::ObjectType::Value>(Cu::ObjectType::Unknown + type + 1);
}

bool isGUIElementObject( Cu::Object&  object ) {
	return object.getType() == getCubrTypeAsCuType(CubrObjectType::GUIElement);
	//return util::equals(obj.typeName(), GUIElementTypeName);
}

bool isImageObject( Cu::Object&  object ) {
	return object.getType() == getCubrTypeAsCuType(CubrObjectType::Image);
	//return util::equals(obj.typeName(), ImageTypeName);
}

bool isTextureObject( Cu::Object&  object ) {
	return object.getType() == getCubrTypeAsCuType(CubrObjectType::Texture);
	//return util::equals(obj.typeName(), TextureTypeName);
}

GUIElement::GUIElement( gui_element_t* e, gui_environment_t* ev )
	: Cu::Object( GUIElement::getTypeAsCuType() )
	, data()
	, environment(ev)
{
	data.set(e);
	// TODO FIXME: Should call assert(e) because the rest of the code assumes the data has been set.
}

gui_element_t*
GUIElement::getElement() {
	return data.get();
}

Cu::Object*
GUIElement::copy() {
	// This line is wrong. It should create a NEW, independent copy of the GUI element.
	//return new GUIElement(data.get());
/*
	// TODO FIXME
	// PROBLEMS:
	// IGUIEnvironment::addElement doesn't create default elements, so some kind of method would need to do this.
	// Custom elements just have a generic type - no way to recognize them - so they aren't possible to copy.
	// Irrlicht needs a virtual IGUIElement::clone() (returning IGUIElement*) method.
	gui_element_t*  e = data.get();
	if ( !e ) {
		e = environment->addElement( irr::gui::GUIElementTypeNames[ e->getType() ], e->getParent() );
		if ( !e ) { // still no element
			e = new gui_element_t( irr::gui::EGUIET_ELEMENT, environment, e->getParent(), -1, irr::core::recti() );	
		}
	} else {
		e->grab(); // Counter the drop() needed below
	}
	GUIElement* ge = new GUIElement(e);
	e->drop(); // In case element is created
	return ge;
*/
/*
	Irrlicht doesn't have a native IGUIElement::clone().
	The closest thing would be to create a template<class T> clone(T&, IGUIEnvironment) function that is used to instantiate elements via specializations. other contextual data may be needed, such as textures, etc. This could be given null values, in most cases.
	Unfortunately, however, the clone specializations would have to be done for every element - something I may forget to
	account for when I add my own elements. Still, it's possible.
	Oh, but children would have to be cloned too. :/ This could be optional, but maybe not.
*/

	// No copy (User has to manually create a new element and pass the attributes by variable)
	// info = get_attrs(elem:)
	// newElem = gui_custom("type", info:)
	this->ref();
	return this;
}

void
GUIElement::writeToString(String& out) const {
	out = "{CuBridge GUI Element}";
}

const char*
GUIElement::typeName() const {
	return GUIElement::StaticTypeName();
}

bool
GUIElement::supportsInterface( Cu::ObjectType::Value  t ) const {
	return t == GUIElement::getTypeAsCuType();
}

bool
GUIElement::initialize( Cu::FunctionObject& infoSource ) {
	AttributeSource s(environment->getVideoDriver(), infoSource);
	data.access().deserializeAttributes(&s);
	return true;
}

//void
//GUIElement::getAttribute( const util::String&  name, AttributeSource& attrs ) {
//
//}

//bool
//GUIElement::setAttribute( const util::String&, Cu::FunctionObject* ) { // name, value storage
//
//}

void
GUIElement::setEnabled( Cu::BoolObject&  settingObject ) {
	data.access().setEnabled(settingObject.getValue());
}

void
GUIElement::setVisible( Cu::BoolObject&  settingObject ) {
	data.access().setVisible(settingObject.getValue());
}

void
GUIElement::setRelativePosition( Cu::FunctionObject& infoSource ) {
	irr::core::recti  pos(0,0,0,0);
	Cu::Function*  f;
	Cu::Scope*  scope;
	util::String  name_x("x"),
					name_y("y"),
					name_x2("x2"),
					name_y2("y2");
	Cu::Variable*  var;
	Cu::Object*  resultObj;

	// Extract the values
	if ( infoSource.getFunction(f) ) {
		scope = &(f->getPersistentScope());

		scope->getVariable(name_x, var);
		if ( var->getFunction(REAL_NULL)->result.obtain(resultObj) ) {
			if ( Cu::isNumericObject( *resultObj ) ) {
				pos.UpperLeftCorner.X = ((Cu::NumericObject*)resultObj)->getIntegerValue();
			}
		}

		scope->getVariable(name_y, var);
		if ( var->getFunction(REAL_NULL)->result.obtain(resultObj) ) {
			if ( Cu::isNumericObject( *resultObj ) ) {
				pos.UpperLeftCorner.Y = ((Cu::NumericObject*)resultObj)->getIntegerValue();
			}
		}

		scope->getVariable(name_x2, var);
		if ( var->getFunction(REAL_NULL)->result.obtain(resultObj) ) {
			if ( Cu::isNumericObject( *resultObj ) ) {
				pos.LowerRightCorner.X = ((Cu::NumericObject*)resultObj)->getIntegerValue();
			}
		}

		scope->getVariable(name_y2, var);
		if ( var->getFunction(REAL_NULL)->result.obtain(resultObj) ) {
			if ( Cu::isNumericObject( *resultObj ) ) {
				pos.LowerRightCorner.Y = ((Cu::NumericObject*)resultObj)->getIntegerValue();
			}
		}

		data.access().setRelativePosition(pos);
	}
}

void
GUIElement::setID( Cu::NumericObject&  settingObject ) {
	data.access().setID( (irr::s32) settingObject.getIntegerValue() );	
}

void
GUIElement::setText( Cu::StringObject&  settingObject ) {
	setText( settingObject.getString() );
}

void
GUIElement::setText( const util::String& text ) {
	data.access().setText( CuStrToIrrStrW(text).c_str() );	
}

bool
GUIElement::bringToFrontOnParent() {
	gui_element_t*  parent = data.access().getParent();
	if ( parent ) {
		return parent->bringToFront(&(data.access()));
	}
	return false;
}

bool
GUIElement::isEnabled() {
	return data.access().isEnabled();
}

bool
GUIElement::isVisible() {
	return data.access().isVisible();
}

void
GUIElement::getRelativePosition( Cu::FunctionObject& storage ) {
	irr::core::recti  pos = ((gui_element_t&)data.access()).getRelativePosition();
	Cu::Function*  f;
	Cu::Scope*  scope;
	util::String  name_x("x"),
				name_y("y"),
				name_x2("x2"),
				name_y2("y2");
	// Note: Declaring a Copper object that will die in this local scope only works because
	// the method Variable::setFuncReturn (via setVariableFrom) defaults to copying.
	Cu::IntegerObject  x( (Cu::Integer) pos.UpperLeftCorner.X ),
					y( (Cu::Integer) pos.UpperLeftCorner.Y ),
					x2( (Cu::Integer) pos.LowerRightCorner.X ),
					y2( (Cu::Integer) pos.LowerRightCorner.Y );

	if ( storage.getFunction(f) ) {
		scope = &(f->getPersistentScope());
		scope->setVariableFrom(name_x, x, false); // Reuse of storage means nothing here. Default is copy.
		scope->setVariableFrom(name_y, y, false);
		scope->setVariableFrom(name_x2, x2, false);
		scope->setVariableFrom(name_y2, y2, false);
	}
}

irr::u32
GUIElement::getID() {
	return ((gui_element_t&)data.access()).getID();
}

util::String
GUIElement::getText() {
	irr::core::stringw  wcs = ((gui_element_t&)data.access()).getText();
	return IrrStrWToCuStr( wcs );
}

gui_element_t*
GUIElement::getChildWithId( Cu::UInteger  id, bool  searchKids ) {
	return data.access().getElementFromId( id, searchKids );
}
/*
gui_element_t*
GUIElement::getChildByIndex( Cu::UInteger  index ) {
	// Cannot convert from const list<IGUIElement> & to list<IGUIElement.
	// Irrlicht doesn't yet provide a way to access a child by index
	irr::core::list<irr::gui::IGUIElement*>&  l = data.access().getChildren();
	irr::core::list<irr::gui::IGUIElement*>::Iterator  i = l.begin();
	if ( index < l.size() ) {
		for (; index > 0; --index )
			++i;
		return *i;
	}
	return nullptr;
}
*/
void
GUIElement::addChild( gui_element_t* child ) {
	data.access().addChild(child);
}

void
GUIElement::removeChild( gui_element_t* child ) {
	data.access().removeChild(child);
}

bool
GUIElement::expandToParentBounds() {
	gui_element_t&  e = data.access();
	if ( !e.getParent() ) {
		return false;
	}
	const irr::core::recti  parentPos = e.getParent()->getRelativePosition();
	e.setAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_LOWERRIGHT,
				irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_LOWERRIGHT);
	e.setRelativePosition( irr::core::recti(0,0, parentPos.getWidth(), parentPos.getHeight()) );
	return true;
}

Cu::Object*
GUIElement::pullResultObject( Cu::FunctionObject* src ) {
	Cu::Function*  f;
	Cu::Object*  obj;
	if ( src->getFunction(f) ) {
		if (f->result.obtain(obj)) {
			return obj;
		} else {
			return nullptr;
		}
	} else {
		return nullptr;
	}
}

Image::Image( image_t* t, video_driver_t* v )
	: Cu::Object( Image::getTypeAsCuType() )
	, data()
	, videoDriver(v)
{
	data.set(t);
}

image_t*
Image::getImage() {
	return data.get();
}

Cu::Object*
Image::copy() {
	image_t*  img = videoDriver->createImage(data.access().getColorFormat(), data.access().getDimension());
	Image*  out = new Image(img, videoDriver);
	img->drop();
	return out;
}

void
Image::writeToString(String& out) const {
	out = "{CuBridge Image}";
}

const char*
Image::typeName() const {
	return Image::StaticTypeName();
}

Texture::Texture( texture_t* t )
	: Cu::Object( Texture::getTypeAsCuType() )
	, data()
{
	data.set(t);
}

texture_t*
Texture::getTexture() {
	return data.get();
}

Cu::Object*
Texture::copy() {
	// Technically, a new texture (copied from the original) should be created with the video driver
	// by means of the environment. However, this texture would need a unique name, and that would mess
	// up whenever two copies of the same texture are made.
	// Finally, it should never be necessary to create a texture copy anyways. Only images need to be editable.
	return new Texture(data.get());
}

void
Texture::writeToString(String& out) const {
	out = "{CuBridge Texture}";
}

const char*
Texture::typeName() const {
	return Texture::StaticTypeName();
}


}
