#ifndef LIBFSPP_HPP
#define LIBFSPP_HPP

// libfs++
#include <fs++/file_iterator.h>
#include <fs++/file_t.h>
#include "simplefile.hpp"

#include "config.h"

#include "gettext.hpp"

using filesystem::file_iterator ;
using filesystem::file_t ;
using filesystem::isDirectory ;

// requires lhs and rhs to be in lowercase
bool sort_order_helper(const std::string& lhs, const std::string& rhs,
		bool lhs_is_dir, bool rhs_is_dir, const std::string& order);

// implementation with Simplefile
bool sort_order(const std::string& lhs_name, const std::string& rhs_name,
		const std::string& lhs_path, const std::string& rhs_path,
		bool lhs_is_dir, bool rhs_is_dir, const std::string& order);

// ugly, use a class
static std::string libfs_folders_first = "directories first";



static void libfs_set_folders_first(std::string folders_first) __attribute__((unused)); /* trick to prevent warnings whenever this file is included somewhere */ 
static void libfs_set_folders_first(std::string folders_first)
{
  libfs_folders_first = folders_first;
}

// decides the order of the files returned from libfs++ when traversing a directory
struct default_order : std::binary_function <const file_t&, const file_t&, bool>
{
  result_type operator() (first_argument_type a, second_argument_type b) const
  {
    return sort_order_helper(a.getName(), b.getName(), isDirectory(a.getName()),
			     isDirectory(b.getName()), libfs_folders_first);
  }
};

#endif
