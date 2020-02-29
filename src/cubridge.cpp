// Copyright 2018 Nicolaus Anderson

#include "cubridge.h"
#include "cubr_messagecodes.h"
#include "cubr_str.h"
#include "cubr_base.h"
#include "cubr_attr.h"
#include "cubr_guiwatcher.h"
#include <IVideoDriver.h>
#include <IGUIButton.h>
#include <IGUIEnvironment.h>

//! Often Checked Attributes Definition
/*
	The default Irrlicht does not support accessing only a limited number of attributes, so setting attributes can be slow.
	One workaround is definining within IAttributeExchangingObject the following:
	- Adding to E_ATTRIBUTE_READ_WRITE_FLAGS: EARWF_OFTEN_CHECKED_ATTRS = 0x00000008
	- Using the flag within the deserializeAttributes() methods of GUI elements for the viewing of limited attributes.

	Optionally, the following helper functions can be added:

//! Indicates the read/write option is for files
inline bool isReadWriteForFile(const io::SAttributeReadWriteOptions* options) {
	if ( options )
		return (options->Flags & EARWF_FOR_FILE) > 0;
	return false;
}

//! Indicates the read/write option is for editors
inline bool isReadWriteForEditor(const io::SAttributeReadWriteOptions* options) {
	if ( options )
		return (options->Flags & EARWF_FOR_EDITOR) > 0;
	return false;
}

inline bool isReadWriteForOftenCheckedAttrs(const io::SAttributeReadWriteOptions* options) {
	if ( options )
		return (options->Flags & EARWF_OFTEN_CHECKED_ATTRS) > 0;
	return false;
}
*/
// Uncomment the following to enable using EARWF_OFTEN_CHECKED_ATTRS for gui_value()
//#define USE_IRR_OFTEN_CHECKED_ATTRS

