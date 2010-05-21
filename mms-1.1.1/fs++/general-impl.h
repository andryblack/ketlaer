#ifndef FS_GENERAL_IMPL_H
#define FS_GENERAL_IMPL_H

#include <dirent.h>

namespace filesystem
	{
		template <class file>
		void
		directory_listing (list<file>& l, string& dirname)
			{
				l.clear () ;

				// this is used for the iterator_end_mark
				if (dirname == "")
					return ;

				// we want a nice (and full qualified) pathname
				dirname == FExpand (dirname);

				DIR* dirHandle = opendir (dirname.c_str ()) ;

				if (!dirHandle)
					return ;

				struct dirent* entry ;

				while ((entry = readdir (dirHandle)))
					{
						string entry_name (entry->d_name) ;

						if ((entry_name != ".") && (entry_name != ".."))
							l.push_back (file (dirname + "/" + entry_name)) ;
					}

				closedir (dirHandle) ;
			}

		template <class file>
		list<file>
		directory_listing (string& dirname)
			{
				list<file> l ;

				directory_listing (l, dirname) ;

				return l ;
			}

	}	// namespace filesystem

#endif
