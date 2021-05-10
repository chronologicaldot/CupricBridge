// (C) 2021 Nicolaus Anderson
/*
	Requires IrrExt for irrTree and irrJSON.
*/

#ifndef _CUBR_JSON_H_
#define _CUBR_JSON_H_

#include <irrJSON.h>
#include <Copper.h>
#include "../cubr_base.h"

namespace cubr {

bool isJSONStorageObject( Cu::Object&  object );

namespace json {

using irr::io::irrJSONElement;
using irr::io::irrJSON;

/*
	A Hub is needed for instantiating JSON storage objects.
	Use addToEngine() to add the JSON-related functions to the Copper virtual machine.
*/
class Hub {
	irr::io::IFileSystem*  fileSystem;
public:
	Hub( irr::io::IFileSystem* );

	void addToEngine( Cu::Engine& );

	//! Create a JSON Storage Copper object
	Cu::ForeignFunc::Result
	Create( Cu::FFIServices& );
};

//----------------------------

class Storage; // predeclaration

/*
	Used for accessing the JSON tree.
	It can only point to a single node but provides a means for accessing other nodes.
*/
class Accessor
	: public Cu::Object
{
	Storage* storage;
	irrTreeNode*  node;

public:
	Accessor( Storage*, irrTreeNode*, bool mustRegister=true );

	~Accessor();

	// ** Cu::Object virtual methods **

	virtual Cu::Object*
	copy();

	virtual void
	writeToString(String& out) const;

	static const char*
	StaticTypeName() {
		return "cubrjsonac";
	}

	virtual const char*
	typeName() const;

	virtual bool
	supportsInterface( Cu::ObjectType::Value ) const;

	// Helper
	static Cu::ObjectType::Value
	getTypeAsCuType() {
		return getCubrTypeAsCuType( CubrObjectType::JSONAccessor );
	}

	//** Class-relationship methods **

	// When the Storage class is about to destruct, this is called.
	void invalidate();

	//** JSON-related methods **

	Cu::UInteger getChildCount();

	Accessor* getChild( Cu::UInteger );

	Accessor* addChild( const util::String& );

	Accessor* getParent();

	//! Stores in the given string object the node name pointed to by the accessor with
	//! the given index
	bool getElementName( util::String& );

	//!
	bool setElementName( const util::String& );

	//! Stores in storage the value and attributes of the node pointed to by the accessor
	//! with the given index
	bool getElementAttributes( Cu::FunctionObject*& storage );

	//!
	bool setElementAttributes( Cu::FunctionObject& source );

private:
	bool valid() { return node != 0 && storage != 0; }
};

//----------------------------

/*
	Where all of the JSON data is loaded to, stored, and saved from.
*/
class Storage
	: public Cu::Object
{
	irr::io::path  filePath;
	irr::io::IFileSystem*  fileSystem;
	irrTreeNode*  rootNode;
	irrJSON  json;
	irr::core::array<Accessor*>  accessors;

public:

	Storage( irr::io::IFileSystem* );

	~Storage();

	// ** Cu::Object virtual methods **

	virtual Cu::Object*
	copy();

	virtual void
	writeToString(String& out) const;

	static const char*
	StaticTypeName() {
		return "cubrjson";
	}

	virtual const char*
	typeName() const;

	virtual bool
	supportsInterface( Cu::ObjectType::Value ) const;

	// Helper
	static Cu::ObjectType::Value
	getTypeAsCuType() {
		return getCubrTypeAsCuType( CubrObjectType::JSONStorage );
	}

	// ** JSON-related methods **

	bool parseFile( const irr::io::path& );

	void setFilePath( const irr::io::path& );

	bool writeToFile();

	// KEEP THE ACCESSORS
	// Even though the tree can be converted into a Copper structure, only the JSON tree can
	// have multiple nodes with identical names, which is often useful in trees that represent scenes.

	//! Create an accessor
	//! Returns a new accessor.
	Accessor* createAccessor();

	//! Register accessor
	//! Adds the given accessor to the list of accessors so it can be notified when the JSON data dies.
	void registerAccessor( Accessor* );

	//! Remove accessor from registry
	//! If an accessor dies, it must be removed so that the Storage doesn't try to notify it.
	void deregisterAccessor( Accessor* );

	//! Convert the entire tree into the corresponding Copper variable/object structure.
	//! The "storage" parameter MUST be initialized. It acts as the root tree node.
	//! Note that multiple nodes with identical names will be merged into a single node.
	bool convertToCopper( Cu::Function& storage );

	//! Convert from Copper, creating a JSON tree from the given Copper object.
	bool convertFromCopper( Cu::Function& source );
};

//-----------------------------------

//! Load JSON from a file.
//! \params JSONStorage storage, String filePath
Cu::ForeignFunc::Result
OpenAndParse( Cu::FFIServices& );

//! Sets the file path of the storage. Useful when saving to a new file.
//! \params JSONStorage storage, String filePath
Cu::ForeignFunc::Result
SetFilePath( Cu::FFIServices& );

//! Creates an accessor and returns it
//! \params JSONStorage storage
Cu::ForeignFunc::Result
CreateAccessor( Cu::FFIServices& );

//! Returns the child count of the node stored in the given accessor.
//! \params JSONAcessor accessor
Cu::ForeignFunc::Result
GetAccessorChildCount( Cu::FFIServices& );

//! Creates an accessor to a child at the given index.
//! \params JSONAccessor accessor, UInteger childIndex
Cu::ForeignFunc::Result
AccessorChild( Cu::FFIServices& );

//! Creates a new child and an accessor to it.
//! \params JSONAccessor accessor, String childName
Cu::ForeignFunc::Result
AddChild( Cu::FFIServices& );

//! TODO: Delete a child
//! FIXME: Unfortunately, we would have to notify all the accessors that point to this child node.
//! Moreover, children of the node would also be deleted, and we would need to notify their accessors too.
//Cu::ForeignFunc::Result
//DeleteChild( Cu::FFIServices& );

//! Creates an accessor to the parent of the node stored in the given accessor.
//! \params JSONAccessor accessor
Cu::ForeignFunc::Result
AccessorParent( Cu::FFIServices& );

//! If two parameters: Sets the name of the JSON element of the node of the given accessor.
//! If one parameter: Gets the name of the JSON element.
//! \params JSONAccessor accessor [, String name]
Cu::ForeignFunc::Result
GetSetElementName( Cu::FFIServices& );

//! If two parameters: Sets the attributes of the JSON element of the node of the given accessor.
//! If one parameter: Gets the attributes of the JSON element.
//! The value is formatted as a Copper function object of the form: [ attrs={} children={} ]
//! You do not need to provide a function object of this form. It could be just [attrs={}]
//! and the children won't be lost.
//! This function overwrites instead of replaces.
//! \params JSONAccessor accessor [, FunctionObject value]
Cu::ForeignFunc::Result
GetSetElementAttrs( Cu::FFIServices& );

//! TODO: Remove because this can be done in Copper
Cu::ForeignFunc::Result
ConvertJSONToCopper( Cu::FFIServices& );

//! TODO: Remove because this can be done in Copper
Cu::ForeignFunc::Result
ConvertCopperToJSON( Cu::FFIServices& );

//! Writes the given JSON object to file.
//! \params JSONStorage storage
Cu::ForeignFunc::Result
WriteJSON( Cu::FFIServices& );

} // end namespace json
} // end namespace cubr

#endif