namespace cubr {

CuBridge::CuBridge(
		Cu::Engine&  eng,
		gui_environment_t*  gui_environment,
		gui_element_t*  gui_root_element
)
	: engine(eng)
	, guiEnvironment(gui_environment)
	, rootElement( gui_root_element )
{
	if ( ! rootElement )
		rootElement = guiEnvironment->getRootGUIElement();

	// Create the bindings based on the settings
	const util::String
				// elements
			s0("gui_root"),
			gsx("gui_create"),
			gsxe("gui_new_empty"),
			gsxw("gui_watcher"),
				// info
			s1p("gui_parent"),
			s1c1("gui_child_with_id"),
			//s1c2("gui_child_at_index"),
			s1c3("gui_add_child"),
			s1c4("gui_remove_child"),
			s1c5("gui_remove_children"),

			s2("gui_attrs"),
			s3("gui_value"),
			//s3b("gui_specifics"),
			s4("gui_to_front"),
			s5("gui_enabled"),
			s6("gui_visible"),
			s7("gui_position"),
			s8("gui_id"),
			s9("gui_text"),
				// image and texture
			ts0("get_texture");

	Cu::addForeignMethodInstance<CuBridge>(engine, s0, this, &CuBridge::gui_getRoot);
	Cu::addForeignMethodInstance<CuBridge>(engine, gsx, this, &CuBridge::gui_create);
	Cu::addForeignMethodInstance<CuBridge>(engine, gsxe, this, &CuBridge::gui_new_empty);
	Cu::addForeignMethodInstance<CuBridge>(engine, gsxw, this, &CuBridge::gui_watcher);

	Cu::addForeignMethodInstance<CuBridge>(engine, s1p, this, &CuBridge::gui_parent);
	Cu::addForeignMethodInstance<CuBridge>(engine, s1c1, this, &CuBridge::gui_child_with_id);
	//Cu::addForeignMethodInstance<CuBridge>(engine, s1c2, this, &CuBridge::gui_child_at_index);
	Cu::addForeignMethodInstance<CuBridge>(engine, s1c3, this, &CuBridge::gui_add_child);
	Cu::addForeignMethodInstance<CuBridge>(engine, s1c4, this, &CuBridge::gui_remove_child);
	Cu::addForeignMethodInstance<CuBridge>(engine, s1c5, this, &CuBridge::gui_remove_children);

	Cu::addForeignMethodInstance<CuBridge>(engine, s2, this, &CuBridge::gui_allAttributes);
	Cu::addForeignMethodInstance<CuBridge>(engine, s3, this, &CuBridge::gui_value);
	//Cu::addForeignMethodInstance<CuBridge>(engine, s3b, this, &CuBridge::gui_specifics);
	Cu::addForeignMethodInstance<CuBridge>(engine, s4, this, &CuBridge::gui_to_front);
	Cu::addForeignMethodInstance<CuBridge>(engine, s5, this, &CuBridge::gui_enabled);
	Cu::addForeignMethodInstance<CuBridge>(engine, s6, this, &CuBridge::gui_visible);
	Cu::addForeignMethodInstance<CuBridge>(engine, s7, this, &CuBridge::gui_position);
	Cu::addForeignMethodInstance<CuBridge>(engine, s8, this, &CuBridge::gui_id);
	Cu::addForeignMethodInstance<CuBridge>(engine, s9, this, &CuBridge::gui_text);

	Cu::addForeignMethodInstance<CuBridge>(engine, ts0, this, &CuBridge::texture_access);

		// gui_set_callback( elem: "gui event name" callback )
	GUIWatcherObject::addSetterToEngine(engine);
}

CuBridge::~CuBridge() {

}

Cu::Engine&
CuBridge::getCuEngine() {
	return engine;
}

void
CuBridge::setGUIEnvironment( gui_environment_t*  env, gui_element_t*  root ) {
	guiEnvironment = env;
	if ( root ) {
		rootElement = root;
	} else {
		rootElement = guiEnvironment->getRootGUIElement();
	}
}

ForeignFunc::Result
CuBridge::gui_getRoot( Cu::FFIServices& ffi ) {
	// Cannot use if there is no root GUI element
	if ( !rootElement ) {
		rootElement = guiEnvironment->getRootGUIElement();
	}
	if ( rootElement ) {
		ffi.setNewResult( new GUIElement(rootElement, guiEnvironment) );
	}
	rootElement->grab(); // Required or there is a segfault for some reason
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_create( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCountRange(1,2)
		|| !ffi.demandArgType(0, Cu::ObjectType::String)
	) {
		return ForeignFunc::NONFATAL;
	}
	// Use name for factory
	const util::String& name = ((Cu::StringObject&)ffi.arg(0)).getString();
	irr::gui::IGUIElement* e = guiEnvironment->addGUIElement( name.c_str(), rootElement );
	return gui_create_element_setup(ffi,e,1);
}

ForeignFunc::Result
CuBridge::gui_new_empty( Cu::FFIServices& ffi ) {
	irr::gui::IGUIElement* e =
		new irr::gui::IGUIElement(
			irr::gui::EGUIET_ELEMENT,
			guiEnvironment,
			rootElement,
			-1,
			irr::core::recti()
		);
	ForeignFunc::Result  r = gui_create_element_setup(ffi,e,0);
	e->drop();
	return r;
}

ForeignFunc::Result
CuBridge::gui_watcher( Cu::FFIServices& ffi ) {

	GUIWatcherObject*  elem =
		new GUIWatcherObject(
			&engine,
			guiEnvironment,
			rootElement
		);

	elem->expandToParentBounds();
	ffi.setNewResult(elem);

	GUIElement*  otherElem = nullptr;
	util::String  callbackEvent;
	Cu::FunctionObject*  callback = REAL_NULL;

	switch( ffi.getArgCount() )
	{
	case 1:
		// Arg 1 == GUI element
		if ( ffi.demandArgType(0, GUIElement::getTypeAsCuType()) )
		{
			otherElem = (GUIElement*)&(ffi.arg(0));
		}
		else {
			ffi.printCustomWarningCode( CuBridgeMessageCode::GUIWatcherWrongConstructionArgs );
		}
		break;

	case 2:
		// Arg 1 == Callback Event, Arg 2 == Callback
		if ( ffi.demandArgType(0, Cu::ObjectType::String)
			&& ffi.demandArgType(1, Cu::ObjectType::Function) )
		{
			callbackEvent = ((Cu::StringObject&)ffi.arg(0)).getString();
			callback = (Cu::FunctionObject*)&(ffi.arg(1));
		}
		else {
			ffi.printCustomWarningCode( CuBridgeMessageCode::GUIWatcherWrongConstructionArgs );
		}
		break;

	case 3:
		// Arg 1 == GUI element, Arg 2 == Callback Event, Arg 3 == Callback
		if ( ffi.demandArgType(0, GUIElement::getTypeAsCuType())
			&& ffi.demandArgType(1, Cu::ObjectType::String)
			&& ffi.demandArgType(2, Cu::ObjectType::Function) )
		{
			otherElem = (GUIElement*)&(ffi.arg(0));
			callbackEvent = ((Cu::StringObject&)ffi.arg(1)).getString();
			callback = (Cu::FunctionObject*)&(ffi.arg(2));
		}
		else {
			ffi.printCustomWarningCode( CuBridgeMessageCode::GUIWatcherWrongConstructionArgs );
		}
		break;

	default: break;
	}

	irr::core::recti  elemRect;
	irr::gui::AlignmentInfo  alignInfo;
	irr::gui::IGUIElement* otherElemElem;
	irr::gui::IGUIElement* elemElem;

	if ( otherElem && otherElem->getElement() && elem->getElement() ) {
		otherElemElem = otherElem->getElement();
		elemElem = elem->getElement();
		elemRect = otherElemElem->getRelativePosition();
		alignInfo = otherElemElem->getAlignment();
		otherElemElem->getParent()->addChild(elemElem);
		elemElem->setRelativePosition( elemRect );
		elemElem->setAlignment( alignInfo );
		elemElem->addChild(otherElemElem);
		otherElem->expandToParentBounds();
	}
	if ( notNull(callback) ) {
		elem->setCallback(
			getGUIEventTypeFromString( callbackEvent ),
			callback
		);
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_instantiate( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgCount(2)
		|| ! ffi.demandArgType(0, GUIElement::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::Function)
	) {
		return ForeignFunc::NONFATAL;
	}
	// Extract the element first, then extract the function container with info
	Cu::Object*  object;
	GUIElement*  elem_obj = &((GUIElement&)ffi.arg(0));

	if ( ! elem_obj ) return ForeignFunc::NONFATAL;

	object = ffi.arg(0);
	if ( elem_obj->initialize( (Cu::FunctionObject&)*object ) ) {
		return ForeignFunc::FINISHED;
	}
	return ForeignFunc::NONFATAL;
}

ForeignFunc::Result
CuBridge::gui_allAttributes( Cu::FFIServices& ffi ) {
	irr::io::SAttributeReadWriteOptions  options;
	return gui_attributeSelector(ffi, options);
}

ForeignFunc::Result
CuBridge::gui_value( Cu::FFIServices& ffi ) {
	irr::io::SAttributeReadWriteOptions  options;
#ifdef USE_IRR_OFTEN_CHECKED_ATTRS // See note at top of file
	options.Flags = irr::io::EARWF_OFTEN_CHECKED_ATTRS;
#endif
	return gui_attributeSelector(ffi, options);
}

/*ForeignFunc::Result
CuBridge::gui_specifics( Cu::FFIServices& ffi ) {
	irr::io::SAttributeReadWriteOptions  options;
	options.Flags = irr::io::EARWF_FOR_SCRIPTING_LANG_EXTRA;
	return gui_attributeSelector(ffi, options);
}*/

ForeignFunc::Result
CuBridge::gui_parent( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCountRange(1,2)
		|| !ffi.demandAllArgsType(GUIElement::getTypeAsCuType())
	) {
		return ForeignFunc::NONFATAL;
	}
	gui_element_t*  child = ((GUIElement&)ffi.arg(0)).getElement();

	if ( ffi.getArgCount() == 2 ) {
		// The parent is being set
		((GUIElement&)ffi.arg(1)).addChild(child);
	} else {
		// The parent is being requested
		ffi.setNewResult( new GUIElement(child->getParent(), guiEnvironment) );
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_child_with_id( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCount(2)
		|| !ffi.demandArgType(0, GUIElement::getTypeAsCuType())
		|| !ffi.demandArgType(1, Cu::ObjectType::Numeric)
	) {
		return ForeignFunc::NONFATAL;
	}
	
	GUIElement&  parent = (GUIElement&)ffi.arg(0);
	Cu::NumericObject&  id = (Cu::NumericObject&)ffi.arg(1);
	gui_element_t*  child = parent.getChildWithId( id.getIntegerValue(), false );
	// Irrlicht returns a zero if the child is not found
	if ( child ) {
		ffi.setNewResult( new GUIElement(child, guiEnvironment) );
	} else {
		ffi.printCustomInfoCode(CuBridgeMessageCode::GUIElementChildNotFound);
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_add_child( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCount(2) || !ffi.demandAllArgsType( GUIElement::getTypeAsCuType() ) ) {
		return ForeignFunc::NONFATAL;
	}
	GUIElement&  parent = (GUIElement&)ffi.arg(0);
	gui_element_t*  child = ((GUIElement&)ffi.arg(1)).getElement();

	if ( child ) {
		parent.addChild(child);
	} else {
		ffi.printCustomInfoCode(CuBridgeMessageCode::GUIElementIsEmpty);
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_remove_child( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCount(2) || !ffi.demandAllArgsType( GUIElement::getTypeAsCuType() ) ) {
		return ForeignFunc::NONFATAL;
	}
	GUIElement&  parent = (GUIElement&)ffi.arg(0);
	gui_element_t*  child = ((GUIElement&)ffi.arg(1)).getElement();

	if ( child ) {
		parent.removeChild(child);
	} else {
		ffi.printCustomInfoCode(CuBridgeMessageCode::GUIElementIsEmpty);
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_remove_children( Cu::FFIServices& ffi ) {
	if ( !ffi.demandAllArgsType( GUIElement::getTypeAsCuType() ) ) {
		return ForeignFunc::NONFATAL;
	}

	GUIElement*  element = nullptr;
	Cu::UInteger  argIndex = 0;
	for ( ; argIndex < ffi.getArgCount(); ++argIndex ) {
		element = & (GUIElement&)(ffi.arg(argIndex) );
		element->removeChildren();
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_to_front( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCount(1) || !ffi.demandArgType(0, GUIElement::getTypeAsCuType()) ) {
		return ForeignFunc::NONFATAL;
	}
	GUIElement& elem = (GUIElement&)ffi.arg(0);
	elem.bringToFrontOnParent();
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_enabled( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCountRange(1,2)
		|| !ffi.demandArgType(0, GUIElement::getTypeAsCuType())
	) {
		return ForeignFunc::NONFATAL;
	}
	GUIElement&  elem = (GUIElement&)ffi.arg(0);

	if ( ffi.getArgCount() == 2 ) {
		if ( !ffi.demandArgType(1, Cu::ObjectType::Bool) )
			return ForeignFunc::NONFATAL;

		elem.setEnabled( (Cu::BoolObject&)ffi.arg(1) );
	} else {
		ffi.setNewResult( new Cu::BoolObject( elem.isEnabled() ) );
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_visible( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCountRange(1,2)
		|| !ffi.demandArgType(0, GUIElement::getTypeAsCuType())
	) {
		return ForeignFunc::NONFATAL;
	}
	GUIElement&  elem = (GUIElement&)ffi.arg(0);

	if ( ffi.getArgCount() == 2 ) {
		if ( !ffi.demandArgType(1, Cu::ObjectType::Bool) )
			return ForeignFunc::NONFATAL;

		elem.setVisible( (Cu::BoolObject&)ffi.arg(1) );
	} else {
		ffi.setNewResult( new Cu::BoolObject( elem.isVisible() ) );
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_position( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCountRange(1, 2)
		|| !ffi.demandArgType(0, GUIElement::getTypeAsCuType())
	) {
		return ForeignFunc::NONFATAL;
	}
	GUIElement&  elem = (GUIElement&)ffi.arg(0);
	Cu::FunctionObject*  pos_data;

	if ( ffi.getArgCount() == 2 ) {
		if ( !ffi.demandArgType(1, Cu::ObjectType::Function) ) {
			return ForeignFunc::NONFATAL;
		}
		pos_data = &((Cu::FunctionObject&)ffi.arg(1));
		elem.setRelativePosition( *pos_data );
	} else {
		pos_data = new Cu::FunctionObject();
		elem.getRelativePosition( *pos_data );
		ffi.setNewResult( pos_data );
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_id( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCountRange(1,2)
		|| !ffi.demandArgType(0, GUIElement::getTypeAsCuType())
	) {
		ffi.setNewResult( new Cu::IntegerObject( 0 ) );
		return ForeignFunc::NONFATAL;
	}
	GUIElement&  elem = (GUIElement&)ffi.arg(0);

	if ( ffi.getArgCount() == 2 ) {
		if ( !ffi.demandArgType(1, Cu::ObjectType::Numeric) ) {
			return ForeignFunc::NONFATAL;
		}
		elem.setID( (Cu::NumericObject&)ffi.arg(1) );
	} else {
		ffi.setNewResult( new Cu::IntegerObject( elem.getID() ) );
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_text( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgCountRange(1,2)
		|| !ffi.demandArgType(0, GUIElement::getTypeAsCuType())
	) {
		return ForeignFunc::NONFATAL;
	}
	GUIElement&  elem = (GUIElement&)ffi.arg(0);

	if ( ffi.getArgCount() == 2 ) {
		if ( !ffi.demandArgType(1, Cu::ObjectType::String) ) {
			return ForeignFunc::NONFATAL;
		}
		elem.setText( (Cu::StringObject&)ffi.arg(1) );
	} else {
		ffi.setNewResult( new Cu::StringObject( elem.getText() ) );
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_expand( Cu::FFIServices& ffi ) {
	if ( !ffi.demandAllArgsType( GUIElement::getTypeAsCuType() )
		|| !ffi.demandMinArgCount(1) )
	{
		return ForeignFunc::NONFATAL;
	}

	Cu::UInteger  i = 0;
	for (; i < ffi.getArgCount(); ++i) {
		if ( ! ((GUIElement&)ffi.arg(i)).expandToParentBounds() ) {
			ffi.printCustomWarningCode( CuBridgeMessageCode::GUIElementLacksParent );
		}
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
CuBridge::gui_create_element_setup( Cu::FFIServices& ffi, gui_element_t* e, irr::s32  startArg ) {
	if ( !e ) {
		ffi.printCustomWarningCode(CuBridgeMessageCode::GUIElementCannotBeCreated);
		return ForeignFunc::NONFATAL;
	}
	if ( ffi.getArgCount() == (u32)startArg ) {
		ffi.setNewResult( new GUIElement(e, guiEnvironment) );
		//e->drop();
		return ForeignFunc::FINISHED;
	}
	// NOTE: Ignore the presence of more than (currentlyUsedArgs+1) (a.k.a startArg+1) arguments.
	if ( !ffi.demandArgType(startArg, Cu::ObjectType::Function) ) {
		ffi.setNewResult( new GUIElement(e, guiEnvironment) );
		//e->drop();
		return ForeignFunc::FINISHED;
	}
	AttributeSource attrs( guiEnvironment->getVideoDriver(), (Cu::FunctionObject&)ffi.arg(startArg) );
	e->deserializeAttributes(&attrs);
	ffi.setNewResult( new GUIElement(e, guiEnvironment) );
	//e->drop();
	return ForeignFunc::FINISHED;	
}

ForeignFunc::Result
CuBridge::gui_attributeSelector( Cu::FFIServices&  ffi, irr::io::SAttributeReadWriteOptions&  options ) {
	if ( !ffi.demandArgCountRange(1,2)
		|| !ffi.demandArgType(0, GUIElement::getTypeAsCuType())
	) {
		return ForeignFunc::NONFATAL;
	}
	GUIElement&  element = (GUIElement&)ffi.arg(0);
	Cu::FunctionObject*  attrsContainer = nullptr;
	bool foundAttrs = false;
	if ( ffi.getArgCount() == 2 ) {
		if ( ffi.demandArgType(1, Cu::ObjectType::Function) ) {
			foundAttrs = true;
			attrsContainer = &((Cu::FunctionObject&)ffi.arg(1));
		} else {
			return ForeignFunc::NONFATAL;
		}
	}
	if ( !attrsContainer ) {
		attrsContainer = new Cu::FunctionObject();
	}
	AttributeSource  attrSource( guiEnvironment->getVideoDriver(), *attrsContainer );
	if ( foundAttrs ) {
		element.getElement()->deserializeAttributes(&attrSource, &options);
	} else {
		element.getElement()->serializeAttributes(&attrSource, &options);
		ffi.setNewResult(attrsContainer);
	}
	return ForeignFunc::FINISHED;
}

texture_t*
CuBridge::getTexture( util::String&  pathStr ) {
	return guiEnvironment->getVideoDriver()->getTexture( CuStrToIrrPath(pathStr) );
}

ForeignFunc::Result
CuBridge::texture_access( Cu::FFIServices& ffi ) {
	// Other arguments are ignored
	if ( !ffi.demandArgType(0, Cu::ObjectType::String) )
		return ForeignFunc::NONFATAL;

	Cu::StringObject&  pathName = (Cu::StringObject&)ffi.arg(0);
	texture_t* tex = getTexture( pathName.getString() );
	if ( tex )
		ffi.setNewResult( new Texture(tex) );
	return ForeignFunc::FINISHED;
}


}
