/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2023  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.36 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              STMicroelectronics International NV, 39, Chemin du Champ-des Filles, 1228 Plan Les Ouates, Geneva, SWITZERLAND
Licensed SEGGER software: emWin
License number:           GUI-00429
License model:            Buyout SRC [Buyout Source Code License, signed November 29th 2012]
Licensed product:         -
Licensed platform:        STMs ARM Cortex-M based 32 BIT CPUs
Licensed number of seats: -
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2012-12-07 - 2023-12-31
Contact to extend SUA:    sales@segger.com
-------------------------- END-OF-HEADER -----------------------------

File    : GUI_VNC_X_StartServer.c
Purpose : Starts the VNC server using plain BSD socket API and
          alternate prepares handling of an incoming WebSocket
          connection handled by the embOS/IP web server.
          By default only the plain BSD socket API will be used.
          The WebSocket handling can be activated in GUIConf.h by
          enabling GUI_VNC_SUPPORT_WEBSOCKET_SERVER .
          The WebSocket based connection is intended to be used
          with the noVNC client written in HTML5 that uses
          WebSockets for communication.
          The noVNC sample per default uses the URI "/websockify"
		  and port 80.
		  
Software: - emWin, with VNC
		  - embOS
		  - emNet (f.k.a. embOS/IP)
		  
How To
Start   : Configure the maximum amount of VNC connections with the
		  VNC_MAX_CLIENTS macro.
		  To start, simply call GUI_VNC_X_StartServer()
		  in your application, incrementing the ServerIndex parameter
		  for each desired connection.
*/

#include "GUI.h"
#include "GUI_VNC.h"
#include "RTOS.h"
#include "IP.h"
#include "TaskPrio.h"
#include <stdio.h>

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

//
// Default configuration. Use GUIConf.h for override.
//
#ifndef   GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER
  #define GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER  1
#endif
#ifndef   GUI_VNC_SUPPORT_WEBSOCKET_SERVER
  #define GUI_VNC_SUPPORT_WEBSOCKET_SERVER     0
#endif

//
// Define maximum number of VNC clients that can connect
//
#define VNC_MAX_CLIENTS  1

#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
#include "IP_WEBSOCKET.h"
#include "IP_Webserver.h"
#include "WEBS_Conf.h"        // Stack size depends on configuration

//
// WebSocket web server configuration.
//
#ifndef   WEBSOCKET_VNC_URI
  #define WEBSOCKET_VNC_URI    "/websockify"
#endif
#ifndef   WEBSOCKET_VNC_PROTO
  #define WEBSOCKET_VNC_PROTO  ""
#endif

#endif

#define USE_RX_TASK   0  // 0: Packets are read in ISR, 1: Packets are read in a task of its own.

//
// Web server and IP stack
//
#define MAX_CONNECTIONS           1
#define BACK_LOG                 20
#define IDLE_TIMEOUT           1000  // Timeout [ms] after which the connection will be closed if no new data is received.
#define SERVER_PORT              80
#define CHILD_ALLOC_SIZE       2560  // NumBytes required from memory pool for one connection. Should be fine tuned according
                                     // to your configuration using IP_WEBS_CountRequiredMem() .
//
// Task priorities.
//
enum {
   TASK_PRIO_WEBS_CHILD = 150
  ,TASK_PRIO_WEBS_PARENT
  ,TASK_PRIO_IP_TASK           // Priority must be higher as all IP application tasks.
#if USE_RX_TASK
  ,TASK_PRIO_IP_RX_TASK        // Must be the highest priority of all IP related tasks.
#endif
};

//
// Task stack sizes that might not fit for some interfaces (multiples of sizeof(int)).
//
#ifndef   WEBS_TASK_STACK_OVERHEAD
  #define WEBS_TASK_STACK_OVERHEAD   0
#endif

#ifndef   STACK_SIZE_SERVER
  #define STACK_SIZE_SERVER           (2304 + WEBS_TASK_STACK_OVERHEAD)
#endif

//
// UDP discover
//
#define ETH_UDP_DISCOVER_PORT    50020
#define PACKET_SIZE              0x80

//
// Task stack sizes that might not fit for some interfaces (multiples of sizeof(int)).
//
#ifndef   APP_TASK_STACK_OVERHEAD
  #define APP_TASK_STACK_OVERHEAD  0
#endif

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
enum {
  SOCKET_TYPE_BSD = 0,
  SOCKET_TYPE_WEBSOCKET
};

typedef struct {
  void              * pConnection;                   // Pointer to connection context. NULL if disconnected.
                                                     // Set (if NULL) when accepting a connection to tell the VNC
                                                     // server task that a new connection has been opened.
  U8                  SocketType;                    // Socket type of this connection instance.
  U8                  MessageType;                   // Last received WebSocket message type.
  GUI_VNC_CONTEXT     Context;                       // VNC context necessary for GUI.
#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
  WEBS_WEBSOCKET_HOOK WebSocketHook;                 // WebSocket resources.
#endif
  U8                  acBuffer[GUI_VNC_BUFFER_SIZE]; // Buffer used for reading RFB protocol.
} GUI_IP_VNC_CONNECTION;

