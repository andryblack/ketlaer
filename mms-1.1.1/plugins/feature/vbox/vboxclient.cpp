/*
 * Define to include both, <time.h> *and* <sys/time.h>:
 */

#define TIME_WITH_SYS_TIME 1

/*
 * Define if you have the <sys/time.h> header file:
 */

#define HAVE_SYS_TIME_H 1

/*
 * Define if you have the vsnprintf() function:
 */

#define HAVE_VSNPRINTF 1

//#include <vdr/interface.h>
#include "libvbox.hpp"
#include "vboxclient.hpp"

const char * vboxtoaubin = "/usr/bin/vboxtoau";


static int sort_message_list(const void *a, const void *b)
{
	struct tVboxMsg *linea = (struct tVboxMsg *)a;
	struct tVboxMsg *lineb = (struct tVboxMsg *)b;

	if (lineb->ctime == linea->ctime) return( 0);
	if (lineb->ctime  < linea->ctime) return(-1);

	return(1);
}

cVBOXClient::cVBOXClient(const char *sName, const char * sVBOXServer,int nVBOXServerPort,const char * sUser, const char * sPass)
{
	m_pLibVBOX = new cLibVBOX;
	m_pMsgListBuffer = NULL;
	m_nMsgCountInBuffer = 0;
	m_sName = strdup(sName);
	m_sVBOXServer = strdup(sVBOXServer);
	m_sUser = strdup(sUser);
	m_sPass = strdup(sPass);
	if (nVBOXServerPort == -1)
	{
		struct servent *vboxdserv;
		if (!(vboxdserv = getservbyname("vboxd", "tcp")))
		{
			fprintf(stderr, "vbox: can't get service 'vboxd/tcp' - set to 20012\n");
			nVBOXServerPort = 20012;
		}
		else
		{
			nVBOXServerPort = ntohs(vboxdserv->s_port);
		}
	}
	m_nVBOXServerPort = nVBOXServerPort;

	m_bNewMsgs = false;
}

cVBOXClient::~cVBOXClient()
{
	Disconnect();
	if(m_pMsgListBuffer)
		free(m_pMsgListBuffer);
	if(m_sVBOXServer)
		free(m_sVBOXServer);
	if(m_sUser)
		free(m_sUser);
	if(m_sPass)
		free(m_sPass);
	delete m_pLibVBOX;
}

cVBOXClient::eError cVBOXClient::Connect()
{
	Disconnect();
	if (m_pLibVBOX->vboxd_connect(m_sVBOXServer, m_nVBOXServerPort) != VBOXC_ERR_OK)
	{
		return vboxCantConnect;
	}

	if (m_pLibVBOX->vboxd_login(m_sUser, m_sPass) != VBOXC_ERR_OK)
	{
		Disconnect();
		return vboxCantLogin;
	}
	return vboxSuccess;
}

cVBOXClient::eError cVBOXClient::Disconnect()
{
	m_pLibVBOX->vboxd_disconnect();
	return vboxSuccess;
}
bool cVBOXClient::IsConnected()
{
/*
 *     bool lt;
	lt = m_pLibVBOX->vboxd_isconnected();
	return lt;
*/
	return m_pLibVBOX->vboxd_isconnected();
}

void cVBOXClient::KeepAlive()
{ 
	m_pLibVBOX->vboxd_keepalive();
}

