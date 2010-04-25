typedef uint8_t  __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;

/* for AVMute */
#define  HDMI_SETAVM 0x01
#define  HDMI_CLRAVM 0x02

/*system/Include/Platform_Lib/HDMIControl/hdmi.h*/
enum hdmi2ap_event {HDMI2AP_SET_PHYSICAL_ADDR};

typedef hdmi2ap_event ENUM_HDMI2AP_EVENT;

typedef int (*HandleHDMIEventFunction)(ENUM_HDMI2AP_EVENT , void * );

class HDMI_TvSystemAndStandard {
public:
  ENUM_VIDEO_SYSTEM videoSystem;
  ENUM_VIDEO_STANDARD videoStandard;
  int bIsWidescreen;
};

class Audio_desc {
public:
  unsigned char coding_type;
  unsigned char channel_count;
  unsigned char sample_freq_all;
  unsigned char sample_size_all;
  unsigned char max_bit_rate_divided_by_8KHz;
};

class HDMI_EDID_data {
public:
  unsigned char Extended[128];
  unsigned char tag;
  unsigned char revision;
  unsigned char offset;
  unsigned char bUnderScan;
  unsigned char bBasicAudio;
  unsigned char YCBCR444;
  unsigned char YCBCR422;
  char native_format_number;
  char VDB_length;
  unsigned char * VDB;
  char ADB_length;
  Audio_desc * ADB;
  char SADB_length;
  unsigned char SADB[3];
  char VSDB_length;
  unsigned char * VSDB;
  char VESA_length;
  unsigned char * VESA;
  char Use_Extended_length;
  unsigned char * Use_Extended;
  unsigned char * native_format;
};



/*system/Include/Platform_Lib/HDMIControl/HDMI-MARS.cpp*/
extern unsigned char pseudo_edid[256];

extern unsigned char DVI_edid[256];

extern unsigned char realtek_edid[256];

extern unsigned char realtek_dvi_edid[256];

extern unsigned char realtek_hdmi_edid_all[256];

extern unsigned char realtek_dvi_edid_all[256];

extern unsigned char realtek_dvi_edid_all2[256];

extern unsigned char dvi_edid_01[256];

extern unsigned char dvi_edid_02[256];

extern unsigned char dvi_edid_03[256];

extern unsigned char edid_data_viewsonic_vt2230[256];

extern unsigned char benq_edid_data[256];

extern unsigned char dvx_edid_data[256];

extern unsigned char VID_support[15];

extern unsigned char PK_A1[280];

extern unsigned char DK_tmp[280];

extern unsigned char Aksv_A1[5];

extern unsigned char PK_real[280];

extern unsigned char Aksv_real[5];

extern unsigned int EDID_Valid;

extern unsigned int EDID_Read;

extern unsigned int Delay_To_CheatSony;

extern unsigned int TV_Status;

extern unsigned int TV_Status_s;

extern unsigned int HDCPAuthenticated;

extern unsigned int AP_SETUP_HDMI_MUTE;

extern char MonitorName[20];

extern long long unsigned int TIME_ST;

extern long long unsigned int TIME_ST2;

extern unsigned int sha_length;

extern unsigned int sha_length_byte;

extern unsigned int sha_block;

extern unsigned int M0_LSW;

extern unsigned int M0_MSW;

extern unsigned int KM_LSW;

extern unsigned int KM_MSW;

extern unsigned char SHA_1_data[64];

extern unsigned char PK[280];

extern __u8 vdb_data_old[30];

extern __u8 vdb_data_new[30];

extern int vdb_data_length_old;

extern int vdb_data_length_new;

extern unsigned char local_HDCPOper_state;

void GetVidName(int VID);

void ShowRegisterStatus();

void ShowRegisterStatus_s();

void GetVidName_s(int VID);

void SaveMBR_s();

void ShowSystemStatus();

void CheckAudioDataBlock_s(unsigned char * ADB);

int Read_EDID_dvi(__u8 * EDID_buf);

int txt2html(char * txt_file_name);

int check_KSV(__u8 * ksv_buf);

void PrintEDID(unsigned char * EDID_buf);

void PrintEDID_128(unsigned char * EDID_buf);

void MakeContents(FILE * fp);

void MakeReference(FILE * fp);

void MakeSendMailForm(FILE * fp);

void MakeOrgLink(FILE * fp);

int IsEDIDValid(unsigned char * EDID_buf);

int IsEDIDValidVideoDataBlock(unsigned char * EDID_buf);

int Check_CTS_Machine(unsigned char * EDID_buf);

int Check_HDMI_Device(__u8 * EDID_buf);

int Check_Merge_AUDIO_Data_Block(__u8 * EDID_buf);

int Check_Detail_Timing_Descriptoins(__u8 * EDID_buf);

