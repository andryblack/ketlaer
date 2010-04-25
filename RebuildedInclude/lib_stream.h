/*system/Include/FlowEvent.h*/
class _tagEVRecordVobuPayload {
public:
  int numOfFields;
  int size;
};

typedef _tagEVRecordVobuPayload EVRecordVobuPayload;

class _tagEventSpicFrame {
public:
  unsigned int startTime;
  unsigned int duration;
  unsigned int type;
  unsigned int size;
  int charsetEncoding;
  char * payload;
};

typedef _tagEventSpicFrame EventSpicFrame;

enum EVCode {EV_FILE_EOF, FE_RECORD_STOP, FE_RECORD_VOBU, FE_RECORD_CELL, FE_RECORD_VOB, FE_RECORD_FULL, EV_DISK_FULL, FE_RECORD_NOSIGNAL, FE_RECORD_SIGNAL, FE_RECORD_COPYFREEDATA, FE_RECORD_COPYPROCTECTEDDATA, FE_RECORD_NTSC, FE_RECORD_PAL, FE_RECORD_VIDEO_FORMAT_CHANGE, FE_RECORD_NO_OUTPUT_SPACE, FE_RECORD_OUTPUT_SPACE_AVAILABLE, FE_RECORD_DISC_ERROR, FE_Playback_ResetSpeed, FE_Playback_VideoEOS, FE_Playback_AudioEOS, FE_Playback_IndexGenEOS, FE_Playback_Completed, FE_Playback_DomainChange, FE_Playback_TitleChange, FE_Playback_ChapterChange, FE_Playback_TimeChange, FE_Playback_RateChange, FE_Playback_CommandUpdate, FE_Playback_RequestTmpPml, FE_Playback_SetAutoStop, FE_Playback_JPEGDecEOS, FE_Playback_ResetRepeatMode, FE_Playback_FatalError, FE_Playback_ReadError, FE_Playback_BitstreamError, FE_Playback_StreamingInfoUpdate, FE_Playback_BufferingStart, FE_Playback_BufferingRestart, FE_Playback_BufferingEnd, FE_Playback_SetAudioPreference, FE_Playback_NoSignal, FE_Playback_DRM_NOT_AUTH_USER, FE_Playback_DRM_RENTAL_EXPIRED, FE_Playback_DRM_REQUEST_RENTAL, FE_Playback_AudioFatalError, FE_Playback_BackwardToBeginning, FE_Playback_UnsupportedFormat, FE_Playback_InvalidFile, FE_Playback_SpicFrame, FE_Playback_ContentEncrypt, FE_Timeshift_PlaybackCatchRecord, FE_Timeshift_RecordCatchPlayback, FE_Timeshift_LivePauseWrapAround, FE_TRANSCODE_STOP, FE_TRANSCODE_VE_STOP, FE_TRANSCODE_AE_STOP};


/*system/Include/StreamClass/EType.h*/
enum PIN_STATUS {PIN_STATUS_NONE, PIN_STATUS_IDLE, PIN_STATUS_RECEIVING, PIN_STATUS_FLUSHING};

enum FILTER_TYPE {FILTER_NONE_SPECIFY, FILTER_SOURCE = 16777216, FILTER_DECODE_VIDEO = 33619968, FILTER_DECODE_AUDIO = 33685504, FILTER_DECODE_SPU = 33816576, FILTER_ENCODE_VIDEO = 67174400, FILTER_ENCODE_AUDIO = 67239936, FILTER_ENCODE_SPU = 67371008, FILTER_RENDER_VIDEO = 134283264, FILTER_RENDER_AUDIO = 134348800, FILTER_POST_VIDEO = 268500992, FILTER_POST_AUDIO = 268566528};

enum FILTER_PROPERTY {FILTER_PROP_PLAYBACK_RATE, FILTER_PROP_SETAUIDIO_PREFERENCE};

enum PIN_DIRECTION {PINDIR_INPUT, PINDIR_OUTPUT};

enum FORMAT_TYPE {FORMAT_None, FORMAT_DvInfo, FORMAT_MPEG1Video, FORMAT_MPEG2Video, FORMAT_AudioInfo};

enum FILTER_STATE {State_Stopped, State_Paused, State_Running, State_Paused_Stopped, State_Paused_Paused, State_Paused_Running, State_Transition};

enum TIME_FORMAT {TIME_FORMAT_NONE, TIME_FORMAT_FRAME, TIME_FORMAT_SAMPLE, TIME_FORMAT_FIELD, TIME_FORMAT_BYTE, TIME_FORMAT_MEDIA_TIME};

enum ALLOCATOR_STYLE {STYLE_RING, STYLE_BUFFERLIST};

enum ALLOCATOR_WAIT {WAIT_BUFFER_NO, WAIT_BUFFER_FOREVER};

enum ENUM_MEDIA_TYPE {MEDIATYPE_None, MEDIATYPE_AnalogAudio, MEDIATYPE_AnalogVideo, MEDIATYPE_Audio, MEDIATYPE_Video, MEDIATYPE_AUXLine21Data, MEDIATYPE_DVD_ENCRYPTED_PACK, MEDIATYPE_MPEG2_PES, MEDIATYPE_ScriptCommand, MEDIATYPE_Stream, MEDIATYPE_Text, MEDIATYPE_Image, MEDIATYPE_Directory, MEDIATYPE_NEPTUNE_TP, MEDIASUBTYPE_PCM, MEDIASUBTYPE_PCM_LITTLE_ENDIAN, MEDIASUBTYPE_MPEG1Packet, MEDIASUBTYPE_MPEG1ES, MEDIASUBTYPE_MPEG2_AUDIO, MEDIASUBTYPE_DVD_LPCM_AUDIO, MEDIASUBTYPE_MLP_AUDIO, MEDIASUBTYPE_DOLBY_AC3, MEDIASUBTYPE_DOLBY_AC3_SPDIF, MEDIASUBTYPE_MPEG_AUDIO, MEDIASUBTYPE_DTS, MEDIASUBTYPE_DTS_HD, MEDIASUBTYPE_DTS_HD_EXT, MEDIASUBTYPE_DTS_HD_CORE, MEDIASUBTYPE_DDP, MEDIASUBTYPE_SDDS, MEDIASUBTYPE_DV, MEDIASUBTYPE_AAC, MEDIASUBTYPE_RAW_AAC, MEDIASUBTYPE_OGG_AUDIO, MEDIASUBTYPE_WMA, MEDIASUBTYPE_WMAPRO, MEDIASUBTYPE_MP3, MEDIASUBTYPE_MP4, MEDIASUBTYPE_LATM_AAC, MEDIASUBTYPE_WAVE, MEDIASUBTYPE_AIFF, MEDIASUBTYPE_RTP, MEDIASUBTYPE_DVD_SUB_PICTURE, MEDIASUBTYPE_DIVX_SUB_PICTURE, MEDIASUBTYPE_DVB_SUBTITLE_SUB_PICTURE, MEDIASUBTYPE_DVB_TT_SUB_PICTURE, MEDIASUBTYPE_ATSC_CC_SUB_PICTURE, MEDIASUBTYPE_BLURAY_PGS_SUB_PICTURE, MEDIASUBTYPE_DIVX3_VIDEO, MEDIASUBTYPE_DIVX_VIDEO, MEDIASUBTYPE_MPEG4_VIDEO, MEDIASUBTYPE_MJPEG_VIDEO, MEDIASUBTYPE_DVSD_VIDEO, MEDIASUBTYPE_H264_VIDEO, MEDIASUBTYPE_H263_VIDEO, MEDIASUBTYPE_VP6_VIDEO, MEDIASUBTYPE_VC1_VIDEO, MEDIASUBTYPE_VC1_ADVANCED_VIDEO, MEDIASUBTYPE_VC1_WINDOWSMEDIA_VIDEO, MEDIATYPE_DVD, MEDIATYPE_SVCD, MEDIATYPE_VCD, MEDIATYPE_CDDA, MEDIATYPE_FILE, MEDIATYPE_PLAYLIST, MEDIATYPE_NET, MEDIATYPE_MMS, MEDIATYPE_TRANSCODE, MEDIATYPE_DV, MEDIATYPE_DV_FILE, MEDIATYPE_STB, MEDIATYPE_FILESET, MEDIATYPE_FILELINK, MEDIATYPE_RTSP, MEDIATYPE_RTP, MEDIATYPE_PPS, MEDIATYPE_THUNDER, MEDIATYPE_CUSTOM_1, MEDIATYPE_CUSTOM_2, MEDIATYPE_CUSTOM_3, MEDIASUBTYPE_MPEG2_VIDEO, MEDIASUBTYPE_DVD_SUBPICTURE, MEDIASUBTYPE_DVD_RTRW_PLUS, MEDIASUBTYPE_DVD_RTRW_MINUS, MEDIASUBTYPE_DVD_RTRW_MINUS_HD, MEDIASUBTYPE_DVD_VIDEO, MEDIASUBTYPE_DVD_VIDEO_ROM, MEDIASUBTYPE_DVD_RTRW_STILL, MEDIASUBTYPE_DVD_MENU, MEDIASUBTYPE_SVCD, MEDIASUBTYPE_SVCD_ROM, MEDIASUBTYPE_VCD, MEDIASUBTYPE_VCD_ROM, MEDIASUBTYPE_CDDA, MEDIASUBTYPE_CDDA_ROM, MEDIASUBTYPE_AVCHD, MEDIASUBTYPE_AVCHD_ROM, MEDIASUBTYPE_HTTP, MEDIASUBTYPE_MMS, MEDIASUBTYPE_TRANSCODE, MEDIASUBTYPE_MPEG_PROGRAM, MEDIASUBTYPE_STB_1394, MEDIASUBTYPE_FILESET, MEDIASUBTYPE_FILELINK, MEDIASUBTYPE_Line21_BytePair, MEDIASUBTYPE_Line21_GOPPacket, MEDIASUBTYPE_Line21_VBIRawData, MEDIASUBTYPE_MPEG1Audio, MEDIASUBTYPE_MPEG1System, MEDIASUBTYPE_MPEG1SystemStream, MEDIASUBTYPE_MPEG1Video, MEDIASUBTYPE_MPEG1VideoCD, MEDIASUBTYPE_MPEG1SuperVCD, MEDIASUBTYPE_MPEG2_PROGRAM, MEDIASUBTYPE_MPEG2_TRANSPORT, MEDIASUBTYPE_AI44, MEDIASUBTYPE_IAK2, MEDIASUBTYPE_IAK4, MEDIASUBTYPE_IAK8, MEDIASUBTYPE_SRT, MEDIASUBTYPE_SSA, MEDIASUBTYPE_ASS, MEDIASUBTYPE_TXT, MEDIASUBTYPE_AnalogVideo_NTSC, MEDIASUBTYPE_AnalogVideo_PAL, MEDIASUBTYPE_IMAGE_JPEG, MEDIASUBTYPE_IMAGE_BITMAP, MEDIASUBTYPE_IMAGE_GIF, MEDIASUBTYPE_IMAGE_TIFF, MEDIASUBTYPE_IMAGE_PNG, MEDIASUBTYPE_IMAGE_DNG, MEDIASUBTYPE_MPEG2_AUDIO_WITH_EXTENSION, MEDIASUBTYPE_MPEG2_AUDIO_WITHOUT_EXTENSION, MEDIASUBTYPE_PPSRM, MEDIASUBTYPE_PPSWMA, MEDIASUBTYPE_RM, MEDIASUBTYPE_RV, MEDIASUBTYPE_RA_COOK, MEDIASUBTYPE_RA_ATRC, MEDIASUBTYPE_RA_RAAC, MEDIASUBTYPE_RA_SIPR, MEDIASUBTYPE_RA_LSD, MEDIASUBTYPE_ADPCM, MEDIASUBTYPE_FLAC, MEDIASUBTYPE_ULAW, MEDIASUBTYPE_ALAW, MEDIASUBTYPE_DMF_0, MEDIASUBTYPE_DMF_1, MEDIASUBTYPE_DMF_2, MEDIASUBTYPE_MP4_VIDEO, MEDIATYPE_Interleaved, MEDIATYPE_FLASHVIDEO, MEDIATYPE_MATROSKA, MEDIASUBTYPE_MATROSKA_DIVX_ENHANCED, MEDIATYPE_ASF, MEDIASUBTYPE_PCM_HDMV, MEDIASUBTYPE_HDMV_MLP_AUDIO, MEDIASUBTYPE_HDMV_DOLBY_AC3, MEDIASUBTYPE_HDMV_DDP, MEDIASUBTYPE_HDMV_DTS, MEDIASUBTYPE_HDMV_DTS_HD};

