#ifndef FS_FILE_T_H
#define FS_FILE_T_H

#include <string>
#include <ostream>

namespace filesystem
	{
		using std::string ;

		class file_t
			{
				public:

					file_t (const string& name) ;

					const string&
					getName () const ;

					bool
					operator== (const file_t& other) const ;

				protected:

					string name ;
			} ;

		std::ostream& operator<< (std::ostream& out, const file_t& f) ;

	}	// namespace filesystem

#endif
