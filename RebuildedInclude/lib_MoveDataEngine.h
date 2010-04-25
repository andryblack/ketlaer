/*system/Include/Platform_Lib/MoveDataEngine/md_api.h*/
typedef uint64_t MD_CMD_HANDLE;


/*system/Include/Platform_Lib/MoveDataEngine/md_mars.cpp*/
class md_dev {
public:
  void * CmdBuf;
  void * CachedCmdBuf;
  void * CmdBase;
  void * CmdLimit;
  int wrptr;
  int v_to_p_offset;
  int size;
  uint64_t hw_counter;
  uint64_t sw_counter;
  pthread_mutex_t lock_mutex;
};

extern int * SMQ_CNTL;

extern int * SMQ_INT_STATUS;

extern int * SMQ_INT_ENABLE;

extern int * SMQ_CmdBase;

extern int * SMQ_CmdLimit;

extern int * SMQ_CmdRdptr;

extern int * SMQ_CmdWrptr;

extern int * SMQ_CmdFifoState;

extern int * SMQ_INST_CNT;

extern "C" void md_open();

extern "C" void md_close();

int64_t WriteCmd(md_dev * dev, uint8_t * pBuf, int nLen);

extern "C" void md_WaitComplete();

void update_hw_counter();

extern "C" bool md_checkfinish(MD_CMD_HANDLE handle);

extern "C" MD_CMD_HANDLE md_memcpy(void * lpDst, void * lpSrc, int len, bool forward);

extern "C" MD_CMD_HANDLE md_memset(void * lpDst, uint8_t data, int len);

extern "C" MD_CMD_HANDLE md_seq_blk_cpy(int addr_idx, int h_in_pixels, int w_in_bytes, int x_in_bytes, int y_in_bytes, void * src, int len);

extern "C" MD_CMD_HANDLE md_blk_seq_cpy(int addr_idx, int h_in_pixels, int w_in_bytes, int x_in_bytes, int y_in_bytes, void * src);

uint64_t md_write(char const * buf, size_t count);

extern "C" MD_CMD_HANDLE md_memcpyex(void * pvDestAddress, void * pvSrcAddress, uint32_t dwDataLength, bool bForward, uint32_t dwSrcPitchValue, uint32_t dwDestPitchValue, uint32_t dwDataHeight, uint32_t dwDataWidth);

extern "C" MD_CMD_HANDLE md_memsetex(void * pvDestAddress, uint8_t dwConstData, uint32_t dwDataLength, uint32_t dwSrcPitchValue, uint32_t dwDestPitchValue, uint32_t dwDataHeight, uint32_t dwDataWidth);

extern "C" MD_CMD_HANDLE md_seq_blk_cpyex(uint32_t dwAddrIdx, void * pvSeqAddr, uint32_t dwSeqLen, uint32_t dwSeqPitch, uint32_t dwHeight, uint32_t dwByteWidth, uint32_t dwByteX, uint32_t dwByteY);

extern "C" MD_CMD_HANDLE md_blk_seq_cpyex(uint32_t dwAddrIdx, void * pvSeqAddr, uint32_t dwSeqPitch, uint32_t dwPixelHeight, uint32_t dwByteWidth, uint32_t dwByteX, uint32_t dwByteY);

