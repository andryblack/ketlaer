typedef unsigned char UINT8;

/*system/Include/video_quality.h*/
enum _rec_quality {RECORD_HQ, RECORD_SP, RECORD_LP, RECORD_EP, RECORD_SLP, RECORD_SEP, RECORD_SPP, RECORD_QUALITY_MAX};

typedef _rec_quality ENUM_REC_QUALITY;

/*system/Include/Application/AppClass/setupdef_rec_device.h*/
enum _rec_device {REC_TO_HDD, REC_TO_USB_AUTO, REC_TO_USB_PORT1, REC_TO_USB_PORT2, REC_TO_USB_PORT3, REC_TO_USB_PORT4, REC_TO_DVD, REC_TO_HDD_ESATA, REC_TO_AUTO};

typedef _rec_device ENUM_REC_DEVICE;


/*system/Include/Application/AppClass/setupdef_source_option.h*/
enum SourceOption {SOURCE_Tuner, SOURCE_CVBS1, SOURCE_CVBS2, SOURCE_SVIDEO0, SOURCE_SVIDEO1, SOURCE_YPbPr0, SOURCE_YPbPr1, SOURCE_SCART, SOURCE_DV, SOURCE_PAYTV, SOURCE_DTV, SOURCE_HDMI, SOURCE_MAX_NUM};


/*system/Include/Application/AppClass/setupdef.h*/
enum on_off {SET_OFF, SET_ON};

typedef on_off ENUM_ON_OFF;

enum osd_language {OSD_LANG_INVALID = -1, OSD_LANG_ENGLISH, OSD_LANG_CHINESE, OSD_LANG_JAPANESE, OSD_LANG_SPANISH, OSD_LANG_FRENCH, OSD_LANG_GERMAN, OSD_LANG_ITALIAN, OSD_LANG_KOREAN, OSD_LANG_DUTCH, OSD_LANG_RUSSIAN, OSD_LANG_SCHINESE, OSD_LANG_PORTUGAL, OSD_LANG_VIETNAMESE, OSD_LANG_MAX_NUM};

typedef osd_language ENUM_OSD_LANGUAGE;

enum text_encoding {TEXT_ENCODING_UTF8, TEXT_ENCODING_GBK, TEXT_ENCODING_BIG5, TEXT_ENCODING_WESTERN, TEXT_ENCODING_TURKISH, TEXT_ENCODING_CENTRAL_EUROPEAN, TEXT_ENCODING_GREEK, TEXT_ENCODING_CYRILLIC, TEXT_ENCODING_HEBREW, TEXT_ENCODING_SOUTH_EASTERN_EUROPEAN, TEXT_ENCODING_JAPANESE, TEXT_ENCODING_KOREAN, TEXT_ENCODING_MAX_NUM};

typedef text_encoding ENUM_TEXT_ENCODING;

enum subtitle_text_color {SUBTITLE_TEXT_WHITE, SUBTITLE_TEXT_BLACK, SUBTITLE_TEXT_RED, SUBTITLE_TEXT_GREEN, SUBTITLE_TEXT_BLUE, SUBTITLE_TEXT_YELLOW, SUBTITLE_TEXT_MAX_NUM};

typedef subtitle_text_color ENUM_SUBTITLE_TEXT_COLOR;

enum audio_language {AUDIO_LANG_ENGLISH, AUDIO_LANG_CHINESE, AUDIO_LANG_JAPANESE, AUDIO_LANG_SPANISH, AUDIO_LANG_FRENCH, AUDIO_LANG_GERMAN, AUDIO_LANG_ITALIAN, AUDIO_LANG_KOREAN, AUDIO_LANG_DUTCH, AUDIO_LANG_RUSSIAN, AUDIO_LANG_SCHINESE, AUDIO_LANG_HUNGARIAN, AUDIO_LANG_MAX_NUM};

typedef audio_language ENUM_AUDIO_LANGUAGE;

enum subtitle_language {SUBTITLE_LANG_OFF, SUBTITLE_LANG_ENGLISH, SUBTITLE_LANG_CHINESE, SUBTITLE_LANG_JAPANESE, SUBTITLE_LANG_SPANISH, SUBTITLE_LANG_FRENCH, SUBTITLE_LANG_GERMAN, SUBTITLE_LANG_ITALIAN, SUBTITLE_LANG_KOREAN, SUBTITLE_LANG_DUTCH, SUBTITLE_LANG_RUSSIAN, SUBTITLE_LANG_SCHINESE, SUBTITLE_LANG_HUNGARIAN, SUBTITLE_LANG_MAX_NUM};

