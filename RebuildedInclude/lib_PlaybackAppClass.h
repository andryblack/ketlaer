/**/
enum SI_CHANNEL_FLAG {SI_CHANNEL_FLAG_IGNORE, SI_CHANNEL_FLAG_IS_DELETED, SI_CHANNEL_FLAG_IS_LOCKED, SI_CHANNEL_FLAG_IS_FAVORITE = 4, SI_CHANNEL_FLAG_IS_HIDDEN = 8, SI_CHANNEL_FLAG_NEW = 256, SI_CHANNEL_FLAG_DISAPPEARED = 512, SI_CHANNEL_FLAG_IS_OLD_KEEP = 1024, SI_CHANNEL_FLAG_IS_NEW_NOT_KEEP = 2048, SI_CHANNEL_FLAG_PAT_NEW = 4096, SI_CHANNEL_FLAG_IS_DTV = 1048576, SI_CHANNEL_FLAG_IS_RADIO = 2097152, SI_CHANNEL_FLAG_IS_SCRAMBLED = 268435456, SI_CHANNEL_FLAG_MASK = 4294967295};

enum SI_MESSAGE {SI_MESSAGE_NOMESSAGE, SI_MESSAGE_STREAM_CHANGE_BEGIN, SI_MESSAGE_STREAM_CHANGE_END, SI_MESSAGE_FOUND_A_NEW_CH, SI_MESSAGE_CH_INFO_READY, SI_MESSAGE_CH_SCAN_FAIL, SI_MESSAGE_RESET_CHANNEL, SI_MESSAGE_NEW_PMT, SI_MESSAGE_NEW_PRESENT_EIT, SI_MESSAGE_NEW_FOLLOWING_EIT, SI_MESSAGE_NEW_SCHEDULE_EIT, SI_MESSAGE_EPG_RESERVATION, SI_MESSAGE_ALARM_TIMEOUT, SI_MESSAGE_RESET_TIME, SI_MESSAGE_LIVE_CHANGECHANNEL_DONE, SI_MESSAGE_CUR_CH_DISAPPEAR, SI_MESSAGE_LAST_CH_APPEAR, SI_MESSAGE_CHANNEL_UPDATE};

enum SI_LANG {SI_LANG_UNKNOWN, SI_LANG_ENG = 6647399, SI_LANG_AUS = 6387059, SI_LANG_GLA = 6777953, SI_LANG_ITA = 6911073, SI_LANG_SPA = 7565409, SI_LANG_JPN = 6975598, SI_LANG_FIN = 6711662, SI_LANG_DAN = 6578542, SI_LANG_TUR = 7632242, SI_LANG_KOR = 7040882, SI_LANG_POR = 7368562, SI_LANG_POL = 7368556, SI_LANG_RUS = 7501171, SI_LANG_SWE = 7567205, SI_LANG_TON = 7630702, SI_LANG_ZHO = 8022127, SI_LANG_CHI = 6514793, SI_LANG_NLD = 7236708, SI_LANG_NLA = 7236705, SI_LANG_DUT = 6583668, SI_LANG_FRA = 6713953, SI_LANG_FRE = 6713957, SI_LANG_DEU = 6579573, SI_LANG_GER = 6776178, SI_LANG_CES = 6514035, SI_LANG_CZE = 6519397, SI_LANG_ORIGINAL_AUDIO = 7430497};

enum SI_CODEC_TYPE {SI_CODEC_UNKNOWN, SI_CODEC_VIDEO_11172, SI_CODEC_VIDEO_13818, SI_CODEC_VIDEO_14496, SI_CODEC_VIDEO_H264, SI_CODEC_VIDEO_VC1, SI_CODEC_AUDIO_11172, SI_CODEC_AUDIO_13818, SI_CODEC_AUDIO_AC3, SI_CODEC_AUDIO_E_AC3, SI_CODEC_AUDIO_DTS, SI_CODEC_AUDIO_AAC, SI_CODEC_AUDIO_LATM_AAC, SI_CODEC_AUDIO_LPCM, SI_CODEC_AUDIO_HDMV_MLP, SI_CODEC_AUDIO_HDMV_AC3, SI_CODEC_AUDIO_HDMV_DDP, SI_CODEC_AUDIO_HDMV_DTS, SI_CODEC_AUDIO_HDMV_DTS_HD, SI_CODEC_SP_HDMV, SI_CODEC_DEFAULT, SI_CODEC_OTHERS};

class SI;
class SI_CHANNEL_EXT;
/**/

