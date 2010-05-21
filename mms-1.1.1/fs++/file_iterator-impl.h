#ifndef FS_FILE_ITERATOR_IMPL_H
#define FS_FILE_ITERATOR_IMPL_H

#include <algorithm>

namespace filesystem
	{
		template <class file>
		typename no_order<file>::result_type
		no_order<file>::operator () (typename no_order<file>::first_argument_type a,
		                             typename no_order<file>::second_argument_type b)
		                             const
			{
				return false ;
			}

		template <class file>
		typename default_order<file>::result_type
		default_order<file>::operator () (
				typename default_order<file>::first_argument_type a,
				typename default_order<file>::second_argument_type b) const
			{
				return (a.getName () <= b.getName ()) ;
			}

		template <class file>
		typename default_reverse_order<file>::result_type
		default_reverse_order<file>::operator () (
				typename default_reverse_order<file>::first_argument_type a,
				typename default_reverse_order<file>::second_argument_type b) const
			{
				return (a.getName () >= b.getName ()) ;
			}

		template <class file, class order>
		file_iterator<file, order>::file_iterator (const string& dir)
			{
				chdir (dir) ;
			}

		/// Copy constructor.
		template <class file, class order>
		file_iterator<file, order>::file_iterator (const file_iterator& other)
			: cur_dir (other.cur_dir),
			  file_list (other.file_list),
				parent_dirs (other.parent_dirs)
			{
				if (other.cur_file == other.file_list.end ())
					{
						// This should only be the case, if we are copying the 
						// iterator_end_mark.
						cur_file = file_list.end () ;
					}
				else
					{
						// This should be the normal case.
						jumpTo (other->getName ()) ;
					}
			}

		template <class file, class order>
		file_iterator<file, order>&
		file_iterator<file, order>::operator= (const file_iterator& other)
			{
				if (this == &other)
					return *this ;

				cur_dir = other.cur_dir ;
				file_list = other.file_list ;
				jumpTo (other.cur_file->getName ()) ;
				parent_dirs = other.parent_dirs ;

				return *this ;
			}

		template <class file, class order>
		file_iterator<file, order>&
		file_iterator<file, order>::operator= (const string& dir)
			{
				chdir (dir) ;
				return *this ;
			}

		template <class file, class order>
		const file_iterator<file, order>&
		file_iterator<file, order>::end () const
			{
				return iterator_end_mark ;
			}


		template <class file, class order>
		const string&
		file_iterator<file, order>::currentDirectory () const
			{
				return cur_dir ;
			}


		//
		// here are the iterator specific operators:
		//

		/// Dereference operator (read only).
		template <class file, class order>
		const file&
		file_iterator<file, order>::operator* () const
			{
				if (cur_file == file_list.end ())
					std::cerr << "[w] dereferencing end mark!" << std::endl ;

				return *cur_file ;
			}

		template <class file, class order>
		const file*
		file_iterator<file, order>::operator-> () const
			{
				if (cur_file == file_list.end ())
					std::cerr << "[w] dereferencing end mark!" << std::endl ;

				return cur_file.operator->() ;
			}

		// Prefix increment operator.
		template <class file, class order>
		file_iterator<file, order>&
		file_iterator<file, order>::operator++ ()
			{
				advance () ;

				return *this ;
			}

		// Postfix increment operator.
		template <class file, class order>
		file_iterator<file, order>&
		file_iterator<file, order>::operator++ (int)
			{
				return this->operator++ () ;
			}

		// Prefix decrement operator.
		template <class file, class order>
		file_iterator<file, order>&
		file_iterator<file, order>::operator-- ()
			{
				advance_back () ;

				return *this ;
			}

		// Postfix decrement operator.
		template <class file, class order>
		file_iterator<file, order>&
		file_iterator<file, order>::operator-- (int)
			{
				return this->operator-- () ;
			}

		/// Equality operator.
		template <class file, class order>
		bool
		file_iterator<file, order>::operator== (const file_iterator& other) const
			{
				// This looks quite strange, but it works ;-)
				return
					(
					 	( // this should speed up comparison with iterator_end_mark
							this == &other
						)
						||
						(
							cur_dir == other.cur_dir
						)
					  &&
						(	// compare cur_file iterators
							(
								(      cur_file ==       file_list.end ())
								&&
								(other.cur_file == other.file_list.end ())
							)
							||
							(
								(      cur_file !=       file_list.end ())
								&&
								(other.cur_file != other.file_list.end ())
								&&
								(*cur_file == *(other.cur_file))
							)
						)
						&&
						(	// compare stack of parent_dirs
							(
								parent_dirs.size () == other.parent_dirs.size ()
							)
							&&
							(
								parent_dirs == other.parent_dirs
							)
						)
					) ;

				// One reason is, that file_list.end () is not equal to
				// other.file_list.end ().
			}

		/// Inequality operator.
		template <class file, class order>
		bool
		file_iterator<file, order>::operator!= (const file_iterator& other) const
			{
				return !(*this == other) ;
			}

		// Advances this iterator by one.
		template <class file, class order>
		void
		file_iterator<file, order>::advance (bool desc)
			{
				//std::cerr << "advance(" << desc << ")" << std::endl ;
				if (isDirectory (cur_file->getName ()) && desc)
					{
						this->descend () ;
					}
				else
					{
						if (cur_file != file_list.end ())
							cur_file++ ;

						if (cur_file == file_list.end ())
							this->ascend () ;
					}
			}

		// Advances this iterator back by one.
		template <class file, class order>
		void
		file_iterator<file, order>::advance_back (bool desc)
			{
				//std::cerr << "advance_back(" << desc << ")" << std::endl ;

				if ( (cur_file != file_list.end   ()) &&
				     (cur_file != file_list.begin ()) )
					{
						//std::cerr << "advance_back: cur_file--" << std::endl ;
						cur_file-- ;
					}
				else
					{
						//std::cerr << "advance_back: ascending" << std::endl ;
						this->ascend (false) ;
						return ;
					}

				if (isDirectory (cur_file->getName ()) && desc)
					{
						this->descend () ;

						// set cur_file iterator to last element
						if (file_list.size () > 0)
							{
								//std::cerr << "advance_back: jumping to end of file_list"
								//	<< std::endl ;
								cur_file = find (file_list.begin (), file_list.end (),
									file_list.back ()) ;
							}
						else
							{
								// the cur_file iterator should already be eq to file_list.end()
								//std::cerr << "advance_back: inside empty dir. Hm?" << endl ;
							}
					}
			}

		/** Descends into current directory and selects first file in
		 * subdirectory.
		 *
		 * If the iterator descends into a subdirectory, the name of the
		 * current file (is in fact a directory) is pushed on the parent_dirs
		 * stack.
		 */
		template <class file, class order>
		void
		file_iterator<file, order>::descend ()
			{
				parent_dirs.push (cur_dir) ;
				chdir (cur_file->getName ()) ;
			}

		// Ascends from a subdirectory.
		template <class file, class order>
		void
		file_iterator<file, order>::ascend (bool forward)
			{
				// check, if we are on the top-level directory
				if (parent_dirs.empty ())
					{
						chdir ("") ;	// goto iterator_end_mark
						return ;
					}

				// otherwise ascend
				string parentdir (parent_dirs.top ()) ;
				parent_dirs.pop () ;

				string subdir = cur_dir ;
				chdir (parentdir) ;

				jumpTo (subdir) ;	// advance iterator to original position

				if (forward)
					advance (false) ;	// position iterator to file after subdir
				else
					{
						// in this case, no movement is necessary (as we are just coming up
						// from some subdirectory)
					}
			}

		/// Updates local directory listing.
		template <class file, class order>
		void
		file_iterator<file, order>::chdir (const string& new_dir, bool forward)
			{
				cur_dir = new_dir ;
				directory_listing<file> (file_list, cur_dir) ;
				file_list.sort (order ()) ;
				cur_file = file_list.begin () ;

				// Very strange behaviour: this causes segfault:
				//std::cerr << "new_dir=\"" << new_dir << "\"" << std::endl ;

				if ((cur_dir != "") && (cur_file == file_list.end ()))
					{
						// if we've entered an empty directory, ascend immediately
						this->ascend (forward) ;
					}
			}

		/** Advances current file pointer to specified file.
		 *
		 * If the specified filename could not be found in the file list, the
		 * iterator is advanced behind the last element.
		 */
		template <class file, class order>
		void
		file_iterator<file, order>::jumpTo (const string& filename)
			{
				//std::cerr << "jumpTo: \"" << filename << "\"" << std::endl ;
				cur_file = find (file_list.begin (), file_list.end (),
					file (filename)) ;
			}

		template <class file, class order>
		const file_iterator<file, order>
		file_iterator<file, order>::iterator_end_mark ("") ;

	}	// namespace filesystem

#endif