enum ENUM_VIDEO_HORIZONTAL_RESOLUTION {VIDEO_SIF, VIDEO_HALF_D1, VIDEO_H480, VIDEO_H544, VIDEO_H704, VIDEO_D1, VIDEO_TotalHResolution};

typedef class {
  public:
    int64_t RCD;
    int64_t referenceClock;
    int64_t videoSystemPTS;
    int64_t audioSystemPTS;
    int64_t videoRPTS;
    int64_t audioRPTS;
    long unsigned int videoContext;
    long unsigned int audioContext;
    long int videoEndOfSegment;
  } PRESENTATION_POSITIONS;


/*system/Include/StreamClass/SType.h*/
class IFlowManager;
class IBaseFilter;
class IMemAllocator;
class IMediaSample;
class IEnumPins;
class IReferenceClock;
class IReadPointerHandle;

class FILTER_INFO {
public:
  char name[128];
  IFlowManager * pFM;
};

class PIN_INFO {
public:
  IBaseFilter * pFilter;
  PIN_DIRECTION dir;
  char achName[128];
};

class Buffer_T {
public:
  BYTE * pBuffer;
  ULONG nLen;
};

class MEDIA_TYPE {
public:
  ENUM_MEDIA_TYPE majortype;
  ENUM_MEDIA_TYPE subtype;
  long int lSampleSize;
  FORMAT_TYPE formatType;
  Buffer_T format;
};

class MEDIA_INFO {
public:
  MEDIA_TYPE mediaType;
  long int unitPerSecond;
};

class SAMPLE_INFO {
public:
  BYTE * BeginBuf;
  BYTE * pNonCachedBuffer;
  long unsigned int bufferPhyAddr;
  Buffer_T ActualBuf;
};

class ALLOCATOR_PROPERTIES {
public:
  long int numberOfBuffers;
  long int bufferSize;
  long int align;
  long int prefix;
};

typedef long int EVId;

class FILTER_SEEKING_CAP {
public:
  long int MaxSpeed;
  long int MaxReverseSpeed;
  long int MinSkip;
  long int MaxSkip;
  long int MinReverseSkip;
  long int MaxReverseSkip;
};

/*
class Array_T<IMediaSample*> {
public:
  IMediaSample * * pArray;
  int nElement;
};
*/


/*system/Include/IStream.h*/
class IPin {
public:
  virtual ~IPin();
  virtual HRESULT Connect(IPin * , MEDIA_TYPE const * );
  virtual HRESULT ConnectedTo(IPin * * * , int * );
  virtual HRESULT ReceiveConnection(IPin * , MEDIA_TYPE const * );
  virtual HRESULT ConnectionMediaType(MEDIA_TYPE * );
  virtual HRESULT UpdateConnectionMediaType(MEDIA_TYPE * );
  virtual HRESULT Disconnect();
  virtual HRESULT EndOfStream(unsigned int );
  virtual HRESULT QueryPinInfo(PIN_INFO * );
  virtual HRESULT QueryAccept(MEDIA_TYPE const * );
  virtual HRESULT NewSegment();
  virtual HRESULT GetAllocator(IMemAllocator * * );
  virtual HRESULT SetAllocator(IMemAllocator * );
  virtual HRESULT GetStatus(PIN_STATUS * );
  virtual HRESULT SetStatus(PIN_STATUS );
};

class IMemInputPin {
public:
  virtual ~IMemInputPin();
  virtual HRESULT Flush();
  virtual HRESULT Receive(IMediaSample * );
  virtual HRESULT Receive(BYTE * , long int );
  virtual HRESULT ReceiveCanBlock();
  virtual HRESULT PrivateInfo(int , BYTE * , int );
  virtual HRESULT PrivateInfo(BYTE * , int );
};

class IMemOutputPin {
public:
  virtual ~IMemOutputPin();
  virtual HRESULT Deliver(IMediaSample * );
  virtual HRESULT Deliver(BYTE * , long int );
  virtual HRESULT DeliverFlush();
  virtual HRESULT DeliverPrivateInfo(int , BYTE * , int );
  virtual HRESULT DeliverPrivateInfo(BYTE * , int );
};

class IBaseFilter {
public:
  virtual ~IBaseFilter();
  virtual HRESULT EnumPins(IEnumPins * * );
  virtual HRESULT FindPin(IPin * * , char * );
  virtual HRESULT JoinFlow(IFlowManager * , char * );
  virtual HRESULT QueryFilterInfo(FILTER_INFO * );
  virtual HRESULT FilterType(FILTER_TYPE * );
  virtual HRESULT SetFilterProperty(FILTER_PROPERTY , void * , unsigned int );
  virtual HRESULT GetAgentInstanceID(long int * );
};

class IEnumPins {
public:
  virtual ~IEnumPins();
  virtual HRESULT Next(IPin * * );
  virtual HRESULT Reset();
};

class IEnumFilters {
public:
  virtual ~IEnumFilters();
  virtual HRESULT Next(IBaseFilter * * );
  virtual HRESULT Reset();
};

class IFlowManager {
public:
  virtual ~IFlowManager();
  virtual HRESULT AddFilter(IBaseFilter * , char * );
  virtual HRESULT ConnectDirect(IPin * , IPin * , MEDIA_TYPE const * );
  virtual HRESULT Disconnect(IPin * );
  virtual HRESULT EnumFilters(IEnumFilters * * );
  virtual HRESULT FindFilterByName(IBaseFilter * * , char * );
  virtual HRESULT RemoveFilter(IBaseFilter * );
  virtual HRESULT SetRefClock(IReferenceClock * );
  virtual HRESULT Notify(EVCode , long int , long int * );
};

class IFilterControl {
public:
  virtual ~IFilterControl();
  virtual HRESULT Stop();
  virtual HRESULT Pause(FILTER_STATE );
  virtual HRESULT Run();
  virtual HRESULT Step();
  virtual HRESULT GetState(FILTER_STATE * );
  virtual HRESULT GetRefClock(IReferenceClock * * );
  virtual HRESULT SetRefClock(IReferenceClock * );
  virtual HRESULT GetMediaInfo(MEDIA_INFO * );
  virtual HRESULT ExecuteCommandLine(char * );
};

class IFlowContorl {
public:
  virtual ~IFlowContorl();
  virtual HRESULT Run();
  virtual HRESULT Pause();
  virtual HRESULT Stop();
  virtual HRESULT Step();
  virtual HRESULT GetState(long int , FILTER_STATE * );
};

class IFlowEvent {
public:
  virtual ~IFlowEvent();
  virtual HRESULT CancelDefaultHandling(EVCode );
  virtual HRESULT FreeEventParams(EVId );
  virtual HRESULT GetEvent(EVId * , EVCode * , long int * , long int * * , long int );
  virtual HRESULT FlushUserEvents();
};

class IFilterSeeking {
public:
  virtual ~IFilterSeeking();
  virtual HRESULT CheckCapabilities(FILTER_SEEKING_CAP * );
  virtual HRESULT SetSeekingInfo(long int , long int );
  virtual HRESULT GetSeekingInfo(long int * , long int * );
};

class IFlowSeeking {
public:
  virtual ~IFlowSeeking();
  virtual HRESULT CheckCapabilities(FILTER_SEEKING_CAP * );
  virtual HRESULT IsFormatSupported(TIME_FORMAT );
  virtual HRESULT GetTimeFormat(TIME_FORMAT * );
  virtual HRESULT SetTimeFormat(TIME_FORMAT );
  virtual HRESULT SetRate(long int );
  virtual HRESULT GetRate(long int * );
  virtual HRESULT SetPositions(long int * , long int , long int * , long int );
  virtual HRESULT GetPositions(long int * , long int * );
  virtual HRESULT ConverTimeFormat(long int * , TIME_FORMAT , long int , TIME_FORMAT );
};

class IMemAllocator {
public:
  virtual ~IMemAllocator();
  virtual HRESULT Commit();
  virtual HRESULT Decommit(bool );
  virtual HRESULT IsCommit();
  virtual HRESULT GetStyle(ALLOCATOR_STYLE * );
};

class IMemListAllocator : public IMemAllocator {
public:
  virtual ~IMemListAllocator();
  virtual HRESULT SetProperties(ALLOCATOR_PROPERTIES * , ALLOCATOR_PROPERTIES * );
  virtual HRESULT GetProperties(ALLOCATOR_PROPERTIES * );
  virtual HRESULT GetBuffer(IMediaSample * * , ALLOCATOR_WAIT );
  virtual HRESULT ReleaseBuffer(IMediaSample * );
};