typedef subtitle_language ENUM_SUBTITLE_LANGUAGE;

enum menu_language {MENU_LANG_ENGLISH, MENU_LANG_CHINESE, MENU_LANG_JAPANESE, MENU_LANG_SPANISH, MENU_LANG_FRENCH, MENU_LANG_GERMAN, MENU_LANG_ITALIAN, MENU_LANG_KOREAN, MENU_LANG_DUTCH, MENU_LANG_RUSSIAN, MENU_LANG_SCHINESE, MENU_LANG_MAX_NUM};

typedef menu_language ENUM_MENU_LANGUAGE;

class _language_setting {
public:
  ENUM_OSD_LANGUAGE osd_lang;
  ENUM_TEXT_ENCODING text_subtitle_encoding;
  ENUM_AUDIO_LANGUAGE audio_lang;
  ENUM_SUBTITLE_LANGUAGE subtitle_lang;
  ENUM_MENU_LANGUAGE menu_lang;
};

typedef _language_setting LANGUAGE_SETTING;

enum _speaker_out {TWO_CHANNEL, FIVE_P_ONE_CHANNEL, SPEAKER_MAX};

typedef _speaker_out ENUM_SPEAKER_OUT;

enum _audio_agc_mode {AUDIO_AGC_DRC_OFF, AUDIO_AGC_DRC_ON, AUDIO_AGC_COMFORT};

typedef _audio_agc_mode ENUM_AUDIO_AGC_MODE;

enum _spdif_mode {SPDIF_RAW, SPDIF_LPCM, HDMI_RAW, HDMI_LPCM, HDMI_LPCM_MULTI_CH, SPDIF_MAX};

typedef _spdif_mode ENUM_SPDIF_MODE;

enum _audio_digital_output_mode {AUDIO_DIGITAL_RAW, AUDIO_DIGITAL_LPCM_DUAL_CH, AUDIO_DIGITAL_LPCM_MULTI_CH, AUDIO_DIGITAL_MAX};

typedef _audio_digital_output_mode ENUM_AUDIO_DIGITAL_OUTPUT_MODE;

class _audio_setting {
public:
  ENUM_SPEAKER_OUT speaker_out;
  ENUM_AUDIO_DIGITAL_OUTPUT_MODE hdmi_output_mode;
  ENUM_AUDIO_DIGITAL_OUTPUT_MODE spdif_output_mode;
  ENUM_SPDIF_MODE spdif_mode;
  ENUM_AUDIO_AGC_MODE agc_mode;
  bool dolby_prologic_enable;
};

typedef _audio_setting AUDIO_SETTING;

enum video_system {VIDEO_NTSC, VIDEO_PAL, VIDEO_HD720_50HZ, VIDEO_HD720_60HZ, VIDEO_HD1080_50HZ, VIDEO_HD1080_60HZ, VIDEO_HD1080_24HZ, VIDEO_SVGA800x600, VIDEO_SYSTEM_NUM};

typedef video_system ENUM_VIDEO_SYSTEM;

enum tv_system_auto_mode {TV_SYSTEM_AUTO_MODE_OFF, TV_SYSTEM_AUTO_MODE_PLAYBACK, TV_SYSTEM_AUTO_MODE_HDMI_EDID};

typedef tv_system_auto_mode ENUM_TV_SYSTEM_AUTO_MODE;

enum video_hd_system {VIDEO_HD_NONE, VIDEO_HD_720P, VIDEO_HD_1080I, VIDEO_HD_SYSTEM_NUM};

typedef video_hd_system ENUM_VIDEO_HD_SYSTEM;

enum video_rec_system {VIDEO_REC_NTSC, VIDEO_REC_PAL, VIDEO_REC_AUTO};

typedef video_rec_system ENUM_VIDEO_REC_SYSTEM;

enum video_standard {VIDEO_INTERLACED, VIDEO_PROGRESSIVE};

typedef video_standard ENUM_VIDEO_STANDARD;

enum _aspect_ratio {PS_4_3, LB_4_3, Wide_16_9, Wide_16_10, ASPECT_RATIO_MAX};

enum ENUM_ASPECT_RATIO {ASPECT_RATIO_Forbidden, ASPECT_RATIO_SquareSample, ASPECT_RATIO_4_3, ASPECT_RATIO_16_9, ASPECT_RATIO_221_1, ASPECT_RATIO_Reserved};

