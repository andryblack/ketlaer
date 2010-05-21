#include <fs++/file_monitor.h>

namespace filesystem
	{
		file_monitor::file_monitor (const string& n)
			: name (n), prev_size (getFileSize (name)),
			  in (name.c_str()), opened (false)
			{
				if (in)
					opened = true ;
			}

		const string&
		file_monitor::getName () const
			{
				return name ;
			}

		const string&
		file_monitor::getLine ()
			{
				if (! this->isReady ())
					return (line = "") ;

				filesystem::getLine (in, line) ;

				return line ;
			}

		bool
		file_monitor::isReady ()
			{
				off_t cur_pos = std::streamoff(in.tellg());

				//std::cerr << "pos " << cur_pos << "/" << prev_size << std::endl ;

				if (opened && in && (cur_pos != static_cast<off_t>(-1)) && (cur_pos < prev_size))
					return true ;

				off_t  cur_size = getFileSize (name) ;
				if (cur_size > prev_size)
					{
						// monitored file has grown
						this->reopen () ;
						prev_size = cur_size ;
					}
				else if (cur_size < prev_size)
					{
						// file has been truncated
						prev_size = 0 ;	// don't seek to previous location
						this->reopen () ;
						prev_size = cur_size ;
					}
				else
					{
						// file is unchanged
						opened = false ;
					}

				return opened ;
			}

		void
		file_monitor::seekToEnd ()
			{
				in.seekg (0, std::ios::end) ;
			}

		void
		file_monitor::reopen ()
			{
				in.close () ;
				in.clear () ;

				in.open (name.c_str ()) ;
				in.seekg (prev_size) ;

				if (in)
					opened = true ;
				else
					opened = false ;
			}

	}	// namespace filesystem
