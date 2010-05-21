#ifndef PYTHON_HPP
#define PYTHON_HPP

#include "module.hpp"
#include "options.hpp"
#include "singleton.hpp"
#include "guiwindowscripts.hpp"
#include <vector>
#include <string>

class Python : public Module 
{
  private:
  
   class PythonOpts : public Options 
   {
     private:
       Option* m_pReload;
       void read_dirs();
     
     public:
        PythonOpts();
       ~PythonOpts();

       std::vector<Option*> values() 
       {
          return val;
       }
   };
  
   PythonOpts m_pyOpts;
   GUIWindowScripts m_scriptWindow;

  public:
   Python();
   std::string findResDepCfg(const std::string& strXml);
   void load_runtime_settings() {}
   void save_runtime_settings() {}

   PythonOpts* get_opts()
   {
     return &m_pyOpts;
   }
  
   void startup_updater();
   std::string mainloop();
};

#endif