typedef struct {
  OS_STACKPTR int VNCServer_Stack[(4000 + APP_TASK_STACK_OVERHEAD)/sizeof(int)];
  OS_TASK         VNCServer_TCB;
  char            acName[16];
} GUI_VNC_TASK;

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/

#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
static int  _cbWebSocket_GenerateAcceptKey (WEBS_OUTPUT* pOutput, void* pSecWebSocketKey, int SecWebSocketKeyLen, void* pBuffer, int BufferSize);
static void _cbWebSocket_DispatchConnection(WEBS_OUTPUT* pOutput, void* pConnection);
static int  _cbWebSocket_Recv              (IP_WEBSOCKET_CONTEXT* pContext, IP_WEBSOCKET_CONNECTION* pConnection,       void* pData, unsigned NumBytes);
static int  _cbWebSocket_Send              (IP_WEBSOCKET_CONTEXT* pContext, IP_WEBSOCKET_CONNECTION* pConnection, const void* pData, unsigned NumBytes);
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GUI_IP_VNC_CONNECTION _aConnections[VNC_MAX_CLIENTS];
static GUI_VNC_TASK          _aTasks      [VNC_MAX_CLIENTS];
static GUI_VNC_CONTEXT     * _pFirstContext;

//
// Task stacks and Task-Control-Blocks for main IP task.
//
static OS_STACKPTR int Stack_IP_Main[1536];
static OS_TASK         TCB_IP_Main;

#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER

static const IP_WEBS_WEBSOCKET_API _WebServerWebSocketAPI = {
  _cbWebSocket_GenerateAcceptKey,  // pfGenerateAcceptKey
  _cbWebSocket_DispatchConnection  // pfDispatchConnection
};

static const IP_WEBSOCKET_TRANSPORT_API _WebSocketTransportAPI = {
  _cbWebSocket_Recv,  // pfReceive
  _cbWebSocket_Send,  // pfSend
  NULL,               // pfGenMaskKey
};

//
// WebServer related
//
static U32                   _aPool[(CHILD_ALLOC_SIZE * MAX_CONNECTIONS) / sizeof(int)];  // Memory pool for the Web server child tasks.

#endif

//
// IP task related
//
static IP_HOOK_ON_STATE_CHANGE _StateChangeHook;
static int                     _IFaceId;
static U8                      _IPStackInit;

//
// Task stacks and Task-Control-Blocks.
//
static OS_STACKPTR int _IPStack[TASK_STACK_SIZE_IP_TASK/sizeof(int)];       // Stack of the IP_Task.
static OS_TASK         _IPTCB;                                              // Task-Control-Block of the IP_Task.

#if USE_RX_TASK
static OS_STACKPTR int _IPRxStack[TASK_STACK_SIZE_IP_RX_TASK/sizeof(int)];  // Stack of the IP_RxTask.
static OS_TASK         _IPRxTCB;                                            // Task-Control-Block of the IP_RxTask.
#endif

#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER

static int             _ConnectCnt;
//
// Webserver TCBs and stacks
//
static OS_TASK         _aWebTasks[MAX_CONNECTIONS];
static OS_STACKPTR int _aWebStacks[MAX_CONNECTIONS][STACK_SIZE_SERVER/sizeof(int)];

//
// File system info
//
static const IP_FS_API *_pFS_API;

#endif

/*********************************************************************
*
*       WebServer related
*
**********************************************************************
*/


/*********************************************************************
*
*       _OnStateChange()
*
* Function description
*   Callback that will be notified once the state of an interface
*   changes.
*
* Parameters
*   IFaceId   : Zero-based interface index.
*   AdminState: Is this interface enabled ?
*   HWState   : Is this interface physically ready ?
*/
static void _OnStateChange(unsigned IFaceId, U8 AdminState, U8 HWState) {
  //
  // Check if this is a disconnect from the peer or a link down.
  // In this case call IP_Disconnect() to get into a known state.
  //
  if (((AdminState == IP_ADMIN_STATE_DOWN) && (HWState == 1)) ||  // Typical for dial-up connection e.g. PPP when closed from peer. Link up but app. closed.
      ((AdminState == IP_ADMIN_STATE_UP)   && (HWState == 0))) {  // Typical for any Ethernet connection e.g. PPPoE. App. opened but link down.
    IP_Disconnect(IFaceId);                                       // Disconnect the interface to a clean state.
  }
}

#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
/*********************************************************************
*
*       _closesocket()
*
*  Function description
*    Wrapper for closesocket()
*/
static int _closesocket(long pConnectionInfo) {
  int r;
  struct linger Linger;

  Linger.l_onoff  = 1;  // Enable linger for this socket to verify that all data is send.
  Linger.l_linger = 1;  // Linger timeout in seconds
  setsockopt((long)pConnectionInfo, SOL_SOCKET, SO_LINGER, &Linger, sizeof(Linger));
  r = closesocket((long)pConnectionInfo);
  return r;
}

/*********************************************************************
*
*       _Recv()
*
*  Function description
*    Wrapper for recv()
*/
static int _Recv(unsigned char *buf, int len, void *pConnectionInfo) {
  int r;

  r = recv((long)pConnectionInfo, (char *)buf, len, 0);
  return r;
}

