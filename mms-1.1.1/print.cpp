#include "print.hpp"

#include "common-output.hpp"
#include "config.hpp"

#include "boost.hpp"

#include <iostream>

Print::Print(const std::string& text, PrintType t, const std::string& mod)
  : module(mod), type(t)
{
  add_line(text);
  print();
}

Print::Print(PrintType t, const std::string& mod)
  : module(mod), type(t)
{}

void Print::add_line(const std::string& text)
{
  if (module != "")
    messages.push_back(module + ": " + text);
  else
    messages.push_back(text);
}

void Print::print()
{
  if (type == SCREEN)
    print_dialog(messages);
  else {
    if (S_Config::get_instance()->p_debug_level() < 2)
      return;

    foreach (std::string& message, messages)
      if (type == INFO)
	std::cout << message << std::endl;
      else if (type == DEBUGGING)
	std::cerr << message << std::endl;
  }
}

DialogWaitPrint::DialogWaitPrint(const std::string& text, int time)
  : Print(SCREEN), wait_time(time), clean(false)
{
  add_line(text);
  print();
}

DialogWaitPrint::DialogWaitPrint(int time)
  : Print(SCREEN), wait_time(time), clean(false)
{}

bool DialogWaitPrint::cleaned()
{
  return clean;
}

void DialogWaitPrint::print()
{
  clean = false;
  print_dialog_wait(messages, wait_time);
  clean = true;
}

DialogStepPrint::DialogStepPrint(const std::string& text, const Overlay& o)
  : Print(SCREEN), overlay(o)
{
  add_line(text);
  print_step1();
}

DialogStepPrint::DialogStepPrint(const Overlay& o)
  : Print(SCREEN), overlay(o)
{}

void DialogStepPrint::print_step1()
{
  print_dialog_step1(messages, overlay);
}

void DialogStepPrint::print_step2()
{
  print_dialog_step2(overlay);
}

DebugPrint::DebugPrint(const std::string& text, PrintType t, DebugType d, const std::string& mod)
  : Print(t, mod), debug_level(d)
{
  // optimization
  if (S_Config::get_instance()->p_debug_level() < debug_level)
    return;

  add_line(text);
  print();
}

DebugPrint::DebugPrint(PrintType t, DebugType d, const std::string& mod)
  : Print(t, mod), debug_level(d)
{}

void DebugPrint::print()
{
  foreach (std::string& message, messages)
    if (S_Config::get_instance()->p_debug_level() >= debug_level)
      std::cerr << message << std::endl;
}