cVBOXClient::eError cVBOXClient::TransferMsgList()
{
	struct tVboxMsg *mrgline;
	struct tVboxMsg *msg;
	char               *newline;
	char               *tmpmessagesmp;
	char               *newmessagesmp;
	int                 newmessagesnr;
	int                 o;
	int                 n;

	newmessagesmp = NULL;
	newmessagesnr = 0;
	m_bNewMsgs     = false;

	m_pLibVBOX->vboxd_put_message("list");

	while ((newline = m_pLibVBOX->vboxd_get_message()))
	{
		if ((!m_pLibVBOX->vboxd_test_response(VBOXD_VAL_LIST)) || (strlen(newline) < 5))
		{
			if (newmessagesmp) free(newmessagesmp);

			return vboxErrorList;
		}

		if (newline[4] == '.') break;

		if (newline[4] == '+')
		{
			newmessagesnr++;

			if ((tmpmessagesmp = (char *)realloc(newmessagesmp, (sizeof(struct tVboxMsg) * newmessagesnr))))
			{
				newmessagesmp = tmpmessagesmp;

				msg = (struct tVboxMsg *)(newmessagesmp + (sizeof(struct tVboxMsg) * (newmessagesnr - 1)));

				msg->ctime        = 0;
				msg->mtime        = 0;
				msg->compression  = 0;
				msg->size         = 0;
				msg->bNew          = false;
				msg->bDelete       = false;

				strcpy(msg->messagename, "");
				strcpy(msg->name       , "");
				strcpy(msg->callerid   , "");
				strcpy(msg->phone      , "");
				strcpy(msg->location   , "");
			}
			else newmessagesnr--;

			continue;
		}

		if (newmessagesnr > 0)
		{
			msg = (struct tVboxMsg *)(newmessagesmp + (sizeof(struct tVboxMsg) * (newmessagesnr - 1)));

			switch (newline[4])
			{
				case 'F':
					xstrncpy(msg->messagename, &newline[6], NAME_MAX);
					break;

				case 'N':
					xstrncpy(msg->name, &newline[6], VAH_MAX_NAME);
					break;

				case 'I':
					xstrncpy(msg->callerid, &newline[6], VAH_MAX_CALLERID);
					break;

				case 'P':
					xstrncpy(msg->phone, &newline[6], VAH_MAX_PHONE);
					break;

				case 'L':
					xstrncpy(msg->location, &newline[6], VAH_MAX_LOCATION);
					break;

				case 'T':
					msg->ctime = (time_t)xstrtoul(&newline[6], 0);
					break;

				case 'M':
					msg->mtime = (time_t)xstrtoul(&newline[6], 0);
					msg->bNew = msg->mtime > 0;
					break;

				case 'C':
					msg->compression = (int)xstrtol(&newline[6], 6);
					break;

				case 'S':
					msg->size = (int)xstrtol(&newline[6], 0);
					break;
			}
		}
	}

	if (newmessagesnr > 0)
	{
		if (m_pMsgListBuffer)
		{
			if (m_nMsgCountInBuffer > 0)
			{
				/*
				 * Try to merge the old and the new message list, so no status
				 * flags are lost.
				 */

				for (n = 0; n < newmessagesnr; n++)
				{
					msg = (struct tVboxMsg *)(newmessagesmp + (sizeof(struct tVboxMsg) * n));

					for (o = 0; o < m_nMsgCountInBuffer; o++)
					{
						mrgline = (struct tVboxMsg *)(m_pMsgListBuffer + (sizeof(struct tVboxMsg) * o));

						if (strcmp(mrgline->messagename, msg->messagename) == 0)
						{
							msg->bDelete = mrgline->bDelete;
						}
					}
				}
			}

			free(m_pMsgListBuffer);
		}

		if (newmessagesnr != m_nMsgCountInBuffer) m_bNewMsgs = true;

		m_pMsgListBuffer = newmessagesmp;
		m_nMsgCountInBuffer = newmessagesnr;

		qsort(m_pMsgListBuffer, m_nMsgCountInBuffer, sizeof(struct tVboxMsg), sort_message_list);
	}
	else
	{
		if (m_pMsgListBuffer)
			free(m_pMsgListBuffer);
		m_pMsgListBuffer = NULL;
		m_nMsgCountInBuffer = 0;
	}
	return vboxSuccess;
}

struct tVboxMsg *cVBOXClient::_GetMsgPtr(int nr)
{

