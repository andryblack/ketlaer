// test version - 2004/06/03
// implements multi options_files from 2003-12-16
// fixes for gcc-3.3
// remove MMS specific stuffs
// functions to read VECTOR_NUM as strings

// This file is part of Configuration Files Definition Tool

// Macros to totaly define the class or subclass, either header or functions
// from the definition file

#undef FULL_CLASSNAME
#undef PARAMETER_NONE
#undef PARAMETER_STR
#undef PARAMETER_BOOL
#undef PARAMETER_NUM
#undef PARAMETER_LIST
#undef PARAMETER_SECTION
#undef PARAMETER_PAIR
#undef PUBLIC_PARM
#undef PRIVATE_PARM
#undef VECTOR_BOOL
#undef VECTOR_NUM
#undef VECTOR_STR
#undef VECTOR_MENU


#ifdef PARENT_CLASS
#define FULL_CLASSNAME(class_name) PARENT_CLASS::class_name
#else
#define FULL_CLASSNAME(class_name) class_name
#endif

#if !defined PARAM_CLASS

#undef PARAM_BASE
#undef CONFIG_INSTANCE
#undef CONFIG_DEFAULT
#undef CONFIG_DEFAULT_DONE
#undef CONFIG_PARSER
#undef CONFIG_PARSER_DONE
#undef CONFIG_WRITER
#undef CONFIG_WRITER_DONE
#undef WRITER_PROTO
#undef CONFIG_CONSTRUCTOR
#undef CONFIG_DESTRUCTOR
#undef CONFIG_CONSTRUCTOR_DONE
#undef CONFIG_DESTRUCTOR_DONE

/*****************************************
        Macros to declare the class
 *****************************************/

#ifdef BASE_PACK_CLASS
#define PARAM_CLASS(class_name) class class_name : public BASE_PACK_CLASS
#else
#define PARAM_CLASS(class_name) class class_name
#endif

// MACRO to declare Base functions, constructor & destructor

#ifdef USE_WRITER
#define WRITER_PROTO(class_name) void save_configuration_file();
#else
#define WRITER_PROTO(class_name)
#endif

#ifdef PARENT_CLASS

#define PARAM_BASE(class_name) \
private: \
  void set_default_configuration(); \
  bool is_true (std::string value) { return ((value == "y") || \
               (value == "yes") || (value == "on") || \
               (value == "true") || (value == "1")); } \
  int lookup(const std::vector<std::string>& v,std::string s) const { int p=0; \
    foreach (const std::string& val, v) \
      if (val==s) \
	return p; \
      else \
	++p; \
    return 0; \
  } \
public: \
  static pthread_mutex_t singleton_mutex; \
  class_name(); \
  ~class_name(); \
  void parse_configuration_file(); \
  WRITER_PROTO(class_name)

#else /* There is no parent class */

#define PARAM_BASE(class_name) \
private: \
  void set_default_configuration(); \
  bool is_true (std::string value) { return ((value == "y") || \
               (value == "yes") || (value == "on") || \
               (value == "true") || (value == "1")); } \
public: \
  static pthread_mutex_t singleton_mutex; \
  class_name(); \
  ~class_name(); \
  void parse_configuration_file(); \
  WRITER_PROTO(class_name)

#define PARAM_BASE_PLUGIN(class_name) \
private: \
  void set_default_configuration(); \
  bool is_true (std::string value) { return ((value == "y") || \
               (value == "yes") || (value == "on") || \
               (value == "true") || (value == "1")); } \
public: \
  static pthread_mutex_t singleton_mutex; \
  class_name(); \
  ~class_name(); \
  void parse_configuration_file(const std::string& homedir); \
  WRITER_PROTO(class_name)

#endif

// MACROs to define constructor, destructor, instanciator

#ifdef PARENT_CLASS

#ifdef BASE_CLASS
#define CONFIG_INSTANCE(class_name)

#define CONFIG_CONSTRUCTOR(class_name) PARENT_CLASS::class_name::class_name() { EXTRA_PRE_CONSTRUCTOR set_default_configuration();
#define CONFIG_DESTRUCTOR(class_name)  PARENT_CLASS::class_name::~ class_name() { EXTRA_PRE_DESTRUCTOR

#else

#define CONFIG_INSTANCE(class_name) \
  PARENT_CLASS::class_name::~ class_name() \
  PARENT_CLASS::class_name::class_name()

