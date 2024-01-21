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
          prepares handling of an incoming WebSocket connection
          handled by the embOS/IP web server.
          The WebSocket based connection is intended to be used
          with the noVNC client written in HTML5 that uses
          WebSockets for communication.
          The noVNC sample per default uses the URI "/websockify"
          and the WebSocket sub protocol "binary".
*/

#include "GUI_VNC.h"
#include "GUI_Debug.h"
#include "IP_WebServer.h"

#include <windows.h>

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
#define VNC_MAX_CLIENTS  3

#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
#include "IP_WEBSOCKET.h"
#include "IP_Webserver.h"

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

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

enum {
  SOCKET_TYPE_BSD = 0,
  SOCKET_TYPE_WEBSOCKET
};

#define LOCK(hLock)    WaitForSingleObject(hLock, INFINITE)
#define UNLOCK(hLock)  ReleaseMutex(hLock)

#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
  #define NUM_THREADS              (2 + MAX_CONNECTIONS) // Server thread, parent webserver thread and the number of child webserver threads
#else
  #define NUM_THREADS              1
#endif
//
// Internal ID for the different threads used for distinction.
//
#define ID_THREAD_SERVER         0
#define ID_THREAD_WEBS_PARENT    1
#define ID_THREAD_WEBS_CHILD     2

/*********************************************************************
*
*       Types, local
*
**********************************************************************
*/

typedef void* SYS_HANDLE;

typedef struct {
  SYS_HANDLE           hLock;
  GUI_VNC_CONTEXT      Context;
  void               * pConnection;
  U8                   acBuffer [GUI_VNC_BUFFER_SIZE];
  int                  aThreadId[NUM_THREADS];     // [0]:     Server thread ID.
                                                   // [1]:     Parent webserver thread ID.
                                                   // [2,...]: Child webserver thread ID. Can be more than one.
  U8                   SocketType;                 // SOCKET_TYPE_BSD or SOCKET_TYPE_WEBSOCKET .
#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
  WEBS_WEBSOCKET_HOOK  WebSocketHook;              // WebSocket resources.
  IP_WEBSOCKET_CONTEXT WebSocketContext;
  U8                   MessageType;                // Last received WebSocket message type.
  GUI_IP_THREAD_INFO   ThreadInfo;
#endif
} GUI_VNC_CONNECTION;

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/

UINT __stdcall WebServerParentTask(void* pContext);

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
static GUI_VNC_CONNECTION _aConnections[VNC_MAX_CLIENTS];
static GUI_VNC_CONTEXT  * _pFirstContext;

#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
static const IP_WEBS_WEBSOCKET_API _WebServerWebSocketAPI = {
  _cbWebSocket_GenerateAcceptKey,  // pfGenerateAcceptKey
  _cbWebSocket_DispatchConnection  // pfDispatchConnection
};

