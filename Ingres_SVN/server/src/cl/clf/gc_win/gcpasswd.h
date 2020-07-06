#include "gcdlg.h"

#define GC_L_NODE_ID	64
#define GC_L_USER_ID	32
#define	GC_L_PASSWORD	64

#define	WM_VNODE_CACHE_LOOKUP	WM_USER+256
#define WM_VNODE_CACHE_STORE	WM_USER+257
#define WM_VNODE_CACHE_DELETE	WM_USER+258
#define WM_VNODE_CACHE_CLEAR	WM_USER+259
#define WM_VNODE_CACHE_DIAG		WM_USER+260

#define IIPROMPTONCE "IIPROMPTONCE"
#define IIPROMPT1 "IIPROMPT1"
#define IIPROMPTALL "IIPROMPTALL"

#define	ERRMSG	"Neither the Username nor Password\r\nfields can be blank"
#define IIW3VNODECACHEWINDOW "iiw3VnodeCacheWindow"
#define IIWEVNODECACHEPROG	"iiw3vche.exe"


typedef struct t_vnodecache
{
	char vnode[GC_L_NODE_ID];
	char username[GC_L_USER_ID];
	char password[GC_L_PASSWORD];
} VNODECACHE;