/*********************************************************************
*
*       _Send()
*
*  Function description
*    Wrapper for send()
*/
static int _Send(const unsigned char *buf, int len, void* pConnectionInfo) {
  int r;

  r = send((long)pConnectionInfo, (const char *)buf, len, 0);
  return r;
}

/*********************************************************************
*
*       WEBS_IP_API
*
*  Description
*   IP related function table
*/
static const WEBS_IP_API _Webs_IP_API = {
  _Send,
  _Recv
};

/*********************************************************************
*
*       _Alloc()
*
*  Function description
*    Wrapper for Alloc(). (emNet: IP_MEM_Alloc())
*/
static void * _Alloc(U32 NumBytesReq) {
  return IP_AllocEx(_aPool, NumBytesReq);
}

/*********************************************************************
*
*       _Free()
*
*  Function description
*    Wrapper for Alloc(). (emNet: IP_MEM_Alloc())
*/
static void _Free(void *p) {
  IP_Free(p);
}

/*********************************************************************
*
*       WEBS_SYS_API
*
*  Description
*   System related function table
*/
static const WEBS_SYS_API _Webs_SYS_API = {
  _Alloc,
  _Free
};

/*********************************************************************
*
*       _AddToConnectCnt
*/
static void _AddToConnectCnt(int Delta) {
  OS_IncDI();
  _ConnectCnt += Delta;
  OS_DecRI();
}

/*********************************************************************
*
*       _WebServerChildTask
*/
static void _WebServerChildTask(void *pContext) {
  WEBS_CONTEXT ChildContext;
  long hSock;
  int  Opt;
  int  r;

  hSock    = (long)pContext;
  Opt      = 1;
  setsockopt(hSock, SOL_SOCKET, SO_KEEPALIVE, &Opt, sizeof(Opt));
  IP_WEBS_Init(&ChildContext, &_Webs_IP_API, &_Webs_SYS_API, _pFS_API, &WebsSample_Application);  // Initialize the context of the child task.
  if (_ConnectCnt < MAX_CONNECTIONS) {
    r = IP_WEBS_ProcessEx(&ChildContext, pContext, NULL);
  } else {
    r = IP_WEBS_ProcessLastEx(&ChildContext, pContext, NULL);
  }
  if (r != WEBS_CONNECTION_DETACHED) {
    //
    // Only close the socket if it is still in web server context and has
    // not been detached to another handler like a WebSocket handler.
    //
    _closesocket(hSock);
  }
  OS_EnterRegion();
  _AddToConnectCnt(-1);
  OS_Terminate(0);
  OS_LeaveRegion();
}

/*********************************************************************
*
*       _WebServerParentTask
*/
static void _WebServerParentTask(void) {
  struct sockaddr_in InAddr;
  U32  Timeout;
  U32  NumBytes;
  long hSockListen;
  long hSock;
  int  i;
  int  t;
  int  t0;
  int  r;
  WEBS_BUFFER_SIZES BufferSizes;

  Timeout = IDLE_TIMEOUT;
  //
  // Assign file system
  //
  _pFS_API = &IP_FS_ReadOnly;  // To use a a real filesystem like emFile replace this line.
//  _pFS_API = &IP_FS_emFile;    // Use emFile
//  IP_WEBS_AddUpload();         // Enable upload
  //
  // Configure buffer size.
  //
  IP_MEMSET(&BufferSizes, 0, sizeof(BufferSizes));
  BufferSizes.NumBytesInBuf       = WEBS_IN_BUFFER_SIZE;
  BufferSizes.NumBytesOutBuf      = IP_TCP_GetMTU(_IFaceId) - 72;  // Use max. MTU configured for the last interface added minus worst case IPv4/TCP/VLAN headers.
                                                                   // Calculation for the memory pool is done under assumption of the best case headers with - 40 bytes.
  BufferSizes.NumBytesParaBuf     = WEBS_PARA_BUFFER_SIZE;
  BufferSizes.NumBytesFilenameBuf = WEBS_FILENAME_BUFFER_SIZE;
  BufferSizes.MaxRootPathLen      = WEBS_MAX_ROOT_PATH_LEN;
  //
  // Configure the size of the buffers used by the Webserver child tasks.
  //
  IP_WEBS_ConfigBufSizes(&BufferSizes);
  //
  // Check memory pool size.
  //
  NumBytes = IP_WEBS_CountRequiredMem(NULL);     // Get NumBytes for internals of one child thread.
  NumBytes = (NumBytes + 64) * MAX_CONNECTIONS;  // Calc. the total amount for x connections (+ some bytes for managing a memory pool).
  IP_Logf_Application("WEBS: Using a memory pool of %lu bytes for %lu connections.", sizeof(_aPool), MAX_CONNECTIONS);
  if (NumBytes > sizeof(_aPool)) {
    IP_Warnf_Application("WEBS: Memory pool should be at least %lu bytes.", NumBytes);
  }
  //
  // Give the stack some more memory to enable the dynamical memory allocation for the Web server child tasks
  //
  IP_AddMemory(_aPool, sizeof(_aPool));
  //
  // Get a socket into listening state
  //
  hSockListen = socket(AF_INET, SOCK_STREAM, 0);
  if (hSockListen == SOCKET_ERROR) {
    while(1); // This should never happen!
  }
  IP_MEMSET(&InAddr, 0, sizeof(InAddr));
  InAddr.sin_family      = AF_INET;
  InAddr.sin_port        = htons(SERVER_PORT);
  InAddr.sin_addr.s_addr = INADDR_ANY;
  bind(hSockListen, (struct sockaddr *)&InAddr, sizeof(InAddr));
  listen(hSockListen, BACK_LOG);
  //
  // Loop once per client and create a thread for the actual server
  //
  do {
Next:
    //
    // Wait for an incoming connection
    //
    hSock = 0;
    if ((hSock = accept(hSockListen, NULL, NULL)) == SOCKET_ERROR) {
      continue;    // Error
    }
    //
    // Create server thread to handle connection.
    // If connection limit is reached, keep trying for some time before giving up and outputting an error message
    //
    t0 = OS_GetTime32() + 1000;
    do {
      if (_ConnectCnt < MAX_CONNECTIONS) {
        for (i = 0; i < MAX_CONNECTIONS; i++) {
          r = OS_IsTask(&_aWebTasks[i]);
          if (r == 0) {
            setsockopt(hSock, SOL_SOCKET, SO_RCVTIMEO, &Timeout, sizeof(Timeout));  // Set receive timeout for the client.
            OS_CREATETASK_EX(&_aWebTasks[i], "Webserver Child", _WebServerChildTask, TASK_PRIO_WEBS_CHILD, _aWebStacks[i], (void *)hSock);
            _AddToConnectCnt(1);
            goto Next;
          }
        }
      }
      //
      // Check time out
      //
      t = OS_GetTime32();
      if ((t - t0) > 0) {
        IP_WEBS_OnConnectionLimit(_Send, _Recv, (void*)hSock);
        _closesocket(hSock);
        break;
      }
      OS_Delay(10);
    } while(1);
  }  while(1);
}

