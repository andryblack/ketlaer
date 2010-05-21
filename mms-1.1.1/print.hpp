#ifndef PRINT_HPP
#define PRINT_HPP

#include "image.hpp"

#include <list>
#include <string>

class Print
{
public:
  enum PrintType { INFO, DEBUGGING, SCREEN };

  // one liners
  Print(const std::string& text, PrintType type,
	const std::string& module = "");

  // next three functions for printing many lines
  Print(PrintType type, const std::string& module = "");

  void add_line(const std::string& text);
  void print();

protected:
  std::list<std::string> messages;

  std::string module;
  PrintType type;
};

class DialogWaitPrint : public Print
{
public:
  DialogWaitPrint(const std::string& text, int time);

  DialogWaitPrint(int time);

  bool cleaned();

  void print();
private:
  int wait_time;
  bool clean;
};

class DialogStepPrint : public Print
{
public:
  DialogStepPrint(const std::string& text, const Overlay& o);

  DialogStepPrint(const Overlay& o);

  void print_step1();
  void print_step2();
private:
  Overlay overlay;
};

class DebugPrint : public Print
{
public:
  // 0 = show always, 3 = show only if debug is set to ALL
  enum DebugType { CRITICAL = 0, WARNING, INFO, ALL };

  // one liners
  DebugPrint(const std::string& text, PrintType type, 
	     DebugType debug_level, const std::string& module = "");

  // next two functions for printing many lines
  DebugPrint(PrintType type, DebugType debug_level,
	     const std::string& module = "");

  void print();

private:
  DebugType debug_level;
};

// helper functions

static void __attribute__((unused)) print_critical(const std::string& message, const std::string& mode = "");
static void __attribute__((unused)) print_warning(const std::string& message, const std::string& mode = "");
static void __attribute__((unused)) print_info(const std::string& message, const std::string& mode = "");
static void __attribute__((unused)) print_debug(const std::string& message, const std::string& mode = "");

 
static void print_critical(const std::string& message, const std::string& mode)
{
  DebugPrint perror(message, Print::DEBUGGING, DebugPrint::CRITICAL, mode);
}

static void print_warning(const std::string& message, const std::string& mode)
{
  DebugPrint perror(message, Print::DEBUGGING, DebugPrint::WARNING, mode);
}

static void print_info(const std::string& message, const std::string& mode )
{
  DebugPrint perror(message, Print::INFO, DebugPrint::INFO, mode);
}

static void print_debug(const std::string& message, const std::string& mode)
{
  DebugPrint perror(message, Print::INFO, DebugPrint::ALL, mode);
}

#endif
