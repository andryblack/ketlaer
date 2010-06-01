/****************************************************************************
**
** Class declaration of settings management.
** It will initialise some variable from file on realtek based hardware.
** The purpose is to have interface with libconfig and let the class do
** maximum to set correctly initialisation variable.
** Write still need to be implemented.
**
**
**
**
**
** Licence
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <assert.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ctype.h>
#include <libconfig.h>
#include <ConfigFile.h>

#include <lib_RPC.h>
#include <lib_OSAL.h>
#include <lib_GPIO.h>
#include <lib_DirectGraphics.h>
#include <lib_DrawScale.h>
#include <lib_MoveDataEngine.h>
#include <lib_StreamingEngine.h>
#include <lib_setupdef.h>
#include <lib_Board.h>
#include <lib_stream.h>
#include <lib_AudioUtil.h>
#include <lib_VoutUtil.h>
#include <lib_PlaybackAppClass.h>
#include <lib_HDMIControl.h>

#include <SetupClass.h>

ConfigFile::ConfigFile(const char*aFileName)
{
  struct stat st;
  char name[PATH_MAX], *str;

  str=getenv("KETLAER");
  snprintf(name, sizeof(name), "%s", str ? str : _QTDefaultBaseFolder);
  if(stat(name,&st) != 0) {
    //hum KETLAER is false or not set and the actual rep is not the standard one
    getcwd(name,sizeof(name));
  }; 
  strcat(name,"/etc");
  if(stat(name,&st) != 0) {
    if (mkdir(name,0777)!=0) 
      perror(name);
  };        
  strcat(name,"/");
  strcat(name,aFileName);
  printf("[ConfigFile]Remote file is '%s'\n",name);
  //Initialisation of required data.
  strcpy(RemoteType,"EKAH110");
  LoadFile(name);
};

ConfigFile::~ConfigFile()
{
};

bool ConfigFile::LoadFile(char *name)
{        
  struct stat buf;
   
  if (stat(name,&buf)) {
    if (CreateDefaultFile(name)) {
      fprintf(stderr,"[ConfigFile]Error while creating setting file !\n");
      return false;
    };     
  }
  if (!ReadFile(name)) {
    fprintf(stderr,"[ConfigFile]Error while reading default setting file !\n");
    return false;
  }
  return true;
};

bool ConfigFile::CreateDefaultFile(char *name)
{
  FILE * f;

  f = fopen(name, "wb");
  if (f != NULL) {
       
    /////////////////////////////////////////////////
    //           Default file settings             //
    /////////////////////////////////////////////////
    // Must be modified if structure change! 
        

    fprintf(f,"//Settings file for Realtek Board/\n");
    fprintf(f,"\n");
    fprintf(f,"name = \"RTD1073 CONFIG\";\n");
    fprintf(f,"\n");
    fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
    fprintf(f,"//  Config output format.                                                     //\n");
    fprintf(f,"//                                                                            //\n");
    fprintf(f,"//  ScreenSize = 0, 1, 2, 3, 4, 5, 6    (Monitor/TV init Size)                //\n");
    fprintf(f,"//      0 =   NTSC = { 0, 0, 720,  480  };                                    //\n");
    fprintf(f,"//      1 =   PAL  = { 0, 0, 720,  576  };                                    //\n");
    fprintf(f,"//      2 =   HD_720 = { 0, 0, 1280, 720  }; 50HZ  Only have progressive!     //\n");
    fprintf(f,"//      3 =   HD_720 = { 0, 0, 1280, 720  }; 60HZ                             //\n");
    fprintf(f,"//      4 =   HD_1080 = { 0, 0, 1920, 1080 }; 50HZ                            //\n");
    fprintf(f,"//      5 =   HD_1080 = { 0, 0, 1920, 1080 }; 60Hz                            //\n");
    fprintf(f,"//      6 =   800x600 = { 0, 0, 800, 600 };                                   //\n");
    fprintf(f,"//                                                                            //\n");
    fprintf(f,"//  Progressive = 0, 1                                                        //\n");
    fprintf(f,"//      0 =   Interlaced                                                      //\n");
    fprintf(f,"//      1 =   Progressive                                                     //\n");
    fprintf(f,"//                                                                            //\n");
    fprintf(f,"//  AspectRatio = 0, 1, 2, 3                                                  //\n");
    fprintf(f,"//      0 =   4/3  (PS)                                                       //\n");
    fprintf(f,"//      1 =   4/3  (LB)                                                       //\n");
    fprintf(f,"//      2 =   16/9                                                            //\n");
    fprintf(f,"//      3 =   zoom                                                            //\n");
    fprintf(f,"//                                                                            //\n");
    fprintf(f,"//                                                                            //\n");
    fprintf(f,"//                                                                            //\n");
    fprintf(f,"//                                                                            //\n");
    fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
    fprintf(f,"\n");
    fprintf(f,"ScreenSize = 3;\n");
    fprintf(f,"Progressive = 1;\n");
    fprintf(f,"AspectRatio = 3;\n");
    fprintf(f,"\n");
    fprintf(f,"\n");        
    fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
    fprintf(f,"//                                                                            //\n");
    fprintf(f,"//  Remote type :                                                             //\n");
    fprintf(f,"//                Emtec :                                                     //\n");
    fprintf(f,"//                         EKAH110                                            //\n");
    fprintf(f,"//                ScreenPlay :                                                //\n");
    fprintf(f,"//                                                                            //\n");
    fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
    fprintf(f,"\n");
    fprintf(f,"Remote = \"EKAH110\";\n");
    fprintf(f,"//EOF\n");
    fclose(f);
  }
  else
    return false;

  return true;
};

bool ConfigFile::ReadFile(char *name)
{

  config_t cfg;
  config_setting_t *setting;
  const char *str;
  int int_v;
  
  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if(!config_read_file(&cfg, name)) {
    fprintf(stderr,"[ConfigFile]%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return false;
  }


  /* Get the store name. */
  if(config_lookup_string(&cfg, "name", &str)) {
    if (strcmp(str,"RTD1073 CONFIG")==0) { 
      printf("[ConfigFile]Loading configuration file... \n");
      if (config_lookup_int(&cfg, "ScreenSize", &int_v)) {
	if ((int_v>=0)&&(int_v<=6)) 
	  setup->SetTvSystem((ENUM_VIDEO_SYSTEM)int_v);
	printf("[ConfigFile]Screen size is set to %d \n",setup->GetTvSystem());
      }
      else {
	fprintf(stderr,"[ConfigFile]'ScreenSize' setting in configuration file is wrong.\n");
	goto EXIT_READFILE;
      }
      if (config_lookup_string(&cfg, "Remote", &str)) {
	if (strcmp(str,"")!=0) strncpy(RemoteType,str,20);
	printf("[ConfigFile]Remote is set to %s \n",RemoteType);
      }
      else {
	fprintf(stderr,"[ConfigFile]'Remote' setting in configuration file is wrong.\n");
	goto EXIT_READFILE;
      }
      if (config_lookup_int(&cfg, "Progressive", &int_v)) {
	if ((int_v>=0)&&(int_v<=1)) 
	  setup->SetTvStandard((ENUM_VIDEO_STANDARD)int_v);
	printf("[ConfigFile]Progressive is set to %d \n",setup->GetTvStandard());
      }
      else {
	fprintf(stderr,"[ConfigFile]'Progressive' setting in configuration file is wrong.\n");
	goto EXIT_READFILE;
      }
      if (config_lookup_int(&cfg, "AspectRatio", &int_v)) {
	if ((int_v>=0)&&(int_v<=3)) 
	  setup->SetAspectRatio((ENUM_ASPECT_RATIO)int_v);
	printf("[ConfigFile]AspectRatio is set to %d \n",setup->GetAspectRatio());
      }
      else {
	fprintf(stderr,"[ConfigFile]'AspectRatio' setting in configuration file is wrong.\n");
	goto EXIT_READFILE;
      }
      if (config_lookup_int(&cfg, "SpdifMode", &int_v)) {
	if ((int_v>=0)&&(int_v<=4)) 
	  setup->SetSpdifMode((ENUM_SPDIF_MODE)int_v);
	printf("[ConfigFile]SpdifMode is set to %d \n",setup->GetSpdifMode());
      }
      else {
	fprintf(stderr,"[ConfigFile]'SpdifMode' setting in configuration file is wrong.\n");
	goto EXIT_READFILE;
      }       
    }
    else {
      fprintf(stderr,"[ConfigFile]'name' setting in configuration file is wrong --> '%s'.\n",str);
      goto EXIT_READFILE;
    }
    
  }
  else {
    fprintf(stderr,"[ConfigFile]No 'name' setting in configuration file.\n");
    goto EXIT_READFILE;
  }


  config_destroy(&cfg);
  return true;
 EXIT_READFILE:
  config_destroy(&cfg);
  return false;
  
};

char *ConfigFile::GetRemoteType()
{
   return RemoteType;
}