	if ((m_pMsgListBuffer) && (m_nMsgCountInBuffer >= nr))
	{
		return (struct tVboxMsg *)(m_pMsgListBuffer + (sizeof(struct tVboxMsg) * nr));
		
	}
	else
		return NULL;
}

cVBOXClient::eError cVBOXClient::GetMsgTime(int nr, struct tm *ptmTime)
{
	struct tVboxMsg *msg;

	msg = _GetMsgPtr(nr);
	if(!msg)
		return vboxOutOfBounds;
	localtime_r(&msg->ctime,ptmTime);
	return vboxSuccess;
}


cVBOXClient::eError cVBOXClient::GetMsgPlaytime(int nr,char **ppsPlaytime)
{
	char   strtime[32];
	struct tVboxMsg *msg;
	int    secs;
	int    mins;

	msg = _GetMsgPtr(nr);
	if(!msg)
		return vboxOutOfBounds;

	secs = m_pLibVBOX->get_message_ptime(msg->compression, msg->size);
	mins = (secs / 60);
	secs = (secs - (mins * 60));

	sprintf(strtime,"%02d:%02d", mins, secs);
	*ppsPlaytime=strdup(strtime);
	return vboxSuccess;
}

cVBOXClient::eError cVBOXClient::GetMsgIsNew(int nr,bool *pbIsNew)
{
	struct tVboxMsg *msg;
	msg = _GetMsgPtr(nr);
	if(!msg)
		return vboxOutOfBounds;
	*pbIsNew = msg->bNew;
	return vboxSuccess;
}

cVBOXClient::eError cVBOXClient::GetMsgIsDeleted(int nr,bool *pbIsDeleted)
{
	struct tVboxMsg *msg;
	msg = _GetMsgPtr(nr);
	if(!msg)
		return vboxOutOfBounds;

	*pbIsDeleted = msg->bDelete;
	return vboxSuccess;

}
cVBOXClient::eError cVBOXClient::GetMsgCallerName(int nr,char **ppsCallerName)
{
	struct tVboxMsg *msg;
  *ppsCallerName = NULL;
	msg = _GetMsgPtr(nr);
	if(!msg)
		return vboxOutOfBounds;
	if(strcmp(msg->name,"*** Unknown ***") != 0)
	  *ppsCallerName = strdup(msg->name);
  else
    *ppsCallerName = NULL;
   // return NULL if no real name is available
	return vboxSuccess;
}
cVBOXClient::eError cVBOXClient::GetMsgId(int nr,char **ppsId)
{
	struct tVboxMsg *msg;
	msg = _GetMsgPtr(nr);
	if(!msg)
		return vboxOutOfBounds;
	
	*ppsId = strdup(msg->messagename);
	return vboxSuccess;
}
cVBOXClient::eError cVBOXClient::GetMsgCallerId(int nr,char **ppsCallerId)
{
	struct tVboxMsg *msg;
	msg = _GetMsgPtr(nr);
	if(!msg)
		return vboxOutOfBounds;
	if ((strcmp(msg->phone, "*** Unknown ***")) != 0 && (strcmp(msg->phone, "<not supported>") != 0) && (strcmp(msg->phone, "") != 0))
	{
		*ppsCallerId =strdup( msg->phone);
	}
	else if(strcmp(msg->callerid,"0")!=0)
    *ppsCallerId =strdup( msg->callerid);
  else
    *ppsCallerId = NULL;
	return vboxSuccess;
	
}

int cVBOXClient::GetMsgTotalCount()
{
	return m_nMsgCountInBuffer;
}
int cVBOXClient::GetMsgNewCount()
{
	struct tVboxMsg *msg;
	int              newmsgs;
	int              i;

	newmsgs = 0;

	if ((m_pMsgListBuffer) && (m_nMsgCountInBuffer > 0))
	{
		for (i = 0; i < m_nMsgCountInBuffer; i++)
		{
			msg = (struct tVboxMsg *)(m_pMsgListBuffer + (sizeof(struct tVboxMsg) * i));

			if (msg->bNew) newmsgs++;
		}
	}
	return(newmsgs);
}

