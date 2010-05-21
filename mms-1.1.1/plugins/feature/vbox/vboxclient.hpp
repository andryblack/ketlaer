
#ifndef _VBOXCLIENT_H
#define _VBOXCLIENT_H


struct tVboxMsg;

class cLibVBOX;

class cVBOXClient
{
public:
	cVBOXClient(const char * sName, const char * sVBOXServer,int nVBOXServerPort,const char * sUser, const char * sPass);
	~cVBOXClient();
	enum eError
	{
		vboxSuccess,
		vboxCantConnect,
		vboxCantLogin,
		vboxErrorList,
		vboxOutOfBounds,
		vboxInvalidServerResponse,
		vboxNoAnswerFromServer,
		vboxErrorCreateTmpFile,
		vboxErrorReadingMsgData
		
	};
	enum eCtrl
	{
		ctrlStop,
		ctrlReject,
		ctrlAnswerNow,
		ctrlAnswerAll,
		ctrlAudio,
		ctrlSuspend
	};
	eError Connect();
	eError Disconnect();
    bool IsConnected();
	void   KeepAlive();
	eError TransferMsgList();
	eError GetMsgTime(int nr, struct tm *ptmTime);
	eError GetMsgPlaytime(int nr,char **ppsPlaytime);
	eError GetMsgIsNew(int nr,bool *pbIsNew);
	eError GetMsgIsDeleted(int nr,bool *pbIsDeleted);
	eError GetMsgCallerName(int nr,char **ppsCallerName);
	eError GetMsgId(int nr,char **ppsId);
	eError GetMsgCallerId(int nr,char **ppsCallerId);

	int GetMsgTotalCount();
	int GetMsgNewCount();
	int GetMsgDeletedCount();

	eError GetServerNewMsgCount(int *pCount,time_t *pTimeLatest);

	eError ToggleNewFlag(int nr);
	eError ToggleDeletedFlag(int nr);

	eError DeleteMarkedMessages(bool bReloadList = true);

	eError CreateAuFileFromMsg(int msgnr,const char* szAuFileName, const char* szAuConvTool);

	eError GetServerStatusCtrl(eCtrl ctrlid,int *pStatus);
	eError GetServerCreateCtrl(eCtrl ctrlid);
	eError GetServerRemoveCtrl(eCtrl ctrlid);
	
	char * 	Name() { return m_sName; } 
	char * 	VBOXServer() { return m_sVBOXServer; }
	int 	Port() { return m_nVBOXServerPort; }
	char * 	Username() { return m_sUser; }
	char *  Password() { return m_sPass; }
	
protected:
	struct tVboxMsg *_GetMsgPtr(int nr);
	int _CreateAndLoadMessageFromServer(int fd, int size);
	const char *_GetCtrlStringFromEnum(eCtrl ctrlid);

private:
	
    cLibVBOX *m_pLibVBOX;

    char * m_sName;
	char * m_sVBOXServer;
	char * m_sUser;
	char * m_sPass;
	int    m_nVBOXServerPort;

	char * m_pMsgListBuffer;
	int   m_nMsgCountInBuffer;
	bool  m_bNewMsgs;

};









#endif