int Check_DTD(int index, int i, __u8 * EDID_buf);

int Get_VID_From_DTD(int W, int H, int HB, int VB, int interlaced);

int Get_VDB_length(__u8 * EDID_buf);

void Get_VDB_data(__u8 * EDID_buf, __u8 * vdb_data_old, int vdb_data_length_old);

int Check_VID_in_VDB_data(__u8 * vdb_data_old, int vdb_data_length_old, int vid);

int Add_DTD_To_VDB_data_new(__u8 * vdb_data_new, int * vdb_data_length_new, int vid);

int Modify_EDID(__u8 * EDID_buf);

int Erase_DTD_From_EDID(__u8 * EDID_buf);

int Merge_EDID(__u8 * EDID_buf);

void print_pk(unsigned char * Aksv, unsigned char * PK);

void TestSHA1Function_mars();

void PrintSHAinput(unsigned char * SHA_buf);

void PrintKSVlist(unsigned char * ksvlist, unsigned char DeviceCount);

void TestSHA1Function_mars_0(unsigned int * sha_buf);

void ChangeTMDSStrength();


/*system/Include/Platform_Lib/HDMIControl/HDMICommon.cpp*/
extern AUDIO_HDMI_SETTING_INFO HDMI_LocalInfo;

extern int CheatSonyNeeded;

extern int VideoCode;

extern FILE * fp;

extern HDMI_EDID_data HDMI_EDID;

extern unsigned char setHDMIPSCANFlag;

extern short unsigned int hdmi_source_address;

int HDMI_CheckAspectRatio();

int HDMI_SetVideoFormatByTvSystem(ENUM_VIDEO_SYSTEM videoSystem, ENUM_VIDEO_STANDARD videoStandard);

char getHDMICapacity();

char getHDMIPscan();

char getHDMIPlugged();

char getHDCPEnabled();

char getYSTMode();

char getDVIMonitor();

char getDVIMonitor_Sink();

char getHDMIKeyFound();

char getHDMISwitchTVSystem();

char getHDMIReady();

char getHDMIStatus();

char getTVSupportYCbCr();

char getCTSMachine();

char getHDMIDevice();

char getTVSupportxvYCC();

char getHDMILatency(unsigned char * state);

char getHDCPReady();

char getHPDFail();

char getTVStatus();

char getHPDStatus();

void init_HDMI_EDID_data();

void free_HDMI_EDID_data();

void setHDMIPscan(char state);

void setHDMISwitchTVSystem(char state);

void setHDMIReady(char state);

void setHDMIStatus(char state);

void setTVSupportYCbCr(char state);

void setCTSMachine(char state);

void setHDMIDevice(char state);

void setHDMILatency(unsigned char * state);

void setTVSupportxvYCC(char state);

void setHDCPReady(char state);

void setHPDFail(char state);

void setTVStatus(char state);

void setHPDStatus(char state);

void setHDMIPlugged(char state);

char getHDMIAuthetication();

void setHDMIAuthetication(char state);

char getHDMIAuth();

void setHDMIAuth(char state);

void call_RPC_TOAGENT_HDMI_OUT_EDID_SVC(int enable, unsigned char * ptr);

int parseHDMI_EDIDInfo(unsigned char * pInfo);

void HDMI_setAVFormat();

unsigned char HDMI_GetCurrentVid();

unsigned char HDMI_isSD();

char HDMI_CheckVideoFormat();

unsigned char HDMI_CheckAutoVideoFormat();

int HDMI_GetAutoVideoFormat(HDMI_TvSystemAndStandard * pTvSS);

int HDMI_SetAutoVideoFormat();

int HDMI_GetEDIDDefaultVideoFormat(HDMI_TvSystemAndStandard * pTvSS);

int HDMI_SetEDIDDefaultVideoFormat();

int HDMI_GetVideoFormat(int * len, HDMI_TvSystemAndStandard * pTvSS);

int HDMI_GetAudioFormat(int * len, unsigned char * buf);

int setHDMIAudioVideoFormat();

int HDMI_GetVideoFormatByVid(int vid, HDMI_TvSystemAndStandard * pTvSS);

int HDMI_SetVideoFormatByVid(int vid);

int HDMI_GetNextVideoFormat(HDMI_TvSystemAndStandard * pTvSS);

int HDMI_GetNextVideoFormat_N(HDMI_TvSystemAndStandard * pTvSS, int next);

int HDMI_SetNextVideoFormat();

int HDMI_GetDVIVideoFormat(HDMI_TvSystemAndStandard * pTvSS);

int HDMI_SetDVIVideoFormat();

int HDMI_GetNextNTSCVideoFormat(HDMI_TvSystemAndStandard * pTvSS);

int HDMI_SetNTSCVideoFormat();