class IMemRingAllocator : public IMemAllocator {
public:
  virtual ~IMemRingAllocator();
  virtual HRESULT GetBuffer(BYTE * * , long int * );
  virtual HRESULT SetBuffer(BYTE * , long int );
  virtual HRESULT GetWritePointer(BYTE * * , long int * );
  virtual HRESULT SetWritePointer(BYTE * );
  virtual HRESULT RequestWriteSpace(BYTE * * , long int * , long int , long int );
  virtual HRESULT GetReadPointerHandle(IReadPointerHandle * * , IMemInputPin * );
  virtual HRESULT ReleaseReadPointerHandle(IReadPointerHandle * );
};

class IMediaSample {
public:
  virtual ~IMediaSample();
  virtual HRESULT GetActualSize(ULONG * );
  virtual HRESULT GetAcutaulPointer(BYTE * * );
  virtual HRESULT GetBeginSize(ULONG * );
  virtual HRESULT GetBeginPointer(BYTE * * );
  virtual ULONG GetPhysicalAddress(ULONG );
  virtual ULONG GetNonCachedAddress(ULONG );
  virtual HRESULT GetNonCachedBeginPointer(BYTE * * );
  virtual HRESULT SetActualPointer(BYTE * , ULONG );
  virtual HRESULT Release();
  virtual HRESULT AddRef();
};

class IReadPointerHandle {
public:
  virtual ~IReadPointerHandle();
  virtual HRESULT GetReadPointer(BYTE * * , long int * );
  virtual HRESULT SetReadPointer(BYTE * );
  virtual HRESULT GetRingBuffer(BYTE * * , long int * );
  virtual HRESULT Flush();
  virtual HRESULT Release();
};

class IReferenceClock {
public:
  virtual ~IReferenceClock();
  virtual HRESULT GetGlobalSCR(int64_t * );
  virtual HRESULT GetGlobalPTS(int64_t * );
  virtual HRESULT GetLocalPTS(int64_t * );
  virtual HRESULT SetLocalPTS(int64_t );
  virtual HRESULT GetRCD(int64_t * );
  virtual HRESULT SetRCD(int64_t );
  virtual HRESULT SetTimeout(int64_t );
  virtual HRESULT SetFreeRunThreshold(long unsigned int , long unsigned int );
  virtual HRESULT SetMastership(MASTERSHIP * );
  virtual HRESULT GetMastership(MASTERSHIP * );
  virtual HRESULT GetPresentationPositions(PRESENTATION_POSITIONS * );
  virtual HRESULT ResetPresentationPositions();
  virtual HRESULT GetUnderflowStates(long int * , long int * , int64_t * );
  virtual HRESULT ResetUnderflowStates();
};


/*system/Include/StreamClass/CFlowManager.h*/

class CObject {};
class CBaseFilter;

#include <ulDArray.h>
#include <ulQueue.h>

typedef ulDArray<IBaseFilter*> IFilterArr;

class CFlowManager : public IFlowManager, public IFlowContorl, public IFlowSeeking, public IFlowEvent, public CObject {
protected:
  osal_thread_t m_thread;
  osal_mutex_t m_setRateMutex;
  bool m_bThreadRunning;
  CVariableSizeQueue<512l,32l> m_EventDefQueue;
  CVariableSizeQueue<512l,32l> m_EventUserQueue;
  long int m_lastEventID;
  IReferenceClock * m_pRefClock;
  IFilterArr m_IFilterArr;
  BYTE m_Flag;
  long int m_rate;
  bool m_bStepping;
  bool m_bNeedRefClock;
  MASTERSHIP m_mastershipBeforeStepping;
public:
  CFlowManager(bool bNeedRefClock);
  virtual ~CFlowManager();
  virtual HRESULT AddFilter(IBaseFilter * pFilter, char * Id);
  virtual HRESULT ConnectDirect(IPin * pPinOut, IPin * pPinIn, MEDIA_TYPE const * pmt);
  virtual HRESULT Disconnect(IPin * pPin);
  virtual HRESULT EnumFilters(IEnumFilters * * ppEnumFilters);
  virtual HRESULT FindFilterByName(IBaseFilter * * ppFilter, char * Id);
  virtual HRESULT RemoveFilter(IBaseFilter * pFilter);
  virtual HRESULT SetRefClock(IReferenceClock * pClock);
  HRESULT SendCommand(char * command);
  virtual HRESULT Run();
  virtual HRESULT Pause();
  virtual HRESULT Stop();
  virtual HRESULT Step();
  virtual HRESULT GetState(long int milliTimeOut, FILTER_STATE * pState);
  virtual HRESULT CheckCapabilities(FILTER_SEEKING_CAP * pSeekingCap);
  virtual HRESULT IsFormatSupported(TIME_FORMAT format);
  virtual HRESULT GetTimeFormat(TIME_FORMAT * pFormat);
  virtual HRESULT SetTimeFormat(TIME_FORMAT format);
  virtual HRESULT SetRate(long int rate);
  HRESULT CanSetRate(long int rate);
  virtual HRESULT GetRate(long int * pRate);
  virtual HRESULT SetPositions(long int * pCurrent, long int dwCurrentFlags, long int * pStop, long int dwStopFlags);
  virtual HRESULT GetPositions(long int * pCurrent, long int * pStop);
  virtual HRESULT ConverTimeFormat(long int * pTarget, TIME_FORMAT format, long int Source, TIME_FORMAT SourceFormat);
  virtual HRESULT GetEvent(EVId * pEventId, EVCode * pEventCode, long int * pParamSize, long int * * ppParams, long int msTimeout);
  virtual HRESULT FlushUserEvents();
  virtual HRESULT FreeEventParams(EVId EventId);
  virtual HRESULT CancelDefaultHandling(EVCode EventCode);
  HRESULT RestoreDefaultHandling(EVCode EventCode);
  virtual HRESULT Notify(EVCode EventCode, long int ParamSize, long int * pEventParams);
protected:
  static void Thread(void * pInstance);
  HRESULT StartDefaultHandlingThread();
  HRESULT StopDefaultHandlingThread();
  virtual void HandleDefaultEvents(EVCode evCode, long int * pParams, long int paramSize);
  virtual bool HandleCriticalEvents(EVCode evCode, long int * pParams, long int paramSize);
  virtual bool HasDefaultHandling(EVCode evCode);
  bool IsFilterSync();
  void SetFilterSync();
  bool ReOrderFilter();
  void InitFilterLevel();
  static int SortFilterByLevel(void const * p1, void const * p2);
  void Leveling(CBaseFilter * pSeed, int level, int direction);
};

/*system/Include/StreamClass/CMediaType.h*/
class CMediaType {
private:
  MEDIA_TYPE m_mt;
public:
  CMediaType();
  ~CMediaType();
  bool operator==(CMediaType const & RHS);
  bool operator==(MEDIA_TYPE const & RHS);
  bool operator==(MEDIA_TYPE const * pRHS);
  //?? const MEDIA_TYPE const * operator const MEDIA_TYPE*();
  void InitMediaType();
  void CopyTo(MEDIA_TYPE * p_mt);
  void CopyTo(MEDIA_TYPE & mt);
  void SetMajorType(ENUM_MEDIA_TYPE MajorType);
  void SetSubType(ENUM_MEDIA_TYPE SubType);
  void SetSampleSize(long int SampleSize);
  void SetFormatType(FORMAT_TYPE FormatType);
  void SetFormat(BYTE const * pBuffer, ULONG nLen);
  ENUM_MEDIA_TYPE GetMajorType();
  ENUM_MEDIA_TYPE GetSubType();
  long int GetSampleSize();
  FORMAT_TYPE GetFormatType();
  void GetFormat(BYTE * & pBuffer, ULONG & nLen);
  MEDIA_TYPE * GetMediaType();
};


/*system/Include/StreamClass/CPin.h*/
class CBasePin : public IPin, public CObject {
protected:
  CBaseFilter * m_pFilter;
  PIN_DIRECTION m_dir;
  char * m_pName;
  CMediaType m_mt;
  PIN_STATUS m_status;
public:
  CBasePin(PIN_DIRECTION dir);
  virtual ~CBasePin();
  virtual HRESULT Connect(IPin * pReceivePin, MEDIA_TYPE const * pmt);
  virtual HRESULT ConnectionMediaType(MEDIA_TYPE * pmt);
  virtual HRESULT UpdateConnectionMediaType(MEDIA_TYPE * pmt);
  virtual HRESULT EndOfStream(unsigned int eventID);
  virtual HRESULT QueryPinInfo(PIN_INFO * pInfo);
  virtual HRESULT QueryAccept(MEDIA_TYPE const * pmt);
  virtual HRESULT NewSegment();
  virtual HRESULT ReceiveConnection(IPin * pConnector, MEDIA_TYPE const * pmt);
  virtual HRESULT GetStatus(PIN_STATUS * pStatus);
  virtual HRESULT SetStatus(PIN_STATUS status);
protected:
  bool IsStopped();
  bool GetState(FILTER_STATE * pState);
  bool JoinFilter(CBaseFilter * pFilter, char * Id);
  virtual bool IsConnected();
};

class CBaseOutputPin;

class CBaseInputPin : public CBasePin, public IMemInputPin {
public:
  osal_mutex_t m_flowMutex;
protected:
  unsigned int m_flag;
  IMemAllocator * m_pAllocator;
  CBaseOutputPin * m_pOutputPin;
public:
  CBaseInputPin();
  virtual ~CBaseInputPin();
  virtual HRESULT Flush();
  virtual HRESULT Receive(IMediaSample * pISample);
  virtual HRESULT Receive(BYTE * pWritePointer, long int WriteBufferSize);
  virtual HRESULT ReceiveCanBlock();
  virtual HRESULT GetAllocator(IMemAllocator * * ppAllocator);
  virtual HRESULT SetAllocator(IMemAllocator * pAllocator);
  virtual HRESULT PrivateInfo(int infoId, BYTE * pInfo, int length);
  virtual HRESULT PrivateInfo(BYTE * pInfo, int length);
  virtual HRESULT Disconnect();
  virtual HRESULT ConnectedTo(IPin * * * ppPinArr, int * pPinArrSize);
  HRESULT Block();
  HRESULT Unblock();
protected:
  virtual HRESULT CheckStreaming();
  virtual bool IsConnected();
};

typedef ulDArray<CBaseInputPin*> CInputPinArr;

