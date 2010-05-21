#include "libfspp.hpp"

#include "gettext.hpp"
#include "global_options.hpp"
#include "common.hpp"
#include "print.hpp"

bool sort_order_helper(const std::string& lhs, const std::string& rhs,
		       bool lhs_is_dir, bool rhs_is_dir, const std::string& order)
{
  if (order == "by name")
    return (lhs <= rhs);
  else if (order == "directories first") {
    if ((lhs_is_dir && !rhs_is_dir) || (!lhs_is_dir && rhs_is_dir))
      return (lhs_is_dir >= rhs_is_dir);
    else
      return (lhs <= rhs);
  } else {
    print_critical(gettext("Unknown sorting order"), "LIBFS++");
    return false;
  }
}

bool sort_order(const std::string& lhs_name, const std::string& rhs_name,
		const std::string& lhs_path, const std::string& rhs_path,
		bool lhs_is_dir, bool rhs_is_dir, const std::string& order)
{
  if (order == "by name" || order == "directories first") {  // order by string name
    return sort_order_helper(lhs_name, rhs_name, lhs_is_dir, rhs_is_dir, order);
  } else if (order == "by date") { // order by file attributes
    struct stat64 file_lhs, file_rhs;
    if (stat64(lhs_path.c_str(), &file_lhs) == 0 && stat64(rhs_path.c_str(), &file_rhs) == 0) {
      return (file_lhs.st_mtime == file_rhs.st_mtime ? lhs_name <= rhs_name : file_lhs.st_mtime <= file_rhs.st_mtime);
    } else {
      return lhs_name <= rhs_name;
    }
  } else {
    std::cout << "sort order " << order << std::endl;
    print_critical(gettext("Unknown sorting order"), "LIBFS++");
    return false;
  }
}
