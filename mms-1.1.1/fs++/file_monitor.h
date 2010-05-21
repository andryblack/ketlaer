#ifndef FS_FILE_MONITOR_H
#define FS_FILE_MONITOR_H

#include <fs++/general.h>
#include <string>
#include <fstream>
#include <unistd.h>


namespace filesystem
	{
		using std::string ;

		/** Reads a text file line by line and watches for lines appended to the
		 * file.
		 *
		 * This is something like "tail -f".
		 */
		class file_monitor
			{
				public:

					file_monitor (const string& name) ;

					/// Returns name of monitored file.
					const string&
					getName () const ;

					/** Returns next available line.
					 *
					 * If the end of file has been reached in the previous call to
					 * getLine(), an empty string is returned.
					 *
					 * To distinguish an empty line from the eof-case, use isReady().
					 */
					const string&
					getLine () ;

					/// Checks if unread lines are available.
					bool
					isReady () ;

					/// Skip current content of file and monitors only new content.
					void
					seekToEnd () ;

				protected:

					/** Reopens the file and seeks to previous position (if file has not
					 * been truncated).
					 */
					void
					reopen () ;

				private:

					const string name ;
					string line ;

					off_t prev_size ;
					std::ifstream in ;
					bool opened ;
			} ;

	}	// namespace filesystem

#endif