class CBaseOutputPin : public CBasePin, public IMemOutputPin {
protected:
  CInputPinArr m_InputPinArr;
public:
  CBaseOutputPin();
  virtual ~CBaseOutputPin();
  virtual HRESULT Deliver(IMediaSample * pISample);
  virtual HRESULT Deliver(BYTE * ppWritePointer, long int WriteBufferSize);
  HRESULT SetWritePointer(BYTE * wrPtr);
  virtual HRESULT DeliverFlush();
  virtual HRESULT GetAllocator(IMemAllocator * * ppAllocator);
  virtual HRESULT SetAllocator(IMemAllocator * pAllocator);
  virtual HRESULT DeliverPrivateInfo(int infoId, BYTE * pInfo, int length);
  virtual HRESULT DeliverPrivateInfo(BYTE * pInfo, int length);
  virtual HRESULT Disconnect();
  virtual HRESULT ConnectedTo(IPin * * * ppPinArr, int * pPinArrSize);
  virtual HRESULT UpdateConnectionMediaType(MEDIA_TYPE * pmt);
  virtual HRESULT EndOfStream(unsigned int eventID);
  virtual HRESULT NewSegment();
protected:
  bool Stop();
  bool Pause();
  bool Run();
  virtual bool IsConnected();
};


/*system/Include/StreamClass/CFilter.h*/
typedef ulDArray<IPin*> IPinArr;

class CBaseFilter : public IBaseFilter, public IFilterControl, public CObject {
public:
  IFlowManager * m_pFlowManager;
protected:
  FILTER_STATE m_State;
  FILTER_STATE m_StateNext;
  IPinArr m_IPinArr;
  char * m_pName;
  BYTE m_Flag;
  void * m_pUserData;
  CCritSec m_Lock;
  CSemaphore m_SemFlush;
  static char const * m_ClassId;
public:
  CBaseFilter();
  virtual ~CBaseFilter();
  virtual HRESULT EnumPins(IEnumPins * * ppEnumPins);
  virtual HRESULT FindPin(IPin * * ppIPin, char * Id);
  virtual HRESULT QueryFilterInfo(FILTER_INFO * pInfo);
  virtual HRESULT JoinFlow(IFlowManager * pFlow, char * Id);
  virtual HRESULT FilterType(FILTER_TYPE * pFilterType);
  virtual HRESULT SetFilterProperty(FILTER_PROPERTY propId, void * propData, unsigned int propSize);
  virtual HRESULT GetAgentInstanceID(long int * ID);
  virtual HRESULT Stop();
  virtual HRESULT Pause(FILTER_STATE State);
  virtual HRESULT Run();
  virtual HRESULT Step();
  virtual HRESULT GetState(FILTER_STATE * pState);
  virtual HRESULT GetRefClock(IReferenceClock * * ppClock);
  virtual HRESULT SetRefClock(IReferenceClock * pClock);
  virtual HRESULT GetMediaInfo(MEDIA_INFO * pInfo);
  virtual HRESULT ExecuteCommandLine(char * command);
protected:
  bool AddPin(CBasePin * pCPin, char * Id);
  bool IsPinSync();
  void SetPinSync();
  bool RemoveFromFlow();
  bool IsFlush();
  bool FlushRequest();
  bool FlushWaitComplete();
  bool FlushPostComplete();
};


/*system/Include/Utility/ResourceMgr/ioResource.h*/
enum RESOURCE_NAME {RESOURCE_DVD_READ, RESOURCE_DVD_WRITE, RESOURCE_HDD_READ, RESOURCE_HDD_WRITE, RESOURCE_DV_READ, RESOURCE_COUNT, RESOURCE_NONE};


/*system/Include/Util.h*/
class DATA_MEDIA_TYPE {
public:
  ENUM_MEDIA_TYPE majortype;
  ENUM_MEDIA_TYPE subtype;
};

class SUGGESTED_MEDIA_TYPE {
public:
  int mediaType;
  ENUM_MEDIA_TYPE fileType;
  ENUM_MEDIA_TYPE ioType;
};

class VIDEO_MEM_CHUNK_STRUCT {
public:
  long int address;
  long int size;
};

class VIDEO_FREEMEMORY_STRUCT {
public:
  long int numMemChunks;
  VIDEO_MEM_CHUNK_STRUCT memChunk[8];
};


/*system/Include/Filters/NavigationFilter/NavDef.h*/
enum NAV_CMD_ID {NAV_COMMAND_INVALID, NAV_COMMAND_PLAYTITLE, NAV_COMMAND_PLAYCHAPTER, NAV_COMMAND_PLAYNEXTCHAPTER, NAV_COMMAND_PLAYPREVCHAPTER, NAV_COMMAND_PLAYATTIME, NAV_COMMAND_PLAYATPOS, NAV_COMMAND_PLAYSEGMENT, NAV_COMMAND_MENUSHOW, NAV_COMMAND_MENUESCAPE, NAV_COMMAND_BUTTONSELECTNUMERIC, NAV_COMMAND_BUTTONSELECTPOINT, NAV_COMMAND_BUTTONACTIVATENUMERIC, NAV_COMMAND_BUTTONACTIVATEPOINT, NAV_COMMAND_BUTTONACTIVATE, NAV_COMMAND_BUTTONMOVESELECTION, NAV_COMMAND_GOUP, NAV_COMMAND_STILLESCAPE, NAV_COMMAND_SETAUDIOSTREAM, NAV_COMMAND_SETSUBPICTURESTREAM, NAV_COMMAND_SETANGLE, NAV_COMMAND_SETGETPROPERTY_ASYNC, NAV_COMMAND_RUN, NAV_COMMAND_PAUSE, NAV_COMMAND_STOP, NAV_COMMAND_SETSPEED, NAV_COMMAND_SETSPEED_POS, NAV_COMMAND_STILL};

enum NAV_CMD_BLK {NAV_CMDBLK_PLAYTITLE = 1, NAV_CMDBLK_PLAYCHAPTER, NAV_CMDBLK_PLAYNEXTCHAPTER = 4, NAV_CMDBLK_PLAYPREVCHAPTER = 8, NAV_CMDBLK_PLAYATTIME = 16, NAV_CMDBLK_MENUESCAPE = 32, NAV_CMDBLK_BUTTON = 64, NAV_CMDBLK_GOUP = 128, NAV_CMDBLK_STILLESCAPE = 256, NAV_CMDBLK_SETVIDEOPRESENTATIONMODE = 512, NAV_CMDBLK_SETAUDIOSTREAM = 1024, NAV_CMDBLK_SETAUDIODOWNMIXMODE = 2048, NAV_CMDBLK_SETSUBPICTURESTREAM = 4096, NAV_CMDBLK_SETANGLE = 8192, NAV_CMDBLK_PAUSE = 16384, NAV_CMDBLK_STOP = 32768, NAV_CMDBLK_FORWARDSCAN = 65536, NAV_CMDBLK_BACKWARDSCAN = 131072};

enum NAV_PROP_ID {NAVPROP_INPUT_GET_PLAYBACK_STATUS, NAVPROP_INPUT_GET_VIDEO_STATUS, NAVPROP_INPUT_GET_AUDIO_STATUS, NAVPROP_INPUT_GET_SPIC_STATUS, NAVPROP_INPUT_GET_MENU_STATUS, NAVPROP_INPUT_GET_DISC_STATUS, NAVPROP_INPUT_GET_TITLE_STATUS, NAVPROP_INPUT_GET_NAV_STATE, NAVPROP_INPUT_GET_CMD_BLK_FLAGS, NAVPROP_INPUT_GET_MEDIA_SIGNATURE, NAVPROP_INPUT_GET_PRIVATE_INFO_FB, NAVPROP_INPUT_GET_TIME_POSITION, NAVPROP_INPUT_SET_NAV_STATE, NAVPROP_INPUT_SET_NAV_STATE_FORCED, NAVPROP_INPUT_SET_REPEAT_MODE, NAVPROP_INPUT_SET_RATE_INFO, NAVPROP_INPUT_SET_VIDEO_PRES_MODE, NAVPROP_INPUT_SET_RINGBUFFER_INFO, NAVPROP_INPUT_SET_FLOW_INTERFACE, NAVPROP_INPUT_SET_THUMBNAIL_TIME_POS, NAVPROP_INPUT_SET_DECODE_THUMBNAIL, NAVPROP_INPUT_SET_DECODE_MULTI_THUMBNAIL, NAVPROP_INPUT_SET_PLAYBACK_MODE, NAVPROP_INPUT_SET_LPCM_INFO, NAVPROP_INPUT_SET_ERROR_CONCEALMENT_LEVEL, NAVPROP_INPUT_SET_AUDIO_PREFERENCE, NAVPROP_AUDIO_GET_PTS, NAVPROP_INPUT_SET_EIO_LEVEL, NAVPROP_DVDV_GET_BUTTON_JUMP_TARGET, NAVPROP_DVDV_GET_PLAYBACK_STATUS_EX, NAVPROP_DVDV_SET_PARENTAL_CONTROL, NAVPROP_DVDV_SET_PLAYER_REGION, NAVPROP_DVDV_SET_LANG_PREFERENCE, NAVPROP_DVDV_APPROVE_TMP_PARENTAL, NAVPROP_VR_GET_RTR_VMGI, NAVPROP_VR_DECODE_KEYFRAME, NAVPROP_VR_SET_BOOKMARK, NAVPROP_VR_RESTORE_BOOKMARK, NAVPROP_VR_SET_PLAYDOMAIN, NAVPROP_VR_DECODE_MULTIPLE_KEYFRAME, NAVPROP_VR_IS_NEW_REC_PG, NAVPROP_VR_DISABLE_AUTO_RESET_NEW_TITLE, NAVPROP_VR_CAN_SPEED, NAVPROP_VCD_GET_PBC_ON_OFF, NAVPROP_VCD_SET_PBC_ON_OFF, NAVPROP_ISCAN_WITH_RATE_AWARE, NAVPROP_DIVX_DRM_QUERY, NAVPROP_DIVX_DRM_APPROVE_RENTAL, NAVPROP_DIVX_EDITIONNUM_QUERY, NAVPROP_DIVX_TITLENUM_QUERY, NAVPROP_CDDA_DISCINFO, NAVPROP_DV_CTRL_CMD, NAVPROP_DV_DISPLAY_CTRL, NAVPROP_DTV_SET_TUNER_HANDLE, NAVPROP_DTV_GET_SI_ENGINE_HANDLE, NAVPROP_DTV_SET_SI_ENGINE_HANDLE, NAVPROP_DTV_SET_PREVIEW_MODE, NAVPROP_DTV_SET_RECORD_MODE, NAVPROP_DTV_GET_RECORD_STATUS, NAVPROP_DTV_GET_EDITING_INTERFACE, NAVPROP_DTV_AB_FAST_COPY, NAVPROP_DTV_GET_SI_STATE, NAVPROP_DTV_GET_AUDIO_FORMAT_FB, NAVPROP_NAV_SET_TIMEOUT_LIMITS, NAVPROP_NAV_SET_STARTUP_FULLNESS, NAVPROP_SUBTITLE_GET_FONTS, NAVPROP_MISC_HACK_UOP = 12345678};