enum video_output {VOUT_YUV, VOUT_RGB, VOUT_MAX};

typedef video_output ENUM_VIDEO_OUTPUT;

enum VOCAL_REMOVAL_MODE {ENUM_VOCAL_REMOVAL_NONE, ENUM_VOCAL_REMOVAL_LEFT_CHANNEL, ENUM_VOCAL_REMOVAL_RIGHT_CHANNEL, ENUM_VOCAL_REMOVAL_LEFT_RIGHT_DIFFERENCE, ENUM_VOCAL_REMOVAL_ADVANCED};

enum ENUM_JUMP_DURATION {JUMP_1_MIN, JUMP_5_MIN, JUMP_10_MIN, JUMP_15_MIN, JUMP_20_MIN, JUMP_25_MIN, JUMP_30_MIN};

enum ENUM_DEFAULT_SUBTITLE {DEF_SUB_NONE, DEF_SUB_INTERNAL, DEF_SUB_EXTERNAL, DEF_SUB_SYSTEM};

class _video_setting {
public:
  ENUM_VIDEO_SYSTEM video_system;
  ENUM_VIDEO_STANDARD video_standard;
  ENUM_TV_SYSTEM_AUTO_MODE tv_system_auto_mode;
  ENUM_ASPECT_RATIO aspect_ratio;
  ENUM_VIDEO_OUTPUT video_output;
  ENUM_VIDEO_REC_SYSTEM video_rec_system;
  ENUM_ON_OFF auto_video_system;
  ENUM_ON_OFF dnr_mode;
  ENUM_ON_OFF zoom_out_mode;
  ENUM_ON_OFF preprocessing;
  ENUM_ON_OFF aware_copy_protection;
  int brightness;
  int contrast;
  int CurrentHdmiTVVid;
  ENUM_VIDEO_SYSTEM sd_system;
  ENUM_VIDEO_HD_SYSTEM hd_system;
  ENUM_ON_OFF video_system_1080p24hz;
  int error_concealment_level;
  ENUM_JUMP_DURATION jump_duration;
  ENUM_DEFAULT_SUBTITLE default_subtitle;
};

typedef _video_setting VIDEO_SETTING;

class _rectangle_dimension {
public:
  int x;
  int y;
  int w;
  int h;
};

typedef _rectangle_dimension RECTANGLE_DIMENSION;

enum _auto_chapter {OFF, FIFTEEN_MINUTES, TWENTY_MINUTES, THIRTY_MINUTES, AUTO_CHAPTER_MAX, FIVE_MINUTES, TEN_MINUTES};

typedef _auto_chapter ENUM_AUTO_CHAPTER;

class _rec_setting {
public:
  ENUM_REC_QUALITY quality;
  ENUM_AUTO_CHAPTER auto_chapter;
  ENUM_ON_OFF auto_timeshift;
  ENUM_REC_DEVICE rec_device;
  ENUM_REC_DEVICE timeshift_device;
  char last_rec_file[256];
};

typedef _rec_setting REC_SETTING;

enum _slide_show_time {SLIDE_SHOW_2S, SLIDE_SHOW_5S, SLIDE_SHOW_10S, SLIDE_SHOW_30S, SLIDE_SHOW_60S, SLIDE_SHOW_120S, SLIDE_SHOW_MAX};

typedef _slide_show_time ENUM_SLIDE_SHOW_TIME;

enum _trans_effect {TRANS_EFFECT_OFF, TRANS_EFFECT_CROSS_FADE, TRANS_EFFECT_LEFT_TO_RIGHT, TRANS_EFFECT_TOP_TO_BOTTOM, TRANS_EFFECT_WATERFALL, TRANS_EFFECT_SNAKE, TRANS_EFFECT_DISSOLVE, TRANS_EFFECT_STRIP_LEFT_DOWN, TRANS_EFFECT_SHUFFLE};

typedef _trans_effect ENUM_TRANS_EFFECT;

enum _mix_play {MIX_PLAY_OFF, MIX_PLAY_FOLDER, MIX_PLAY_PLAYLIST};

typedef _mix_play ENUM_MIX_PLAY;

class _play_setting {
public:
  ENUM_SLIDE_SHOW_TIME slide_show_time;
  ENUM_TRANS_EFFECT trans_effect;
  ENUM_MIX_PLAY mix_play;
};

typedef _play_setting PLAY_SETTING;