int HDMI_GetNextPALVideoFormat(HDMI_TvSystemAndStandard * pTvSS);

int HDMI_SetPALVideoFormat();

int HDMI_getAudioCurrentChannel();

short unsigned int getHDMISourceAddress();

void setHDMISourceAddress(short unsigned int addr);

char getHDMIEDIDReady();

void setHDMIEDIDReady(char state);

void setHDCPEnabled(char state);

void setYSTMode(char state);

void ResetHDMIAUDIO();

void setDVIMonitor(char state);

void setDVIMonitor_Sink(char state);

void setHDMIKeyFound(char state);

int Register_HdmiEvent_Func(HandleHDMIEventFunction function);

int UnRegister_HdmiEvent_Func();

int HDMI_getAudioCurrentIndex();

void GetVidNameByTV(ENUM_VIDEO_SYSTEM videoSystem, ENUM_VIDEO_STANDARD videoStandard);

void GetVidNameByTV_s(ENUM_VIDEO_SYSTEM videoSystem, ENUM_VIDEO_STANDARD videoStandard);

unsigned char HDMI_GetCurrentVid_s();

void HDMI_PrepareToDoSwitchTVSystem();

void HDMI_TurnOff();


/*system/Include/Platform_Lib/HDMIControl/HDMIControl.h*/
class HDMI_Controller {
public:
  __u8 HDCPOper_state;
  __u8 Chk_HotPlugFlag;
private:
  __u32 Ri_err;
  __u32 Ri_count;
  __u32 Repeater_Device;
  __u32 M0_LSW;
  __u32 M0_MSW;
  __u8 Aksv[5];
  __u8 Bksv[5];
  __u8 An[8];
  __u8 Ainfo;
  __u8 Bcaps;
  __u8 GetRi[2];
  __u8 Ri[2];
  __u8 Ri_old[2];
  __u8 GetPj;
  __u8 Pj;
  __u8 Bstatus[2];
  __u8 Ksvlist[5];
  __u8 DeviceCount;
  __u8 VH0[4];
  __u8 VH1[4];
  __u8 VH2[4];
  __u8 VH3[4];
  __u8 VH4[4];
public:
  HDMI_Controller();
  ~HDMI_Controller();
  HRESULT InitIF();
  HRESULT Set_PD_H(__u8 );
  HRESULT Set_AUDIO_ENABLE(__u8 );
  HRESULT Chk_HotPlug(__u8 * state);
  HRESULT Chk_HotPlug_s(__u8 * state);
  HRESULT Send_Video_Type(__u8 , __u8 );
  HRESULT Send_Audio_Type(__u8 , __u8 , __u8 , __u8 , __u8 );
  HRESULT Send_Audio_Infoframe(__u8 , __u8 , __u8 , __u8 , __u8 );
  HRESULT Send_Avi_Infoframe(__u8 , __u8 , __u8 , __u8 );
  HRESULT Send_AVMute(__u8 Select);
  HRESULT Send_AudioMute(__u8 Select);
  HRESULT Send_AudioMute_0(__u8 Select);
  HRESULT Send_Audio_VSDB_DATA(__u8 * latency, int interlaced);
  HRESULT Send_Video_Format_To_Audio(__u8 Select);
  HRESULT HDCP_Authenticate(__u8 OneSec_Flg, __u8 TwoSec_Flg, __u8 * EDID_Info);
  HRESULT Read_EDID(__u8 * EDID_buf);
  HRESULT Read_EDID_From_TV(__u8 * EDID_buf);
  HRESULT Read_EDID_block(int block, __u8 * EDID_buf);
  HRESULT Read_EDID_0(__u8 * EDID_buf);
  HRESULT Write_EDID();
  HRESULT Read_EDID_s(__u8 * EDID_buf);
  HRESULT Check_EDID(__u8 * EDID_buf);
  HRESULT Check_EDID_0(__u8 * EDID_buf);
  HRESULT Check_EDID_s(__u8 * );
  HRESULT Parse_EDID(__u8 * EDID_buf);
  HRESULT Parse_EDID_s(__u8 * EDID_buf);
  HRESULT Save_EDID_s(char * edidfilename);
  HRESULT HDCP_ForceUnAuth();
  HRESULT CheatSony();
  HRESULT RestartHDMI();
  HRESULT Setup_HPD_GPIO();
  HRESULT HDMI_HPD_test();
  HRESULT HDMI_HPD_test_s();
  HRESULT Chk_TvStatus();
  HRESULT CheckRegisterStatus();
  HRESULT HDMI_ResetAUDIO();
  HRESULT getTVSupportHDCP();
  HRESULT Check_Save_HDMI_Info_To_Flashdisk();
  HRESULT Save_HDMI_Info_To_Flashdisk(char * filename);
  HRESULT txt2html_test(char * filename);
  HRESULT showTVHDCPStatus();
  HRESULT TestHDCP();
  HRESULT TestHDCP00();
  HRESULT TestHDCP01();
  HRESULT TestHDCP02(int selection, int change_An);
  HRESULT TestHDCP03();
  HRESULT TestHDCP04();
  HRESULT TestHDCP05();
private:
  HRESULT Send_Packet_Read(__u8 i2c_addr, __u8 * packet_data, __u32 ndata, __u8 * data_buf);
  HRESULT Send_Packet_Write(__u8 i2c_addr, __u8 * packet_data);
  HRESULT HDCP_ReadBksv(__u8 * Bksv_buf);
  HRESULT HDCP_WriteBksv(__u8 * Bksv_buf);
  HRESULT HDCP_FreeRunGetAn(__u8 * An);
  HRESULT HDCP_ReadAksv(__u8 * Aksv);
  HRESULT HDCP_WriteAn(__u8 * An);
  HRESULT HDCP_WriteAn_4(__u8 * An, __u8 part);
  HRESULT HDCP_WriteAinfo(__u8 * Ainfo);
  HRESULT HDCP_WriteAksv(__u8 * Aksv);
  HRESULT HDCP_ReadBcaps(__u8 * Bcaps);
  HRESULT HDCP_SetRX_RPTR();
  HRESULT HDCP_ClrRX_RPTR();
  HRESULT HDCP_AuthSeq(__u8 * Bksv, __u8 * GetRi);
  HRESULT HDCP_ReadRi(__u8 * Ri);
  HRESULT HDCP_ChkRi(__u8 * GetRi);
  HRESULT HDCP_SetAuth();
  HRESULT I2C_SegRead(__u8 , __u8 , __u8 , __u16 , __u8 * );
  HRESULT I2C_DDCRead(__u8 , __u8 , __u16 , __u8 * );
  HRESULT I2C_DDCWrite(__u8 , __u8 , __u16 , __u8 * );
  HRESULT I2C_Read_Segment(unsigned char Segment, short unsigned int nSubAddrByte, unsigned char * SubAddr, short unsigned int nDataByte, unsigned char * pData);
  HRESULT HDCP_GenerateKm(__u8 * Bksv);
  HRESULT Chk_DVI();
  HRESULT HDCP_ReadBstatus(__u8 * Bstatus);
  HRESULT HDCP_ReadKsvlist(__u8 * Ksvlist, __u8 DeviceCount);
  HRESULT HDCP_ChkPj(__u8 * GetPj);
  HRESULT HDCP_ReadPj(__u8 * Pj);
  HRESULT HDCP_ReadSHA(__u8 * VH0, __u8 * VH1, __u8 * VH2, __u8 * VH3, __u8 * VH4);
  HRESULT HDCP_ReadPK(__u8 * PK, __u8 * Aksv);
  HRESULT HDCP_ReadPK_A1(__u8 * PK, __u8 * Aksv);
};


