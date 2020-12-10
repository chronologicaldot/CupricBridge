// Copyright 2018 Nicolaus Anderson

#include "cubr_mfrunner.h"
#include <FileInStream.h>

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	#include <io.h> // for _access
	#define  file_access(x,y)  _access(x,y)
#else
	// Posix or Mac
	#include <unistd.h>
	#define  file_access(x,y)  access(x,y)
#endif
#ifdef F_OK
	#define  FLAG_file_access_F_OK  F_OK
#else
	#define  FLAG_file_access_F_OK  0
#endif

namespace cubr {

MultifileRunner::MultifileRunner( Cu::Engine& e )
	: rootPathSet(false)
	, rootFilePath()
	, projectFilePath()
	, importedFilePaths()
	, cuengine(e)
	, lastLine(1)
	, lastColumn(0)
	, importOn(false)
{
	Cu::addForeignMethodInstance<MultifileRunner>(cuengine, util::String("import"), this, &MultifileRunner::import);
	Cu::addForeignMethodInstance<MultifileRunner>(cuengine, util::String("require"), this, &MultifileRunner::require);
}

MultifileRunner::~MultifileRunner() {

}

void
MultifileRunner::setRootDirectoryPath( irr::io::path  p ) {
	rootFilePath = p;
	rootPathSet = p.size() > 0;

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	char  dirChar = '\\';
#else
	char  dirChar = '/';
#endif

	if ( rootPathSet ) {
		if ( rootFilePath[ rootFilePath.size() - 1 ] != dirChar ) {
			rootFilePath.append(dirChar);
		}
	}

	// TODO: Check for a valid path?
}

bool
MultifileRunner::run( irr::io::path  startFilePath ) {
	errorFlags = ERROR_NONE;
	irr::io::path  currentFilePath = rootFilePath + startFilePath;

	importOn = true;
	bool okResult = runFile(currentFilePath);
	importOn = false;

	if ( ! okResult )
		return false;

	PathList::ConstIter  pathsIter = importedFilePaths.constStart();
	if ( pathsIter.has() )
	do {
		currentFilePath = pathsIter.getItem();
		okResult = runFile(currentFilePath); // rootFilePath was prepended in filterPathFromArgs()
		if ( !okResult )
			return false;
	} while ( pathsIter.next() );

	return true;
}

MultifileRunner::ErrorFlags
MultifileRunner::getErrorFlags() {
	return errorFlags;
}

Cu::UInteger
MultifileRunner::getLastLine() {
	return lastLine;
}

Cu::UInteger
MultifileRunner::getLastColumn() {
	return lastColumn;
}

Cu::ForeignFunc::Result
MultifileRunner::import( Cu::FFIServices&  ffi ) {
	if ( ! importOn ) {
		ffi.printCustomWarningCode(MessageCode::ImportUsageInWrongContext);
		return Cu::ForeignFunc::NONFATAL;
	}

	irr::io::path  p = filterPathFromArgs(ffi, 0);

	if ( p.size() > 0 ) {
		importedFilePaths.push_back(p);
	} else {
		ffi.printCustomErrorCode(MessageCode::InvalidPathArg);
		errorFlags = ERROR_IMPORT_FAILED;
		return Cu::ForeignFunc::FATAL;
	}

	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
MultifileRunner::require( Cu::FFIServices&  ffi ) {
	if ( importOn ) {
		ffi.printCustomWarningCode(MessageCode::RequireUsageInWrongContext);
		return Cu::ForeignFunc::NONFATAL;
	}

	irr::io::path  p = filterPathFromArgs(ffi, 0);
	if ( p.size() == 0 ) {
		ffi.printCustomWarningCode(MessageCode::InvalidPathArg);
		ffi.setNewResult(new Cu::BoolObject(true));
		return Cu::ForeignFunc::FINISHED;
	}

	PathList::ConstIter  pathsIter = importedFilePaths.constStart();
	if ( pathsIter.has() )
	do {
		if ( (*pathsIter) == p ) {
			// Desired path found
			ffi.setNewResult(new Cu::BoolObject(true));
			return Cu::ForeignFunc::FINISHED;
		}
	} while ( pathsIter.next() );

	// Failed to find the file, so throw an error.
	errorFlags = ERROR_REQUIRE_FAILED;
	return Cu::ForeignFunc::FATAL;
}

bool
MultifileRunner::checkFileExists( irr::io::path  filePath ) {
	return file_access(filePath.c_str(), FLAG_file_access_F_OK) != -1;
}

bool
MultifileRunner::runFile( irr::io::path  filePath ) {

	if ( ! checkFileExists(filePath) ) {
		errorFlags = ERROR_FILE_NONEXISTENT;
		return false;
	}
	Cu::FileInStream  instream( filePath.c_str() );
	Cu::EngineResult::Value  result;
	do {
		result = cuengine.run(instream);
	} while ( result == Cu::EngineResult::Ok );

	lastLine = instream.getLine();
	lastColumn = instream.getColumn();

	if ( result == Cu::EngineResult::Error ) {
		errorFlags = ERROR_ENGINE_ERROR;
		return false;
	}

	return true;
}

irr::io::path
MultifileRunner::filterPathFromArgs( Cu::FFIServices&  ffi, Cu::UInteger  startArg ) {
	if ( !ffi.demandMinArgCount(startArg+1)
		|| !ffi.demandArgType(startArg, Cu::ObjectType::String)
	) {
		return irr::io::path();
	}

	Cu::StringObject&  pathArg = (Cu::StringObject&)ffi.arg(startArg);
	irr::io::path  filePath = irr::io::path( pathArg.getString().c_str() );
	filePath = rootFilePath + filePath;

	// TODO Filter for true path string? (Remove invalid characters?)

	// Check if the filename is a directory an a file
		// If a directory, set the path to empty
	//if ( ! fileSystem.existFile(filePath) ) { // Unfortunately, Irrlicht also checks its archive
	if ( !checkFileExists(filePath) ) {
		ffi.printCustomWarningCode(MessageCode::FileNonExistent);
		return irr::io::path();
	}
		// If a valid file, do nothing

	return filePath;
}

}

#undef  FLAG_file_access_F_OK
#undef  file_access