cVBOXClient::eError cVBOXClient::ToggleNewFlag(int nr)
{
	char *answer;

	if ((m_pMsgListBuffer) && (m_nMsgCountInBuffer > 0))
	{
		struct tVboxMsg *msg = (struct tVboxMsg *)(m_pMsgListBuffer + (sizeof(struct tVboxMsg) * nr));

		m_pLibVBOX->vboxd_put_message("toggle %s", msg->messagename);

		if ((answer = m_pLibVBOX->vboxd_get_message()))
		{
			if (m_pLibVBOX->vboxd_test_response(VBOXD_VAL_TOGGLE))
			{
				msg->mtime = xstrtoul(&answer[4], 0);
				msg->bNew   = msg->mtime > 0;

				return vboxSuccess;
			}
			else return vboxInvalidServerResponse;
		}
		else return vboxNoAnswerFromServer;
	}
	return vboxOutOfBounds;
}



cVBOXClient::eError cVBOXClient::ToggleDeletedFlag(int nr)
{
	struct tVboxMsg *msg;

	if ((m_pMsgListBuffer) && (m_nMsgCountInBuffer > 0))
	{
		msg = (struct tVboxMsg *)(m_pMsgListBuffer + (sizeof(struct tVboxMsg) * nr));

		msg->bDelete = msg->bDelete ? false : true;
		return vboxSuccess;
	}
	return vboxOutOfBounds;
}

int cVBOXClient::GetMsgDeletedCount()
{
	struct tVboxMsg *msg;
	int                 i;
	int                 s;
	s = 0;
	if ((m_pMsgListBuffer) && (m_nMsgCountInBuffer > 0))
	{
		for (i = 0; i < m_nMsgCountInBuffer; i++)
		{
			msg = (struct tVboxMsg *)(m_pMsgListBuffer + (sizeof(struct tVboxMsg) * i));

			if (msg->bDelete) s++;
		}
	}
	return s;
}

cVBOXClient::eError cVBOXClient::DeleteMarkedMessages(bool bReloadList)
{
	for (int i = 0; i < m_nMsgCountInBuffer; i++)
	{
		struct tVboxMsg *msg = (struct tVboxMsg *)(m_pMsgListBuffer + (sizeof(struct tVboxMsg) * i));

		if (msg->bDelete)
		{	
			m_pLibVBOX->vboxd_put_message("delete %s", msg->messagename);

			if (m_pLibVBOX->vboxd_get_message())
			{
				if (!m_pLibVBOX->vboxd_test_response(VBOXD_VAL_DELETEOK))
				{
					return vboxInvalidServerResponse;
				}
			}
			else 
			{
				return vboxNoAnswerFromServer;
			}
		}
	}
	if(m_pMsgListBuffer)
		free(m_pMsgListBuffer);
	m_pMsgListBuffer = NULL;
	m_nMsgCountInBuffer = 0;
	if(bReloadList)
		return TransferMsgList();//reload list
	else
		return vboxSuccess;
}

cVBOXClient::eError cVBOXClient::GetServerNewMsgCount(int *pCount,time_t *pTimeLatest)
{
	char *answer = NULL;
	*pCount = 0;
	m_pLibVBOX->vboxd_put_message("count");

	if ((answer = m_pLibVBOX->vboxd_get_message()))
	{
		if (m_pLibVBOX->vboxd_test_response(VBOXD_VAL_COUNT))
		{
			char *pStop = NULL;
			*pCount = strtoul(&answer[4],&pStop,10 );
			char *szTime = pStop;
			*pTimeLatest = (time_t) strtoul(szTime,&pStop,10 );
			return vboxSuccess;
		}
		else return vboxInvalidServerResponse;
	}
	else return vboxNoAnswerFromServer;

}

