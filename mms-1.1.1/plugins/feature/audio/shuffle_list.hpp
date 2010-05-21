#ifndef SHUFFLE_LIST_HPP
#define SHUFFLE_LIST_HPP

#include "simplefile.hpp"

#include <vector>

// random shuffle
#include <algorithm>

class ShuffleList
{
 public:
  const Simplefile prev_track();
  const Simplefile next_track();
  const Simplefile peek_next_track();

  void track_played(const Simplefile& e);

  // just adds to end
  void simple_add_track(const Simplefile& e);

  void add_track(const Simplefile& e);
  void remove_track(const Simplefile& e);
  void clear() { shuffle_list.clear(); }
  int size() { return shuffle_list.size(); }
  void randomize() { std::random_shuffle(shuffle_list.begin(), shuffle_list.end()); }

  ShuffleList(const std::vector<Simplefile>& list);
  ShuffleList() : shuffle_pos(0) {}

  int shuffle_pos;

 private:
  std::vector<Simplefile> shuffle_list;
};

#endif