/*********************************************************************
*
*       _OnRx
*
*  Function description
*    Discover client UDP callback. Called from stack
*    whenever we get a discover request.
*
*  Return value
*    IP_RX_ERROR  if packet is invalid for some reason
*    IP_OK        if packet is valid
*/
#if ETH_UDP_DISCOVER_PORT
static int _OnRx(IP_PACKET *pInPacket, void *pContext) {
  char *      pInData;
  IP_PACKET * pOutPacket;
  char *      pOutData;
  U32         TargetAddr;
  U32         IPAddr;
  unsigned    IFaceId;

  (void)pContext;

  IFaceId = IP_UDP_GetIFIndex(pInPacket);  // Find out the interface that the packet came in.
  IPAddr  = htonl(IP_GetIPAddr(IFaceId));
  if (IPAddr == 0) {
    goto Done;
  }
  pInData = (char*)IP_UDP_GetDataPtr(pInPacket);
  if (memcmp(pInData, "Discover", 8)) {
    goto Done;
  }
  //
  // Alloc packet
  //
  pOutPacket = IP_UDP_AllocEx(IFaceId, PACKET_SIZE);
  if (pOutPacket == NULL) {
    goto Done;
  }
  //
  // Fill packet with data
  //
  pOutData = (char*)IP_UDP_GetDataPtr(pOutPacket);
  IP_UDP_GetSrcAddr(pInPacket, &TargetAddr, sizeof(TargetAddr));    // Send back Unicast
  memset(pOutData, 0, PACKET_SIZE);
  strcpy(pOutData + 0x00, "Found");
  IPAddr = htonl(IP_GetIPAddr(IFaceId));
  memcpy(pOutData + 0x20, (void*)&IPAddr, 4);      // 0x20: IP address
  IP_GetHWAddr(IFaceId, (U8*)pOutData + 0x30, 6);  // 0x30: MAC address
  //
  // Send packet
  //
  IP_UDP_SendAndFree(IFaceId, TargetAddr, ETH_UDP_DISCOVER_PORT, ETH_UDP_DISCOVER_PORT, pOutPacket);
Done:
  return IP_OK;
}
#endif

#endif

