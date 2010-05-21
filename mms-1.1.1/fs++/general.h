#ifndef FS_GENERAL_H
#define FS_GENERAL_H

#	ifdef CXX_NAMESPACES_WORKAROUND

		/* This quick'n'dirty workaround is necessary for some older versions of
		 * libstdc++ which seem to handle namespaces in the wron way...
		 *
		 * Comments and suggestions on this are *very* welcome ;-)
		 *
		 * Tobias Jahn <tjahn@users.sourceforge.net>
		 */
#		ifndef CXX_NAMESPACES_WORKAROUND_DONE
#		define CXX_NAMESPACES_WORKAROUND_DONE
#			include <stl_config.h>
#			define __STL_USE_NAMESPACES
#			undef  __STL_NO_NAMESPACES
#		endif

#	endif

#include <string>
#include <list>
#include <iostream>

namespace filesystem
	{
		using std::string ;
		using std::list ;

		///	Convert path to fullqualified one
		string
		FExpand (const string& path) ;

		///	Determines if the specified file is a directory.
		bool
		isDirectory (const string& filename) ;

		/* Determines if the specified file is a normal file */
		bool
		isFile  (const string& filename) ;

		/// Tries to determine the current working directory.
		const string
		getWorkingDirectory () ;

		/** Determines the size of the specified file (or -1 if something
		 * went wrong).
		 */
		off_t
		getFileSize (const string& filename) ;

		/** Returns a list of files in specified directory.
		 *
		 * Note: The specified directory name may be modified by this
		 * function. This is the case, if it is not fully qualified.
		 */
		template <class file>
		list <file>
		directory_listing (string& dirname) ;

		/// Reads a line from specified istream and stores it in the passed buffer.
		void
		getLine (std::istream& in, string& buf) ;

	}	// namespace filesystem

// Include the implementation of the general template functions:
#include <fs++/general-impl.h>

#endif
