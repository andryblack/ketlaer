#include "bench.hpp"

#include "print.hpp"

#include <iostream>
#include <sstream>

void Bench::print(const std::string& type)
{
  std::ostringstream out;
  out << type << ": ";

  if ((timeval2.tv_sec - timeval1.tv_sec) >= 1 && (timeval2.tv_usec - timeval1.tv_usec) < 0)
    out << (timeval2.tv_sec - timeval1.tv_sec)*1000 + (timeval2.tv_usec - timeval1.tv_usec)/1000;
  else
    out << (timeval2.tv_usec - timeval1.tv_usec)/1000;

  out << " ms";
  Print print(out.str(), Print::INFO);
}

