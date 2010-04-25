/*system/Include/Platform_Lib/Board/board.h*/
enum _tags_audio_input_enum {AUDIO_INPUT_ADC0, AUDIO_INPUT_ADC1, AUDIO_INPUT_SPDIF, AUDIO_INPUT_BITSTREAM, AUDIO_INPUT_NONE};

typedef _tags_audio_input_enum AUDIO_INPUT_ENUM;

enum _video_in_format_enum {AUTO_DETECT, NTSC, NTSC_443, PAL_I, PAL_M, PAL_CN, SECAM, VIDEO_IN_FORMAT_NUM};

typedef _video_in_format_enum VIDEO_IN_FORMAT;

enum _video_in_adjust_enum {BRIGHTNESS, CONTRAST, SATURATION, HUE, VIDEO_IN_ADJUST_NUM};

typedef _video_in_adjust_enum VIDEO_IN_ADJUST;

enum POWER_ON_MODE {POWER_ON_COLD_BOOT, POWER_ON_WARM_BOOT, POWER_ON_TIME_BOOT};

typedef unsigned int USB_CAPABILITY;

typedef unsigned int HOT_PLUG_CAPABILITY;

class board_driver {
public:
  char name[32];
  AUDIO_INPUT_ENUM (*board_codec_switch_input)(SourceOption );
  AUDIO_INPUT_ENUM (*board_codec_switch_mic)();
  void (*board_codec_mute)(bool );
  void (*board_dac_mute)(bool );
  bool (*vin_exist)(SourceOption );
  ENUM_TVD_INPUT_FORMAT (*getINFMT)(SourceOption );
  bool (*vout_exist)(OutputOption );
  TVE_BOARD_TYPE (*getTVE)();
  void (*getTVS)(video_system , VO_STANDARD * , VO_PEDESTAL_TYPE * );
  unsigned char (*getVideoInDefaultValue)(VIDEO_IN_ADJUST , VIDEO_IN_FORMAT );
  void (*setFormatSCART)(bool );
  void (*setAVModeSCART)(bool );
  void (*setScaleModeSCART)(bool );
  bool (*getFormatSCART)();
  void (*setPowerLED)(bool );
  void (*setRecordLED)(bool );
  void (*setHddLED)(bool );
  void (*setDvdLED)(bool );
  void (*setTimerRecordLED)(bool );
  void (*setSATRecordLED)(bool );
  void (*setEthernetLED)(bool );
  char (*KeypadTranslate)(char );
  int (*USB_GetCardreaderPortNumber)();
  int (*USB_GetCardreaderPortNumber_string)(char * );
  USB_CAPABILITY (*getCapabilityByUsbPort)(int );
  HOT_PLUG_CAPABILITY (*getCapabilityBySataPort)(int );
  bool (*HDMI_HotPlug)();
  bool (*PCUSBConnection)();
  bool (*RegisterGPIOPanel)(int );
  char (*GPIOPanelTranslate)(char );
  int (*getSartDecoderResult)();
  void (*RPC_HDMI_Mute)(long int * , RPC_STRUCT * , HRESULT * );
  void (*RPC_HDMI_Setting)(AUDIO_HDMI_SETTING_INFO * , RPC_STRUCT * , HRESULT * );
  POWER_ON_MODE (*getBootMode)();
  int (*getBootKey)();
  void (*ODDPowerSwitch)(bool );
  void (*HDDPowerSwitch)(bool );
  void (*SysFanPowerSwitch)(bool );
  void (*TunerPowerSwitch)(bool );
  void (*TunerPowerReset)(bool );
  void (*AntennaFeedPowerSwitch)(bool );
  void (*FrontPanelPowerSwitch)(bool );
};



/*system/Include/Platform_Lib/Board/BoardCommon.cpp*/
extern "C" void board_init();

extern "C" void board_uninit();

extern "C" AUDIO_INPUT_ENUM board_codec_switch_input(SourceOption input);