/*********************************************************************
*
*       _StartIPTask
*
*  Function description
*    Task that initializes the IP stack and starts the WebServer tasks
*    if required.
*/
static void _StartIPTask(void) {
  int NumInterfaces;
  int i;
  int Ready;
  IP_Init();
  _IFaceId = IP_INFO_GetNumInterfaces() - 1;                                           // Get the last registered interface ID as this is most likely the interface we want to use in this sample.
  //
  // Start TCP/IP task
  //
  OS_CREATETASK(&_IPTCB,   "IP_Task",   IP_Task,   TASK_PRIO_IP_TASK,    _IPStack);
#if USE_RX_TASK
  OS_CREATETASK(&_IPRxTCB, "IP_RxTask", IP_RxTask, TASK_PRIO_IP_RX_TASK, _IPRxStack);  // Start the IP_RxTask, optional.
#endif
  IP_AddStateChangeHook(&_StateChangeHook, _OnStateChange);                            // Register hook to be notified on disconnects.
  NumInterfaces = IP_INFO_GetNumInterfaces();
  for (i = 0; i < NumInterfaces; i++) {
    IP_Connect(i);                                                                // Connect the interface if necessary.
  }
  //
  // IPv4 address configured ?
  //
  Ready = 0;
  while (Ready == 0) {
    for (i = 0; i < NumInterfaces; i++) {
      if (IP_IFaceIsReadyEx(i)) {
        _IFaceId = i;
        Ready = 1;
        break;
      }
    }
    OS_Delay(200);
  }
#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
  OS_SetPriority(OS_GetTaskID(), TASK_PRIO_WEBS_PARENT);                                  // This task has highest prio!
  OS_SetTaskName(OS_GetTaskID(), "IP_WebServer");
#if ETH_UDP_DISCOVER_PORT
  //
  // Open UDP port ETH_UDP_DISCOVER_PORT for emNet discover
  //
  IP_UDP_Open(0L /* any foreign host */,  ETH_UDP_DISCOVER_PORT, ETH_UDP_DISCOVER_PORT,  _OnRx, 0L);
#endif

  IP_WEBS_X_SampleConfig();  // Load a web server sample config that might add other resources like REST.
  _WebServerParentTask();
#endif
#if GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER
  //
  // When IP has been initialized, we are done here and the task can be terminated.
  //
  OS_Terminate(OS_GetTaskID());
#endif
}

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetTaskIndex()
*
*  Function description
*    Returns the zero-based index of the currently scheduled VNC task.
*
*  Return value
*    Task index : >=  0
*    Error      : == -1
*/
static int _GetTaskIndex(void) {
  OS_TASK * pTask;
  int       i;

  pTask = OS_GetTaskID();
  i     = 0;
  //
  // Look for index of this task.
  //
  while (&_aTasks[i].VNCServer_TCB != pTask) {
    if (i >= VNC_MAX_CLIENTS) {
      return -1;
    }
    i++;
  }
  //
  return i;
}

#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER

/*********************************************************************
*
*       _cbWebSocket_GenerateAcceptKey()
*
*  Function description
*    Generates the value to send back for the Sec-WebSocket-Accept
*    field when accepting the connection.
*
*  Parameters
*    pOutput           : Web server connection context.
*    pSecWebSocketKey  : Pointer to a buffer containing the string of
*                        the Sec-WebSocket-Key from the HTTP request.
*    SecWebSocketKeyLen: Number of characters of the Sec-WebSocket-Key
*                        (without string termination).
*    pBuffer           : Buffer where to store the accept key.
*    BufferSize        : Size of buffer where to store the accept key.
*
*  Return value
*    Length of accept key        : >  0
*    Error, buffer not big enough: == 0
*/
static int _cbWebSocket_GenerateAcceptKey(WEBS_OUTPUT* pOutput, void* pSecWebSocketKey, int SecWebSocketKeyLen, void* pBuffer, int BufferSize) {
  WEBS_USE_PARA(pOutput);

  return IP_WEBSOCKET_GenerateAcceptKey(pSecWebSocketKey, SecWebSocketKeyLen, pBuffer, BufferSize);
}

/*********************************************************************
*
*       _cbWebSocket_DispatchConnection()
*
*  Function description
*    Dispatches the web server connection to the VNC server for
*    further handling and signals the VNC server task.
*
*  Parameters
*    pOutput    : Web server connection context.
*    pConnection: Network connection handle.
*/
static void _cbWebSocket_DispatchConnection(WEBS_OUTPUT* pOutput, void* pConnection) {
  int i;

  WEBS_USE_PARA(pOutput);
  OS_EnterRegion();  // Make sure that _pConnection does not get set from another task after being read by us.
  //
  // Check current connections
  //
  for (i = 0; i < VNC_MAX_CLIENTS; i++) {
    if (_aConnections[i].pConnection == pConnection) {
      break;
    }
    if (_aConnections[i].pConnection == NULL) {
      _aConnections[i].pConnection = pConnection;
      _aConnections[i].SocketType  = SOCKET_TYPE_WEBSOCKET;
      OS_LeaveRegion();
      return;
    }
  }
  //
  OS_LeaveRegion();
  closesocket((long)pConnection);
}

/*********************************************************************
*
*       _cbWebSocket_Recv()
*
*  Function description
*    WebSocket callback that for sending data using the underlying
*    network communication API (typically BSD socket API).
*
*  Parameters
*    pContext   : WebSocket context.
*    pConnection: Network connection handle.
*    pData      : Where to store the received data.
*    NumBytes   : Maximum amount of data to receive.
*
*  Return value
*    Amount of data received: >  0
*    Connection closed      : == 0
*    Error                  : <  0
*/
static int _cbWebSocket_Recv(IP_WEBSOCKET_CONTEXT* pContext, IP_WEBSOCKET_CONNECTION* pConnection, void* pData, unsigned NumBytes) {
  IP_USE_PARA(pContext);

  return recv((long)pConnection, (char*)pData, NumBytes, 0);
}

