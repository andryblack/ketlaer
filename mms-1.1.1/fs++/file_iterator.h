#ifndef FS_FILE_ITERATOR_H
#define FS_FILE_ITERATOR_H

#include <fs++/general.h>

#include <string>
#include <iterator>
#include <list>
#include <stack>
#include <iostream>
#include <functional>

#include <fs++/file_t.h>

namespace filesystem
	{
		using std::string ;
		using std::list ;
		using std::stack ;
		using std::pair ;

		/// Binary function object which does not sort at all.
		template <class file = file_t>
		struct no_order : std::binary_function<const file&, const file&, bool>
			{
				typename no_order<file>::result_type
				operator () (typename no_order<file>::first_argument_type a,
				             typename no_order<file>::second_argument_type b) const ;
			} ;

		/// Binary function object which sorts files alphabetically.
		template <class file = file_t>
		struct default_order : std::binary_function<const file&, const file&, bool>
			{
				typename default_order<file>::result_type
				operator () (typename default_order<file>::first_argument_type a,
				             typename default_order<file>::second_argument_type b) const ;
			} ;

		/// Binary function object which sorts files reverse alphabetically.
		template <class file = file_t>
		struct default_reverse_order
			: std::binary_function<const file&, const file&, bool>
			{
				typename default_reverse_order<file>::result_type
				operator () (typename default_reverse_order<file>::first_argument_type a,
				             typename default_reverse_order<file>::second_argument_type b)
				             const ;
			} ;

		template <class file = file_t, class order = no_order<file> >
		class file_iterator
			: public std::iterator <std::input_iterator_tag, void, void, void, void>
			{
				public:

					file_iterator (const string& cur_dir = ".") ;

					/// Copy constructor.
					file_iterator (const file_iterator& other) ;

					file_iterator&
					operator= (const file_iterator& other) ;

					file_iterator&
					operator= (const string& cur_dir) ;

					const file_iterator&
					end () const ;

					const string&
					currentDirectory () const ;


					//
					// here are the iterator specific operators:
					//

					/// Dereference operator (read only).
					const file&
					operator* () const ;

					const file*
					operator-> () const ;

					/// Prefix increment operator.
					file_iterator&
					operator++ () ;

					/// Postfix increment operator.
					file_iterator&
					operator++ (int) ;

					/// Prefix decrement operator.
					file_iterator&
					operator-- () ;

					/// Postfix decrement operator.
					file_iterator&
					operator-- (int) ;

					/// Equality operator.
					bool
					operator== (const file_iterator& other) const ;

					/// Inequality operator.
					bool
					operator!= (const file_iterator& other) const ;

					/** Advances this iterator by one.
					 *
					 * The optional parameter desc indicates, if this call should descend
					 * into a subdirectory (if the current file is a directory).
					 *
					 * This is not wanted in the file_iterator::ascend () method.
					 */
					void
					advance (bool desc = true) ;

					/** Advances this iterator backwards by one.
					 *
					 * The optional parameter desc indicates, if this call should descend
					 * into a subdirectory (if the current file is a directory).
					 *
					 * This is not wanted in the file_iterator::ascend () method.
					 */
					void
					advance_back (bool desc = true) ;

					/** Descends into current directory and selects first file in
					 * subdirectory.
					 *
					 * If the iterator descends into a subdirectory, the name of the
					 * current file (is in fact a directory) is pushed on the parent_dirs
					 * stack.
					 */
					void
					descend () ;

					/** Ascends from a subdirectory.
					 *
					 * If the iterator reaches the end of the list of files in the current
					 * directory (is behind the last element in the file_list) and the
					 * parent_dirs stack is not empty, this method ascends from the
					 * subdirectory.
					 *
					 * \param forward If true, this iterator is advanced forward after
					 * ascending, otherwise backward.
					 */
					void
					ascend (bool forward = true) ;


				protected:

					/// Updates local directory listing.
					void
					chdir (const string& new_dir, bool forward = true) ;

					/** Advances current file pointer to specified file.
					 *
					 * If the specified filename could not be found in the file list, the
					 * iterator is advanced behind the last element.
					 */
					void
					jumpTo (const string& filename) ;

					static const file_iterator iterator_end_mark ;

				private:

					/// Name (full path) of current directory.
					string cur_dir ;

					/// List of files in current directory.
					list <file> file_list ;

					/// Iterator in list of files (in current directory).
					typename list <file>::const_iterator cur_file ;

					/** Stack of parent directory names.
					 *
					 * See also \see file_iterator::descend ()
					 */
					stack <string> parent_dirs ;
			} ;

	}	// namespace filesystem

// Include the implementation of the file_iterator template class:
#include <fs++/file_iterator-impl.h>

#endif
