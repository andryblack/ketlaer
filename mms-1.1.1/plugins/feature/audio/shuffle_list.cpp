#include "shuffle_list.hpp"

#include "common.hpp"
#include "rand.hpp"
#include "print.hpp"
#include "gettext.hpp"

#include "boost.hpp"

#include <cassert>

using std::vector;

ShuffleList::ShuffleList(const vector<Simplefile>& files)
  : shuffle_list(files.begin(), files.end()), shuffle_pos(0)
{}

const Simplefile ShuffleList::prev_track()
{
  assert(shuffle_list.size() > 0);

  if (shuffle_pos == 0)
    shuffle_pos = shuffle_list.size()-1;
  else
    --shuffle_pos;
  return vector_lookup(shuffle_list, shuffle_pos);
}

const Simplefile ShuffleList::next_track()
{
  assert(shuffle_list.size() > 0);

  if (shuffle_pos == shuffle_list.size()-1)
    shuffle_pos = 0;
  else
    ++shuffle_pos;
  return vector_lookup(shuffle_list, shuffle_pos);
}

const Simplefile ShuffleList::peek_next_track()
{
  assert(shuffle_list.size() > 0);

  int sp = shuffle_pos;

  if (sp == shuffle_list.size()-1)
    sp = 0;
  else
    ++sp;
  return vector_lookup(shuffle_list, sp);
}

void ShuffleList::track_played(const Simplefile& e)
{
  int r = 0;

  foreach (const Simplefile& file, shuffle_list)
    if (file == e)
      break;
    else
      ++r;

  shuffle_pos = r;
}

void ShuffleList::add_track(const Simplefile& e)
{
  if (shuffle_list.size() == 0) {
    shuffle_pos = 0;
    shuffle_list.push_back(e);
  } else {
    int r = S_Rand::get_instance()->number(shuffle_list.size()-shuffle_pos+1);
    shuffle_list.insert(shuffle_list.begin() + shuffle_pos + r, e);
  }
}

void ShuffleList::simple_add_track(const Simplefile& e)
{
  if (shuffle_list.size() == 0)
    shuffle_pos = 0;

  shuffle_list.push_back(e);
}

void ShuffleList::remove_track(const Simplefile& e)
{
  if (shuffle_list.size() == 0) {
    print_warning(dgettext("mms-audio", "List is empty, removal failed"), "SHUFFLELIST");
    return;
  }

  int r = 0;

  vector<Simplefile>::iterator iter, iter_end;
  for (iter = shuffle_list.begin(), iter_end = shuffle_list.end();
       iter != iter_end; ++iter, ++r)
    if (*iter == e)
      break;

  if (r < shuffle_pos)
    --shuffle_pos;

  shuffle_list.erase(iter);
}