cVBOXClient::eError cVBOXClient::CreateAuFileFromMsg(int nr,const char* szAuFileName, const char *szAuConvTool)
{
	struct tVboxMsg	   *msg;
	char                msgname[1024];
	char               *command;
	char               *answer;
	int                 size;
	int                 have;
	int                 fd;

	if ((!m_pMsgListBuffer) || nr >= m_nMsgCountInBuffer) return vboxOutOfBounds;

	msg = (struct tVboxMsg *)(m_pMsgListBuffer + (sizeof(struct tVboxMsg) * nr));
	if (!msg)
	{// could not happen, but who knows
		return vboxOutOfBounds; // could not happen
	}

	strcpy(msgname, "/tmp/vboxXXXXXX");
	if ((fd = mkstemp(msgname)) == -1)
	{
		return vboxErrorCreateTmpFile;
	}
	cVBOXClient::eError err = vboxSuccess;
	m_pLibVBOX->vboxd_put_message("message %s", msg->messagename);

	if ((answer = m_pLibVBOX->vboxd_get_message()))
	{
		if (m_pLibVBOX->vboxd_test_response(VBOXD_VAL_MESSAGE))
		{
			if ((size = (int)xstrtol(&answer[4], 0)) > 0)
			{
				if ((have = m_pLibVBOX->vboxd_CreateAndLoadMessageFromServer(fd, size)) == size)
				{
					if ((answer = m_pLibVBOX->vboxd_get_message()))
					{
						if (!m_pLibVBOX->vboxd_test_response(VBOXD_VAL_MESSAGE))
						{
							err = vboxErrorReadingMsgData;
						}
					}
					else err = vboxErrorReadingMsgData;

					close(fd);
					fd=-1;
					if(err == vboxSuccess)
					{
						size = 100 +strlen(szAuFileName) + strlen(msgname) + strlen(szAuConvTool);
		
						if ((command = (char*)malloc(size)))
						{
							sprintf(command, "%s %s %s 2>/dev/null", szAuConvTool, msgname, szAuFileName);
							// isyslog(command);
							// printf(command, "%s %s %s\n", szAuConvTool, msgname, szAuFileName);
							
							system(command);
							free(command);
						}
					}
				}
				else err = vboxErrorReadingMsgData;
			}
			else err = vboxErrorReadingMsgData;
		}
		else err = vboxErrorReadingMsgData;
	}
	else err = vboxErrorReadingMsgData;

	if (fd != -1) close(fd);

	unlink(msgname);
	return err;
}

cVBOXClient::eError cVBOXClient::GetServerStatusCtrl(eCtrl ctrlid,int *pStatus)
{
	if(m_pLibVBOX->vboxd_statusctrl(_GetCtrlStringFromEnum(ctrlid),pStatus) == VBOXC_ERR_OK)
		return vboxSuccess;
	return vboxInvalidServerResponse;
}

cVBOXClient::eError cVBOXClient::GetServerCreateCtrl(eCtrl ctrlid)
{
	if(m_pLibVBOX->vboxd_createctrl(_GetCtrlStringFromEnum(ctrlid)) == VBOXC_ERR_OK)
		return vboxSuccess;
	return vboxInvalidServerResponse;

}
cVBOXClient::eError cVBOXClient::GetServerRemoveCtrl(eCtrl ctrlid)
{
	if(m_pLibVBOX->vboxd_removectrl(_GetCtrlStringFromEnum(ctrlid)) == VBOXC_ERR_OK)
		return vboxSuccess;
	return vboxInvalidServerResponse;

}

const char * cVBOXClient::_GetCtrlStringFromEnum(eCtrl ctrlid)
{
	static const char *szCtrls[] =
	{
		CTRL_NAME_STOP     , 
		CTRL_NAME_REJECT   , 
		CTRL_NAME_ANSWERNOW, 
		CTRL_NAME_ANSWERALL, 
		CTRL_NAME_AUDIO    , 
		CTRL_NAME_SUSPEND  , 
		NULL               , 
	};
	return szCtrls[(int)ctrlid];
}