/*********************************************************************
*
*       _cbWebSocket_Send()
*
*  Function description
*    WebSocket callback that for sending data using the underlying
*    network communication API (typically BSD socket API).
*
*  Parameters
*    pContext   : WebSocket context.
*    pConnection: Network connection handle.
*    pData      : Data to send.
*    NumBytes   : Amount of data to send.
*
*  Return value
*    Amount of data sent: >  0
*    Connection closed  : == 0
*    Error              : <  0
*/
static int _cbWebSocket_Send(IP_WEBSOCKET_CONTEXT* pContext, IP_WEBSOCKET_CONNECTION* pConnection, const void* pData, unsigned NumBytes) {
  IP_USE_PARA(pContext);

  return send((long)pConnection, (const char*)pData, NumBytes, 0);
}

/*********************************************************************
*
*       _cbVNC_RecvWebSocket()
*
*  Function description
*    This function is called indirectly by the server; it's address
*    is passed to the actual server code as function pointer. It is
*    needed because the server is independent of the TCP/IP stack
*    implementation, so details for the TCP/IP stack can be placed
*    here.
*
*  Parameters
*    pData      : Where to store the received data.
*    NumBytes   : Maximum amount of data to receive.
*    pConnection: Connection handle (WebSocket context).
*
*  Return value
*    Amount of data received: >  0
*    Connection closed      : == 0
*    Error                  : <  0
*/
static int _cbVNC_RecvWebSocket(U8* pData, int NumBytes, void* pConnection) {
  IP_WEBSOCKET_CONTEXT * pContext;
  int                    r;
  int                    NumBytesAtOnce;
  int                    i;
  U8                   * pMsgType;
  U8                     Close;

  r = -1;  // Assume error.
  //
  // Get ID of task to get the correct pointer to the message type.
  // If for some reason an incorrect task index is returned, close the socket.
  //
  i        = _GetTaskIndex();
  Close    = IP_WEBSOCKET_FRAME_TYPE_CLOSE;
  pMsgType = (i < 0) ? &Close : &_aConnections[i].MessageType;
  //
  pContext = (IP_WEBSOCKET_CONTEXT*)pConnection;
  if (NumBytes != 0) {
    do {
      if (pContext->NumBytesLeft != 0u) {
        r = (int)pContext->NumBytesLeft;
      } else {
        r = IP_WEBSOCKET_WaitForNextMessage(pContext, pMsgType);
      }
      if (r > 0) {  // O.K. ?
        if        (*pMsgType == IP_WEBSOCKET_FRAME_TYPE_CLOSE) {
          IP_WEBSOCKET_Close(pContext, NULL, IP_WEBSOCKET_CLOSE_CODE_NORMAL_CLOSURE);
          r = 0;    // Pretend connection closed.
        } else if (*pMsgType == IP_WEBSOCKET_FRAME_TYPE_BINARY) {  // VNC uses binary messages.
          NumBytesAtOnce = SEGGER_MIN((U32)NumBytes, pContext->NumBytesLeft);
          r = IP_WEBSOCKET_Recv(pContext, pData, NumBytesAtOnce);
          if (r > 0) {
            NumBytes -= r;
          }
        } else {
          r = -1;   // Error, pretend communication error as we expect binary messages.
        }
      }
    } while ((r > 0) && (NumBytes != 0));
  }
  return r;
}

/*********************************************************************
*
*       _cbVNC_SendWebSocket()
*
*  Function description
*    This function is called indirectly by the server; it's address
*    is passed to the actual server code as function pointer. It is
*    needed because the server is independent of the TCP/IP stack
*    implementation, so details for the TCP/IP stack can be placed
*    here.
*
*  Parameters
*    pData      : Data to send.
*    NumBytes   : Amount of data to send.
*    pConnection: Connection handle.
*
*  Return value
*    Amount of data sent: >  0
*    Connection closed  : == 0
*    Error              : <  0
*/
static int _cbVNC_SendWebSocket(const U8* pData, int NumBytes, void* pConnection) {
  IP_WEBSOCKET_CONTEXT* pContext;
  int r;

  pContext = (IP_WEBSOCKET_CONTEXT*)pConnection;
  r        = IP_WEBSOCKET_Send(pContext, (void *)pData, NumBytes, IP_WEBSOCKET_FRAME_TYPE_BINARY, 0);  // Send a single frame (FIN bit set of data).
  if (r == 0) {  // Connection closed ? => Send our close.
    IP_WEBSOCKET_Close(pContext, NULL, IP_WEBSOCKET_CLOSE_CODE_NORMAL_CLOSURE);
  }
  return r;
}

#endif

#if GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER

/*********************************************************************
*
*       _cbVNC_Recv()
*
*  Function description
*    This function is called indirectly by the server; it's address
*    is passed to the actual server code as function pointer. It is
*    needed because the server is independent of the TCP/IP stack
*    implementation, so details for the TCP/IP stack can be placed
*    here.
*
*  Parameters
*    pData      : Where to store the received data.
*    NumBytes   : Maximum amount of data to receive.
*    pConnection: Connection handle (WebSocket context).
*
*  Return value
*    Amount of data received: >  0
*    Connection closed      : == 0
*    Error                  : <  0
*/
static int _cbVNC_Recv(U8* pData, int NumBytes, void* pConnection) {
  return recv((long)pConnection, (char*)pData, NumBytes, 0);
}

