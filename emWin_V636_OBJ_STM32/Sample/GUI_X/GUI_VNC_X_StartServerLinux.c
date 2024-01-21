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
----------------------------------------------------------------------
File        : GUI_VNC_X_StartServerLinux.c
Purpose     : Starts the VNC server via TCP/IP.
              This code works with Linux.
              However, it can be easily modified to work with
              different kernel and IP Stack
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI_VNC.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GUI_VNC_CONTEXT    _Context;
static struct sockaddr_in _Addr;

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _Send
*
* Function description
*   This function is called indirectly by the server; it's address is passed to the actual
*   server code as function pointer. It is needed because the server is independent
*   of the TCP/IP stack implementation, so details for the TCP/IP stack can be placed here.
*/
static int _Send(const U8 * buf, int len, void * pConnectionInfo) {
  return send((long)pConnectionInfo, (const char *)buf, len, 0);
}

/*********************************************************************
*
*       _Recv
*
* Function description
*   This function is called indirectly by the server; it's address is passed to the actual
*   server code as function pointer. It is needed because the server is independent
*   of the TCP/IP stack implementation, so details for the TCP/IP stack can be placed here.
*/
static int _Recv(U8 * buf, int len, void * pConnectionInfo) {
  return recv((long)pConnectionInfo, (char *)buf, len, 0);
}

/*********************************************************************
*
*       _ListenAtTcpAddr
*
* Starts listening at the given TCP port.
*/
static int _ListenAtTcpAddr(U16 Port) {
  int sock;
  struct sockaddr_in addr;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(Port);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  listen(sock, 1);
  return sock;
}

/*********************************************************************
*
*       _ServerTask
*
* Function description
*   This routine is the actual server task.
*   It executes some one-time initialization code, then runs in an endless loop.
*   It therefore does not terminate.
*   In the endless loop it
*     - Waits for a connection from a client
*     - Runs the server code
*     - Closes the connection
*/
static void * _ServerTask(void * pVoid) {
  int s;
  unsigned AddrLen;
  long Sock;
  U16 Port;

  GUI_USE_PARA(pVoid);
  //
  // Prepare socket (one time setup)
  //
  Port = 5900 + _Context.ServerIndex;  // Default port for VNC is is 590x, where x is the 0-based layer index
  //
  // Loop until we get a socket into listening state
  //
  do {
    s = _ListenAtTcpAddr(Port);
    if (s != -1) {
      break;
    }
    sleep(100);  // Try again
  } while (1);
  //
  // Loop once per client and create a thread for the actual server
  //
  while (1) {
    //
    // Wait for an incoming connection
    //
    AddrLen = sizeof(_Addr);
    if ((Sock = accept(s, (struct sockaddr *)&_Addr, &AddrLen)) == -1) {
      continue;  // Error
    }
    //
    // Run the actual server
    //
    GUI_VNC_Process(&_Context, _Send, _Recv, (void *)Sock);
    //
    // Close the connection
    //
    close(Sock);
    memset(&_Addr, 0, sizeof(struct sockaddr_in));
  }
  return NULL;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_VNC_X_StartServer
*
* Function description
*   To start the server, the following steps are executed
*   - Make sure the TCP-IP stack is up and running
*   - Init the server context and attach it to the layer
*   - Start the thread (task) which runs the VNC server
* Notes:
*   (1) The first part of the code initializes the TCP/IP stack. In a typical
*       application, this is not required, since the stack should have already been
*       initialized some other place.
*       This could be done in a different module. (TCPIP_AssertInit() ?)
*/
int GUI_VNC_X_StartServer(int LayerIndex, int ServerIndex) {
  int err;
  pthread_t tid;

  //
  // Initialize VNC context and attach to layer (so context is updated if the display-layer-contents change
  //
  GUI_VNC_AttachToLayer(&_Context, LayerIndex);
  _Context.ServerIndex = ServerIndex;
  //
  // Create task for VNC Server
  //
  err = pthread_create(&tid, NULL, &_ServerTask, NULL);
  if (err != 0) {
    printf("\ncan't create thread :[%s]", strerror(err));
  } else {
    printf("\n Thread created successfully\n");
  }
  sleep(5);
  return 0;
}

/*************************** End of file ****************************/