enum NAV_BTNDIR_ID {NAV_BTNDIR_UP, NAV_BTNDIR_DOWN, NAV_BTNDIR_LEFT, NAV_BTNDIR_RIGHT};

enum NAV_DISPLAY_STATE {NAV_DISPLAY_OFF, NAV_DISPLAY_ON, NAV_DISPLAY_UNCHANGED};

enum NAV_REPEAT_MODE {NAV_REPEAT_NONE, NAV_REPEAT_TITLE, NAV_REPEAT_CHAPTER, NAV_REPEAT_AB, NAV_REPEAT_TITLE_ONCE};

enum NAV_VIDEO_PRES_MODE {NAV_VPM_NORMAL, NAV_VPM_WIDE, NAV_VPM_PAN_SCAN, NAV_VPM_LETTER_BOX};

enum NAV_TV_SYSTEM {NAV_TVSYSTEM_NTSC, NAV_TVSYSTEM_PAL, NAV_TVSYSTEM_CUSTOM, NAV_TVSYSTEM_UNKNOWN};

enum NAV_SRC_LTRBOX {NAV_SRCLTRBOX_NONE, NAV_SRCLTRBOX_NORMAL, NAV_SRCLTRBOX_14_9_CENTER, NAV_SRCLTRBOX_14_9_TOP, NAV_SRCLTRBOX_16_9_CENTER, NAV_SRCLTRBOX_16_9_TOP, NAV_SRCLTRBOX_16_9_PLUS_CENTER, NAV_SRCLTRBOX_14_9_FULL_FORMAT};

enum NAV_AUDIO_DESC {NAV_AUDIODESC_NOT_SPECIFIED, NAV_AUDIODESC_NORMAL, NAV_AUDIODESC_FOR_VISUALLY_IMPAIRED, NAV_AUDIODESC_DIRECTOR_COMMENT};

enum NAV_SPIC_DESC {NAV_SPICDESC_NOT_SPECIFIED, NAV_SPICDESC_NORMAL, NAV_SPICDESC_BIGGER, NAV_SPICDESC_CHILDREN, NAV_SPICDESC_CC_NORMAL, NAV_SPICDESC_CC_BIGGER, NAV_SPICDESC_CC_CHILDREN, NAV_SPICDESC_FORCED, NAV_SPICDESC_DIRECTOR_COMMENT_NORMAL, NAV_SPICDESC_DIRECTOR_COMMENT_BIGGER, NAV_SPICDESC_DIRECTOR_COMMENT_CHILDREN};

enum NAV_DOMAIN {NAV_DOMAIN_STOP, NAV_DOMAIN_TITLE, NAV_DOMAIN_MENU, NAV_DOMAIN_VR_PG, NAV_DOMAIN_VR_PL};

enum NAV_AVSYNC_MODE {NAV_AVSYNC_AUDIO_MASTER_AUTO, NAV_AVSYNC_AUDIO_MASTER_AUTO_SKIP, NAV_AVSYNC_AUDIO_MASTER_AUTO_AF, NAV_AVSYNC_AUDIO_MASTER, NAV_AVSYNC_SYSTEM_MASTER, NAV_AVSYNC_AUDIO_ONLY, NAV_AVSYNC_VIDEO_ONLY, NAV_AVSYNC_VIDEO_ONLY_SLIDESHOW, NAV_AVSYNC_SLIDESHOW};

enum NAV_STREAM_TYPE {NAV_STREAM_VIDEO, NAV_STREAM_AUDIO, NAV_STREAM_SPIC, NAV_STREAM_TELETEXT};

class _tagNAVTIMEOUTLIMITS {
public:
  long int maxTimeout;
  long int minTimeout;
};

typedef _tagNAVTIMEOUTLIMITS NAVTIMEOUTLIMITS;

class _tagNAVMEDIAINFO {
public:
  ENUM_MEDIA_TYPE mediaType;
  ENUM_MEDIA_TYPE videoType;
  ENUM_MEDIA_TYPE audioType;
  ENUM_MEDIA_TYPE spicType;
  NAV_AVSYNC_MODE AVSyncMode;
  long int AVSyncStartupFullness;
  long int minForwardSkip;
  long int maxForwardSkip;
  long int minReverseSkip;
  long int maxReverseSkip;
  bool bSmoothReverseCapable;
  bool bUseDDRCopy;
  bool bFlushBeforeDelivery;
};

typedef _tagNAVMEDIAINFO NAVMEDIAINFO;

class _tagNAVMEDIAINFOEX {
public:
  ENUM_MEDIA_TYPE mediaType;
  ENUM_MEDIA_TYPE videoType;
  ENUM_MEDIA_TYPE audioType;
  ENUM_MEDIA_TYPE spicType;
  void * audioPrivateData;
  unsigned int audioPrivateDataSize;
  void * videoPrivateData;
  unsigned int videoPrivateDataSize;
  void * spicPrivateData;
  unsigned int spicPrivateDataSize;
};

typedef _tagNAVMEDIAINFOEX NAVMEDIAINFOEX;

class _tagNAVIOINFO {
public:
  int64_t totalBytes;
  int totalSeconds;
  int averageBytesPerSecond;
  int seekAlignmentInBytes;
  int readSizeAlignmentInBytes;
  bool bSeekable;
  bool bSlowSeek;
  bool bStreaming;
  bool bDirectIOCapable;
  ENUM_MEDIA_TYPE ioType;
  void * ioInternalHandle;
  ENUM_MEDIA_TYPE preDeterminedType;
  NAVMEDIAINFOEX mediaInfo;
};

typedef _tagNAVIOINFO NAVIOINFO;

class _tagNAVSTREAMINGINFO {
public:
  int totalSecondsPrepared;
  int64_t totalBytesPrepared;
  int totalSeconds;
  int secondsNeededToFinishPreparation;
  NAV_STREAM_TYPE bitstreamType;
  int64_t pts;
};

typedef _tagNAVSTREAMINGINFO NAVSTREAMINGINFO;

class _tagNAVAUTOSTOPINFO {
public:
  unsigned char bActive;
  unsigned char bHasVideo;
  unsigned char bHasAudio;
  unsigned char bHasIndexGen;
};

typedef _tagNAVAUTOSTOPINFO NAVAUTOSTOPINFO;

class _tagNAVDEMUXPTSINFO {
public:
  int64_t videoPTS;
  int64_t audioPTS;
  int64_t spicPTS;
};

typedef _tagNAVDEMUXPTSINFO NAVDEMUXPTSINFO;

class _tagNAVTIME {
public:
  int seconds;
  int frameIndex;
};

typedef _tagNAVTIME NAVTIME;

class _tagNAVVIDEOATTR {
public:
  ENUM_MEDIA_TYPE type;
  NAV_TV_SYSTEM tvSystem;
  int aspectRatioX;
  int aspectRatioY;
  int frameSizeX;
  int frameSizeY;
  int frameRate;
  NAV_SRC_LTRBOX srcLtrBox;
  bool line21Switch1;
  bool line21Switch2;
  bool bAllowPanScanMode;
  bool bAllowLetterBoxMode;
  unsigned int languageCode;
  unsigned int countryCode;
  unsigned int typeCode;
};

typedef _tagNAVVIDEOATTR NAVVIDEOATTR;

class _tagNAVAUDIOATTR {
public:
  ENUM_MEDIA_TYPE type;
  unsigned int languageCode;
  NAV_AUDIO_DESC description;
  int bitsPerSample;
  int samplingRate;
  int numChannels;
  bool bEnabled;
  unsigned int countryCode;
  unsigned int typeCode;
};

typedef _tagNAVAUDIOATTR NAVAUDIOATTR;

class _tagNAVSPICATTR {
public:
  ENUM_MEDIA_TYPE type;
  unsigned int languageCode;
  NAV_SPIC_DESC description;
  bool bEnabled;
  unsigned int countryCode;
  unsigned int typeCode;
};

typedef _tagNAVSPICATTR NAVSPICATTR;

class _tagNAVPLAYBACKSTATUS {
public:
  NAV_DOMAIN domain;
  int numTitles;
  int currentTitle;
  int numChapters;
  int currentChapter;
  int numAngles;
  int currentAngle;
  int numButtons;
  NAVTIME elapsedTime;
  NAVTIME totalTime;
  NAVTIME currChapterStartTime;
  NAVTIME currChapterEndTime;
  bool bPaused;
  bool bStill;
  bool bInAngleBlock;
  NAV_REPEAT_MODE repeatMode;
  ENUM_MEDIA_TYPE mediaType;
  unsigned int languageCode;
  int bufferingSecondsLeftPrepare;
  int bufferingSecondsLeftPlayback;
  int64_t elapsedPTS;
  int64_t basePTS;
  NAVTIME transmittedTime;
};

typedef _tagNAVPLAYBACKSTATUS NAVPLAYBACKSTATUS;

class _tagNAVPLAYBACKSTATUS_DVDV {
public:
  NAVPLAYBACKSTATUS status;
  int vtsN;
  int pgcN;
  int pgN;
  int cellN;
  uint32_t vobuStartAddress;
  int frameIdxInVOBU;
  int currentButtonNumber;
  int currentButtonTargetTitle;
  int currentButton_X_Start;
  int currentButton_X_End;
  int currentButton_Y_Start;
  int currentButton_Y_End;
};

typedef _tagNAVPLAYBACKSTATUS_DVDV NAVPLAYBACKSTATUS_DVDV;

class _tagNAVVIDEOSTATUS {
public:
  int numStreams;
  int indexCurrentStream;
  NAVVIDEOATTR streamAttr[1];
  NAV_VIDEO_PRES_MODE presentationMode;
};

typedef _tagNAVVIDEOSTATUS NAVVIDEOSTATUS;

class _tagNAVAUDIOSTATUS {
public:
  int numStreams;
  int indexCurrentStream;
  NAVAUDIOATTR streamAttr[16];
};