/*system/Include/Application/AppClass/VideoPlayback.h*/
enum _PLAYBACK_TYPE {PLAYBACK_TYPE_NORMAL, PLAYBACK_TYPE_TIMESHIFT, PLAYBACK_TYPE_LIVEPAUSE, PLAYBACK_TYPE_AUDIO_ONLY, PLAYBACK_TYPE_THUMBNAIL_DECODING, PLAYBACK_TYPE_UNINITIALIZED};

typedef _PLAYBACK_TYPE PLAYBACK_TYPE;

enum _SCAN_FILE_STATE {SCAN_FILE_STATE_OFF, SCAN_FILE_STATE_ON, SCAN_FILE_STATE_STOPPING, SCAN_FILE_STATE_COMPLETED};

typedef _SCAN_FILE_STATE SCAN_FILE_STATE;

enum _EIO_HANLDE_MODE {EIO_TREATED_AS_FATAL_ERROR, EIO_TREATED_AS_READ_ERROR};

typedef _EIO_HANLDE_MODE EIO_HANDLE_MODE;

enum _SI_HANDLE_STATE {SI_HANDLE_STATE_UNINITIALIZED, SI_HANDLE_STATE_TIMESHIFT, SI_HANDLE_STATE_LIVEPAUSE, SI_HANDLE_STATE_NORMAL};

typedef _SI_HANDLE_STATE SI_HANDLE_STATE;

enum _TS_START_MODE {TS_START_TIME, TS_START_POS};

typedef _TS_START_MODE TS_START_MODE;

class convert_language_code {
public:
  unsigned int language_code;
  int string_id;
};

typedef convert_language_code ISO_639_LANGUAGE_TABLE;

class _tag_disc_identify_key {
public:
  int size;
  unsigned char signature[16];
};

typedef _tag_disc_identify_key DISC_IDENTIFY_KEY;

class _tag_disc_identify_value {
public:
  int size;
  unsigned int flags;
};

typedef _tag_disc_identify_value DISC_IDENTIFY_VALUE;

enum _PLAYBACK_SUBTITLE_TYPE {PLAYBACK_TYPE_DVB_SUBTITLE, PLAYBACK_TYPE_TT_SUBTITLE, PLAYBACK_TYPE_NO_SUBTITLE};

typedef _PLAYBACK_SUBTITLE_TYPE PLAYBACK_SUBTITLE_TYPE;

class CPlaybackFlowManager;
class CNavigationFilter;
class CReferenceClock;
class CMPEG2Decoder;
class CVideoOutFilter;
class CAudioDecFilter;
class CAudioPPAOutFilter;
class CFilterSpu;
class INavControl;

