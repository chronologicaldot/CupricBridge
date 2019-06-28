// Copyright 2018 Nicolaus Anderson

#ifndef _CUBR_MULTIFILE_RUNNER_
#define _CUBR_MULTIFILE_RUNNER_

#include <path.h>
#include <Copper.h>

namespace cubr {

// The starting code value of the MultifileRunner when logging values. Change how you wish.
#define MULTIFILE_RUNNER_MESSAGE_CODES_START  100

//! Multi-file Runner
/*
	This class is meant to simplify the process of loading and running multiple Copper files.
	The class takes a "project" file name, attempts to load it, and runs it contents along
	with an added foreign function called "import".
	Once the main project file has been run, the foreign function "import" is disabled,
	and the file names collected with the "import" command are run.
	It is possible to set a root folder in which all files (project and imports) are to be found.
*/
class MultifileRunner {
public:
	// Logging codes
	struct MessageCode {
	enum Value {
		FileNonExistent = MULTIFILE_RUNNER_MESSAGE_CODES_START,
		ImportUsageInWrongContext, // Cannot use "import" in the active context
		RequireUsageInWrongContext, // Cannot use "require" in the active context
		InvalidPathArg, // Bad argument given/determined for the path
	};};

	// Error flags enumeration
	enum ErrorFlags {
		ERROR_NONE = 0,
		ERROR_FILE_NONEXISTENT = 0x01,
		ERROR_ENGINE_ERROR = 0x02, // Could be caused by a specific file
		ERROR_IMPORT_FAILED = 0x04,
		ERROR_REQUIRE_FAILED = 0x08,
	};

private:
	typedef  util::List<irr::io::path>  PathList;

	bool rootPathSet;
	irr::io::path  rootFilePath;
	irr::io::path  projectFilePath;
	PathList  importedFilePaths;

	Cu::Engine&  cuengine;
	ErrorFlags  errorFlags;

	Cu::UInteger  lastLine;
	Cu::UInteger  lastColumn;
	bool  importOn;

public:

	//! cstor
	MultifileRunner( Cu::Engine& );

	//! dstor
	~MultifileRunner();

	//! Set Root Directory Path
	/*
		Sets the path in which all other files are to be found.
		This path will be searched whenever import() is used.
	*/
	void setRootDirectoryPath( irr::io::path );

	//! Run
	/*
		Loads the given file and runs it, compiling a list of files called using "import()".
		Once the given file has completed, each successive file called with "import" is run.
		\return - true if everything was successful, false if failure.
		Errors are reported via error flags obtainable through getErrorFlags().
	*/
	bool run( irr::io::path );

	//! Get the Error Flags
	/*
		These flags are set based on the kinds of errors that occured during the run.
	*/
	ErrorFlags  getErrorFlags();

	//! Get the last run line
	Cu::UInteger  getLastLine();

	//! Get the last run column
	Cu::UInteger  getLastColumn();

	//! Import
	/*
		Adds a file to the list of files to be run once the main project file has been run.
	*/
	Cu::ForeignFunc::Result  import( Cu::FFIServices& );

	//! Require
	/*
		Added as the foreign function "require".
		This may be called within Copper files to assert that a particular file has been run first.
		However, this does not guarantee the integrity of the variables created in the "required"
		file since these may have been changed by any file running between the required file and
		the current one.
	*/
	Cu::ForeignFunc::Result  require( Cu::FFIServices& );


protected:
	bool checkFileExists( irr::io::path );
	bool runFile( irr::io::path );
	irr::io::path  filterPathFromArgs( Cu::FFIServices&, Cu::UInteger );
};

}

#endif