enum time_zone {BTN_TIME_ZONE01, BTN_TIME_ZONE02, BTN_TIME_ZONE03, BTN_TIME_ZONE04, BTN_TIME_ZONE05, BTN_TIME_ZONE06, BTN_TIME_ZONE07, BTN_TIME_ZONE08, BTN_TIME_ZONE09, BTN_TIME_ZONE10, BTN_TIME_ZONE11, BTN_TIME_ZONE12, BTN_TIME_ZONE13, BTN_TIME_ZONE14, BTN_TIME_ZONE15, BTN_TIME_ZONE16, BTN_TIME_ZONE17, BTN_TIME_ZONE18, BTN_TIME_ZONE19, BTN_TIME_ZONE20, BTN_TIME_ZONE21, BTN_TIME_ZONE22, BTN_TIME_ZONE23, BTN_TIME_ZONE24, BTN_TIME_ZONE25, BTN_TIME_ZONE26, BTN_TIME_ZONE_NUM};

typedef time_zone ENUM_TIME_ZONE;

class _OPENVCHIP_VALUE {
public:
  unsigned char abbrevRatingLen;
  unsigned char abbrevRating[10];
};

typedef _OPENVCHIP_VALUE OPENVCHIP_VALUE;

class _OPENVCHIP_DIMENSION {
public:
  unsigned char nameLen;
  unsigned char graduatedScale;
  unsigned char valueCount;
  unsigned char name[21];
  OPENVCHIP_VALUE value[15];
};

typedef _OPENVCHIP_DIMENSION OPENVCHIP_DIMENSION;

class _OPENVCHIP_TABLE {
public:
  unsigned char nameLen;
  unsigned char dimensionCount;
  char isValid;
  unsigned char name[36];
  OPENVCHIP_DIMENSION dimension[255];
};

typedef _OPENVCHIP_TABLE OPENVCHIP_TABLE;

class _OPEN_VCHIP_TABLE_INFO {
public:
  unsigned char tableCount;
  OPENVCHIP_TABLE table[1];
};

typedef _OPEN_VCHIP_TABLE_INFO OPENVCHIP_TABLE_INFO;

class _OPENVCHIP_RATING_VALUE {
public:
  char rating;
};

typedef _OPENVCHIP_RATING_VALUE OPENVCHIP_RATING_VALUE;

class _OPENVCHIP_RATING_DIMENSION {
public:
  OPENVCHIP_RATING_VALUE value[15];
};

typedef _OPENVCHIP_RATING_DIMENSION OPENVCHIP_RATING_DIMENSION;

class _OPENVCHIP_RATING {
public:
  OPENVCHIP_RATING_DIMENSION dimension[255];
};

typedef _OPENVCHIP_RATING OPENVCHIP_RATING;

class _OPEN_VCHIP_RATING_INFO {
public:
  OPENVCHIP_RATING table[1];
};

typedef _OPEN_VCHIP_RATING_INFO OPEN_VCHIP_RATING_INFO;

enum _rating_level {RATING_OFF, RATE_1, RATE_2, RATE_3, RATE_4, RATE_5, RATE_6, RATE_7, RATE_8, RATE_MAX};

typedef _rating_level ENUM_RATING_LEVEL;

enum _rating_region {RATING_DEFAULT, RATING_FRANCE, RATING_GERMANY, RATING_ITALY};

typedef _rating_region ENUM_RATING_REGION;

class _rating_setting {
public:
  int isEnable;
  int isBlockUnrated;
  ENUM_RATING_REGION rating_region;
  int passwd;
  int timeout;
  ENUM_RATING_LEVEL rating_level;
  int tv_rating_level;
  int tv_x_rating_level;
  unsigned char rating_type[9];
  OPENVCHIP_TABLE_INFO openVChip;
  OPEN_VCHIP_RATING_INFO openVChipRating;
};

typedef class {
  public:
    UINT8 SystemType;
    UINT8 Level;
    UINT8 DVSL;
  } RATING;

enum region_setting {REGION_CODE_ZERO, REGION_CODE_ONE, REGION_CODE_TWO, REGION_CODE_THREE, REGION_CODE_FOUR, REGION_CODE_FIVE, REGION_CODE_SIX, REGION_CODE_SEVEN, REGION_CODE_EIGHT, REGION_CODE_MAX_NUM};

typedef region_setting ENUM_REGION_SETTING;

enum security_setting {SECURITY_ANONYMOUS, SECURITY_USER};