class VideoPlayback {
public:
  CPlaybackFlowManager * m_pFManager;
  CNavigationFilter * m_pSource;
  CReferenceClock * m_pRefClock;
  CMPEG2Decoder * m_pVDecoder;
  CVideoOutFilter * m_pVideoOut;
  CAudioDecFilter * m_pADecoder;
  CAudioPPAOutFilter * m_pAudioOut;
  CFilterSpu * m_pSPUDecoder;
  INavControl * m_pNavControl;
  char * m_mediaPath;
  SCAN_FILE_STATE m_scanFileState;
  bool m_scanFile_StopByOthers;
  SI * m_siHandle;
private:
  osal_mutex_t m_mutex;
  osal_event_t m_event;
  osal_thread_t m_fileScan_thread_id;
  ENUM_MEDIA_TYPE m_decoderType;
  NAVMEDIAINFO m_mediaInfo;
  short unsigned int m_subtitlePid;
  VOCAL_REMOVAL_MODE m_vocal_removal_mode;
  PLAYBACK_TYPE m_currType;
  PLAYBACK_SUBTITLE_TYPE m_SubtitleType;
  char m_iCurSubtitleIdx;
  EIO_HANDLE_MODE m_eioMode;
  SI_HANDLE_STATE m_siState;
  TS_START_MODE m_tsStartMode;
  int64_t m_tsStartPos;
  unsigned int m_tsStartSeconds;
  VIDEO_DNR_MODE m_dnrMode;
  int m_videoStreamBufSize;
  int m_videoCmdBufSize;
public:
  VideoPlayback(ENUM_MEDIA_TYPE type);
  ~VideoPlayback();
  void TearDown();
  void ClearEventQueue();
  HRESULT LoadMedia(char * mediapath,
  	            NAVMEDIAINFO *pMediaInfo = NULL,
  	            ENUM_MEDIA_TYPE preDeterminedType = MEDIATYPE_None,
  	            PLAYBACK_TYPE playbackType = PLAYBACK_TYPE_NORMAL,
  	            bool *pbContinue = NULL,
  	            NAVLOADFAILURE *pFailure = NULL,
  	            int videoStreamBufSize = 8388608, int videoCmdBufSize = 262144);
  HRESULT UnloadMedia();
  int ScanFileStart(bool forceScan);
  void ScanFileStop();
  HRESULT GetSiMessage(SI_MESSAGE * pMessage, unsigned int * pData);
  HRESULT PlayChannel(int chIndex, bool showVideo, bool bAudioFocus);
  HRESULT PlayLivepause();
  HRESULT ResetCurFreqChannel();
  HRESULT SetNextAudio(int * audio_index, int * tot_stream_num, NAVAUDIOATTR * audio_attr);
  HRESULT SetNextSubtitle();
  HRESULT SetSubtitle(short unsigned int subtitlePid);
  HRESULT RemoveSubtitle();
  HRESULT SetStartPoint(int64_t pos);
  HRESULT SetStartTime(unsigned int startSeconds);
  HRESULT SetAudio(short unsigned int pid, unsigned int langCode, short unsigned int index);
  HRESULT SetDNR(VIDEO_DNR_MODE mode);
  HRESULT PlayPrevChannel(bool showVideo, bool bAudioFocus, SI_CHANNEL_FLAG filterOutFilter);
  HRESULT PlayNextChannel(bool showVideo, bool bAudioFocus, SI_CHANNEL_FLAG filterOutFilter);
  HRESULT SetSubtitle(int index, PLAYBACK_SUBTITLE_TYPE subsType, bool bShow);
  HRESULT SetSubtitle();
  HRESULT RemoveSubtitle(PLAYBACK_SUBTITLE_TYPE subsType);
  HRESULT SetTeletext(short unsigned int teletextPid, SI_LANG langCode, unsigned char index);
  HRESULT RemoveTeletext();
  HRESULT GetCurChRating(char & rating);
  HRESULT GetLivePauseProgRating(unsigned char & rating);
  HRESULT HideSubtitle(PLAYBACK_SUBTITLE_TYPE subsType, bool bHide);
  HRESULT GetCurChannel(SI_CHANNEL_EXT * channel);
  HRESULT RenewChannelList();
  HRESULT GetSubtitleMsg(char * pStr);
  PLAYBACK_SUBTITLE_TYPE GetSubtitleType();
  int GetSubtitleCurrIdx();
  NAVMEDIAINFO GetCurMediaType();
  PLAYBACK_TYPE GetCurType();
  void SetVocalRemovalMode(VOCAL_REMOVAL_MODE mode);
  int CurrentPositionOfProgram(int whole);
  HRESULT SetVideoOut(VO_VIDEO_PLANE layer, unsigned char realTimeSrc, unsigned char zeroBuffer);
  HRESULT SetFocus();
  void SetEIO(EIO_HANDLE_MODE );
  FILTER_STATE GetFlowState();
  HRESULT SetRepeatMode(NAV_REPEAT_MODE );
  HRESULT DecodeThumbnail(NAVTHUMBNAILPARA param, int threshold, int startPicNum, int endPicNum, bool bSpecifyTime, int titleNum, unsigned int startSeconds, unsigned int startFrameIdx, bool restoreFlow, bool * bAbort);
  void ReconstructFlow(PLAYBACK_TYPE type, int videoStreamBufSize, int videoCmdBufSize);
  static ENUM_MEDIA_TYPE SI_AUDIO_CODEC_TO_ENUM(SI_CODEC_TYPE );
  static unsigned int SI_LANG_TO_ISO639(SI_LANG );
private:
  static void fileScan_thread(void * param);
  HRESULT Lock();
  HRESULT Unlock();
  bool playing_iso_image(char * path);
  HRESULT load_iso_image(char * mediapath, ENUM_MEDIA_TYPE preDeterminedType, bool * pbContinue, NAVLOADFAILURE * pFailure);
  void unmount_iso_image();
  HRESULT load_avchd_media(char * mediapath, bool * pbContinue, NAVLOADFAILURE * pFailure);
  void set_si_state(SI_HANDLE_STATE state);
};

class CVideoOutFilter {
public:
  static HRESULT SetSystemRescaleMode(VO_RESCALE_MODE RESMode);
};