#endif

#else

#define CONFIG_INSTANCE(class_name) \
  class_name::~ class_name() { EXTRA_PRE_DESTRUCTOR EXTRA_POST_DESTRUCTOR } \
\
  class_name::class_name() { EXTRA_PRE_CONSTRUCTOR set_default_configuration();  EXTRA_POST_CONSTRUCTOR }

#endif

// MACROs to define default_init

#define CONFIG_DEFAULT(class_name) \
  void FULL_CLASSNAME(class_name)::set_default_configuration() {

#define CONFIG_DEFAULT_DONE }

// MACROS to define reader/parser

#ifdef USE_SECTION

// Special line in case we parse a config file with section
// Does not interpret it as section, only extract section name

#define SECTION_EXCEPTION \
    if (str[p0]=='[') { \
      p0=str.find_first_not_of("\t [",p0); \
      std::string::size_type p1 = str.find_last_not_of("\t ]"); \
      param = "[]"; \
      value = str.substr(p0,++p1-p0); \
   } else {

#define END_SECTION_EXCEPTION }

#else

#define SECTION_EXCEPTION
#define END_SECTION_EXCEPTION

#endif

#define CONFIG_PARSER_TOP(class_name, class_name_string) \
pthread_mutex_t FULL_CLASSNAME(class_name)::singleton_mutex = PTHREAD_MUTEX_INITIALIZER; \
void FULL_CLASSNAME(class_name)::parse_configuration_file() \
{ \
  std::ostringstream dev_null; \
  std::string config_location = file_tools::FExpand(mms_config) + "/" + class_name_string; \
  struct stat64 file_stat; \
  if (stat64((file_tools::FExpand(homedir) + "/" + class_name_string).c_str(), &file_stat) == 0) \
    config_location = file_tools::FExpand(homedir) + "/" + class_name_string;	\
  Print print(gettext("Opening configuration file ") + config_location , \
	      Print::INFO, "Config");

#define CONFIG_PARSER_PLUGIN_TOP(class_name, class_name_string) \
pthread_mutex_t FULL_CLASSNAME(class_name)::singleton_mutex = PTHREAD_MUTEX_INITIALIZER; \
void FULL_CLASSNAME(class_name)::parse_configuration_file(const std::string& homedir) \
{ \
  std::ostringstream dev_null; \
  std::string config_location = file_tools::FExpand(mms_config) + "/" + class_name_string; \
  struct stat64 file_stat; \
  if (stat64((file_tools::FExpand(homedir) + "/" + class_name_string).c_str(), &file_stat) == 0) \
    config_location = file_tools::FExpand(homedir) + "/" + class_name_string;	\
  Print print(gettext("Opening configuration file ") + config_location , \
	      Print::INFO, "Config");

#define CONFIG_PARSER(class_name) \
  std::ifstream In(config_location.c_str()); \
  std::string str; \
  bool version_correct = false; \
\
  if(!In) \
    Print perror(gettext("Config file not found ") + config_location, \
		 Print::DEBUGGING, "CONFIG"); \
  else { \
  for( int line=1;getline(In, str);line++) { \
    std::string param, value; \
\
    std::string::size_type p0 = str.find_first_not_of("\t "); \
    if ((p0 > str.size()) || (str[p0] == '#')) continue; \
\
    SECTION_EXCEPTION \
\
    std::string::size_type p = str.find('='); \
\
    if (!p) { \
      Print perror(gettext("Error while reading line ") + \
		   conv::itos(line) + gettext("missing '='"), \
		   Print::DEBUGGING, "CONFIG"); \
      continue; \
    } \
    std::string::size_type p1=str.find_last_not_of("\t :=", p); \
    param = str.substr(p0, ++p1-p0); \
    p0=str.find_first_not_of("\t =",p); \
    p1=str.find_last_not_of("\t "); \
    value = p1<p0?"":str.substr(p0, ++p1-p0); \
\
    END_SECTION_EXCEPTION \
    if (param == "version") { \
      if (value == MMS_CONFIG_VERSION) \
	version_correct = true; \
      else { \
        Print perror(Print::DEBUGGING, "CONFIG"); \
        perror.add_line(config_location + gettext(" is version ") + value + gettext(" and MMS expecting version ") + MMS_CONFIG_VERSION); \
        perror.add_line(gettext("Please upgrade your config file")); \
        perror.print(); \
	version_correct = true; \
      } \
      continue; \
    } else if (!version_correct) { \
      Print perror(Print::DEBUGGING, "CONFIG"); \
      perror.add_line(gettext("Version string not found")); \
      perror.add_line(gettext("Please upgrade your config file")); \
      perror.print(); \
      exit(1); \
    } \
\
/* do not remove because it prevent a preprocess mis-optimization */ \
if (0) std::cerr << "PARSER PARAM =" << param << std::endl << "PARSED VALUE =" << value << std::endl;

#define CONFIG_PARSER_DONE \
    Print perror(gettext("Unknown token '") + param + \
	         gettext("' on line ") + conv::itos(line), \
	         Print::DEBUGGING, "CONFIG"); \
    } \
    In.close();\
  } \
}


#ifdef USE_WRITER

// Macros to  define writer/formatter

#define CONFIG_WRITER(class_name) \
\
void FULL_CLASSNAME(class_name)::save_configuration_file()\
{ \
\
  std::ofstream Out((homedir + "config").c_str()); \
\
  if (!Out) { \
    print_critical(gettext("Can't write configuration file!"), "CONFIG"); \
    return; \
  } else { \
    Out << "# Config File Definition Tool" << std::endl << std::endl;

#define CONFIG_WRITER_DONE \
    Out.close(); \
  } \
}

#else

#define CONFIG_WRITER(class_name)

#endif

// MACROs to inhibit unneeded config parameters

#define PARAMETER_NONE(name)
#define PARAMETER_STR(name,variable,default)
#define PARAMETER_BOOL(name,variable,default)
#define PARAMETER_NUM(name,variable,default)
#define PARAMETER_LIST(name,variable)
#define PARAMETER_SECTION(variable)
#define PARAMETER_PAIR(name,variable)
#define PUBLIC_PARM(x)
#define PRIVATE_PARM(x)
#define VECTOR_BOOL(name,variable,default)
#define VECTOR_NUM(name,variable,lowbound,hibound,default)
#define VECTOR_STR(name,variable,value_list,default)
#define VECTOR_MENU(name,variable,value_list)


#elif !defined PRIVATE_PAR

/*****************************************
  Macros to declare Private Variables
 *****************************************/

#define PRIVATE_PAR

#define PARAMETER_NONE(name)
#define PARAMETER_STR(name,variable,default)  std::string variable;
#define PARAMETER_BOOL(name,variable,default) bool variable;
#define PARAMETER_NUM(name,variable,default)  int  variable;
#define PARAMETER_LIST(name,variable)         std::list<std::string> variable;
#ifdef USE_SECTION
#define PARAMETER_SECTION(variable)           std::list<std::string>variable;
#endif
#define PARAMETER_PAIR(name,variable)         std::list<std::pair<std::string,std::string> > variable;
#define PUBLIC_PARM(x)
#define PRIVATE_PARM(x)                       x;

#define VECTOR_BOOL(name,variable,default) 
#define VECTOR_NUM(name,variable,lowbound,hibound,default) 
#define VECTOR_STR(name,variable,value_list,default) 
#define VECTOR_MENU(name,variable,value_list) 


#elif !defined PUBLIC_PAR

/*****************************************
    Macros to declare public Variables
 *****************************************/

#define PUBLIC_PAR

#define ADD_PARAM(variable,type) type p_##variable() const { return variable; }
#define PARAMETER_NONE(name)
#define PARAMETER_STR(name,variable,default)  ADD_PARAM(variable,std::string)
#define PARAMETER_BOOL(name,variable,default) ADD_PARAM(variable,bool)
#define PARAMETER_NUM(name,variable,default)  ADD_PARAM(variable,int)
#define PARAMETER_LIST(name,variable)         ADD_PARAM(variable,std::list<std::string>)
#ifdef USE_SECTION
#define PARAMETER_SECTION(variable)           ADD_PARAM(variable,std::list<std::string>)
#endif
#define PARAMETER_PAIR(name,variable)         std::list<std::pair<std::string,std::string> > p_##variable() const { return variable; }
#define PUBLIC_PARM(x)                        x;
#define PRIVATE_PARM(x)

#ifdef PARENT_CLASS

#undef  __INSTANCE
#ifdef BASE_CLASS
#define __INSTANCE(variable) BASE_CLASS *variable##_p;
#else
#define __INSTANCE(variable)
#endif

#define VECTOR_BOOL(name,variable,default) \
 std::vector<std::string> variable##_values ; \
 int variable##_pos;\
 bool variable() const { return (variable##_pos == 0);} \
 __INSTANCE(variable)

#define VECTOR_NUM(name,variable,lowbound,hibound,default) \
 std::vector<std::string> variable##_values ; \
 int variable##_pos; \
 int variable() const { return atoi(variable##_values[variable##_pos]);} \
 std::string variable##_str() const { return variable##_values[variable##_pos];} \
 __INSTANCE(variable)

#define VECTOR_MENU(name,variable,value_list) \
 std::vector<std::string> variable ; \
 int variable##_pos; \
 std::string variable() const { return variable##_values[variable##_pos];} \
 __INSTANCE(variable)

#define VECTOR_STR(name,variable,value_list,default) \
 std::vector<std::string> variable##_values ; \
 int variable##_pos; \
 std::string variable() const { return variable##_values[variable##_pos];} \
 __INSTANCE(variable)

#else

#define VECTOR_BOOL(name,variable,default) \
 std::vector<std::string> variable##_values ; \
 bool variable;

#define VECTOR_NUM(name,variable,lowbound,hibound,default) \
 std::vector<std::string> variable##_values ; \
 int variable;

#define VECTOR_MENU(name,variable,value_list) \
 std::vector<std::string> variable ; \
 std::string variable;

#define VECTOR_STR(name,variable,value_list,default) \
 std::vector<std::string> variable##_values ; \
 std::string variable;

#endif


#elif defined CONFIG_DESTRUCTOR && !defined CONFIG_DESTRUCTOR_DONE


/*****************************************
   Macros to declare external destructor
 *****************************************/

#define CONFIG_DESTRUCTOR_DONE EXTRA_POST_DESTRUCTOR }

#define PARAMETER_NONE(name)
#define PARAMETER_STR(name,variable,default)
#define PARAMETER_BOOL(name,variable,default)
#define PARAMETER_NUM(name,variable,default)
#define PARAMETER_LIST(name,variable)
#define PARAMETER_SECTION(variable)
#define PARAMETER_PAIR(name,variable)
#define PUBLIC_PARM(x)
#define PRIVATE_PARM(x)

#undef __INSTANCE

#if defined PARENT_CLASS && defined BASE_CLASS

#define __INSTANCE(variable)                               delete variable##_p;
#define VECTOR_BOOL(name,variable,default)                 __INSTANCE( variable)
#define VECTOR_NUM(name,variable,lowbound,hibound,default) __INSTANCE( variable)
#define VECTOR_MENU(name,variable,value_list)              __INSTANCE( variable)
#define VECTOR_STR(name,variable,value_list,default)       __INSTANCE( variable)

#else

#define __INSTANCE(variable)
#define VECTOR_BOOL(name,variable,default)
#define VECTOR_NUM(name,variable,lowbound,hibound,default)
#define VECTOR_MENU(name,variable,value_list)
#define VECTOR_STR(name,variable,value_list,default)

#endif

#elif defined CONFIG_CONSTRUCTOR && !defined CONFIG_CONSTRUCTOR_DONE

/*****************************************
   Macros to declare external constructor
 *****************************************/

#define CONFIG_CONSTRUCTOR_DONE EXTRA_POST_CONSTRUCTOR }

#define PARAMETER_NONE(name)
#define PARAMETER_STR(name,variable,default)
#define PARAMETER_BOOL(name,variable,default)
#define PARAMETER_NUM(name,variable,default)
#define PARAMETER_LIST(name,variable)
#define PARAMETER_SECTION(variable)
#define PARAMETER_PAIR(name,variable)
#define PUBLIC_PARM(x)
#define PRIVATE_PARM(x)

#undef __INSTANCE

#if defined PARENT_CLASS && defined BASE_CLASS
#define __INSTANCE(name, variable) \
  variable##_p = new BASE_CLASS (false, name, & variable##_pos, variable##_values); \
  val.push_back(variable##_p);

#define VECTOR_BOOL(name,variable,default)                 __INSTANCE(name, variable)
#define VECTOR_NUM(name,variable,lowbound,hibound,default) __INSTANCE(name, variable)
#define VECTOR_MENU(name,variable,value_list)              __INSTANCE(name, variable)
#define VECTOR_STR(name,variable,value_list,default)       __INSTANCE(name, variable)

#else

#define __INSTANCE(name, variable)

#define VECTOR_BOOL(name,variable,default)
#define VECTOR_NUM(name,variable,lowbound,hibound,default)
#define VECTOR_MENU(name,variable,value_list)
#define VECTOR_STR(name,variable,value_list,default)

#endif


#elif !defined DEFAULT_PAR

/*****************************************
    Macros to define init/default values
 *****************************************/

#define DEFAULT_PAR

// These MACROS are working the opposite as indirect stringifier STR_
// they remove parenthesis from a parameter and insert a comma
// making a list for an array

// THIS IS NOT ISO C++
#define UNPACK_0(x...)    x
#define CHOICE_(x)     x,

#define PARAMETER_NONE(name) 
#define PARAMETER_STR(name,variable,default)  variable = default;
#define PARAMETER_BOOL(name,variable,default) variable = default;
#define PARAMETER_NUM(name,variable,default)  variable = default;
#define PARAMETER_LIST(name,variable) 
#define PARAMETER_SECTION(variable)
#define PARAMETER_PAIR(name,variable) 
#define PUBLIC_PARM(x) 
#define PRIVATE_PARM(x)

#ifdef BASE_CLASS

#define VECTOR_BOOL(name,variable,default) \
 variable##_values.push_back("true"); \
 variable##_values.push_back("false"); \
 variable##_pos=(default)?0:1;

#define VECTOR_NUM(name,variable,lowbound,hibound,default) \
 for (int i=lowbound;i<=hibound;i++) { \
   variable##_values.push_back(itos(i)); \
   if (i==default) variable##_pos=(i-lowbound); \
}

#define VECTOR_MENU(name,variable,value_list) \
{ \
  char *tmp[]={ UNPACK_0 value_list 0 }; \
  for(int i=0;tmp[i];i++) variable##_values.push_back(tmp[i]); \
  variable##_pos=0;

#define VECTOR_STR(name,variable,value_list,default)  \
{ \
  char *tmp[]={ UNPACK_0 value_list 0 }; \
  for(int i=0;tmp[i];i++) { \
    variable##_values.push_back(tmp[i]) ; \
    if (tmp[i]==default) variable##_pos=i;\
  } \
}

#else

#define VECTOR_BOOL(name,variable,default) \
 variable##_values.push_back("true"); \
 variable##_values.push_back("false"); \
 variable=default;

#define VECTOR_NUM(name,variable,lowbound,hibound,default) \
 for (int i=lowbound;i<=hibound;i++) \
   variable##_values.push_back(itos(i)); \
 variable=default; \
}

#define VECTOR_MENU(name,variable,value_list) \
{ \
  char *tmp[]={ UNPACK_0 value_list 0 }; \
  for(int i=0;tmp[i];i++) variable##_values.push_back(tmp[i]) ;

#define VECTOR_STR(name,variable,value_list,default)  \
{ \
  char *tmp[]={ UNPACK_0 value_list 0 }; \
  for(int i=0;tmp[i];i++) variable##_values.push_back(tmp[i]); \
  variable=default;\
}

#endif

#elif !defined CONFIG_READER


/*****************************************
       Macros to fill parser boddy
 *****************************************/

#define CONFIG_READER

// MACROs for Configuration Parser

#define PUBLIC_PARM(x)
#define PRIVATE_PARM(x)

#define PARAMETER_NONE(name) \
 if (param == name) continue; \
 else

#define PARAMETER_STR(name,variable,default)  \
 if (param == name) \
  variable = value; \
 else

#define PARAMETER_BOOL(name,variable,default) \
 if (param == name) variable = is_true(value); \
 else

#define PARAMETER_NUM(name,variable,default) \
 if (param == name) \
   variable = atoi(value.c_str()); \
 else

#define PARAMETER_LIST(name,variable) \
 if (param == name) \
  while (!value.empty()) { \
    std::string::size_type pos1 = value.find(','); \
    std::string::size_type pos2 = value.find_last_not_of("\t ,",pos1); \
    variable.push_back(value.substr(0,++pos2)); \
    pos1 = value.find_first_not_of("\t ,",pos1); \
    value.replace(0,pos1,""); \
  } else

#ifdef USE_SECTION
#define PARAMETER_SECTION(variable) \
  if (param == "[]") { \
    std::string::size_type pos1 = value.find_first_not_of("\t ["); \
    std::string::size_type pos2 = value.find_last_not_of("\t ]"); \
    variable.push_back(value.substr(pos1,++pos2)); \
  } else
#endif

#define PARAMETER_PAIR(name,variable) \
 if (param == name) { \
  std::string::size_type pos1 = value.find(','); \
  std::string::size_type pos2 = value.find_last_not_of("\t ,",pos1); \
  std::string first = value.substr(0, ++pos2); \
  pos2 = value.find_first_not_of("\t ,",pos2); \
  std::string::size_type pos3 = value.find_last_not_of("\t "); \
  std::string second = value.substr(pos2, ++pos3-pos2); \
  MyPair temp = MyPair(first, second); \
  variable.push_back(temp); \
} else

#ifndef BASE_CLASS

#define VECTOR_BOOL(name,variable,default)                 PARAMETER_BOOL(name,variable,default)
#define VECTOR_NUM(name,variable,lowbound,hibound,default) PARAMETER_NUM(name,variable,default)
#define VECTOR_STR(name,variable,value_list,default)       PARAMETER_STR(name,variable,default)
#define VECTOR_MENU(name,variable,value_list)

#else

#define VECTOR_BOOL(name,variable,default) \
 if (param == name) variable##_pos = lookup(variable##_values,value); \
 else

#define VECTOR_NUM(name,variable,lowbound,hibound,default) \
 if (param == name) \
   variable##_pos = lookup(variable##_values, value); \
 else

#define VECTOR_STR(name,variable,value_list,default)  \
 if (param == name) variable##_pos = lookup(variable##_values,value); \
 else

#define VECTOR_MENU(name,variable,value_list)

#endif


#elif !defined CONFIG_WRITER

/*****************************************
       Macros to fill writer body
 *****************************************/

#define CONFIG_WRITER

#define PARAMETER_NONE(name)
#define PUBLIC_PARM(x)
#define PRIVATE_PARM(x)

#define PARAMETER_STR(name,variable,default)  if (!(variable == default)) \
  Out << name << " = " << variable << std::endl;

#define PARAMETER_BOOL(name,variable,default) if (!(variable == default)) \
  Out << name << " = " << variable << std::endl;

#define PARAMETER_NUM(name,variable,default)  if (!(variable == default)) \
  Out << name << " = " << variable << std::endl;

#define PARAMETER_LIST(name,variable) \
  foreach (std::string& value, variable) \
    Out << name << " = " << value << std::endl;

#ifdef USE_SECTION
#define PARAMETER_SECTION(variable) \
  foreach (std::string& value, variable) \
    Out << '[' << value << ']' << std::endl;
#endif

#define PARAMETER_PAIR(name,variable) \
  foreach (MyPair& pair, variable) \
    Out  << name << " = " << pair.first << ',' << pair.second << std::endl;

#if !defined BASE_CLASS

#define VECTOR_BOOL(name,variable,default)                 PARAMETER_BOOL(name,variable,default)
#define VECTOR_NUM(name,variable,lowbound,hibound,default) PARAMETER_NUM(name,variable,default)
#define VECTOR_STR(name,variable,value_list,default)       PARAMETER_STR(name,variable,default)
#define VECTOR_MENU(name,variable,value_list)

#else

#define VECTOR_BOOL(name,variable,default) if (!(is_true(variable##_values[variable##_pos]) == default)) \
  Out << name << " = " << variable##_values[variable##_pos] << std::endl;

#define VECTOR_NUM(name,variable,lowbound,hibound,default) if (!(variable##_values[variable##_pos] == itos(default))) \
  Out << name << " = " << variable##_values[variable##_pos] << std::endl;

#define VECTOR_STR(name,variable,value_list,default) if (!(variable##_values[variable##_pos] == default)) \
  Out << name << " = " << variable##_values[variable##_pos] << std::endl;

#define VECTOR_MENU(name,variable,value_list) \
  Out << name << " = " << variable##_values[variable##_pos] << std::endl;

#endif

#endif
