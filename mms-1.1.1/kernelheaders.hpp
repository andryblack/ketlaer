#ifndef KERNELHEADERS_HPP
#define KERNELHEADERS_HPP

#define CDROM_SELECT_SPEED	0x5322  /* Set the CD-ROM speed */
#define CDROM_DISC_STATUS	0x5327  /* Get disc type, etc. */
#define CDROMREADTOCHDR		0x5305  /* Read TOC header (struct cdrom_tochdr) */

#define CDROMREADTOCENTRY    0x5306 /* Read TOC entry*/
#define CDROMMULTISESSION    0x5310 /* Obtain the start-of-last-session */

/* drive status possibilities returned by CDROM_DRIVE_STATUS ioctl */
#define CDS_NO_INFO             0       /* if not implemented */
#define CDS_NO_DISC		1
#define CDS_TRAY_OPEN		2
#define CDS_DRIVE_NOT_READY	3
#define CDS_DISC_OK		4

#define CDROM_LBA 		0x01   /* "logical block": first frame is #0 */
#define CDROM_LEADOUT           0xAA

/* return values for the CDROM_DISC_STATUS ioctl */
/* can also return CDS_NO_[INFO|DISC], from above */
#define CDS_AUDIO		100
#define CDS_DATA_1		101
#define CDS_DATA_2		102
#define CDS_XA_2_1		103
#define CDS_XA_2_2		104
#define CDS_MIXED		105

#define CD_FRAMES            75 /* frames per second */

typedef unsigned char __u8;

/* This struct is used by the CDROMREADTOCHDR ioctl */
struct cdrom_tochdr 	
{
  __u8	cdth_trk0;	/* start track */
  __u8	cdth_trk1;	/* end track */
};

/* Address in MSF format */
struct cdrom_msf0
{
  __u8    minute;
  __u8    second;
  __u8    frame;
};

/* Address in either MSF or logical format */
union cdrom_addr
{
  struct cdrom_msf0       msf;
  int                     lba;
};


struct cdrom_multisession
{
  union cdrom_addr addr; /* frame address: start-of-last-session
			    (not the new "frame 16"!).  Only valid
			    if the "xa_flag" is true. */
  __u8 xa_flag;          /* 1: "is XA disk" */
  __u8 addr_format;    /* CDROM_LBA or CDROM_MSF */
};

/* This struct is used by the CDROMREADTOCENTRY ioctl */
struct cdrom_tocentry
{
  __u8    cdte_track;
  __u8    cdte_adr        :4;
  __u8    cdte_ctrl       :4;
  __u8    cdte_format;
  union cdrom_addr cdte_addr;
  __u8    cdte_datamode;
};

#endif