/*system/Include/Platform_Lib/HDMIControl/bd-HDMI-MARS.cpp*/
void WRITE_REG_INT32U(unsigned int addr, unsigned int value);

unsigned int READ_REG_INT32U(unsigned int addr);

extern VIDEO_RPC_VOUT_CONFIG_HDMI_INFO_FRAME argp;

extern int num_audio;

extern int num_hdmi;

extern int SampleFreq_old;

int HDMI_SetVideoFormat(__u8 type, __u8 Vid, __u8 RGB_or_YCbCr, __u8 PixelRepeat);

int ReloadHDMI();

int DisplayRegisterStatus();

int TestSHA1Function();

int TestHDCPFunction();

int HDMI_SetAudioFormat();

int HDMI_SetAudioFormatRPC();

void SET_HDMI_Audio(unsigned char video_code, unsigned char HDMI_Frequency, unsigned char Sampling_Frequency, unsigned char Audio_Channel_Count);

int HDMI_GetAudioMute();

int HDMI_SetAudioMute(int isMute);

int HDMI_SetPowerState(int isOn);

int HDMI_SetHDCPState(char state);

int HDMI_SetDVIState(char state);

int Check_SET_HDMI_AUDIO(unsigned char vid, unsigned char HDMI_Frequency, unsigned char Sampling_Frequency, unsigned char Audio_Channel_Count, unsigned int N, unsigned int CTS);

void HDMI_gen_metadata();

void HDMI_SetRamPacket(unsigned int pkt_no, unsigned char * pkt);

void SET_HDMI_Frequency(unsigned char video_code, unsigned char HDMI_Frequency);

void SET_HDMI_Register(unsigned char video_code);

int SET_HDMI_TMDS_1080p();

int hex2dec(char * hhh);

int pow(int t1, int t2);