typedef _tagNAVAUDIOSTATUS NAVAUDIOSTATUS;

class _tagNAVSPICSTATUS {
public:
  int numStreams;
  int indexCurrentStream;
  NAVSPICATTR streamAttr[32];
  bool bDummyStream;
  bool bDisplay;
};

typedef _tagNAVSPICSTATUS NAVSPICSTATUS;

class _tagNAVMENUSTATUS {
public:
  int numMenus;
  unsigned int menuID[8];
};

typedef _tagNAVMENUSTATUS NAVMENUSTATUS;

class _tagNAVDISCSTATUS {
public:
  unsigned int allowedRegions;
  ENUM_MEDIA_TYPE discType;
  ENUM_MEDIA_TYPE discSubtype;
  int isEncrypt;
};

typedef _tagNAVDISCSTATUS NAVDISCSTATUS;

class _tagNAVTITLESTATUS {
public:
  int numChapters;
  int numAngles;
  NAVTIME totalTime;
};

typedef _tagNAVTITLESTATUS NAVTITLESTATUS;

class _tagNAVCMDSTATUS {
public:
  NAV_CMD_ID executedCmdType;
  unsigned int executedCmdID;
  HRESULT executedCmdResult;
  int numOfPendingCmds;
};

typedef _tagNAVCMDSTATUS NAVCMDSTATUS;

class _tagNAVFILESESSIONINFO {
public:
  int64_t startAddress;
  int64_t endAddress;
  int64_t stcOffset;
  int channelIndex;
  ENUM_MEDIA_TYPE mediaType;
};

typedef _tagNAVFILESESSIONINFO NAVFILESESSIONINFO;

class _tagNAVRINGBUFFERINFO {
public:
  long unsigned int bufferBeginAddr_physical;
  BYTE * bufferBeginAddr_cached;
  BYTE * bufferBeginAddr_noncached;
  long unsigned int bufferSize;
  long unsigned int * pWritePointer;
  long unsigned int * pNumOfReadPointers;
  long unsigned int * pReadPointers[4];
};

typedef _tagNAVRINGBUFFERINFO NAVRINGBUFFERINFO;

class _tagNAVPARENTALCONTROL {
public:
  char countryCode[2];
  int level;
};

typedef _tagNAVPARENTALCONTROL NAVPARENTALCONTROL;

class _tagNAVLANGPREFERENCE {
public:
  short unsigned int prefMenuLangCode;
  short unsigned int prefAudioLangCode;
  NAV_AUDIO_DESC prefAudioLangDesc;
  short unsigned int prefSpicLangCode;
  NAV_SPIC_DESC prefSpicLangDesc;
};

typedef _tagNAVLANGPREFERENCE NAVLANGPREFERENCE;

class _tagNAVVCDPBCINFO {
public:
  unsigned int pbcCapability;
  unsigned int pbcState;
};

typedef _tagNAVVCDPBCINFO NAVVCDPBCINFO;

enum NAV_DIVX_DRM_ID {NAV_DIVX_DRM_NONE, NAV_DIVX_DRM_NOT_AUTH_USER, NAV_DIVX_DRM_RENTAL_EXPIRED, NAV_DIVX_DRM_REQUEST_RENTAL, NAV_DIVX_DRM_AUTHORIZED, NAV_DIVX_DRM_RENTAL_AUTHORIZED};

class _tagDIVXDRM {
public:
  NAV_DIVX_DRM_ID drmStatus;
  unsigned char useLimit;
  unsigned char useCount;
  unsigned char digitalProtection;
  unsigned char cgms_a;
  unsigned char acpt_b;
  unsigned char ict;
};

typedef _tagDIVXDRM NAVDIVXDRMINFO;

enum NAV_PREVIEW_MODE {NAV_PREVIEW_OFF, NAV_PREVIEW_ON};

enum NAV_RECORD_MODE {NAV_RECORD_OFF, NAV_RECORD_ON, NAV_RECORD_PAUSE, NAV_RECORD_RESUME};

class _tagNAVRECORDMODE {
public:
  NAV_RECORD_MODE mode;
  char * path;
  unsigned int handle;
  unsigned char bCircular;
  unsigned char bLinkingList;
  int64_t maxFileSize;
};

typedef _tagNAVRECORDMODE NAVRECORDMODE;

class _tagNAVRECORDSTATUS {
public:
  unsigned int elapsedTime;
  unsigned int recordedTime;
  unsigned int recordedBlocks;
  unsigned int totalFileBlocks;
};

typedef _tagNAVRECORDSTATUS NAVRECORDSTATUS;

class _tagNAVTHUMBNAILPARA {
public:
  unsigned int colorFormat;
  unsigned int pTargetLuma;
  unsigned int pTargetChroma;
  unsigned int pitch;
  unsigned int targetRectX;
  unsigned int targetRectY;
  unsigned int targetRectWidth;
  unsigned int targetRectHeight;
  unsigned int timePosition;
};

typedef _tagNAVTHUMBNAILPARA NAVTHUMBNAILPARA;

class _tagNAVMULTITHUMBNAILPARA {
public:
  unsigned int numThumbnail;
  unsigned int maxDecFrameCount;
  unsigned char * * pFileName;
  NAVTHUMBNAILPARA * pThumbnailPara;
};

typedef _tagNAVMULTITHUMBNAILPARA NAVMULTITHUMBNAILPARA;

typedef   enum {NAV_PLAYBACK_NORMAL, NAV_PLAYBACK_KARAOKE} NAV_PLAYBACK_MODE;

class _tagNAVLPCMINFO {
public:
  int bitsPerSample;
  int samplingRate;
  int numChannels;
  int64_t fileSize;
};

typedef _tagNAVLPCMINFO NAVLPCMINFO;

enum NAV_FAILURE_REASON {NAV_FAILURE_UNIDENTIFIED, NAV_FAILURE_UNSUPPORTED_VIDEO_CODEC, NAV_FAILURE_UNSUPPORTED_AUDIO_CODEC, NAV_FAILURE_VIDEO_PIN_UPDATE_FAILURE, NAV_FAILURE_UNSUPPORTED_DIVX_DRM_VERSION};

class _tagNAVLOADFAILURE {
public:
  NAV_FAILURE_REASON reason;
  ENUM_MEDIA_TYPE mediaType;
  ENUM_MEDIA_TYPE videoType;
  unsigned int fourCC;
  ENUM_MEDIA_TYPE audioType;
};

typedef _tagNAVLOADFAILURE NAVLOADFAILURE;


/*system/Include/Filters/NavigationFilter/NavPlugins.h*/
enum NAV_BUF_ID {NAVBUF_NONE, NAVBUF_DATA, NAVBUF_WAIT, NAVBUF_SHORT_WAIT, NAVBUF_STOP, NAVBUF_FLUSH, NAVBUF_NEW_SEGMENT, NAVBUF_END_SEGMENT, NAVBUF_NEW_MEDIA_TYPE, NAVBUF_NEW_AVSYNC_MODE, NAVBUF_ADJUST_CLOCK, NAVBUF_EVENT, NAVBUF_NEW_VIDEO_TYPE};

class _tagNAVBUF {
public:
  NAV_BUF_ID type;
  unsigned int bDoNotInterruptWithUserCmd;
  union {
      class {
    public:
      unsigned int pinIndex;
      unsigned int channelIndex;
      unsigned char * payloadData;
      unsigned int payloadSize;
      int64_t pts;
      unsigned int dataOffsetForPTS;
      unsigned int infoId;
      unsigned char * infoData;
      unsigned int infoSize;
      long unsigned int startAddress;
      long unsigned int ArrayAddress;
      unsigned char Array_num;
      long unsigned int Array_size;
      long unsigned int dv_frame_size;
    } data;
      class {
    public:
      long int waitTime;
    } wait;
      class {
    public:
      unsigned int lastChannelIndex;
    } stop;
      class {
    public:
      unsigned int channelIndex;
    } segment;
      class {
    public:
      unsigned int channelIndex;
      ENUM_MEDIA_TYPE mediaType;
    } media;
      class {
    public:
      NAV_AVSYNC_MODE mode;
    } avsync;
      class {
    public:
      int __TO_BE_DEFINED__;
    } clock;
      class {
    public:
      unsigned int eventId;
      unsigned char * eventData;
      unsigned int eventSize;
    } event;
  } ;
};

typedef _tagNAVBUF NAVBUF;

class _tagNAVDEMUXIN {
public:
  unsigned int channelIndex;
  unsigned char * pBegin;
  unsigned char * pEnd;
  unsigned char * pCurr;
  int64_t pts;
  unsigned int dataOffsetForPTS;
  long unsigned int startAddress;
};

typedef _tagNAVDEMUXIN NAVDEMUXIN;

class _tagNAVDEMUXCONFIG {
public:
  int pinIndexVideo;
  int pinIndexAudio;
  int pinIndexSpic;
  int pinIndexTeletext;
};

typedef _tagNAVDEMUXCONFIG NAVDEMUXCONFIG;

class _tagNAVDEMUXOUT {
public:
  NAVBUF * pNavBuffers;
  unsigned int numBuffers;
};

typedef _tagNAVDEMUXOUT NAVDEMUXOUT;

class _tagIOPLUGIN {
public:
  long int pInstance;
  void * (*open)(char * , int );
  void (*getIOInfo)(void * , NAVIOINFO * );
  void (*setBlockingMode)(void * , int , int );
  int (*read)(void * , unsigned char * , int , NAVBUF * );
  long long int (*seek)(void * , long long int , int );
  int (*close)(void * );
  int (*dispose)(void * );
  int (*getBufferFullness)(void * , int * , int * );
};

typedef _tagIOPLUGIN IOPLUGIN;

class _tagINPUTPLUGIN {
public:
  void * pInstance;
  HRESULT (*loadMedia)(void * , char * , bool * , NAVLOADFAILURE * );
  HRESULT (*unloadMedia)(void * );
  HRESULT (*getMediaInfo)(void * , NAVMEDIAINFO * );
  HRESULT (*read)(void * , NAVBUF * , unsigned int , NAV_STREAM_TYPE * );
  HRESULT (*updatePlaybackPosition)(void * , PRESENTATION_POSITIONS * , NAVDEMUXPTSINFO * );
  HRESULT (*privateDataFeedback)(void * , unsigned int , unsigned char * , unsigned int , int64_t );
  HRESULT (*execUserCmd)(void * , NAV_CMD_ID , void * , unsigned int , unsigned int * );
  HRESULT (*propertySetGet)(void * , NAV_PROP_ID , void * , unsigned int , void * , unsigned int , unsigned int * );
  HRESULT (*dispose)(void * );
  HRESULT (*identify)(void * , char * , unsigned char * , unsigned int , unsigned int );
  HRESULT (*registerIOPlugin)(void * , IOPLUGIN * , bool , bool , void * );
  HRESULT (*getIOBufferFullness)(void * , int * , int * );
};