/*********************************************************************
*
*       _cbVNC_Send()
*
*  Function description
*    This function is called indirectly by the server; it's address
*    is passed to the actual server code as function pointer. It is
*    needed because the server is independent of the TCP/IP stack
*    implementation, so details for the TCP/IP stack can be placed
*    here.
*
*  Parameters
*    pData      : Data to send.
*    NumBytes   : Amount of data to send.
*    pConnection: Connection handle.
*
*  Return value
*    Amount of data sent: >  0
*    Connection closed  : == 0
*    Error              : <  0
*/
static int _cbVNC_Send(const U8* pData, int NumBytes, void* pConnection) {
  return send((long)pConnection, (const char*)pData, NumBytes, 0);
}

/*********************************************************************
*
*       _ListenAtTCPAddr()
*
*  Function description
*    Creates a non-blocking socket for accepting a VNC connection.
*
*  Parameters
*    Port: Port to use for accepting connections.
*
*  Return value
*    Parent socket handle.
*/
static int _ListenAtTCPAddr(U16 Port) {
         int         hSock;
  struct sockaddr_in InAddr;

  hSock = socket(AF_INET, SOCK_STREAM, 0);
  if (hSock <= 0) {
    while (1);  // Should not happen.
  }
  memset(&InAddr, 0, sizeof(InAddr));
  InAddr.sin_family      = AF_INET;
  InAddr.sin_port        = htons(Port);
  InAddr.sin_addr.s_addr = INADDR_ANY;
  bind(hSock, (struct sockaddr*)&InAddr, sizeof(InAddr));
  listen(hSock, 1);
  setsockopt(hSock, SOL_SOCKET, SO_NBIO, NULL, 0);  // Set socket non-blocking.
  return hSock;
}

#endif

