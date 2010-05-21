#include "python.hpp"
#include "libfspp.hpp"
#include "input.hpp"
#include "busy_indicator.hpp"
#include "updater.hpp"
#include "global.hpp"
#include "touch.hpp"
#include "guiwindowmanager.hpp"
#include "python_config.hpp"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

using std::string;
using std::vector;
using namespace pymms::gui;

void Python::startup_updater()
{
   S_WindowManager::get_instance()->registerWindow(&m_scriptWindow);
}

string Python::findResDepCfg(const string& strXml)
{
  int i = strXml.rfind('.');

  if (i != string::npos) {

    Config* pConfig = S_Config::get_instance();

    string strName = strXml.substr(0, i);
    string strExt = strXml.substr(i, strXml.size()-i);

    if ((pConfig->p_h_res()/16.0)/(pConfig->p_v_res()/10) == 1)
        return strName + "-16-10" + strExt;
    else if ((pConfig->p_h_res()/16.0)/(pConfig->p_v_res()/9) == 1)
      return strName + "-16-9" + strExt;
    else if ((pConfig->p_h_res()/4.0)/(pConfig->p_v_res()/3) == 1)
      return strName + "-4-3" + strExt;
  else {
      std::cerr << "WARNING: unsupported resolution, trying to find the best ratio" << std::endl;
      float fRatio = (pConfig->p_h_res()*1.)/pConfig->p_v_res();
      std::cerr << "WARNING: ratio is : " << fRatio << std::endl;
      if (fRatio < 1.47) {
        std::cerr << "WARNING: using 4/3 ratio" << std::endl;
        return strName + "-4-3" + strExt;
      }
      else if ((fRatio >= 1.47) and (fRatio < 1.69)) {
        std::cerr << "WARNING: using 16/10 ratio" << std::endl;
        return strName + "-16-10" + strExt;
      }
      else {
        std::cerr << "WARNING: using 16/9 ratio" << std::endl;
        return strName + "-16-9" + strExt;
      }
    }
  }

  std::cerr << "ERROR: can't find . in " << strXml << std::endl;
  return "";
}

std::string Python::mainloop()
{
   PythonConfig* pPythonConf = S_PythonConfig::get_instance();
   GUIWindowManager* pWindowManager = S_WindowManager::get_instance();
   InputMaster* pInputMaster = S_InputMaster::get_instance();
   BusyIndicator* pBusyIndicator = S_BusyIndicator::get_instance();
   Global* pGlobal = S_Global::get_instance();

   Render *render = S_Render::get_instance();
   render->device->animation_section_begin();
   int curlayer = render->device->get_current_layer();
   render->device->switch_to_layer(1);
   render->device->animation_zoom(0,0,1,1,20,1);
   render->device->animation_fade(0,1,40,1);
   render->device->animation_section_end();

   m_scriptWindow.clear();
   m_scriptWindow.load(findResDepCfg("python/scriptwindow.xml"));

   if(file_exists(pPythonConf->p_script_dir()))
   {
     pInputMaster->parse_keys("python", "1");
     pInputMaster->set_map("python");

     pWindowManager->setDefaultActive(true);

     string strInput = "";
     bool bContinue = true;
     do 
     {
        pBusyIndicator->idle();

        strInput = pWindowManager->getInput();

	pBusyIndicator->busy();

        if(strInput == "quit")
          break;
        else if(strInput.empty())
 	  continue;

	bContinue = pWindowManager->onAction(strInput);
	
	//if(bContinue)
	//  pGlobal->check_commands(strInput);

     } while(bContinue);

     pWindowManager->setDefaultActive(false);

     pInputMaster->set_map("default");
   }
   else
     DialogWaitPrint pdialog(pPythonConf->p_script_dir() + " " + dgettext("mms-python", "does not exist"), 2000);
   
   render->device->animation_section_begin();
   render->device->layer_active_no_wait(false);
   render->device->switch_to_layer(curlayer);
   render->device->animation_fade(0,1,80,curlayer);
   render->device->animation_section_end();

   return "";
}
  
Python::PythonOpts::PythonOpts() 
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-python", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-python", nl_langinfo(CODESET));
#endif

  // header
  header = "Python Options";
  translated_header = dgettext("mms-python", "Python Options");
  save_name = "PythonOptions";

  // reload
  vector<string> vecNone;
  m_pReload = new Option(false, dgettext("mms-python", "reload"), "reload", 
			 0, vecNone, vecNone);
  val.push_back(m_pReload);

  // sorting order
  val.push_back(go->dir_order());
}

Python::Python()
{
}


void Python::PythonOpts::read_dirs()
{
}

Python::PythonOpts::~PythonOpts() 
{
  delete m_pReload;
}