typedef _tagINPUTPLUGIN INPUTPLUGIN;

class _tagDEMUXPLUGIN {
public:
  void * pInstance;
  HRESULT (*parse)(void * , NAVDEMUXIN * , NAVDEMUXOUT * );
  HRESULT (*handlePrivateInfo)(void * , unsigned int , unsigned char * , unsigned int );
  HRESULT (*flush)(void * );
  HRESULT (*propertySetGet)(void * , NAV_PROP_ID , void * , unsigned int , void * , unsigned int , unsigned int * );
  HRESULT (*dispose)(void * );
};

typedef _tagDEMUXPLUGIN DEMUXPLUGIN;

typedef HRESULT (*PFUNC_OPEN_INPUT_PLUGIN)(ENUM_MEDIA_TYPE , ENUM_MEDIA_TYPE , osal_mutex_t * , INPUTPLUGIN * );

typedef HRESULT (*PFUNC_OPEN_DEMUX_PLUGIN)(ENUM_MEDIA_TYPE , int , INPUTPLUGIN * , NAVDEMUXCONFIG * , DEMUXPLUGIN * );

typedef HRESULT (*PFUNC_OPEN_IO_PLUGIN)(IOPLUGIN * );


/*system/Include/Filters/INavControl.h*/
class INavControl {
public:
  virtual HRESULT LoadMedia(char * , NAVMEDIAINFO * , ENUM_MEDIA_TYPE , bool * , bool , NAVLOADFAILURE * );
  virtual HRESULT UnloadMedia();
  virtual HRESULT SetGetProperty(NAV_PROP_ID , void * , unsigned int , void * , unsigned int , unsigned int * );
  virtual HRESULT GetPlaybackStatus(NAVPLAYBACKSTATUS * );
  virtual HRESULT GetVideoStatus(NAVVIDEOSTATUS * );
  virtual HRESULT GetAudioStatus(NAVAUDIOSTATUS * );
  virtual HRESULT GetSubpictureStatus(NAVSPICSTATUS * );
  virtual HRESULT GetMenuStatus(NAVMENUSTATUS * );
  virtual HRESULT GetDiscStatus(NAVDISCSTATUS * );
  virtual HRESULT GetTitleStatus(int , NAVTITLESTATUS * );
  virtual HRESULT GetLatestCmdStatus(NAVCMDSTATUS * );
  virtual HRESULT GetNavState(void * , unsigned int , unsigned int * );
  virtual HRESULT SetNavState(void * , unsigned int , bool );
  virtual HRESULT PlayTitle(int , unsigned int * );
  virtual HRESULT PlayChapter(int , int , unsigned int * );
  virtual HRESULT PlayNextChapter(unsigned int * );
  virtual HRESULT PlayPrevChapter(unsigned int * );
  virtual HRESULT PlayAtTime(int , unsigned int , unsigned int , unsigned int * );
  virtual HRESULT PlayAtPos(int , long long int , unsigned int * );
  virtual HRESULT PlaySegment(int , unsigned int , unsigned int , unsigned int , unsigned int , bool , unsigned int * );
  virtual HRESULT MenuShow(unsigned int , unsigned int * );
  virtual HRESULT MenuEscape(unsigned int * );
  virtual HRESULT ButtonSelectNumeric(int , unsigned int * );
  virtual HRESULT ButtonSelectPoint(unsigned int , unsigned int , unsigned int * );
  virtual HRESULT ButtonActivateNumeric(int , unsigned int * );
  virtual HRESULT ButtonActivatePoint(unsigned int , unsigned int , unsigned int * );
  virtual HRESULT ButtonActivate(unsigned int * );
  virtual HRESULT ButtonMoveSelection(NAV_BTNDIR_ID , unsigned int * );
  virtual HRESULT GoUp(unsigned int * );
  virtual HRESULT StillEscape(unsigned int * );
  virtual HRESULT SetAudioStream(int , unsigned int * );
  virtual HRESULT SetSubpictureStream(int , NAV_DISPLAY_STATE , bool , unsigned int * );
  virtual HRESULT SetAngle(int , unsigned int * );
  virtual HRESULT SetGetProperty_Async(NAV_PROP_ID , void * , unsigned int , void * , unsigned int , unsigned int * , unsigned int * );
  virtual HRESULT SendNavCommand(NAV_CMD_ID , void * , unsigned int , unsigned int * );
  virtual HRESULT GetBufferFullness(int * , FILTER_TYPE , int );
  virtual HRESULT GetIOBufferFullness(int * , int * );
};


/*system/Include/Flows/CPlaybackFlowManager.h*/
class CPlaybackFlowManager : public CFlowManager {
protected:
  NAVAUTOSTOPINFO m_autoStopInfo;
  unsigned int m_receivedEOS;
public:
  CPlaybackFlowManager(bool bNeedRefClock);
  ~CPlaybackFlowManager();
protected:
  virtual void HandleDefaultEvents(EVCode evCode, long int * pParams, long int paramSize);
  virtual bool HandleCriticalEvents(EVCode evCode, long int * pParams, long int paramSize);
  virtual bool HasDefaultHandling(EVCode evCode);
  void CheckFlowStopCondition();
};

/*system/Include/Filters/NavigationFilter/CNavigationFilter.h*/
class CNavCmd {
private:
  static unsigned int m_latestCmdID;
public:
  unsigned int m_cmdID;
  unsigned int m_size;
  unsigned char m_buffer[32];
  CNavCmd(unsigned int * cmdID);
  void Pack(unsigned int data);
  void Pack(void * data, unsigned int size);
};

typedef ulCircularQueue<CNavCmd> CNavCmdQueue;

class CNavigationPinInfo {
public:
  IMemRingAllocator * pAllocator;
  BYTE * pBufferLower;
  BYTE * pBufferUpper;
  int cachedAddrOffset;
};

