
/*
 * NEVER change function order in this classes, they have to be
 * binary compatible with the library!
 */

#define setup SetupClass::m_ptr

class _IPV4 {
public:
  unsigned char ucIP[4];
};

typedef _IPV4 IPV4;

#define TRACEOK \
printf("OK:SetupClass::%s(), %s:%d\n", __FUNCTION__, __FILE__, __LINE__)
#define TRACENI \
printf("NI:SetupClass::%s(), %s:%d\n", __FUNCTION__, __FILE__, __LINE__)

/*system/Include/Application/AppClass/SetupClass.h*/
class SetupClass {
private:
  ENUM_VIDEO_SYSTEM   video_system;
  ENUM_VIDEO_STANDARD video_standard;
  ENUM_ASPECT_RATIO   aspect_ratio;
  int                 brightness;
  int                 contrast;
  ENUM_VIDEO_OUTPUT   video_output;
  ENUM_SPEAKER_OUT    speaker_out;
  ENUM_AUDIO_AGC_MODE agc_mode;
  ENUM_SPDIF_MODE     spdif_mode;

public:
  static SetupClass * m_ptr;
  SetupClass()
  {
    /*USED*/
    video_system   = VIDEO_HD720_50HZ;
    video_standard = VIDEO_PROGRESSIVE;
    aspect_ratio   = Wide_16_9;
    brightness     = 31;
    contrast       = 31;
    video_output   = VOUT_YUV;
    speaker_out    = TWO_CHANNEL;
    agc_mode       = AUDIO_AGC_DRC_OFF;
    spdif_mode     = HDMI_LPCM;
  }
  virtual ~SetupClass()
  {
  }
  virtual void Init()
  {
    TRACENI;
  }
  virtual void Create()
  {
    TRACENI;
  }
  virtual HRESULT Sync()
  {
    TRACENI;
    return 0;
  }
  virtual HRESULT AutoSync()
  {
    TRACENI;
    return 0;
  }
  virtual void RestoreFactoryDefault()
  {
    TRACENI;
  }
  virtual int SetNetWlan0ProfileCopy(int, int)
  {
    TRACENI;
    return 0;
  }
  virtual int SetNetWlan0ProfileToDefault(int)
  {
    TRACENI;
    return 0;
  }
  virtual RECTANGLE_DIMENSION GetDimension(ENUM_VIDEO_SYSTEM)
  {
    RECTANGLE_DIMENSION ret;
    TRACENI;
    memset(&ret, 0, sizeof(ret));
    return ret;
  }
  virtual ENUM_OSD_LANGUAGE GetOsdLanguage()
  {
    TRACENI;
    return OSD_LANG_ENGLISH;
  }
  virtual void SetOsdLanguage(ENUM_OSD_LANGUAGE)
  {
    TRACENI;
  }
  virtual ENUM_AUDIO_LANGUAGE GetAudioLanguage()
  {
    TRACENI;
    return AUDIO_LANG_ENGLISH;
  }
  virtual void SetAudioLanguage(ENUM_AUDIO_LANGUAGE)
  {
    TRACENI;
  }
  virtual ENUM_SUBTITLE_LANGUAGE GetSubtitleLanguage()
  {
    TRACENI;
    return SUBTITLE_LANG_ENGLISH;
  }
  virtual void SetSubtitleLanguage(ENUM_SUBTITLE_LANGUAGE)
  {
    TRACENI;
  }
  virtual ENUM_MENU_LANGUAGE GetMenuLanguage()
  {
    TRACENI;
    return MENU_LANG_ENGLISH;
  }
  virtual void SetMenuLanguage(ENUM_MENU_LANGUAGE)
  {
    TRACENI;
  }
  virtual ENUM_TEXT_ENCODING GetTextSubtitleEncoding()
  {
    TRACENI;
    return TEXT_ENCODING_UTF8;
  }
  virtual void SetTextSubtitleEncoding(ENUM_TEXT_ENCODING)
  {
    TRACENI;
  }
  virtual int GetTextSubtitleYOffset()
  {
    TRACENI;
    return 0;
  }
  virtual void SetTextSubtitleYOffset(int)
  {
    TRACENI;
  }
  virtual int GetTextSubtitleSize()
  {
    TRACENI;
    return 10;
  }
  virtual void SetTextSubtitleSize(int)
  {
    TRACENI;
  }
  virtual ENUM_SUBTITLE_TEXT_COLOR GetTextSubtitleColor()
  {
    TRACENI;
    return SUBTITLE_TEXT_WHITE;
  }
  virtual void SetTextSubtitleColor(ENUM_SUBTITLE_TEXT_COLOR)
  {
    TRACENI;
  }
  virtual int GetTextSubtitleOffset()
  {
    TRACENI;
    return 0;
  }
  virtual void SetTextSubtitleOffset(int)
  {
    TRACENI;
  }
  virtual ENUM_VIDEO_SYSTEM GetTvSystem()
  {
    /*USED*/
    TRACEOK;
    return video_system;
  }
  virtual void SetTvSystem(ENUM_VIDEO_SYSTEM a1)
  {
    /*USED*/
    TRACEOK;
    video_system = a1;
  }
  virtual ENUM_VIDEO_SYSTEM GetVideoSDSystem()
  {
    TRACENI;
    return VIDEO_HD720_50HZ;
  }
  virtual void SetVideoSDSystem(ENUM_VIDEO_SYSTEM)
  {
    TRACENI;
  }
  virtual ENUM_VIDEO_HD_SYSTEM GetVideoHDSystem()
  {
    TRACENI;
    return VIDEO_HD_720P;
  }
  virtual void SetVideoHDSystem(ENUM_VIDEO_HD_SYSTEM)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetVideoSystem1080P24HZ()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetVideoSystem1080P24HZ(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual int GetErrorConcealmentLevel()

  {
    TRACENI;
    return 0;
  }
  virtual void SetErrorConcealmentLevel(int)
  {
    TRACENI;
  }
  virtual ENUM_TV_SYSTEM_AUTO_MODE GetTvSystemAutoMode()
  {
    TRACENI;
    return TV_SYSTEM_AUTO_MODE_HDMI_EDID;
  }
  virtual void SetTvSystemAutoMode(ENUM_TV_SYSTEM_AUTO_MODE)
  {
    TRACENI;
  }
  virtual ENUM_VIDEO_STANDARD GetTvStandard()
  {
    /*USED*/
    TRACEOK;
    return video_standard;
  }
  virtual void SetTvStandard(ENUM_VIDEO_STANDARD a1)
  {
    /*USED*/
    TRACEOK;
    video_standard = a1;
  }
  virtual ENUM_VIDEO_REC_SYSTEM GetRecTvSystem()
  {
    TRACENI;
    return VIDEO_REC_PAL;
  }
  virtual void SetRecTvSystem(ENUM_VIDEO_REC_SYSTEM)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetAutoTvSystem()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetAutoTvSystem(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetVideoPreprocessing()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetVideoPreprocessing(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetCopyProtection()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetCopyProtection(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_REC_QUALITY GetRecQuality()
  {
    TRACENI;
    return RECORD_HQ;
  }
  virtual void SetRecQuality(ENUM_REC_QUALITY)
  {
    TRACENI;
  }
  virtual ENUM_AUTO_CHAPTER GetAutoChapter()
  {
    TRACENI;
    return OFF;
  }
  virtual void SetAutoChapter(ENUM_AUTO_CHAPTER)
  {
    TRACENI;
  }
  virtual ENUM_REC_DEVICE GetRecDevice()
  {
    TRACENI;
    return REC_TO_USB_AUTO;
  }
  virtual void SetRecDevice(ENUM_REC_DEVICE)
  {
    TRACENI;
  }
  virtual ENUM_REC_DEVICE GetTimeShiftDevice()
  {
    TRACENI;
    return REC_TO_USB_AUTO;
  }
  virtual void SetTimeShiftDevice(ENUM_REC_DEVICE)
  {
    TRACENI;
  }
  virtual char * GetLastRecFile()
  {
    TRACENI;
    return "/tmp/LastRecFile";
  }
  virtual void SetLastRecFile(char*)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetAutoTimeshift()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetAutoTimeshift(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_SLIDE_SHOW_TIME GetSlideShowTime()
  {
    TRACENI;
    return SLIDE_SHOW_2S;
  }
  virtual void SetSlideShowTime(ENUM_SLIDE_SHOW_TIME)
  {
    TRACENI;
  }
  virtual ENUM_TRANS_EFFECT GetTeansEffect()
  {
    TRACENI;
    return TRANS_EFFECT_OFF;
  }
  virtual void SetTeansEffect(ENUM_TRANS_EFFECT)
  {
    TRACENI;
  }
  virtual ENUM_MIX_PLAY GetMixPlay()
  {
    TRACENI;
    return MIX_PLAY_OFF;
  }
  virtual void SetMixPlay(ENUM_MIX_PLAY)
  {
    TRACENI;
  }
  virtual ENUM_TIME_ZONE GetTimeZone()
  {
    TRACENI;
    return BTN_TIME_ZONE01;
  }
  virtual void SetTimeZone(ENUM_TIME_ZONE)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetDayLightSaving()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetDayLightSaving(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetPowerSaving()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetPowerSaving(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual int GetRatingIsBlockUnrated()
  {
    TRACENI;
    return 0;
  }
  virtual void SetRatingIsBlockUnrated(int)
  {
    TRACENI;
  }
  virtual int GetRatingIsEnable()
  {
    TRACENI;
    return 0;
  }
  virtual void SetRatingIsEnable(int)
  {
    TRACENI;
  }
  virtual ENUM_RATING_LEVEL GetRatingLevel()
  {
    TRACENI;
    return RATING_OFF;
  }
  virtual void SetRatingLevel(ENUM_RATING_LEVEL)
  {
    TRACENI;
  }
  virtual ENUM_RATING_REGION GetRatingRegion()
  {
    TRACENI;
    return RATING_DEFAULT;
  }
  virtual void SetRatingRegion(ENUM_RATING_REGION)
  {
    TRACENI;
  }
  virtual unsigned char GetRatingType(ENUM_RATING_LEVEL)
  {
    TRACENI;
    return 0;
  }
  virtual void SetRatingType(ENUM_RATING_LEVEL, unsigned char)
  {
    TRACENI;
  }
  virtual OPENVCHIP_TABLE_INFO * GetOpenVChipTable()
  {
    TRACENI;
    return NULL;
  }
  virtual OPEN_VCHIP_RATING_INFO * GetOpenVChipRatingInfo()
  {
    TRACENI;
    return NULL;
  }
  virtual void SetOpenVChip()
  {
    TRACENI;
  }
  virtual int GetRatingTimeout()
  {
    TRACENI;
    return 0;
  }
  virtual void SetRatingTimeout(int)
  {
    TRACENI;
  }
  virtual int GetRatingPasswd()
  {
    TRACENI;
    return 0;
  }
  virtual void SetRatingPasswd(int)
  {
    TRACENI;
  }
  virtual bool IsRatingPasswdCorrect(int)
  {
    TRACENI;
    return true;
  }
  virtual int GetTVRatingBitwiseValue()
  {
    TRACENI;
    return 0;
  }
  virtual void SetTVRatingBitwiseValue(int)
  {
    TRACENI;
  }
  virtual int GetTVxRatingBitwiseValue()
  {
    TRACENI;
    return 0;
  }
  virtual void SetTVxRatingBitwiseValue(int)
  {
    TRACENI;
  }
  virtual bool GetAutoPlay()
  {
    TRACENI;
    return false;
  }
  virtual void SetAutoPlay(bool)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetSeamlessPlayback()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetSeamlessPlayback(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_ASPECT_RATIO GetAspectRatio()
  {
    /*USED*/
    TRACEOK;
    return aspect_ratio;
  }
  virtual void SetAspectRatio(ENUM_ASPECT_RATIO a1)
  {
    /*USED*/
    TRACEOK;
    aspect_ratio = a1;
  }
  virtual int GetBrightness()
  {
    /*USED*/
    TRACEOK;
    return brightness;
  }
  virtual void SetBrightness(int a1)
  {
    /*USED*/
    TRACEOK;
    brightness = a1;
  }
  virtual int GetContrast()
  {
    /*USED*/
    TRACEOK;
    return contrast;
  }
  virtual void SetContrast(int a1)
  {
    /*USED*/
    TRACEOK;
    contrast = a1;
  }
  virtual ENUM_VIDEO_OUTPUT GetVideoOutput()
  {
    /*USED*/
    TRACEOK;
    return video_output;
  }
  virtual void SetVideoOutput(ENUM_VIDEO_OUTPUT a1)
  {
    /*USED*/
    TRACEOK;
    video_output = a1;
  }
  virtual ENUM_ON_OFF GetVideoDNRMode()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetVideoDNRMode(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetVideoZoomOutMode()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetVideoZoomOutMode(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual int GetCurrentHdmiTVVid()
  {
    TRACENI;
    return 0;
  }
  virtual void SetCurrentHdmiTVVid(int)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetAngleMark()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetAngleMark(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_REGION_SETTING GetRegionCode()
  {
    TRACENI;
    return REGION_CODE_ZERO;
  }
  virtual void SetRegionCode(ENUM_REGION_SETTING)
  {
    TRACENI;
  }
  virtual ENUM_SECURITY_SETTING GetSecuritySetting()
  {
    TRACENI;
    return SECURITY_ANONYMOUS;
  }
  virtual void SetSecuritySetting(ENUM_SECURITY_SETTING)
  {
    TRACENI;
  }
  virtual bool GetInitialFlag()
  {
    TRACENI;
    return false;
  }
  virtual void SetInitialFlag(bool a1)
  {
    TRACENI;
  }
  virtual ENUM_SPEAKER_OUT GetSpeakerOut()
  {
    /*USED*/
    TRACEOK;
    return speaker_out;
  }
  virtual void SetSpeakerOut(ENUM_SPEAKER_OUT a1)
  {
    /*USED*/
    TRACEOK;
    speaker_out = a1;
  }
  virtual ENUM_AUDIO_DIGITAL_OUTPUT_MODE GetHdmiAudioOutputMode()
  {
    TRACENI;
    return AUDIO_DIGITAL_LPCM_DUAL_CH;
  }
  virtual void SetHdmiAudioOutputMode(ENUM_AUDIO_DIGITAL_OUTPUT_MODE)
  {
    TRACENI;
  }
  virtual ENUM_AUDIO_DIGITAL_OUTPUT_MODE GetSpdifAudioOutputMode()
  {
    TRACENI;
    return AUDIO_DIGITAL_LPCM_DUAL_CH;
  }
  virtual void SetSpdifAudioOutputMode(ENUM_AUDIO_DIGITAL_OUTPUT_MODE)
  {
    TRACENI;
  }
  virtual ENUM_SPDIF_MODE GetSpdifMode()
  {
    /*USED*/
    TRACENI;
    return spdif_mode;
  }
  virtual void SetSpdifMode(ENUM_SPDIF_MODE a1)
  {
    /*USED*/
    TRACENI;
    spdif_mode = a1;
  }
  virtual bool GetDolbyPrologicEnable()
  {
    TRACENI;
    return false;
  }
  virtual void SetDolbyPrologicEnable(bool)
  {
    TRACENI;
  }
  virtual ENUM_AUDIO_AGC_MODE GetAudioAGCMode()
  {
    /*USED*/
    TRACEOK;
    return agc_mode;
  }
  virtual void SetAudioAGCMode(ENUM_AUDIO_AGC_MODE a1)
  {
    /*USED*/
    TRACEOK;
    agc_mode = a1;
  }
  virtual void SetSortingModeHDD(SortMode)
  {
    TRACENI;
  }
  virtual SortMode GetSortingModeHDD()
  {
    TRACENI;
    return SORT_BY_NAME;
  }
  virtual void SetSortingModeDISC(SortMode)
  {
    TRACENI;
  }
  virtual SortMode GetSortingModeDISC()
  {
    TRACENI;
    return SORT_BY_NAME;
  }
  virtual void SetKaraokeMode(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetKaraokeMode()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual int GetMic1Volume()
  {
    TRACENI;
    return 0;
  }
  virtual void SetMic1Volume(int)
  {
    TRACENI;
  }
  virtual int GetMic2Volume()
  {
    TRACENI;
    return 0;
  }
  virtual void SetMic2Volume(int)
  {
    TRACENI;
  }
  virtual int GetMusicVolume()
  {
    TRACENI;
    return 0;
  }
  virtual void SetMusicVolume(int)
  {
    TRACENI;
  }
  virtual int GetMicEcho()
  {
    TRACENI;
    return 0;
  }
  virtual void SetMicEcho(int)
  {
    TRACENI;
  }
  virtual VOCAL_REMOVAL_MODE GetVocalRemovalMode()
  {
    TRACENI;
    return ENUM_VOCAL_REMOVAL_NONE;
  }
  virtual void SetVocalRemovalMode(VOCAL_REMOVAL_MODE)
  {
    TRACENI;
  }
  virtual ENUM_KEY_SHIFT_STONE GetKeyShiftStone()
  {
    TRACENI;
    return ENUM_KEY_SHIFT_STONE_0;
  }
  virtual void SetKeyShiftStone(ENUM_KEY_SHIFT_STONE)
  {
    TRACENI;
  }
  virtual ENUM_EQUALIZER_MODE GetEqualizerMode()
  {
    TRACENI;
    return ENUM_EQUALIZER_RESERVED;
  }
  virtual void SetEqualizerMode(ENUM_EQUALIZER_MODE)
  {
    TRACENI;
  }
  virtual KARAOKE_REVERB_MODE GetReverbMode()
  {
    TRACENI;
    return REVERB_OFF;
  }
  virtual void SetReverbMode(KARAOKE_REVERB_MODE)
  {
    TRACENI;
  }
  virtual SourceOption GetSource()
  {
    TRACENI;
    return SOURCE_Tuner;
  }
  virtual void SetSource(SourceOption)
  {
    TRACENI;
  }
  virtual DeviceOption GetDevice()
  {
    TRACENI;
    return DEVICE_HDD;
  }
  virtual void SetDevice(DeviceOption)
  {
    TRACENI;
  }
  virtual int GetVolume()
  {
    TRACENI;
    return 31;
  }
  virtual void SetVolume(int)
  {
    TRACENI;
  }
  virtual int GetCurrChannel()
  {
    TRACENI;
    return 0;
  }
  virtual void SetCurrChannel(int)
  {
    TRACENI;
  }
  virtual int GetCurDtvChannelIndex()
  {
    TRACENI;
    return 0;
  }
  virtual void SetCurDtvChannelIndex(int)
  {
    TRACENI;
  }
  virtual DTV_REGION GetDtvRegion()
  {
    TRACENI;
    return DTV_REGION_DEFAULT;
  }
  virtual void SetDtvRegion(DTV_REGION)
  {
    TRACENI;
  }
  virtual RESUME_SETTING GetResumeMode()
  {
    TRACENI;
    return RESUME_NONE;
  }
  virtual void SetResumeMode(RESUME_SETTING)
  {
    TRACENI;
  }
  virtual unsigned int GetSignature(void **sig)
  {
    char *s = "GetSignature";
    TRACENI;
    *sig = (void*)s;
    return strlen(s);
  }
  virtual void SetSignature(void*, unsigned int)
  {
    TRACENI;
  }
  virtual unsigned int GetBookmark(void **book)
  {
    char *b = "GetBookmark";
    TRACENI;
    *book = (void*)b;
    return strlen(b);
  }
  virtual void SetBookmark(void*, unsigned int)
  {
    TRACENI;
  }
  virtual bool GetNetEth0Enable()
  {
    TRACENI;
    return true;
  }
  virtual void SetNetEth0Enable(bool)
  {
    TRACENI;
  }
  virtual bool GetNetEth0DHCPEnable()
  {
    TRACENI;
    return true;
  }
  virtual void SetNetEth0DHCPEnable(bool)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetEth0IPAddr()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetEth0IPAddr(IPV4*)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetEth0Mask()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetEth0Mask(IPV4*)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetEth0Gateway()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetEth0Gateway(IPV4*)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetDNS()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetDNS(IPV4*)
  {
    TRACENI;
  }
  virtual bool GetNetWlan0Enable()
  {
    TRACENI;
    return false;
  }
  virtual void SetNetWlan0Enable(bool)
  {
    TRACENI;
  }
  virtual int GetNetWlan0Profile()
  {
    TRACENI;
    return 0;
  }
  virtual void SetNetWlan0Profile(int)
  {
    TRACENI;
  }
  virtual int GetNetWlan0ProfilePreSetting()
  {
    TRACENI;
    return 0;
  }
  virtual void SetNetWlan0ProfilePreSetting(int)
  {
    TRACENI;
  }
  virtual char *GetNetWlan0ProfileName()
  {
    TRACENI;
    return "GetNetWlan0ProfileName";
  }
  virtual char *GetNetWlan0ProfileName(int)
  {
    TRACENI;
    return "GetNetWlan0ProfileName_idx";
  }
  virtual void SetNetWlan0ProfileName(char*, int)
  {
    TRACENI;
  }
  virtual ENUM_WIRELESS_MODE GetNetWlan0Mode()
  {
    TRACENI;
    return WL_MODE_INFRASTRUCTURE;
  }
  virtual ENUM_WIRELESS_MODE GetNetWlan0Mode(int)
  {
    TRACENI;
    return WL_MODE_INFRASTRUCTURE;
  }
  virtual void SetNetWlan0Mode(ENUM_WIRELESS_MODE, int)
  {
    TRACENI;
  }
  virtual bool GetNetWlan0DHCPEnable()
  {
    TRACENI;
    return true;
  }
  virtual bool GetNetWlan0DHCPEnable(int)
  {
    TRACENI;
    return true;
  }
  virtual void SetNetWlan0DHCPEnable(bool, int)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetWlan0IPAddr()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual IPV4 * GetNetWlan0IPAddr(int)
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetWlan0IPAddr(IPV4*, int)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetWlan0Mask()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual IPV4 *GetNetWlan0Mask(int)
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetWlan0Mask(IPV4*, int)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetWlan0Gateway()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual IPV4 *GetNetWlan0Gateway(int)
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetWlan0Gateway(IPV4 *a1, int a2)
  {
    TRACENI;
  }
  virtual IPV4 * GetNetWlan0DNS()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual IPV4 *GetNetWlan0DNS(int)
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetWlan0DNS(IPV4*, int)
  {
    TRACENI;
  }
  virtual char *GetNetWlan0ApName()
  {
    TRACENI;
    return "GetNetWlan0ApName";
  }
  virtual char *GetNetWlan0ApName(int)
  {
    TRACENI;
    return "GetNetWlan0ApName_idx";
  }
  virtual void SetNetWlan0ApName(char*, int)
  {
    TRACENI;
  }
  virtual ENUM_WIRELESS_SECURITY GetNetWlan0Security()
  {
    TRACENI;
    return WL_SECURITY_OPEN;
  }
  virtual ENUM_WIRELESS_SECURITY GetNetWlan0Security(int)
  {
    TRACENI;
    return WL_SECURITY_OPEN;
  }
  virtual void SetNetWlan0Security(ENUM_WIRELESS_SECURITY, int)
  {
    TRACENI;
  }
  virtual char *GetNetWlan0WepKey()
  {
    TRACENI;
    return "GetNetWlan0WepKey";
  }
  virtual char *GetNetWlan0WepKey(int)
  {
    TRACENI;
    return "GetNetWlan0WepKey_idx";
  }
  virtual void SetNetWlan0WepKey(char*, int)
  {
    TRACENI;
  }
  virtual int GetNetWlan0ActiveWepKey()
  {
    TRACENI;
    return 0;
  }
  virtual int GetNetWlan0ActiveWepKey(int)
  {
    TRACENI;
    return 0;
  }
  virtual void SetNetWlan0ActiveWepKey(int, int)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetWlan0DhcpdHostIP()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual IPV4 *GetNetWlan0DhcpdHostIP(int)
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetWlan0DhcpdHostIP(IPV4*, int)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetWlan0DhcpdStartIP()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual IPV4 *GetNetWlan0DhcpdStartIP(int)
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetWlan0DhcpdStartIP(IPV4*, int)
  {
    TRACENI;
  }
  virtual IPV4 *GetNetWlan0DhcpdEndIP()
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual IPV4 *GetNetWlan0DhcpdEndIP(int)
  {
    static IPV4 dummy;
    TRACENI;
    return &dummy;
  }
  virtual void SetNetWlan0DhcpdEndIP(IPV4*, int)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetScreenSaver()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetScreenSaver(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetKenBurns()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetKenBurns(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_ON_OFF GetMoviePreview()
  {
    TRACENI;
    return SET_OFF;
  }
  virtual void SetMoviePreview(ENUM_ON_OFF)
  {
    TRACENI;
  }
  virtual ENUM_JUMP_DURATION GetJumpDuration()
  {
    TRACENI;
    return JUMP_5_MIN;
  }
  virtual void SetJumpDuration(ENUM_JUMP_DURATION)
  {
    TRACENI;
  }
  virtual ENUM_DEFAULT_SUBTITLE GetDefaultSubtitle()
  {
    TRACENI;
    return DEF_SUB_NONE;
  }
  virtual void SetDefaultSubtitle(ENUM_DEFAULT_SUBTITLE)
  {
    TRACENI;
  }
  virtual ENUM_SLEEP_TIMER GetSleepTimer()
  {
    TRACENI;
    return SLEEP_TIMER_OFF;
  }
  virtual void SetSleepTimer(ENUM_SLEEP_TIMER)
  {
    TRACENI;
  }
  virtual bool GetDivXCode(char **code)
  {
    char *c = "GetDivXCode";
    TRACENI;
    *code = c;
    return true;
  }
  virtual void SetDivXCode(char*)
  {
    TRACENI;
  }
};
