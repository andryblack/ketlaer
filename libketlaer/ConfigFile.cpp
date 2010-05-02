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
#include <sys/stat.h>
#include <ctype.h>
#include <libconfig.h>
#include <ConfigFile.h>


ConfigFile::ConfigFile(const char* aFileName)
{

        
        FileLoaded=false;
        strcpy(FileName,aFileName);
        
        //Initialisation of required data.
        strcpy(RemoteType,"EKAH110");
        ScreenSize=0;
        AspectRatio=0;
        VideoStandardPI=0;
        
        LoadFile();
};

ConfigFile::~ConfigFile()
{
};

bool ConfigFile::LoadFile()
{        

   struct stat buf;
   
   
   if (stat(FileName,&buf))
   {
      if (CreateDefaultFile())
      {
         if (ReadFile())
         {
            FileLoaded=true;
         }
         else
         {
            fprintf(stderr,"Error while reading default setting file !\n");
         }
       }
       else
       {  
          fprintf(stderr,"Error while creating setting file !\n");
       };     
    }
    else
    {
      if (ReadFile())
      {
         FileLoaded=true;
      }
      else
      {
          fprintf(stderr,"Error while reading default setting file !\n");
      }
    }   
};

bool ConfigFile::CreateDefaultFile()
{
    FILE * f;

    f = fopen(FileName, "wb");
    if (f != NULL)
    {
       
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
        fprintf(f,"//  VideoStandardPI = 0, 1                                                    //\n");
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
        fprintf(f,"VideoStandardPI = 1;\n");
        fprintf(f,"AspectRatio = 3;\n");
        fprintf(f,"\n");
        fprintf(f,"\n");        
        fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//  Remote type :                                                             //\n");
        fprintf(f,"//                Emtec :                                                     //\n");
        fprintf(f,"//                         EKAH110                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                ScreanPlay :                                                //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
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

bool ConfigFile::ReadFile()
{

  config_t cfg;
  config_setting_t *setting;
  const char *str;
  int int_v;
  
  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, FileName))
  {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return false;
  }


  /* Get the store name. */
  if(config_lookup_string(&cfg, "name", &str))
  {
    if (strcmp(str,"RTD1073 CONFIG")==0)
    { 
       printf("Loading configuration file... \n\n");
       if (config_lookup_int(&cfg, "ScreenSize", &int_v))
       {
         if ((int_v<0)||(int_v>6)) ScreenSize=0; else ScreenSize=int_v;
         printf("Screen size is set to %d \n",ScreenSize);
       }
       else
       {
         fprintf(stderr, "'ScreenSize' setting in configuration file is wrong.\n");
         goto EXIT_READFILE;
       }
       if (config_lookup_string(&cfg, "Remote", &str))
       {
         if (strcmp(str,"")!=0) strncpy(RemoteType,str,20);
         printf("Remote is set to %s \n",RemoteType);
       }
       else
       {
         fprintf(stderr, "'Remote' setting in configuration file is wrong.\n");
         goto EXIT_READFILE;
       }
       if (config_lookup_int(&cfg, "VideoStandardPI", &int_v))
       {
         if ((int_v<0)||(int_v>1)) VideoStandardPI=0; else VideoStandardPI=int_v;
         printf("VideoStandardPI is set to %d \n",VideoStandardPI);
       }
       else
       {
         fprintf(stderr, "'VideoStandardPI' setting in configuration file is wrong.\n");
         goto EXIT_READFILE;
       }
       if (config_lookup_int(&cfg, "AspectRatio", &int_v))
       {
         if ((int_v<0)||(int_v>3)) AspectRatio=0; else AspectRatio=int_v;
         printf("AspectRatio is set to %d \n",AspectRatio);
       }
       else
       {
         fprintf(stderr, "'AspectRatio' setting in configuration file is wrong.\n");
         goto EXIT_READFILE;
       }
       

    }
    else
    {
       fprintf(stderr, "'name' setting in configuration file is wrong --> '%s'.\n",str);
       goto EXIT_READFILE;
    }
    
  }
  else
  {
    fprintf(stderr, "No 'name' setting in configuration file.\n");
    goto EXIT_READFILE;
  }


  config_destroy(&cfg);
  return true;
EXIT_READFILE:
  config_destroy(&cfg);
  return false;
  
};

char * ConfigFile::GetRemoteType()
{
   if (FileLoaded) return strdup(RemoteType);
   return NULL;
}

int ConfigFile::GetTvSystem()
{
   if (FileLoaded) return ScreenSize;
   return 0;
}

int ConfigFile::GetAspectRatio()
{
   if (FileLoaded) return AspectRatio;
   return 0;
}

int ConfigFile::GetVideoStandardPI()
{
   if (FileLoaded) return VideoStandardPI;
   return 0;
}