class CNavigationFilter : public CBaseFilter, public IFilterSeeking, public INavControl {
protected:
  CBaseOutputPin m_outputPin[2][4];
  CBaseOutputPin m_infoPin;
  CBaseOutputPin m_passThruPin;
  IReferenceClock * m_pRefClock;
  IOPLUGIN m_ioPlugin;
  INPUTPLUGIN m_inputPlugin;
  DEMUXPLUGIN m_demuxPlugin[2];
  int m_currentDemuxIndex[2];
  NAVDEMUXIN m_demuxIn;
  NAVDEMUXIN m_demuxInReserved[2];
  NAVDEMUXCONFIG m_demuxConfig;
  NAVDEMUXOUT m_demuxOut;
  NAVDEMUXOUT m_demuxOutReserved[2];
  NAV_STREAM_TYPE m_channelFullnessInfo[2];
  DATA_MEDIA_TYPE m_inputType;
  NAV_AVSYNC_MODE m_AVSyncMode;
  long int m_AVSyncStartupFullness;
  NAVTIMEOUTLIMITS m_timeoutLimits;
  NAVDEMUXPTSINFO m_demuxPTSInfo;
  bool m_bDeliverPassThru;
  bool m_bDeliverPassThru_redeliver;
  unsigned char * m_bDeliverPassThru_data;
  unsigned int m_bDeliverPassThru_size;
  bool m_bStopDelivered[2][4];
  bool m_bResetAVSync;
  bool m_bPendingOnFullness;
  bool m_bCheckingAFModeStatus;
  bool m_bCheckCmd;
  bool m_bSmoothReverseCapable;
  long int m_minForwardSkip;
  long int m_maxForwardSkip;
  long int m_minReverseSkip;
  long int m_maxReverseSkip;
  bool m_bUseDDRCopy;
  bool m_bFlushBeforeDelivery;
  uint64_t m_latestDDRCopyHandle;
  int m_bytesReadSinceStart;
  int64_t m_AFModeStatusCheckingTimer;
  void * m_pInputPluginLibrary;
  void * m_pDemuxPluginLibrary[2];
  CNavCmdQueue m_navCmdQueue;
  NAVCMDSTATUS m_latestCmdStatus;
  int m_speed;
  int m_skip;
  osal_thread_t m_thread;
  osal_mutex_t m_streamingMutex;
  CNavigationPinInfo m_pinInfo[2][4];
  RESOURCE_NAME m_ioDevice;
  int m_videoErrConcealmentLevel;
public:
  CNavigationFilter();
  virtual ~CNavigationFilter();
  virtual HRESULT Stop();
  virtual HRESULT Pause(FILTER_STATE State);
  virtual HRESULT Run();
  virtual HRESULT FilterType(FILTER_TYPE * pFilterType);
  virtual HRESULT SetFilterProperty(FILTER_PROPERTY propId, void * propData, unsigned int propSize);
protected:
  static void ThreadProcEntry(void * pInstance);
  void ThreadProc();
  void Read();
  void StartStreaming();
  void StopStreaming();
  void CheckPin(CBaseOutputPin * pPin, CNavigationPinInfo * pPinInfo);
  bool HandleUserCmd(CNavCmd * pCmd);
  HRESULT ExecUserCmd(NAV_CMD_ID id, void * cmdData, unsigned int cmdDataSize, unsigned int * pIsFlushRequired);
  HRESULT DeliverUserCmd(CNavCmd * pCmd);
  HRESULT DeliverData(NAVDEMUXOUT * pDemuxOut);
  HRESULT SyncWritePointers(int channelIndex, unsigned int WP_update_flags, BYTE * * WP);
  HRESULT DeliverPrivateInfo(NAVBUF * pNavBuffer);
  HRESULT DeliverNavBufCommands(NAVBUF * pNavBuffer);
  void DeliverDV(CNavigationPinInfo * pPinInfo, BYTE * pBufferCurr, BYTE * & pBufferNext, NAVBUF * pNavBuffer);
  HRESULT DeliverStop(int channelIndex);
  HRESULT DeliverFlush(PRESENTATION_POSITIONS * pLastPosition);
  HRESULT DeliverEndOfStream(int channelIndex, bool bPostEvent);
  HRESULT DeliverEndOfStream(int channelIndex, unsigned int pinIndex, bool bPostEvent);
  HRESULT DeliverNewSegment(int channelIndex);
  void Sleep(long int milliseconds, bool bCancelFullnessPending);
  void UpdatePlaybackPosition();
  void ResetAVSync(NAVBUF * pNavBuffer);
  HRESULT SelectInputPlugin(char * , ENUM_MEDIA_TYPE , ENUM_MEDIA_TYPE , bool * , bool , NAVLOADFAILURE * );
  HRESULT SelectDemuxPlugin(int , ENUM_MEDIA_TYPE );
  HRESULT OpenInputPlugin(int , ENUM_MEDIA_TYPE , ENUM_MEDIA_TYPE );
  HRESULT OpenDemuxPlugin(int , int , ENUM_MEDIA_TYPE );
  HRESULT CloseInputPlugin(bool );
  HRESULT CloseDemuxPlugin(int );
  HRESULT OpenIOPlugin(int );
  HRESULT DispatchProperty(NAV_PROP_ID id, void * inData, unsigned int inDataSize, void * outData, unsigned int outDataSize, unsigned int * returnedSize);
  bool IsVideoConnected();
  bool IsAudioConnected();
  bool IsIndexGenConnected();
  void FlushPin(int channelIndex, unsigned int pinIndex);
  HRESULT DeliverPrivateInfo(CBaseOutputPin * pPin, int infoId, BYTE * pInfo, int length);
public:
  virtual HRESULT SetSeekingInfo(long int Speed, long int Skip);
  virtual HRESULT GetSeekingInfo(long int * pSpeed, long int * pSkip);
  virtual HRESULT CheckCapabilities(FILTER_SEEKING_CAP * pSeekingCap);
  virtual HRESULT LoadMedia(char * path, NAVMEDIAINFO * pMediaInfo, ENUM_MEDIA_TYPE preDeterminedType, bool * pbContinue, bool bForcedIdentification, NAVLOADFAILURE * pFailure);
  virtual HRESULT UnloadMedia();
  virtual HRESULT SetGetProperty(NAV_PROP_ID id, void * inData, unsigned int inDataSize, void * outData, unsigned int outDataSize, unsigned int * returnedSize);
  virtual HRESULT GetPlaybackStatus(NAVPLAYBACKSTATUS * status);
  virtual HRESULT GetVideoStatus(NAVVIDEOSTATUS * status);
  virtual HRESULT GetAudioStatus(NAVAUDIOSTATUS * status);
  virtual HRESULT GetSubpictureStatus(NAVSPICSTATUS * status);
  virtual HRESULT GetMenuStatus(NAVMENUSTATUS * status);
  virtual HRESULT GetDiscStatus(NAVDISCSTATUS * status);
  virtual HRESULT GetTitleStatus(int titleNum, NAVTITLESTATUS * status);
  virtual HRESULT GetLatestCmdStatus(NAVCMDSTATUS * status);
  virtual HRESULT GetNavState(void * state, unsigned int size, unsigned int * returnedSize);
  virtual HRESULT SetNavState(void * state, unsigned int size, bool bForced);
  virtual HRESULT PlayTitle(int titleNum, unsigned int * cmdID);
  virtual HRESULT PlayChapter(int titleNum, int chapterNum, unsigned int * cmdID);
  virtual HRESULT PlayNextChapter(unsigned int * cmdID);
  virtual HRESULT PlayPrevChapter(unsigned int * cmdID);
  virtual HRESULT PlayAtTime(int titleNum, unsigned int startSeconds, unsigned int startFrameIdx, unsigned int * cmdID);
  virtual HRESULT PlayAtPos(int titleNum, long long int pos, unsigned int * cmdID);
  virtual HRESULT PlaySegment(int titleNum, unsigned int startSeconds, unsigned int startFrameIdx, unsigned int endSeconds, unsigned int endFrameIdx, bool bRepeat, unsigned int * cmdID);
  virtual HRESULT MenuShow(unsigned int menuID, unsigned int * cmdID);
  virtual HRESULT MenuEscape(unsigned int * cmdID);
  virtual HRESULT ButtonSelectNumeric(int buttonNum, unsigned int * cmdID);
  virtual HRESULT ButtonSelectPoint(unsigned int x, unsigned int y, unsigned int * cmdID);
  virtual HRESULT ButtonActivateNumeric(int buttonNum, unsigned int * cmdID);
  virtual HRESULT ButtonActivatePoint(unsigned int x, unsigned int y, unsigned int * cmdID);
  virtual HRESULT ButtonActivate(unsigned int * cmdID);
  virtual HRESULT ButtonMoveSelection(NAV_BTNDIR_ID direction, unsigned int * cmdID);
  virtual HRESULT GoUp(unsigned int * cmdID);
  virtual HRESULT StillEscape(unsigned int * cmdID);
  virtual HRESULT SetAudioStream(int streamNum, unsigned int * cmdID);
  virtual HRESULT SetSubpictureStream(int streamNum, NAV_DISPLAY_STATE displayState, bool bDummyStream, unsigned int * cmdID);
  virtual HRESULT SetAngle(int angleNum, unsigned int * cmdID);
  virtual HRESULT SetGetProperty_Async(NAV_PROP_ID id, void * inData, unsigned int inDataSize, void * outData, unsigned int outDataSize, unsigned int * returnedSize, unsigned int * cmdID);
  virtual HRESULT SendNavCommand(NAV_CMD_ID id, void * cmdData, unsigned int cmdDataSize, unsigned int * cmdID);
  virtual HRESULT GetBufferFullness(int * pFullness, FILTER_TYPE filterType, int channelIndex);
  virtual HRESULT GetIOBufferFullness(int * pFullness, int * pFlag);
  virtual HRESULT ExecuteCommandLine(char * command);
};


/*system/Include/Filters/AudioFilter_c/CAudioFilter.h*/
class CSystemRPCCall {
public:
  virtual HRESULT ProcessAgentPrivateInfo(BYTE * , long int );
  virtual HRESULT EndOfStream(long int , long int );
  virtual HRESULT DeliverFlush(long int );
};

class CAudioFilter : public CBaseFilter, public CSystemRPCCall, public IFilterSeeking {
protected:
  long int m_agentInstanceID;
  long int m_speed;
  long int m_skip;
public:
  CAudioFilter();
  virtual ~CAudioFilter();
  virtual HRESULT ProcessAgentPrivateInfo(BYTE * pData, long int length);
  virtual HRESULT ProcessPrivateInfo(BYTE * pData, long int length);
  virtual HRESULT ProcessPrivateInfo(int infoId, BYTE * pData, long int length);
  virtual HRESULT CheckCapabilities(FILTER_SEEKING_CAP * pSeekingCap);
  virtual HRESULT SetSeekingInfo(long int Speed, long int Skip);
  virtual HRESULT GetSeekingInfo(long int * pSpeed, long int * pSkip);
  virtual long int GetAgentInstanceID();
protected:
  virtual void SetAgentInstanceID(long int instanceID);
  virtual HRESULT CreateAgent(AUDIO_MODULE_TYPE type, long int instanceID);
  virtual HRESULT GetPinRingBufferHeaderList(CBasePin * pPin, long int *RBufHeaderList[8], long int & listSize);
  virtual HRESULT InitAgentRingBuffer();
};

class CAudioOutInputPin;
class CAudioOutOutputPin;
class CMemRingAllocator;

/*system/Include/Filters/AudioFilter_c/CAudioOut.h*/
class CAudioOutFilter : public CAudioFilter {
protected:
  CAudioOutInputPin * m_pInPin;
  CAudioOutOutputPin * m_pOutPin;
  CMemRingAllocator * m_pInputAllocator;
  CMemRingAllocator * m_pOutputAllocator;
  CBaseInputPin * m_pCHInPinArr[8];
  CBaseOutputPin * m_pCHOutPinArr[8];
  CMemRingAllocator * m_pInAllocatorArr[8];
  CMemRingAllocator * m_pOutAllocatorArr[8];
  CMemRingAllocator * m_pUsedInputAllocator;
  CMemRingAllocator * m_pUsedOutputAllocator;
  IReadPointerHandle * m_pInputReadHdl;
  static long int m_aoutInstanceID;
  static long int m_aoutInstanceNum;
  bool m_Init;
  IMemInputPin * m_pOutConnectedPin;
public:
  CAudioOutFilter();
  virtual ~CAudioOutFilter();
  virtual HRESULT FilterType(FILTER_TYPE * pFilterType);
  virtual HRESULT Run();
  virtual HRESULT Pause(FILTER_STATE State);
  virtual HRESULT Stop();
  virtual HRESULT SetFocus();
  virtual HRESULT SetUnFocus();
  virtual HRESULT ProcessPrivateInfo(int infoId, BYTE * pData, long int length);
  virtual HRESULT SetBilingual(int lingaul);
  virtual HRESULT SetVolume(int volume);
  virtual HRESULT SetSPDIFSource(int L, int R);
  virtual HRESULT DropSample(long int PTS);
protected:
  virtual HRESULT ProcessAgentPrivateInfo(BYTE * pData, long int length);
  virtual HRESULT EndOfStream(long int pinID, long int eventID);
  virtual HRESULT DeliverFlush(long int pinID);
  virtual HRESULT InitAgentRingBuffer();
  virtual HRESULT SetupAgentConnection(IPin * pConnectedPin);
  HRESULT SetOutConnetedPin(IMemInputPin * pin);
};

/*system/Include/Filters/AudioFilter_c/CPPAOut.h*/
enum OUT_PIN_TYPE {OUT_PIN_FOR_AOUT, OUT_PIN_FOR_APP};

class CAudioPPAOutFilter : public CAudioOutFilter {
protected:
  static OUT_PIN_TYPE m_outPinType;
  static CMemRingAllocator * m_pPPAoutAllocator;
  static CMemRingAllocator * m_pPPAoutAllocatorArr[8];
  static long int m_appInstanceID;
  static long int m_pinMap[4];
  static long int m_focusPinID;
public:
  CAudioPPAOutFilter(OUT_PIN_TYPE type);
  virtual ~CAudioPPAOutFilter();
  HRESULT InitAgentInnerRingBuffer();
  virtual HRESULT Run();
  virtual HRESULT Stop();
  virtual HRESULT Pause(FILTER_STATE State);
  virtual HRESULT SetFocus();
  virtual HRESULT SetUnFocus();
  virtual HRESULT SetSeekingInfo(long int Speed, long int Skip);
  virtual HRESULT InitAgentRingBuffer();
  virtual HRESULT SetupAgentConnection(IPin * pConnectedPin);
  HRESULT SetPPReverb(u_char ena, int reverb_mode);
  HRESULT SetPPKeyShift(u_char ena, int semitone);
  HRESULT SetPPVocalRemover(u_char ena, int mode);
};