/*********************************************************************
*
*       _ServerTask()
*
*  Function description
*    This routine is the actual server task.
*    It waits in an endless loop for a new WebSocket connection to be
*    signaled and handles the VNC connection.
*/
static void _ServerTask(void) {
#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
  IP_WEBSOCKET_CONTEXT WebSocketContext[VNC_MAX_CLIENTS];
#elif GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER
  int                  hSockParent;
  int                  SoError;
  int                  hSock;
#endif
  int                  i;
  //
  // Get ID of this task.
  //
  i = _GetTaskIndex();
#if GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER
  //
  // Start listening on a BSD socket for a connection.
  // WebSocket connections will be dispatched by the Web Server.
  //
  hSockParent = _ListenAtTCPAddr(5900 + _aConnections[i].Context.ServerIndex);  // Default port for VNC is is 590x, where x is the 0-based layer index.
#endif
  while (1) {
    //
    // Check that the connection handle is actually valid.
    // A handle of NULL is treated as invalid although some
    // TCP/IP stacks like Windows treat a socket handle of
    // 0 as valid. However this occurs very rare and in this
    // situation the peer should simply close the connection
    // after some time due to a timeout.
    // We will first check if a connection handle is present
    // which means checking for a WebSocket handle. If no
    // WebSocket handle is active we will check non-blocking
    // if there is any BSD socket to accept.
    //
#if (GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER == 0)
    if (_aConnections[i].pConnection == NULL) {
      OS_Delay(50);  // Grant the system some time between retrying.
      continue;
    }
#else
    OS_EnterRegion();  // Make sure that _pConnection does not get set from another task after being read by us.
    if (_aConnections[i].pConnection == NULL) {
      //
      // Check if we have a connection to accept on the regular VNC port.
      //
      hSock = accept(hSockParent, NULL, NULL);
      if (hSock == SOCKET_ERROR) {
        OS_LeaveRegion();
        //
        // Check if this is an IP_ERR_WOULD_BLOCK or a real error.
        //
        getsockopt(hSockParent, SOL_SOCKET, SO_ERROR, &SoError, sizeof(SoError));
        if (SoError != IP_ERR_WOULD_BLOCK) {
          while(1);    // Should not happen.
        }
        OS_Delay(50);  // Grant the system some time between retrying.
        continue;
      }
      //
      // We have a connection on the regular VNC port.
      //
      setsockopt(hSock, SOL_SOCKET, SO_BIO, NULL, 0);  // Set child socket blocking.
      _aConnections[i].SocketType  = SOCKET_TYPE_BSD;
      _aConnections[i].pConnection = (void*)hSock;
    }
    OS_LeaveRegion();
#endif
#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
    if (_aConnections[i].SocketType == SOCKET_TYPE_WEBSOCKET) {
      //
      // Initialize the WebSocket context for the server.
      //
      IP_WEBSOCKET_InitServer(&WebSocketContext[i], &_WebSocketTransportAPI, (IP_WEBSOCKET_CONNECTION*)_aConnections[i].pConnection);
      //
      // Run the actual server using the WebSocket context as connection handle.
      //
      GUI_VNC_Process(&_aConnections[i].Context, _cbVNC_SendWebSocket, _cbVNC_RecvWebSocket, (void*)&WebSocketContext[i]);
    }
#endif
#if GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER
    if (_aConnections[i].SocketType == SOCKET_TYPE_BSD) {
      //
      // Run the actual server using the BSD socket handle as connection handle.
      //
      GUI_VNC_Process(&_aConnections[i].Context, _cbVNC_Send, _cbVNC_Recv, _aConnections[i].pConnection);
    }
#endif
    //
    // Close the connection and invalidate the connection handle
    // so we can accept a new client.
    //
    closesocket((long)_aConnections[i].pConnection);
    _aConnections[i].pConnection = NULL;
  }
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_VNC_X_StartServer()
*
*  Function description
*    This routine starts a separate task for the VNC server. It
*    requires that the OS and TCP/IP stack are already initialized.
*
*  Parameters
*    LayerIndex : Index of the GUI layer that is shown via VNC.
*    ServerIndex: Zero-based server index.
*
*  Return value
*    O.K.: 0
*
*  Additional information
*    This sample is intended to be used for starting one VNC server.
*    In case multiple layers shall be accessed by different server
*    instances (ServerIndex) this sample needs to be modified.
*/
int GUI_VNC_X_StartServer(int LayerIndex, int ServerIndex) {
  GUI_VNC_CONTEXT * pContext;
  GUI_VNC_CONTEXT * pContextTemp;

  //
  // Check if maximum number of VNC clients is not exceeded.
  //
  if (ServerIndex >= VNC_MAX_CLIENTS) {
    return -1;
  }
  pContext = &_aConnections[ServerIndex].Context;
  //
  // Init VNC context and attach to layer (so context is
  // updated if the display-layer-contents change).
  //
  // If we already have a device, set the device pointer
  // to the already created one.
  //
  if (_pFirstContext == NULL) {
    GUI_VNC_AttachToLayer(pContext, LayerIndex);
    _pFirstContext = pContext;
  } else {
    //
    // New context makes use of existing device.
    //
    pContext->pDevice      = _pFirstContext->pDevice;
    pContext->xSize        = _pFirstContext->xSize;
    pContext->ySize        = _pFirstContext->ySize;
    pContext->BitsPerPixel = _pFirstContext->BitsPerPixel;
    pContext->pNext        = NULL;
    //
    // Update pNext pointers
    //
    pContextTemp = _pFirstContext;
    while (pContextTemp->pNext) {
      pContextTemp = pContextTemp->pNext;
    }
    pContextTemp->pNext = pContext;
  }
  pContext->ServerIndex = ServerIndex;
  //
  // Set up pointer to RFB buffer.
  //
  pContext->pBuffer      = _aConnections[ServerIndex].acBuffer;
  pContext->SizeOfBuffer = GUI_VNC_BUFFER_SIZE;
  //
  // If this is the first time a VNC server is opened, start the IP stack.
  //
  if (!_IPStackInit) {
    OS_CREATETASK(&TCB_IP_Main, "IP_MainTask", _StartIPTask, 110, Stack_IP_Main);
    _IPStackInit = 1;
  }
  //
  // Create task for VNC server. It will wait for the signal
  // of a dispatched WebSocket connection.
  //
  sprintf(_aTasks[ServerIndex].acName, "VNC Server %d", ServerIndex);
  OS_CREATETASK(&_aTasks[ServerIndex].VNCServer_TCB, _aTasks[ServerIndex].acName, _ServerTask, TASKPRIO_VNC, _aTasks[ServerIndex].VNCServer_Stack);
#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
  //
  // Enable WebSockets for the web server and setup the
  // URI "/websockify" with sub protocol "binary" for the
  // VNC data transfer.
  //
  IP_WEBS_WEBSOCKET_AddHook(&_aConnections[ServerIndex].WebSocketHook, &_WebServerWebSocketAPI, WEBSOCKET_VNC_URI, WEBSOCKET_VNC_PROTO);
#endif
  return 0;
}

/*********************************************************************
*
*       GUI_VNC_X_getpeername()
*
*  Function description
*    Retrieves the IP addr. of the currently connected VNC client.
*
*  Parameters
*    ServerIndex: Index of server instance.
*    pIPAddr:[OUT] VNC client connected: U32 IP addr. in network endianess.
*                  No client connected : 0
*/
void GUI_VNC_X_getpeername(int ServerIndex, U32 * pIPAddr) {
  struct sockaddr_in Client;
  int                Len;
  void             * pConnection;

  if ((ServerIndex >= VNC_MAX_CLIENTS) || (ServerIndex < 0)) {
    return;
  }
  if (pIPAddr) {
    pConnection = _aConnections[ServerIndex].pConnection;
    if (pConnection != NULL) {
      Len   = sizeof(Client);
      getpeername((long)pConnection, (struct sockaddr*)&Client, &Len);
      *pIPAddr = Client.sin_addr.s_addr;
    } else {
      *pIPAddr = 0;
    }
  }
}

/*************************** End of file ****************************/