typedef security_setting ENUM_SECURITY_SETTING;

enum ENUM_SLEEP_TIMER {SLEEP_TIMER_OFF, SLEEP_TIMER_5_MIN, SLEEP_TIMER_10_MIN, SLEEP_TIMER_15_MIN, SLEEP_TIMER_20_MIN, SLEEP_TIMER_30_MIN, SLEEP_TIMER_45_MIN, SLEEP_TIMER_60_MIN, SLEEP_TIMER_90_MIN, SLEEP_TIMER_120_MIN, SLEEP_TIMER_NUM};

class _system_setting {
public:
  ENUM_TIME_ZONE time_zone;
  ENUM_ON_OFF day_light_saving;
  bool bAutoPlay;
  ENUM_ON_OFF seamless_playback;
  ENUM_ON_OFF angle_mark;
  ENUM_REGION_SETTING region;
  ENUM_SECURITY_SETTING security;
  ENUM_ON_OFF power_saving;
  bool initial;
  RATING rating;
  ENUM_SLEEP_TIMER sleep_timer;
};

typedef _system_setting SYSTEM_SETTING;

enum OutputOption {OUTPUT_CVBS, OUTPUT_SVIDEO, OUTPUT_COMPONENT, OUTPUT_SCART, OUTPUT_DSUB, OUTPUT_HDMI, OUTPUT_MAX_NUM};

enum SortMode {SORT_BY_NAME, SORT_BY_TIME, SORT_BY_NUMBER, SORT_BY_NEW_TITLE};

enum DeviceOption {DEVICE_HDD, DEVICE_DISC};

enum KARAOKE_REVERB_MODE {REVERB_OFF, REVERB_ON_CATHEDRAL_1, REVERB_ON_CATHEDRAL_2, REVERB_ON_HALL, REVERB_ON_ROOM, REVERB_ON_BASEMENT};

enum DTV_REGION {DTV_REGION_TAIWAN, DTV_REGION_DEFAULT, DTV_REGION_BERLIN, DTV_REGION_BIRMINGHAM, DTV_REGION_CHINA, DTV_REGION_COPENHAGEN, DTV_REGION_HAMBURG, DTV_REGION_HANNOVER, DTV_REGION_HELSINKI, DTV_REGION_LONDON, DTV_REGION_MADRID, DTV_REGION_PARIS, DTV_REGION_ROMA, DTV_REGION_STOCKHOLM, DTV_REGION_SYDNEY};

class _misc_setting {
public:
  SourceOption m_source;
  DeviceOption m_device;
  int m_channel;
  int m_channel_d;
  int m_volume;
  SortMode m_SortingModeHDD;
  SortMode m_SortingModeDISC;
  DTV_REGION m_dtvRegion;
  ENUM_ON_OFF m_KaraokeMode;
  int m_mic_1_volume;
  int m_mic_2_volume;
  int m_music_volume;
  int m_mic_echo_strength;
  VOCAL_REMOVAL_MODE m_vocal_removal_mode;
  ENUM_KEY_SHIFT_STONE m_keyshift_stone;
  ENUM_EQUALIZER_MODE m_equalizer_mode;
  KARAOKE_REVERB_MODE m_reverb_mode;
  ENUM_ON_OFF screen_saver;
  ENUM_ON_OFF ken_burns;
  ENUM_ON_OFF movie_preview;
  int m_subtitleYOffset;
  int m_subtitleSize;
  ENUM_SUBTITLE_TEXT_COLOR m_subtitleColor;
  int m_subOffset;
  char m_divx_code[32];
};

typedef _misc_setting MISC_SETTING;

enum resume_setting {RESUME_NONE, RESUME_STOP, RESUME_POWERON};

typedef resume_setting RESUME_SETTING;

class _bookmark_setting {
public:
  RESUME_SETTING resume_mode;
  unsigned int signatureSize;
  char signature[16];
  unsigned int stateSize;
  char state[4096];
};

typedef _bookmark_setting BOOKMARK_SETTING;

enum wireless_security {WL_SECURITY_OPEN, WL_SECURITY_WEP, WL_SECURITY_WPA, WL_SECURITY_WEP_SHAREKEY, WL_SECURITY_UNKNOWN};

typedef wireless_security ENUM_WIRELESS_SECURITY;

enum wireless_mode {WL_MODE_INFRASTRUCTURE, WL_MODE_P2P};

typedef wireless_mode ENUM_WIRELESS_MODE;


