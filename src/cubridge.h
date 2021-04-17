// Copyright 2018 Nicolaus Anderson

#ifndef _COPPER_BRIDGE_H_
#define _COPPER_BRIDGE_H_

#include <IGUIEnvironment.h> // from Irrlicht
#include <Copper.h>
#include "cubr_base.h"
#include "json/cubr_json.h"

namespace cubr {

using Cu::ForeignFunc;

//! Copper Bridge
/*
	The main class for wrapping the engine and adding functionality.
	Instantiate directly.
*/
class CuBridge {
protected:
	Cu::Engine&  engine;
	gui_environment_t*  guiEnvironment;
	gui_element_t*  rootElement;
	json::Hub jsonHub;

public:

	struct InitFlags {
		bool  enableImageModifying;
		bool  enableJSON;

		InitFlags()
			: enableImageModifying(false)
			, enableJSON(false)
		{}
	};


	CuBridge(
		Cu::Engine&  eng,
		gui_environment_t*  gui_environment,
		gui_element_t*  gui_root_element,
		InitFlags  flags = InitFlags()
	);

	virtual ~CuBridge();

	Cu::Engine&
	getCuEngine();

	// Set the GUI environment a new root GUI element
	void
	setGUIEnvironment( gui_environment_t*  env, gui_element_t*  root=nullptr);

	// Methods added to the Copper as foreign functions
	// (Added via addForeignMethodInstance())
		// GUI element methods
			// gui_root()
	ForeignFunc::Result  gui_getRoot( Cu::FFIServices& );
			// gui_create( info: )
	ForeignFunc::Result  gui_create( Cu::FFIServices& );
			// gui_new_empty( info: )
	ForeignFunc::Result  gui_new_empty( Cu::FFIServices& );
			// gui_watcher( info: )
	ForeignFunc::Result  gui_watcher( Cu::FFIServices& );
			// Instantiation of attributes of a single GUI element
			// gui_instantiate( element: info: )
	ForeignFunc::Result  gui_instantiate( Cu::FFIServices& );
			// gui_attrs( element: )
	ForeignFunc::Result  gui_allAttributes( Cu::FFIServices& );
			// Returns serialized values for the most commonly-changed attributes.
			// For some elements, this returns all attributes anyways.
			// gui_value( element: )
	ForeignFunc::Result  gui_value( Cu::FFIServices& );
			// Returns serialized values for all attributes specific to an element (but not inherited attributes)
			// gui_specifics( element: )
	//ForeignFunc::Result gui_specifics( Cu::FFIServices& );
			// gui_parent( element: [parent:] )
	ForeignFunc::Result  gui_parent( Cu::FFIServices& );
			// gui_child_with_id( element: child_id: )
	ForeignFunc::Result  gui_child_with_id( Cu::FFIServices& );
			// gui_child_at_index( element: child_index: )
	//ForeignFunc::Result gui_child_at_index( Cu::FFIServices& );
			// gui_add_child( element: child_element: )
	ForeignFunc::Result  gui_add_child( Cu::FFIServices& );
			// gui_remove_child( element: child_element: )
	ForeignFunc::Result  gui_remove_child( Cu::FFIServices& );
			// gui_remove_children( element: )
	ForeignFunc::Result  gui_remove_children( Cu::FFIServices& );
			// gui_type( element: ) // Returns integer value casted from enum or a string name
	// TODO
			// gui_to_front( element: )
	ForeignFunc::Result  gui_to_front( Cu::FFIServices& );
			// gui_enabled( element: [new value:] )
	ForeignFunc::Result  gui_enabled( Cu::FFIServices& );
			// gui_visible( element: [new value:] )
	ForeignFunc::Result  gui_visible( Cu::FFIServices& );
			// gui_position( element: [new value:] ) // new value = [x, y, x2, y2]
	ForeignFunc::Result  gui_position( Cu::FFIServices& );
			// gui_id( element: [new value:] )
	ForeignFunc::Result  gui_id( Cu::FFIServices& );
			// gui_text( element: [new value:] )
	ForeignFunc::Result  gui_text( Cu::FFIServices& );
			// gui_expand( element: )
	ForeignFunc::Result  gui_expand( Cu::FFIServices& );

protected:
	// Expects a "new"-created element (one whose reference count it can drop once).
	ForeignFunc::Result
	gui_create_element_setup( Cu::FFIServices&, gui_element_t*, irr::s32 startArg );

	ForeignFunc::Result
	gui_attributeSelector( Cu::FFIServices&, irr::io::SAttributeReadWriteOptions& );

public:
	// Converts string values:
	// "A1R5G5B5" = ECF_A1R5G5B5
	// "R5G6B5" = ECF_R5G6B5
	// "R8G8B8" = ECF_R8G8B8
	// "A8R8G8B8" = ECF_A8R8G8B8
	irr::video::ECOLOR_FORMAT  stringToColorFormat( util::String );

			// image_create( width: height: [color_format_string:] ) - Default is "A8R8G8B8" (ECF_A8R8G8B8)
	ForeignFunc::Result  image_create( Cu::FFIServices& );

			// image_to_texture( image: )
	ForeignFunc::Result  image_to_texture( Cu::FFIServices& );

		// Image and Texture methods
	virtual texture_t*
	getTexture( util::String&  pathStr ); // Override to prohibit

			// get_texture( path: )
	virtual ForeignFunc::Result
	texture_access( Cu::FFIServices& );
};

}

#endif
