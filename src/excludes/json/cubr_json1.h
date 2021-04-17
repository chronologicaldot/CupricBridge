// (C) 2021 Nicolaus Anderson
/*
	Requires IrrExt for irrTree and irrJSON.

	DEPRECATED!
	This version creates a list of accessors within the Storage and requires accessing/controlling
	them via IDs.
	In Copper, the code is relatively cumbersome.
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

/*
	Rather than having two Cu::Object classes (one for storage and one for accessing nodes),
	it's better they are combined so that when the storage is deleted, the accessors die too.

	TODO:
	Loading to Copper will require Cu::AccessHelper. Moreover, you will need to convert
	numeric strings to numbers and distinguish between an integer and a number with a decimal.
	You can do this with util::String::toFloat() or toDouble().
*/
class Storage
	: public Cu::Object
{
	struct Accessor {
		irrTreeNode*  node;
		Cu::UInteger  id;
	};

	irr::io::path  filePath;
	irr::io::IFileSystem*  fileSystem;
	irrTreeNode*  rootNode;
	irrJSON  json;
	irr::core::array<Accessor>  accessors;
	Cu::UInteger IDs;

public:

	Storage( irr::io::IFileSystem* );

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
	// Returns true if an accessor was created. Will return false if the JSON tree doesn't exist.
	bool createAccessor();

	//! Delete an accessor
	bool deleteAccessor( Cu::UInteger accessorId );

	//! Returns the ID of the last created accessor
	Cu::UInteger getLastAccessorId();

	//! Returns the number of accessors.
	Cu::UInteger getAccessorCount();

	//! Stores in the given string object the node name pointed to by the accessor with
	//! the given index
	bool getElementName( Cu::UInteger accessorId, util::String& );

	//! Stores in storage the value and attributes of the node pointed to by the accessor
	//! with the given index
	bool getElementValue( Cu::UInteger accessorId, Cu::Object*& storage );

	//! Move accessor
	bool moveAccessorToSibling( Cu::Integer accessorId, bool moveDown=true );
	bool moveAccessorToChild( Cu::UInteger accessorId, Cu::UInteger childIndex );
	bool moveAccessorToParent( Cu::UInteger accessorId );

	//! Convert the entire tree into the corresponding Copper variable/object structure.
	//! The "storage" parameter MUST be initialized. It acts as the root tree node.
	//! Note that multiple nodes with identical names will be merged into a single node.
	bool convertToCopper( Cu::Function& storage );

	//! Convert from Copper, creating a JSON tree from the given Copper object.
	bool convertFromCopper( Cu::Function& source );

private:
	bool noRootOrBadAccessorIndex( Cu::Integer );
	bool getAccessor( Cu::UInteger id, Accessor*& );
};

//-----------------------------------

//! Load JSON from a file
Cu::ForeignFunc::Result
OpenAndParse( Cu::FFIServices& );

//! Sets the file path of the storage. Useful when saving to a new file.
Cu::ForeignFunc::Result
SetFilePath( Cu::FFIServices& );

//! Creates an accessor and returns its index
Cu::ForeignFunc::Result
CreateAccessor( Cu::FFIServices& );

Cu::ForeignFunc::Result
DeleteAccessor( Cu::FFIServices& );

Cu::ForeignFunc::Result
GetAccessorCount( Cu::FFIServices& );

Cu::ForeignFunc::Result
GetElementName( Cu::FFIServices& );

Cu::ForeignFunc::Result
GetElementValue( Cu::FFIServices& );

Cu::ForeignFunc::Result
AccessorToSibling( Cu::FFIServices& );

Cu::ForeignFunc::Result
AccessorToChild( Cu::FFIServices& );

Cu::ForeignFunc::Result
AccessorToParent( Cu::FFIServices& );

Cu::ForeignFunc::Result
ConvertJSONToCopper( Cu::FFIServices& );

Cu::ForeignFunc::Result
ConvertCopperToJSON( Cu::FFIServices& );

Cu::ForeignFunc::Result
WriteJSON( Cu::FFIServices& );

} // end namespace json
} // end namespace cubr

#endif
