
class _IPV4 {
public:
  unsigned char ucIP[4];
};

typedef _IPV4 IPV4;

#define TRACE \
printf("SetupClass::%s(), %s:%d\n", __FUNCTION__, __FILE__, __LINE__)

/*system/Include/Application/AppClass/SetupClass.h*/
class SetupClass {
public:
  static SetupClass * m_ptr;
  SetupClass()
  {
    TRACE;
  }
  virtual ~SetupClass()
  {
    TRACE;
  }
  virtual void Init()
  {
    TRACE;
  }
  virtual void Create()
  {
    TRACE;
  }
  virtual HRESULT Sync()
  {
    TRACE;
    return 0;
  }
  virtual HRESULT AutoSync()
  {
    TRACE;
    return 0;
  }
  virtual void RestoreFactoryDefault()
  {
    TRACE;
  }
  virtual int SetNetWlan0ProfileCopy(int, int)
  {
    TRACE;
    return 0;
  }
  virtual int SetNetWlan0ProfileToDefault(int)
  {
    TRACE;
    return 0;
  }
  //static void SetInstance(SetupClass * );
  //static void DeleteInstance();
  //static SetupClass * GetInstance();
  virtual RECTANGLE_DIMENSION GetDimension(ENUM_VIDEO_SYSTEM)
  {
    RECTANGLE_DIMENSION ret;

    TRACE;
    memset(&ret, 0, sizeof(ret));
    return ret;
  }
  virtual ENUM_OSD_LANGUAGE GetOsdLanguage()
  {
    TRACE;
    return OSD_LANG_ENGLISH;
  }
  virtual void SetOsdLanguage(ENUM_OSD_LANGUAGE)
  {
    TRACE;
  }
  virtual ENUM_AUDIO_LANGUAGE GetAudioLanguage()
  {
    TRACE;
    return AUDIO_LANG_ENGLISH;
  }
  virtual void SetAudioLanguage(ENUM_AUDIO_LANGUAGE)
  {
    TRACE;
  }
  virtual ENUM_SUBTITLE_LANGUAGE GetSubtitleLanguage()
  {
    TRACE;
    return SUBTITLE_LANG_ENGLISH;
  }
  virtual void SetSubtitleLanguage(ENUM_SUBTITLE_LANGUAGE)
  {
    TRACE;
  }
  virtual ENUM_MENU_LANGUAGE GetMenuLanguage()
  {
    TRACE;
    return MENU_LANG_ENGLISH;
  }
  virtual void SetMenuLanguage(ENUM_MENU_LANGUAGE)
  {
    TRACE;
  }
  virtual ENUM_TEXT_ENCODING GetTextSubtitleEncoding()
  {
    TRACE;
    return TEXT_ENCODING_UTF8;
  }
  virtual void SetTextSubtitleEncoding(ENUM_TEXT_ENCODING)
  {
    TRACE;
  }
  virtual int GetTextSubtitleYOffset()
  {
    TRACE;
    return 0;
  }
  virtual void SetTextSubtitleYOffset(int)
  {
    TRACE;
  }
  virtual int GetTextSubtitleSize()
  {
    TRACE;
    return 10;
  }
  virtual void SetTextSubtitleSize(int)
  {
    TRACE;
  }
  virtual ENUM_SUBTITLE_TEXT_COLOR GetTextSubtitleColor()
  {
    TRACE;
    return SUBTITLE_TEXT_WHITE;
  }
  virtual void SetTextSubtitleColor(ENUM_SUBTITLE_TEXT_COLOR)
  {
    TRACE;
  }
  virtual int GetTextSubtitleOffset()
  {
    TRACE;
    return 0;
  }
  virtual void SetTextSubtitleOffset(int)
  {
    TRACE;
  }
  virtual ENUM_VIDEO_SYSTEM GetTvSystem()
  {
    TRACE;
    return VIDEO_HD720_50HZ;
  }
  virtual void SetTvSystem(ENUM_VIDEO_SYSTEM)
  {
    TRACE;
  }
  virtual ENUM_VIDEO_SYSTEM GetVideoSDSystem()
  {
    TRACE;
    return VIDEO_HD720_50HZ;
  }
  virtual void SetVideoSDSystem(ENUM_VIDEO_SYSTEM)
  {
    TRACE;
  }
  virtual ENUM_VIDEO_HD_SYSTEM GetVideoHDSystem()
  {
    TRACE;
    return VIDEO_HD_720P;
  }
  virtual void SetVideoHDSystem(ENUM_VIDEO_HD_SYSTEM)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetVideoSystem1080P24HZ()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetVideoSystem1080P24HZ(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual int GetErrorConcealmentLevel()

  {
    TRACE;
    return 0;
  }
  virtual void SetErrorConcealmentLevel(int)
  {
    TRACE;
  }
  virtual ENUM_TV_SYSTEM_AUTO_MODE GetTvSystemAutoMode()
  {
    TRACE;
    return TV_SYSTEM_AUTO_MODE_OFF;
  }
  virtual void SetTvSystemAutoMode(ENUM_TV_SYSTEM_AUTO_MODE)
  {
    TRACE;
  }
  virtual ENUM_VIDEO_STANDARD GetTvStandard()
  {
    TRACE;
    return VIDEO_PROGRESSIVE;
  }
  virtual void SetTvStandard(ENUM_VIDEO_STANDARD)
  {
    TRACE;
  }
  virtual ENUM_VIDEO_REC_SYSTEM GetRecTvSystem()
  {
    TRACE;
    return VIDEO_REC_PAL;
  }
  virtual void SetRecTvSystem(ENUM_VIDEO_REC_SYSTEM)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetAutoTvSystem()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetAutoTvSystem(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetVideoPreprocessing()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetVideoPreprocessing(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetCopyProtection()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetCopyProtection(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_REC_QUALITY GetRecQuality()
  {
    TRACE;
    return RECORD_HQ;
  }
  virtual void SetRecQuality(ENUM_REC_QUALITY)
  {
    TRACE;
  }
  virtual ENUM_AUTO_CHAPTER GetAutoChapter()
  {
    TRACE;
    return OFF;
  }
  virtual void SetAutoChapter(ENUM_AUTO_CHAPTER)
  {
    TRACE;
  }
  virtual ENUM_REC_DEVICE GetRecDevice()
  {
    TRACE;
    return REC_TO_USB_AUTO;
  }
  virtual void SetRecDevice(ENUM_REC_DEVICE)
  {
    TRACE;
  }
  virtual ENUM_REC_DEVICE GetTimeShiftDevice()
  {
    TRACE;
    return REC_TO_USB_AUTO;
  }
  virtual void SetTimeShiftDevice(ENUM_REC_DEVICE)
  {
    TRACE;
  }
  virtual char * GetLastRecFile()
  {
    TRACE;
    return "/tmp/LastRecFile";
  }
  virtual void SetLastRecFile(char *)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetAutoTimeshift()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetAutoTimeshift(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_SLIDE_SHOW_TIME GetSlideShowTime()
  {
    TRACE;
    return SLIDE_SHOW_2S;
  }
  virtual void SetSlideShowTime(ENUM_SLIDE_SHOW_TIME)
  {
    TRACE;
  }
  virtual ENUM_TRANS_EFFECT GetTeansEffect()
  {
    TRACE;
    return TRANS_EFFECT_OFF;
  }
  virtual void SetTeansEffect(ENUM_TRANS_EFFECT)
  {
    TRACE;
  }
  virtual ENUM_MIX_PLAY GetMixPlay()
  {
    TRACE;
    return MIX_PLAY_OFF;
  }
  virtual void SetMixPlay(ENUM_MIX_PLAY)
  {
    TRACE;
  }
  virtual ENUM_TIME_ZONE GetTimeZone()
  {
    TRACE;
    return BTN_TIME_ZONE01;
  }
  virtual void SetTimeZone(ENUM_TIME_ZONE)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetDayLightSaving()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetDayLightSaving(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetPowerSaving()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetPowerSaving(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual int GetRatingIsBlockUnrated()
  {
    TRACE;
    return 0;
  }
  virtual void SetRatingIsBlockUnrated(int)
  {
    TRACE;
  }
  virtual int GetRatingIsEnable()
  {
    TRACE;
    return 0;
  }
  virtual void SetRatingIsEnable(int)
  {
    TRACE;
  }
  virtual ENUM_RATING_LEVEL GetRatingLevel()
  {
    TRACE;
    return RATING_OFF;
  }
  virtual void SetRatingLevel(ENUM_RATING_LEVEL)
  {
    TRACE;
  }
  virtual ENUM_RATING_REGION GetRatingRegion()
  {
    TRACE;
    return RATING_DEFAULT;
  }
  virtual void SetRatingRegion(ENUM_RATING_REGION)
  {
    TRACE;
  }
  virtual unsigned char GetRatingType(ENUM_RATING_LEVEL)
  {
    TRACE;
    return 0;
  }
  virtual void SetRatingType(ENUM_RATING_LEVEL, unsigned char)
  {
    TRACE;
  }
  virtual OPENVCHIP_TABLE_INFO * GetOpenVChipTable()
  {
    TRACE;
    return NULL;
  }
  virtual OPEN_VCHIP_RATING_INFO * GetOpenVChipRatingInfo()
  {
    TRACE;
    return NULL;
  }
  virtual void SetOpenVChip()
  {
    TRACE;
  }
  virtual int GetRatingTimeout()
  {
    TRACE;
    return 0;
  }
  virtual void SetRatingTimeout(int)
  {
    TRACE;
  }
  virtual int GetRatingPasswd()
  {
    TRACE;
    return 0;
  }
  virtual void SetRatingPasswd(int)
  {
    TRACE;
  }
  virtual bool IsRatingPasswdCorrect(int)
  {
    TRACE;
    return true;
  }
  virtual int GetTVRatingBitwiseValue()
  {
    TRACE;
    return 0;
  }
  virtual void SetTVRatingBitwiseValue(int)
  {
    TRACE;
  }
  virtual int GetTVxRatingBitwiseValue()
  {
    TRACE;
    return 0;
  }
  virtual void SetTVxRatingBitwiseValue(int)
  {
    TRACE;
  }
  virtual bool GetAutoPlay()
  {
    TRACE;
    return false;
  }
  virtual void SetAutoPlay(bool)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetSeamlessPlayback()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetSeamlessPlayback(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_ASPECT_RATIO GetAspectRatio()
  {
    TRACE;
    return ASPECT_RATIO_16_9;
  }
  virtual void SetAspectRatio(ENUM_ASPECT_RATIO)
  {
    TRACE;
  }
  virtual int GetBrightness()
  {
    TRACE;
    return 25;
  }
  virtual void SetBrightness(int)
  {
    TRACE;
  }
  virtual int GetContrast()
  {
    TRACE;
    return 25;
  }
  virtual void SetContrast(int)
  {
    TRACE;
  }
  virtual ENUM_VIDEO_OUTPUT GetVideoOutput()
  {
    TRACE;
    return VOUT_YUV;
  }
  virtual void SetVideoOutput(ENUM_VIDEO_OUTPUT)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetVideoDNRMode()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetVideoDNRMode(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetVideoZoomOutMode()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetVideoZoomOutMode(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual int GetCurrentHdmiTVVid()
  {
    TRACE;
    return 0;
  }
  virtual void SetCurrentHdmiTVVid(int)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetAngleMark()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetAngleMark(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_REGION_SETTING GetRegionCode()
  {
    TRACE;
    return REGION_CODE_ZERO;
  }
  virtual void SetRegionCode(ENUM_REGION_SETTING)
  {
    TRACE;
  }
  virtual ENUM_SECURITY_SETTING GetSecuritySetting()
  {
    TRACE;
    return SECURITY_ANONYMOUS;
  }
  virtual void SetSecuritySetting(ENUM_SECURITY_SETTING)
  {
    TRACE;
  }
  virtual bool GetInitialFlag()
  {
    TRACE;
    return false;
  }
  virtual void SetInitialFlag(bool)
  {
    TRACE;
  }
  virtual ENUM_SPEAKER_OUT GetSpeakerOut()
  {
    TRACE;
    return TWO_CHANNEL;
  }
  virtual void SetSpeakerOut(ENUM_SPEAKER_OUT)
  {
    TRACE;
  }
  virtual ENUM_AUDIO_DIGITAL_OUTPUT_MODE GetHdmiAudioOutputMode()
  {
    TRACE;
    return AUDIO_DIGITAL_LPCM_DUAL_CH;
  }
  virtual void SetHdmiAudioOutputMode(ENUM_AUDIO_DIGITAL_OUTPUT_MODE)
  {
    TRACE;
  }
  virtual ENUM_AUDIO_DIGITAL_OUTPUT_MODE GetSpdifAudioOutputMode()
  {
    TRACE;
    return AUDIO_DIGITAL_LPCM_DUAL_CH;
  }
  virtual void SetSpdifAudioOutputMode(ENUM_AUDIO_DIGITAL_OUTPUT_MODE)
  {
    TRACE;
  }
  virtual ENUM_SPDIF_MODE GetSpdifMode()
  {
    TRACE;
    return SPDIF_LPCM;
  }
  virtual void SetSpdifMode(ENUM_SPDIF_MODE)
  {
    TRACE;
  }
  virtual bool GetDolbyPrologicEnable()
  {
    TRACE;
    return false;
  }
  virtual void SetDolbyPrologicEnable(bool)
  {
    TRACE;
  }
  virtual ENUM_AUDIO_AGC_MODE GetAudioAGCMode()
  {
    TRACE;
    return AUDIO_AGC_DRC_OFF;
  }
  virtual void SetAudioAGCMode(ENUM_AUDIO_AGC_MODE)
  {
    TRACE;
  }
  virtual void SetSortingModeHDD(SortMode)
  {
    TRACE;
  }
  virtual SortMode GetSortingModeHDD()
  {
    TRACE;
    return SORT_BY_NAME;
  }
  virtual void SetSortingModeDISC(SortMode)
  {
    TRACE;
  }
  virtual SortMode GetSortingModeDISC()
  {
    TRACE;
    return SORT_BY_NAME;
  }
  virtual void SetKaraokeMode(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetKaraokeMode()
  {
    TRACE;
    return SET_OFF;
  }
  virtual int GetMic1Volume()
  {
    TRACE;
    return 0;
  }
  virtual void SetMic1Volume(int)
  {
    TRACE;
  }
  virtual int GetMic2Volume()
  {
    TRACE;
    return 0;
  }
  virtual void SetMic2Volume(int)
  {
    TRACE;
  }
  virtual int GetMusicVolume()
  {
    TRACE;
    return 0;
  }
  virtual void SetMusicVolume(int)
  {
    TRACE;
  }
  virtual int GetMicEcho()
  {
    TRACE;
    return 0;
  }
  virtual void SetMicEcho(int)
  {
    TRACE;
  }
  virtual VOCAL_REMOVAL_MODE GetVocalRemovalMode()
  {
    TRACE;
    return ENUM_VOCAL_REMOVAL_NONE;
  }
  virtual void SetVocalRemovalMode(VOCAL_REMOVAL_MODE)
  {
    TRACE;
  }
  virtual ENUM_KEY_SHIFT_STONE GetKeyShiftStone()
  {
    TRACE;
    return ENUM_KEY_SHIFT_STONE_0;
  }
  virtual void SetKeyShiftStone(ENUM_KEY_SHIFT_STONE)
  {
    TRACE;
  }
  virtual ENUM_EQUALIZER_MODE GetEqualizerMode()
  {
    TRACE;
    return ENUM_EQUALIZER_RESERVED;
  }
  virtual void SetEqualizerMode(ENUM_EQUALIZER_MODE)
  {
    TRACE;
  }
  virtual KARAOKE_REVERB_MODE GetReverbMode()
  {
    TRACE;
    return REVERB_OFF;
  }
  virtual void SetReverbMode(KARAOKE_REVERB_MODE)
  {
    TRACE;
  }
  virtual SourceOption GetSource()
  {
    TRACE;
    return SOURCE_Tuner;
  }
  virtual void SetSource(SourceOption)
  {
    TRACE;
  }
  virtual DeviceOption GetDevice()
  {
    TRACE;
    return DEVICE_HDD;
  }
  virtual void SetDevice(DeviceOption)
  {
    TRACE;
  }
  virtual int GetVolume()
  {
    TRACE;
    return 31;
  }
  virtual void SetVolume(int)
  {
    TRACE;
  }
  virtual int GetCurrChannel()
  {
    TRACE;
    return 0;
  }
  virtual void SetCurrChannel(int)
  {
    TRACE;
  }
  virtual int GetCurDtvChannelIndex()
  {
    TRACE;
    return 0;
  }
  virtual void SetCurDtvChannelIndex(int)
  {
    TRACE;
  }
  virtual DTV_REGION GetDtvRegion()
  {
    TRACE;
    return DTV_REGION_DEFAULT;
  }
  virtual void SetDtvRegion(DTV_REGION)
  {
    TRACE;
  }
  virtual RESUME_SETTING GetResumeMode()
  {
    TRACE;
    return RESUME_NONE;
  }
  virtual void SetResumeMode(RESUME_SETTING)
  {
    TRACE;
  }
  virtual unsigned int GetSignature(void **sig)
  {
    char *s = "GetSignature";
    TRACE;
    *sig = (void*)s;
    return strlen(s);
  }
  virtual void SetSignature(void *, unsigned int)
  {
    TRACE;
  }
  virtual unsigned int GetBookmark(void **book)
  {
    char *b = "GetBookmark";
    TRACE;
    *book = (void*)b;
    return strlen(b);
  }
  virtual void SetBookmark(void * , unsigned int)
  {
    TRACE;
  }
  virtual bool GetNetEth0Enable()
  {
    TRACE;
    return true;
  }
  virtual void SetNetEth0Enable(bool)
  {
    TRACE;
  }
  virtual bool GetNetEth0DHCPEnable()
  {
    TRACE;
    return true;
  }
  virtual void SetNetEth0DHCPEnable(bool)
  {
    TRACE;
  }
  virtual IPV4 * GetNetEth0IPAddr()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetEth0IPAddr(IPV4 *)
  {
    TRACE;
  }
  virtual IPV4 * GetNetEth0Mask()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetEth0Mask(IPV4 *)
  {
    TRACE;
  }
  virtual IPV4 * GetNetEth0Gateway()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetEth0Gateway(IPV4 *)
  {
    TRACE;
  }
  virtual IPV4 * GetNetDNS()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetDNS(IPV4 *)
  {
    TRACE;
  }
  virtual bool GetNetWlan0Enable()
  {
    TRACE;
    return false;
  }
  virtual void SetNetWlan0Enable(bool)
  {
    TRACE;
  }
  virtual int GetNetWlan0Profile()
  {
    TRACE;
    return 0;
  }
  virtual void SetNetWlan0Profile(int)
  {
    TRACE;
  }
  virtual int GetNetWlan0ProfilePreSetting()
  {
    TRACE;
    return 0;
  }
  virtual void SetNetWlan0ProfilePreSetting(int)
  {
    TRACE;
  }
  virtual char * GetNetWlan0ProfileName()
  {
    TRACE;
    return "GetNetWlan0ProfileName";
  }
  virtual char * GetNetWlan0ProfileName(int)
  {
    TRACE;
    return "GetNetWlan0ProfileName_idx";
  }
  virtual void SetNetWlan0ProfileName(char *, int)
  {
    TRACE;
  }
  virtual ENUM_WIRELESS_MODE GetNetWlan0Mode()
  {
    TRACE;
    return WL_MODE_INFRASTRUCTURE;
  }
  virtual ENUM_WIRELESS_MODE GetNetWlan0Mode(int)
  {
    TRACE;
    return WL_MODE_INFRASTRUCTURE;
  }
  virtual void SetNetWlan0Mode(ENUM_WIRELESS_MODE, int)
  {
    TRACE;
  }
  virtual bool GetNetWlan0DHCPEnable()
  {
    TRACE;
    return true;
  }
  virtual bool GetNetWlan0DHCPEnable(int)
  {
    TRACE;
    return true;
  }
  virtual void SetNetWlan0DHCPEnable(bool, int)
  {
    TRACE;
  }
  virtual IPV4 * GetNetWlan0IPAddr()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual IPV4 * GetNetWlan0IPAddr(int)
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetWlan0IPAddr(IPV4 *, int)
  {
    TRACE;
  }
  virtual IPV4 * GetNetWlan0Mask()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual IPV4 * GetNetWlan0Mask(int)
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetWlan0Mask(IPV4 *, int)
  {
    TRACE;
  }
  virtual IPV4 * GetNetWlan0Gateway()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual IPV4 * GetNetWlan0Gateway(int)
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetWlan0Gateway(IPV4 *, int)
  {
    TRACE;
  }
  virtual IPV4 * GetNetWlan0DNS()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual IPV4 * GetNetWlan0DNS(int)
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetWlan0DNS(IPV4 *, int)
  {
    TRACE;
  }
  virtual char * GetNetWlan0ApName()
  {
    TRACE;
    return "GetNetWlan0ApName";
  }
  virtual char * GetNetWlan0ApName(int)
  {
    TRACE;
    return "GetNetWlan0ApName_idx";
  }
  virtual void SetNetWlan0ApName(char *, int)
  {
    TRACE;
  }
  virtual ENUM_WIRELESS_SECURITY GetNetWlan0Security()
  {
    TRACE;
    return WL_SECURITY_OPEN;
  }
  virtual ENUM_WIRELESS_SECURITY GetNetWlan0Security(int)
  {
    TRACE;
    return WL_SECURITY_OPEN;
  }
  virtual void SetNetWlan0Security(ENUM_WIRELESS_SECURITY, int)
  {
    TRACE;
  }
  virtual char * GetNetWlan0WepKey()
  {
    TRACE;
    return "GetNetWlan0WepKey";
  }
  virtual char * GetNetWlan0WepKey(int)
  {
    TRACE;
    return "GetNetWlan0WepKey_idx";
  }
  virtual void SetNetWlan0WepKey(char *, int)
  {
    TRACE;
  }
  virtual int GetNetWlan0ActiveWepKey()
  {
    TRACE;
    return 0;
  }
  virtual int GetNetWlan0ActiveWepKey(int)
  {
    TRACE;
    return 0;
  }
  virtual void SetNetWlan0ActiveWepKey(int, int)
  {
    TRACE;
  }
  virtual IPV4 * GetNetWlan0DhcpdHostIP()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual IPV4 * GetNetWlan0DhcpdHostIP(int)
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetWlan0DhcpdHostIP(IPV4 *, int)
  {
    TRACE;
  }
  virtual IPV4 * GetNetWlan0DhcpdStartIP()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual IPV4 * GetNetWlan0DhcpdStartIP(int)
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetWlan0DhcpdStartIP(IPV4 *, int)
  {
    TRACE;
  }
  virtual IPV4 * GetNetWlan0DhcpdEndIP()
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual IPV4 * GetNetWlan0DhcpdEndIP(int)
  {
    static IPV4 dummy;
    TRACE;
    return &dummy;
  }
  virtual void SetNetWlan0DhcpdEndIP(IPV4 *, int)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetScreenSaver()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetScreenSaver(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetKenBurns()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetKenBurns(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_ON_OFF GetMoviePreview()
  {
    TRACE;
    return SET_OFF;
  }
  virtual void SetMoviePreview(ENUM_ON_OFF)
  {
    TRACE;
  }
  virtual ENUM_JUMP_DURATION GetJumpDuration()
  {
    TRACE;
    return JUMP_5_MIN;
  }
  virtual void SetJumpDuration(ENUM_JUMP_DURATION)
  {
    TRACE;
  }
  virtual ENUM_DEFAULT_SUBTITLE GetDefaultSubtitle()
  {
    TRACE;
    return DEF_SUB_NONE;
  }
  virtual void SetDefaultSubtitle(ENUM_DEFAULT_SUBTITLE)
  {
    TRACE;
  }
  virtual ENUM_SLEEP_TIMER GetSleepTimer()
  {
    TRACE;
    return SLEEP_TIMER_OFF;
  }
  virtual void SetSleepTimer(ENUM_SLEEP_TIMER)
  {
    TRACE;
  }
  virtual bool GetDivXCode(char **code)
  {
    char *c = "GetDivXCode";
    TRACE;
    *code = c;
    return true;
  }
  virtual void SetDivXCode(char)
  {
    TRACE;
  }
};