static const IP_WEBSOCKET_TRANSPORT_API _WebSocketTransportAPI = {
  _cbWebSocket_Recv,  // pfReceive
  _cbWebSocket_Send   // pfSend
};
#endif

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetConnection()
*
*  Function description
*    Returns a pointer to the current connection struct based on the thread ID.
*
*  Return value
*    == NULL: Error.
*    != NULL: Current connection.
*/
static GUI_VNC_CONNECTION * _GetConnection(void) {
  DWORD Id;
  int   i, j;

  Id = GetCurrentThreadId();
  //
  // Look for index of this task.
  //
  for (j = 0; j < VNC_MAX_CLIENTS; j++) {
    for (i = 0; i < NUM_THREADS; i++) {
      if (_aConnections[j].aThreadId[i] == Id) {
        return &_aConnections[j];
      }
    }
  }
  return NULL;
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
  GUI_VNC_CONNECTION * pConnect;
  int                  OnOff;

  WEBS_USE_PARA(pOutput);
  pConnect = _GetConnection();
  if (pConnect == NULL) {
    return;
  }
  LOCK(pConnect->hLock);  // Make sure that _pConnection does not get set from another task after being read by us.
  //
  // Check if no other VNC client is in process right now.
  //
  if (pConnect->pConnection == NULL) {
    //
    // Mark a VNC client in process before re-enabling
    // task switches and signaling the VNC server task.
    //
    OnOff = 0;
    ioctlsocket((PTR_ADDR)pConnection, FIONBIO, &OnOff);  // Set socket blocking.
    pConnect->SocketType  = SOCKET_TYPE_WEBSOCKET;
    pConnect->pConnection = pConnection;
    UNLOCK(pConnect->hLock);
  } else {
    UNLOCK(pConnect->hLock);
    //
    // Another VNC client is handled. Simply
    // close the new socket to close the WebSocket
    // connection as well.
    // Instead of closing the new connection the old
    // connection could be closed for continuation
    // with the new client.
    //
    closesocket((PTR_ADDR)pConnection);
  }
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
  WEBS_USE_PARA(pContext);

  return recv((PTR_ADDR)pConnection, (char*)pData, NumBytes, 0);
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
  WEBS_USE_PARA(pContext);

  return send((PTR_ADDR)pConnection, (const char*)pData, NumBytes, 0);
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
  GUI_VNC_CONNECTION   * pConnect;
  U8                   * pMsgType;
  U8                     Close;

  r = -1;  // Assume error.
  //
  // Get ID of task to get the correct pointer to the message type.
  // If for some reason an incorrect task index is returned, close the socket.
  //
  pConnect = _GetConnection();
  if (pConnect == NULL) {
    return r;
  }
  Close    = (U8)IP_WEBSOCKET_FRAME_TYPE_CLOSE;
  pMsgType = (pConnect == NULL) ? &Close : &pConnect->MessageType;
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
          NumBytesAtOnce = SEGGER_MIN(NumBytes, pContext->NumBytesLeft);
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
  return recv((PTR_ADDR)pConnection, (char*)pData, NumBytes, 0);
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
  return send((PTR_ADDR)pConnection, (const char*)pData, NumBytes, 0);
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
static PTR_ADDR _ListenAtTCPAddr(U16 Port) {
  PTR_ADDR           hSock;
  int                OnOff;
  struct sockaddr_in InAddr;

  hSock = socket(AF_INET, SOCK_STREAM, 0);
  if (hSock > 0) {
    memset(&InAddr, 0, sizeof(InAddr));
    InAddr.sin_family      = AF_INET;
    InAddr.sin_port        = htons(Port);
    InAddr.sin_addr.s_addr = INADDR_ANY;
    bind(hSock, (struct sockaddr*)&InAddr, sizeof(InAddr));
    listen(hSock, 1);
    OnOff = 1;
    ioctlsocket(hSock, FIONBIO, &OnOff);  // Set socket non-blocking.
  }
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
*
*  Parameters
*    pPara: Unused.
*
*  Return value
*    Error: 1
*/
static UINT __stdcall _ServerTask(void* pPara) {
  int                  r;
  GUI_VNC_CONNECTION * pConnect;
#if GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER
  PTR_ADDR             hSockParent;
  int                  SoError;
  PTR_ADDR             hSock;
  int                  OnOff;
  WORD                 wVersionRequested;
  WSADATA              wsaData;
#endif

  //
  // Get connection pointer.
  //
  pConnect = _GetConnection();
  if (pConnect == NULL) {
    return 1; // Error.
  }
#if GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER
  //
  // Startup Winsock.
  //
  wVersionRequested = MAKEWORD(2, 0);
  if (WSAStartup(wVersionRequested, &wsaData) != 0) {
    return 1;  // Error.
  }
  //
  // Start listening on a BSD socket for a connection.
  // WebSocket connections will be dispatched by the Web Server.
  //
  hSockParent = _ListenAtTCPAddr(5900 + pConnect->Context.ServerIndex);  // Default port for VNC is is 590x, where x is the 0-based layer index.
  if (hSockParent == 0) {                                                // 0 is a valid socket for Windows but we reject it.
    closesocket(hSockParent);
  }
  if (hSockParent <= 0) {
    return 1;  // Error.
  }
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
    if (pConnect->pConnection == NULL) {
      Sleep(50);  // Grant the system some time between retrying.
      continue;
    }
#else
    LOCK(pConnect->hLock);  // Make sure that _pConnection does not get set from another task after being read by us.
    if (pConnect->pConnection == NULL) {
      //
      // Check if we have a connection to accept on the regular VNC port.
      //
      hSock = accept(hSockParent, NULL, NULL);
      if (hSock == SOCKET_ERROR) {
        SoError = WSAGetLastError();
        UNLOCK(pConnect->hLock);
        //
        // Check if this is an IP_ERR_WOULD_BLOCK or a real error.
        //
        if (SoError != WSAEWOULDBLOCK) {
          while(1);    // Should not happen.
        }
        Sleep(50);  // Grant the system some time between retrying.
        continue;
      }
      //
      // We have a connection on the regular VNC port.
      //
      OnOff        = 0;
      ioctlsocket(hSock, FIONBIO, &OnOff);  // Set child socket blocking.
      pConnect->SocketType  = SOCKET_TYPE_BSD;
      pConnect->pConnection = (void *)hSock;
    }
    UNLOCK(pConnect->hLock);
#endif
    r = 0;
#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
    if (pConnect->SocketType == SOCKET_TYPE_WEBSOCKET) {
      //
      // Initialize the WebSocket context for the server.
      //
      IP_WEBSOCKET_InitServer(&pConnect->WebSocketContext, &_WebSocketTransportAPI, (IP_WEBSOCKET_CONNECTION*)pConnect->pConnection);
      //
      // Run the actual server using the WebSocket context as connection handle.
      //
      r = GUI_VNC_Process(&pConnect->Context, _cbVNC_SendWebSocket, _cbVNC_RecvWebSocket, (void*)&pConnect->WebSocketContext);
    }
#endif
#if GUI_VNC_SUPPORT_PLAIN_SOCKET_SERVER
    if (pConnect->SocketType == SOCKET_TYPE_BSD) {
      //
      // Run the actual server using the BSD socket handle as connection handle.
      //
      r = GUI_VNC_Process(&pConnect->Context, _cbVNC_Send, _cbVNC_Recv, (void *)pConnect->pConnection);
    }
#endif
    //
    // Close the connection and invalidate the connection handle
    // so we can accept a new client.
    //
    closesocket((PTR_ADDR)pConnect->pConnection);
    pConnect->pConnection = NULL;
    switch (r) {
    case GUI_VNC_ERROR_WRONGFORMAT:
      GUI_DEBUG_ERROROUT("VNC: Client requests unsupported pixel format");
      break;
    }
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_VNC_X_StartServer()
*
*  Function description
*    This function has to start a thread listening for an incoming connection. If a
*    connection is established it has to execute the actual VNC server GUI_VNC_Process().
*    The function has to be supplied by the customer because the implementation
*    depends on the used TCP/IP stack and on the used operating system.
*    The emWin shipment contains an example implementation under
*    \c{Sample\\GUI_X\\GUI_VNC_X_StartServer.c}. It could be used as a starting point for
*    adapting it to other systems.
*
*  Parameters
*    LayerIndex: Layer to be shown by the viewer.
*    ServerIndex: Server index.
*
*  Return value
*    ==  0: OK.
*    == -1: Error.
*
*  Additional information
*    There is no difference to start a VNC server in the simulation or on the target. In
*    both cases you should call this function. The simulation contains an implementation
*    of this function, the hardware implementation has to be done by the customer.
*/
int GUI_VNC_X_StartServer(int LayerIndex, int ServerIndex) {
  GUI_VNC_CONNECTION * pConnection;
  GUI_VNC_CONTEXT    * pContext;
  GUI_VNC_CONTEXT    * pContextTemp;

  //
  // Check if maximum number of VNC clients is not exceeded.
  //
  if (ServerIndex >= VNC_MAX_CLIENTS) {
    return -1;
  }
  pContext    = &_aConnections[ServerIndex].Context;
  pConnection = &_aConnections[ServerIndex];
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
    if (_pFirstContext->ServerIndex == ServerIndex) {
      //
      // Return error if server index is already in use.
      //
      return -1;
    }
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
  pContext->pBuffer      = pConnection->acBuffer;
  pContext->SizeOfBuffer = GUI_VNC_BUFFER_SIZE;
  //
  // Create task for VNC server. It will wait for the signal
  // of a dispatched WebSocket connection.
  //
  CreateThread(NULL, 0, _ServerTask, NULL, 0, &pConnection->aThreadId[ID_THREAD_SERVER]);
#if GUI_VNC_SUPPORT_WEBSOCKET_SERVER
  //
  // Start the web server thread.
  //
  // The GUI_IP_THREAD_INFO structure is also passed to the thread so that the thread can make
  // use of the correct port and have the correct pointer to save the child thread IDs.
  //
  // The server port is 8181 + ServerIndex
  //
  pConnection->ThreadInfo.Port      = SERVER_PORT + ServerIndex;
  pConnection->ThreadInfo.pThreadId = &pConnection->aThreadId[ID_THREAD_WEBS_CHILD];
  CreateThread(NULL, 0, WebServerParentTask, (LPVOID)&pConnection->ThreadInfo, 0, &pConnection->aThreadId[ID_THREAD_WEBS_PARENT]);
  //
  // Enable WebSockets for the web server and setup the
  // URI "/websockify" with sub protocol "" (binary) for the VNC data transfer.
  //
  IP_WEBS_WEBSOCKET_AddHook(&pConnection->WebSocketHook, &_WebServerWebSocketAPI, WEBSOCKET_VNC_URI, WEBSOCKET_VNC_PROTO);
#endif
  return 0;
}

/*********************************************************************
*
*       GUI_VNC_X_StartServerFT()
*
*  Function description
*    Function which has to be implemented by the customer to start the VNC server with
*    file transfer support. Additionally to starting a server thread the function has to
*    enable the file transfer extensions by calling GUI_VNC_EnableFileTransfer() and it
*    has to set a function table to be used for file access by GUI_VNC_SetFS_API().
*
*  Parameters
*    LayerIndex: Layer to be shown by the viewer.
*    ServerIndex: Server index.
*
*  Return value
*    Returns 0.
*
*  Additional information
*    Under \c{Sample\\GUI_X\\GUI_VNC_X_StartServer.c} a sample is available which shows
*    a sample implementation using embOS/IP and emFile.
*/
int GUI_VNC_X_StartServerFT(int LayerIndex, int ServerIndex) {
  int r;

  //
  // Set file access API (IP_FS_Win32, IP_FS_FS, ...).
  //
  GUI_VNC_SetFS_API(&IP_FS_Win32);
  //
  // Enable file transfer of VNC-server.
  //
  GUI_VNC_EnableFileTransfer(1);
  //
  // Start the VNC server.
  //
  r = GUI_VNC_X_StartServer(LayerIndex, ServerIndex);
  return r;
}

/*************************** End of file ****************************/