extern "C" AUDIO_INPUT_ENUM board_codec_switch_mic();

extern "C" void board_codec_mute(bool bValue);

extern "C" void board_dac_mute(bool bValue);

extern "C" bool vin_exist(SourceOption input);

extern "C" bool vout_exist(OutputOption option);

extern "C" ENUM_TVD_INPUT_FORMAT getINFMT(SourceOption input);

extern "C" char KeypadTranslate(char keypadNum);

extern "C" TVE_BOARD_TYPE getTVE();

extern "C" void setFormatSCART(bool bIsRGB);

extern "C" void setAVModeSCART(bool bIsAVMode);

extern "C" void setScaleModeSCART(bool bIsWidescreen);

extern "C" bool getFormatSCART();

extern "C" void setPowerLED(bool bLight);

extern "C" void setRecordLED(bool bLight);

extern "C" void setHddLED(bool bLight);

extern "C" void setDvdLED(bool bLight);

extern "C" void setTimerRecordLED(bool bLight);

extern "C" void setSATRecordLED(bool bLight);

extern "C" void setEthernetLED(bool bLight);

extern "C" unsigned char getVideoInDefaultValue(VIDEO_IN_ADJUST vin_adjust, VIDEO_IN_FORMAT vin_format);

extern "C" USB_CAPABILITY getCapabilityByUsbPort(int port);

extern "C" HOT_PLUG_CAPABILITY getCapabilityBySataPort(int port);

extern "C" bool HDMI_HotPlug();

extern "C" void getTVS(video_system videoSystem, VO_STANDARD * standard, VO_PEDESTAL_TYPE * pedType);

extern "C" bool PCUSBConnection();

extern "C" bool RegisterGPIOPanel(int gpio_fd);

extern "C" char GPIOPanelTranslate(char gpioNum);

extern "C" POWER_ON_MODE getBootMode();

extern "C" int getBootKey();

extern "C" int USB_GetCardreaderPortNumber();

extern "C" int USB_GetCardreaderPortNumber_string(char * ptr);

extern "C" int getSartDecoderResult();

extern "C" HRESULT * AUDIO_RPC_ToSystem_ADC_HardwareMute_0_svc(long int * enable, RPC_STRUCT * rpc, HRESULT * retval);

extern "C" HRESULT * AUDIO_RPC_ToSystem_DAC_HardwareMute_0_svc(long int * enable, RPC_STRUCT * rpc, HRESULT * retval);

extern "C" HRESULT * VIDEO_RPC_ToSystem_ScaleModeSCART_0_svc(long int * WideScreen, RPC_STRUCT * rpc, HRESULT * retval);

extern "C" HRESULT * AUDIO_RPC_ToSystem_HDMI_Mute_0_svc(long int * bMute, RPC_STRUCT * rpc, HRESULT * retval);

extern "C" HRESULT * AUDIO_RPC_ToSystem_HDMI_Setting_0_svc(AUDIO_HDMI_SETTING_INFO * pInfo, RPC_STRUCT * rpc, HRESULT * retval);

extern "C" void switchOnODDPower(bool bSwitch);

extern "C" void switchOnHDDPower(bool bSwitch);

extern "C" void switchOnSysFanPower(bool bSwitch);

extern "C" void switchOnTunerPower(bool bSwitch);

extern "C" void resetTunerPower(bool bReset);

extern "C" void switchAntennaFeedPower(bool bSwitch);

extern "C" void switchFrontPanelPower(bool bSwitch);


/*system/Include/Platform_Lib/Board/HDP-R3/board.cpp*/
extern int SampleFreq_new;

extern int CheatSonyNeeded;

extern int SampleFreq_old;

extern AUDIO_HDMI_SETTING_INFO HDMI_LocalInfo;

int HDMI_HotPlug_by_I2C();

/*____UNKNOWN:<31337>*/

extern "C" void BoardRegister(board_driver * board);

extern "C" void BoardUnregister();


