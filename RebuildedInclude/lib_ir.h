
#define VENUS_IR_IOC_MAGIC                      'r'
#define VENUS_IR_IOC_SET_IRIOTCDP               _IOW(VENUS_IR_IOC_MAGIC, 1, int)
#define VENUS_IR_IOC_FLUSH_IRRP                 _IOW(VENUS_IR_IOC_MAGIC, 2, int)
#define VENUS_IR_IOC_SET_PROTOCOL               _IOW(VENUS_IR_IOC_MAGIC, 3, int)
#define VENUS_IR_IOC_SET_DEBOUNCE               _IOW(VENUS_IR_IOC_MAGIC, 4, int)
#define VENUS_IR_IOC_SET_IRPSR                  _IOW(VENUS_IR_IOC_MAGIC, 5, int)
#define VENUS_IR_IOC_SET_IRPER                  _IOW(VENUS_IR_IOC_MAGIC, 6, int)
#define VENUS_IR_IOC_SET_IRSF                   _IOW(VENUS_IR_IOC_MAGIC, 7, int)
#define VENUS_IR_IOC_SET_IRCR                   _IOW(VENUS_IR_IOC_MAGIC, 8, int)
#define VENUS_IR_IOC_SET_DRIVER_MODE    _IOW(VENUS_IR_IOC_MAGIC, 9, int)
#define VENUS_IR_IOC_MAXNR                      9

/*system/Include/Platform_Lib/RemoteControl/RemoteControl.h*/
enum _tagRemoteProtocol {IR_PROTOCOL_NEC = 1, IR_PROTOCOL_RC5, IR_PROTOCOL_SHARP, IR_PROTOCOL_SONY, IR_PROTOCOL_C03, IR_PROTOCOL_RC6};

typedef _tagRemoteProtocol ENUM_REMOTE_PROTOCOL;

