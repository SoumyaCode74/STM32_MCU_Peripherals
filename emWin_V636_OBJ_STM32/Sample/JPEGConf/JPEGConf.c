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
File    : JPEGConf.c
Purpose : Configuration file for emWin.
--------  END-OF-HEADER  ---------------------------------------------
*/
#include <stdlib.h>
#include "GUI_Private.h"
#include "JPEGConf.h"
#include "stm32f7xx_hal.h"
#include "RTOS.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define JPEG_TIMEOUT       (200)

#define NUMBYTES_INBUFFER  (4096 * 2)

#define EVENT_DATAREADY (1 << 0)
#define EVENT_GETDATA   (1 << 1)
#define EVENT_DESTROY   (1 << 2)
#define EVENT_READY     (1 << 3)
#define EVENT_ABORT     (1 << 4)
#define EVENT_CONTINUE  (1 << 5)
#define EVENT_MEMREADY  (1 << 6)
#define EVENT_INFOREADY (1 << 7)

#define LIMIT(x) _aLimit[x + 0x100]  // Limiting a value per table works much faster than calculating value by value

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/                 
static const U8 _aLimit[] = {
//
// Values in the range of 0xffffffff - 0xffffff00
//
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//
// Values in the range of 0x00 - 0xff
//
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
//
// Values in the range of 0x100 - 0x1ff
//
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

static const int _aCRR[] = {
  -179, -178, -177, -175, -174, -172, -171, -170, -168, -167, -165, -164, -163, -161, -160, -158,
  -157, -156, -154, -153, -151, -150, -149, -147, -146, -144, -143, -142, -140, -139, -137, -136,
  -135, -133, -132, -130, -129, -128, -126, -125, -123, -122, -121, -119, -118, -116, -115, -114,
  -112, -111, -109, -108, -107, -105, -104, -102, -101, -100,  -98,  -97,  -95,  -94,  -93,  -91,
   -90,  -88,  -87,  -86,  -84,  -83,  -81,  -80,  -79,  -77,  -76,  -74,  -73,  -72,  -70,  -69,
   -67,  -66,  -64,  -63,  -62,  -60,  -59,  -57,  -56,  -55,  -53,  -52,  -50,  -49,  -48,  -46,
   -45,  -43,  -42,  -41,  -39,  -38,  -36,  -35,  -34,  -32,  -31,  -29,  -28,  -27,  -25,  -24,
   -22,  -21,  -20,  -18,  -17,  -15,  -14,  -13,  -11,  -10,   -8,   -7,   -6,   -4,   -3,   -1,
     0,    1,    3,    4,    6,    7,    8,   10,   11,   13,   14,   15,   17,   18,   20,   21,
    22,   24,   25,   27,   28,   29,   31,   32,   34,   35,   36,   38,   39,   41,   42,   43,
    45,   46,   48,   49,   50,   52,   53,   55,   56,   57,   59,   60,   62,   63,   64,   66,
    67,   69,   70,   72,   73,   74,   76,   77,   79,   80,   81,   83,   84,   86,   87,   88,
    90,   91,   93,   94,   95,   97,   98,  100,  101,  102,  104,  105,  107,  108,  109,  111,
   112,  114,  115,  116,  118,  119,  121,  122,  123,  125,  126,  128,  129,  130,  132,  133,
   135,  136,  137,  139,  140,  142,  143,  144,  146,  147,  149,  150,  151,  153,  154,  156,
   157,  158,  160,  161,  163,  164,  165,  167,  168,  170,  171,  172,  174,  175,  177,  178,
};

static const int _aCBB[] = {
  -227, -225, -223, -221, -220, -218, -216, -214, -213, -211, -209, -207, -206, -204, -202, -200,
  -198, -197, -195, -193, -191, -190, -188, -186, -184, -183, -181, -179, -177, -175, -174, -172,
  -170, -168, -167, -165, -163, -161, -159, -158, -156, -154, -152, -151, -149, -147, -145, -144,
  -142, -140, -138, -136, -135, -133, -131, -129, -128, -126, -124, -122, -120, -119, -117, -115,
  -113, -112, -110, -108, -106, -105, -103, -101,  -99,  -97,  -96,  -94,  -92,  -90,  -89,  -87,
   -85,  -83,  -82,  -80,  -78,  -76,  -74,  -73,  -71,  -69,  -67,  -66,  -64,  -62,  -60,  -58,
   -57,  -55,  -53,  -51,  -50,  -48,  -46,  -44,  -43,  -41,  -39,  -37,  -35,  -34,  -32,  -30,
   -28,  -27,  -25,  -23,  -21,  -19,  -18,  -16,  -14,  -12,  -11,   -9,   -7,   -5,   -4,   -2,
     0,    2,    4,    5,    7,    9,   11,   12,   14,   16,   18,   19,   21,   23,   25,   27,
    28,   30,   32,   34,   35,   37,   39,   41,   43,   44,   46,   48,   50,   51,   53,   55,
    57,   58,   60,   62,   64,   66,   67,   69,   71,   73,   74,   76,   78,   80,   82,   83,
    85,   87,   89,   90,   92,   94,   96,   97,   99,  101,  103,  105,  106,  108,  110,  112,
   113,  115,  117,  119,  120,  122,  124,  126,  128,  129,  131,  133,  135,  136,  138,  140,
   142,  144,  145,  147,  149,  151,  152,  154,  156,  158,  159,  161,  163,  165,  167,  168,
   170,  172,  174,  175,  177,  179,  181,  183,  184,  186,  188,  190,  191,  193,  195,  197,
   198,  200,  202,  204,  206,  207,  209,  211,  213,  214,  216,  218,  220,  221,  223,  225,
};

static const I32 _aCRG[] = {
   5990656,  5943854,  5897052,  5850250,  5803448,  5756646,  5709844,  5663042,
   5616240,  5569438,  5522636,  5475834,  5429032,  5382230,  5335428,  5288626,
   5241824,  5195022,  5148220,  5101418,  5054616,  5007814,  4961012,  4914210,
   4867408,  4820606,  4773804,  4727002,  4680200,  4633398,  4586596,  4539794,
   4492992,  4446190,  4399388,  4352586,  4305784,  4258982,  4212180,  4165378,
   4118576,  4071774,  4024972,  3978170,  3931368,  3884566,  3837764,  3790962,
   3744160,  3697358,  3650556,  3603754,  3556952,  3510150,  3463348,  3416546,
   3369744,  3322942,  3276140,  3229338,  3182536,  3135734,  3088932,  3042130,
   2995328,  2948526,  2901724,  2854922,  2808120,  2761318,  2714516,  2667714,
   2620912,  2574110,  2527308,  2480506,  2433704,  2386902,  2340100,  2293298,
   2246496,  2199694,  2152892,  2106090,  2059288,  2012486,  1965684,  1918882,
   1872080,  1825278,  1778476,  1731674,  1684872,  1638070,  1591268,  1544466,
   1497664,  1450862,  1404060,  1357258,  1310456,  1263654,  1216852,  1170050,
   1123248,  1076446,  1029644,   982842,   936040,   889238,   842436,   795634,
    748832,   702030,   655228,   608426,   561624,   514822,   468020,   421218,
    374416,   327614,   280812,   234010,   187208,   140406,    93604,    46802,
         0,   -46802,   -93604,  -140406,  -187208,  -234010,  -280812,  -327614,
   -374416,  -421218,  -468020,  -514822,  -561624,  -608426,  -655228,  -702030,
   -748832,  -795634,  -842436,  -889238,  -936040,  -982842, -1029644, -1076446,
  -1123248, -1170050, -1216852, -1263654, -1310456, -1357258, -1404060, -1450862,
  -1497664, -1544466, -1591268, -1638070, -1684872, -1731674, -1778476, -1825278,
  -1872080, -1918882, -1965684, -2012486, -2059288, -2106090, -2152892, -2199694,
  -2246496, -2293298, -2340100, -2386902, -2433704, -2480506, -2527308, -2574110,
  -2620912, -2667714, -2714516, -2761318, -2808120, -2854922, -2901724, -2948526,
  -2995328, -3042130, -3088932, -3135734, -3182536, -3229338, -3276140, -3322942,
  -3369744, -3416546, -3463348, -3510150, -3556952, -3603754, -3650556, -3697358,
  -3744160, -3790962, -3837764, -3884566, -3931368, -3978170, -4024972, -4071774,
  -4118576, -4165378, -4212180, -4258982, -4305784, -4352586, -4399388, -4446190,
  -4492992, -4539794, -4586596, -4633398, -4680200, -4727002, -4773804, -4820606,
  -4867408, -4914210, -4961012, -5007814, -5054616, -5101418, -5148220, -5195022,
  -5241824, -5288626, -5335428, -5382230, -5429032, -5475834, -5522636, -5569438,
  -5616240, -5663042, -5709844, -5756646, -5803448, -5850250, -5897052, -5943854,
};

static const I32 _aCBG[] = {
   2919680,  2897126,  2874572,  2852018,  2829464,  2806910,  2784356,  2761802,
   2739248,  2716694,  2694140,  2671586,  2649032,  2626478,  2603924,  2581370,
   2558816,  2536262,  2513708,  2491154,  2468600,  2446046,  2423492,  2400938,
   2378384,  2355830,  2333276,  2310722,  2288168,  2265614,  2243060,  2220506,
   2197952,  2175398,  2152844,  2130290,  2107736,  2085182,  2062628,  2040074,
   2017520,  1994966,  1972412,  1949858,  1927304,  1904750,  1882196,  1859642,
   1837088,  1814534,  1791980,  1769426,  1746872,  1724318,  1701764,  1679210,
   1656656,  1634102,  1611548,  1588994,  1566440,  1543886,  1521332,  1498778,
   1476224,  1453670,  1431116,  1408562,  1386008,  1363454,  1340900,  1318346,
   1295792,  1273238,  1250684,  1228130,  1205576,  1183022,  1160468,  1137914,
   1115360,  1092806,  1070252,  1047698,  1025144,  1002590,   980036,   957482,
    934928,   912374,   889820,   867266,   844712,   822158,   799604,   777050,
    754496,   731942,   709388,   686834,   664280,   641726,   619172,   596618,
    574064,   551510,   528956,   506402,   483848,   461294,   438740,   416186,
    393632,   371078,   348524,   325970,   303416,   280862,   258308,   235754,
    213200,   190646,   168092,   145538,   122984,   100430,    77876,    55322,
     32768,    10214,   -12340,   -34894,   -57448,   -80002,  -102556,  -125110,
   -147664,  -170218,  -192772,  -215326,  -237880,  -260434,  -282988,  -305542,
   -328096,  -350650,  -373204,  -395758,  -418312,  -440866,  -463420,  -485974,
   -508528,  -531082,  -553636,  -576190,  -598744,  -621298,  -643852,  -666406,
   -688960,  -711514,  -734068,  -756622,  -779176,  -801730,  -824284,  -846838,
   -869392,  -891946,  -914500,  -937054,  -959608,  -982162, -1004716, -1027270,
  -1049824, -1072378, -1094932, -1117486, -1140040, -1162594, -1185148, -1207702,
  -1230256, -1252810, -1275364, -1297918, -1320472, -1343026, -1365580, -1388134,
  -1410688, -1433242, -1455796, -1478350, -1500904, -1523458, -1546012, -1568566,
  -1591120, -1613674, -1636228, -1658782, -1681336, -1703890, -1726444, -1748998,
  -1771552, -1794106, -1816660, -1839214, -1861768, -1884322, -1906876, -1929430,
  -1951984, -1974538, -1997092, -2019646, -2042200, -2064754, -2087308, -2109862,
  -2132416, -2154970, -2177524, -2200078, -2222632, -2245186, -2267740, -2290294,
  -2312848, -2335402, -2357956, -2380510, -2403064, -2425618, -2448172, -2470726,
  -2493280, -2515834, -2538388, -2560942, -2583496, -2606050, -2628604, -2651158,
  -2673712, -2696266, -2718820, -2741374, -2763928, -2786482, -2809036, -2831590,
};

static JPEG_HandleTypeDef     JPEG_Handle;
static DMA_HandleTypeDef      hdmaIn;
static DMA_HandleTypeDef      hdmaOut;

static OS_STACKPTR int   JPEGStack[512];
static OS_TASK           JPEGTCB;
static OS_TASK         * _pMainTask;

static int _StayAlive;
static int _IsInitialized;

static void JPEG_Task(void);

static JPEG_X_CONTEXT _Context;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Convert_420_ARGB
*/
static U32 _Convert_420_ARGB(U32 BlockIndex) {
  U32  numberMCU;
  U32  i;
  U32  j;
  U32  k;
  U32  currentMCU;
  U32  xRef;
  U32  yRef;
  U32  refline;
  I32  ycomp;
  I32  crcomp;
  I32  cbcomp;
  I32  c_red;
  I32  c_blue;
  I32  c_green;
  U8 * pOutAddr;
  U8 * pOutAddr2;
  U8 * pChrom;
  U8 * pLum;
  U8 * pInBuffer;
  U32  ImageSize;
  
  numberMCU  = _Context.WorkBufferSize / _Context.BlockSize;
  currentMCU = BlockIndex;
  ImageSize  = _Context.BytesPerLine * _Context.ySize;
  pInBuffer  = _Context.pWorkBuffer;
  while(currentMCU < (numberMCU + BlockIndex)) {
    xRef    = ((currentMCU * 16) / _Context.xSizeExtended) * 16;    
    yRef    = ((currentMCU * 16) % _Context.xSizeExtended);    
    refline = _Context.BytesPerLine * xRef + ((_Context.BitsPerPixel >> 3) * yRef);
    currentMCU++;    
    pChrom = pInBuffer + 256;    
    pLum   = pInBuffer;    
    for (i = 0; i < 16; i += 2) {
      if(i == 8) {
        pLum = pInBuffer + 128;
      }      
      if(refline < ImageSize) {
        pOutAddr = _Context.pOutBuffer + refline;
        pOutAddr2 = pOutAddr + _Context.BytesPerLine;        
        for (k = 0; k < 2; k++) {
          for (j = 0; j < 8; j += 2) {           
            cbcomp  = (I32)(*(pChrom));
            c_blue  = (I32)(*(_aCBB + cbcomp));            
            crcomp  = (I32)(*(pChrom + 64));
            c_red   = (I32)(*(_aCRR + crcomp));                      
            c_green = ((I32)(*(_aCRG + crcomp)) + (I32)(*(_aCBG + cbcomp))) >> 16;
            //
            // 32bit destination
            //
            if (_Context.ColorFormat == JPEG_ARGB8888) {
              ycomp = (I32)(*(pLum +j));            
              *(U32 *)pOutAddr = (LIMIT(ycomp + c_red) << 16) | (LIMIT( ycomp + c_green) << 8) | (LIMIT(ycomp + c_blue));
              ycomp = (I32)(*(pLum +j +1));            
              *((U32 *)(pOutAddr + 4)) = (LIMIT(ycomp + c_red) << 16) | (LIMIT( ycomp + c_green) << 8) | (LIMIT(ycomp + c_blue));
              ycomp = (I32)(*(pLum +j +8));            
              *(U32 *)pOutAddr2 = (LIMIT(ycomp + c_red) << 16) | (LIMIT( ycomp + c_green) << 8) | (LIMIT(ycomp + c_blue));
              ycomp = (I32)(*(pLum +j +8 +1));            
              *((U32 *)(pOutAddr2 +4)) = (LIMIT(ycomp + c_red) << 16) | (LIMIT( ycomp + c_green) << 8) | (LIMIT(ycomp + c_blue));
            }
            //
            // 16bit destination
            //
            else if (_Context.ColorFormat == JPEG_RGB565) {          
              ycomp = (I32)(*(pLum +j));            
              *(U16 *)pOutAddr = ((LIMIT(ycomp + c_red) >> 3) << 11) | ((LIMIT( ycomp + c_green) >> 2) << 5) | ((LIMIT(ycomp + c_blue) >> 3));
              ycomp = (I32)(*(pLum +j +1));            
              *((U16 *)(pOutAddr + 2)) = ((LIMIT(ycomp + c_red) >> 3) << 11) | ((LIMIT( ycomp + c_green) >> 2) << 5) | ((LIMIT(ycomp + c_blue) >> 3));
              ycomp = (I32)(*(pLum +j +8));            
              *(U16 *)pOutAddr2 = ((LIMIT(ycomp + c_red) >> 3) << 11) | ((LIMIT( ycomp + c_green) >> 2) << 5) | ((LIMIT(ycomp + c_blue) >> 3));
              ycomp = (I32)(*(pLum +j +8 +1));            
              *((U16 *)(pOutAddr2 +2)) = ((LIMIT(ycomp + c_red) >> 3) << 11) | ((LIMIT( ycomp + c_green) >> 2) << 5) | ((LIMIT(ycomp + c_blue) >> 3));
            }
            pOutAddr  += (_Context.BitsPerPixel >> 3) * 2;
            pOutAddr2 += (_Context.BitsPerPixel >> 3) * 2;
            pChrom++;
          }
          pLum += 64;                      
        }
        pLum = pLum - 128 + 16;        
        refline += 2 * _Context.BytesPerLine;          
      }
    }
    pInBuffer += _Context.BlockSize;
  }
  return numberMCU;
}

/*********************************************************************
*
*       _Convert_422_ARGB
*/
static U32 _Convert_422_ARGB(U32 BlockIndex) {
  U32  numberMCU;
  U32  i;
  U32  j;
  U32  k;
  U32  currentMCU;
  U32  xRef;
  U32  yRef;
  U32  refline;
  I32  ycomp;
  I32  crcomp;
  I32  cbcomp;
  I32  c_red;
  I32  c_blue;
  I32  c_green;
  U8 * pOutAddr;
  U8 * pChrom;
  U8 * pLum;
  U8 * pInBuffer;
  U32  ImageSize;
  
  numberMCU  = _Context.WorkBufferSize / _Context.BlockSize;
  currentMCU = BlockIndex;
  ImageSize  = _Context.BytesPerLine * _Context.ySize;
  pInBuffer  = _Context.pWorkBuffer;
  while(currentMCU < (numberMCU + BlockIndex)) {
    xRef    = ((currentMCU * 16) / _Context.xSizeExtended) * 8;    
    yRef    = ((currentMCU * 16) % _Context.xSizeExtended);    
    refline = _Context.BytesPerLine * xRef + ((_Context.BitsPerPixel >> 3) * yRef);
    currentMCU++;    
    pChrom = pInBuffer + 128;    
    pLum   = pInBuffer;
    for (i = 0; i < 8; i++) {
      if (refline < ImageSize) {
        pOutAddr = _Context.pOutBuffer + refline;        
        for (k = 0; k < 2; k++) {
          for (j = 0; j < 8; j += 2) {           
            cbcomp = (I32)(*(pChrom));
            c_blue = (I32)(*(_aCBB + cbcomp));            
            crcomp = (I32)(*(pChrom + 64));
            c_red = (I32)(*(_aCRR + crcomp));
            c_green = ((I32)(*(_aCRG + crcomp)) + (I32)(*(_aCBB + cbcomp))) >> 16;
            if (_Context.ColorFormat == JPEG_ARGB8888) {
              ycomp = (I32)(*(pLum +j));
              *(U32 *)pOutAddr = (LIMIT(ycomp + c_red) << 16) | (LIMIT( ycomp + c_green) << 8) | (LIMIT(ycomp + c_blue));
              ycomp = (I32)(*(pLum +j +1));
              *((U32 *)(pOutAddr + 4)) = (LIMIT(ycomp + c_red) << 16) | (LIMIT( ycomp + c_green) << 8) | (LIMIT(ycomp + c_blue));
            } else if (_Context.ColorFormat == JPEG_RGB565) {
              ycomp = (I32)(*(pLum +j));            
              *(U16 *)pOutAddr = ((LIMIT(ycomp + c_red) >> 3) << 11) | ((LIMIT( ycomp + c_green) >> 2) << 5) | ((LIMIT(ycomp + c_blue) >> 3));
              ycomp = (I32)(*(pLum +j +1));            
              *((U16 *)(pOutAddr + 2)) = ((LIMIT(ycomp + c_red) >> 3) << 11) | ((LIMIT( ycomp + c_green) >> 2) << 5) | ((LIMIT(ycomp + c_blue) >> 3));         
            }          
            pOutAddr += (_Context.BitsPerPixel >> 3) * 2;          
            pChrom++;
          }
          pLum += 64;                      
        }        
        pLum = pLum - 128 + 8;        
        refline += _Context.BytesPerLine;
      }
    }       
    pInBuffer +=  _Context.BlockSize;
  }
  return numberMCU;
}

/*********************************************************************
*
*       _Convert_440_ARGB
*/
static U32 _Convert_440_ARGB(U32 BlockIndex) {
  U32  numberMCU;
  U32  i;
  U32  j;
  U32  k;
  U32  currentMCU;
  U32  xRef;
  U32  yRef;
  U32  refline;
  I32  ycomp;
  I32  crcomp;
  I32  cbcomp;
  I32  c_red;
  I32  c_blue;
  I32  c_green;
  U8 * pOutAddr;
  U8 * pChrom;
  U8 * pLum;
  U8 * pInBuffer;
  U32  ImageSize;
  
  numberMCU  = _Context.WorkBufferSize / _Context.BlockSize;
  currentMCU = BlockIndex;
  ImageSize  = _Context.BytesPerLine * _Context.ySize;
  pInBuffer  = _Context.pWorkBuffer;
  while(currentMCU < (numberMCU + BlockIndex)) {
    xRef    = ((currentMCU * 8) / _Context.xSizeExtended) * 16;
    yRef    = ((currentMCU * 8) % _Context.xSizeExtended);    
    refline = _Context.BytesPerLine * xRef + ((_Context.BitsPerPixel >> 3) * yRef);
    currentMCU++;    
    pChrom = pInBuffer + 128;    
    pLum   = pInBuffer;
    
    for (i = 0; i < 8; i++) {
      if (refline < ImageSize) {
        pOutAddr = _Context.pOutBuffer + refline;        
        for (k = 0; k < 2; k++) {
          for (j = 0; j < 8; j += 2) {           
            cbcomp = (I32)(*(pChrom));
            c_blue = (I32)(*(_aCBB + cbcomp));            
            crcomp = (I32)(*(pChrom + 64));
            c_red = (I32)(*(_aCRR + crcomp));
            c_green = ((I32)(*(_aCRG + crcomp)) + (I32)(*(_aCBB + cbcomp))) >> 16;
            if (_Context.ColorFormat == JPEG_ARGB8888) {
              ycomp = (I32)(*(pLum +j));
              *(U32 *)pOutAddr = (LIMIT(ycomp + c_red) << 16) | (LIMIT( ycomp + c_green) << 8) | (LIMIT(ycomp + c_blue));
              ycomp = (I32)(*(pLum +j +1));
              *((U32 *)(pOutAddr + 4)) = (LIMIT(ycomp + c_red) << 16) | (LIMIT( ycomp + c_green) << 8) | (LIMIT(ycomp + c_blue));
            } else if (_Context.ColorFormat == JPEG_RGB565) {
              ycomp = (I32)(*(pLum +j));            
              *(U16 *)pOutAddr = ((LIMIT(ycomp + c_red) >> 3) << 11) | ((LIMIT( ycomp + c_green) >> 2) << 5) | ((LIMIT(ycomp + c_blue) >> 3));
              ycomp = (I32)(*(pLum +j +1));            
              *((U16 *)(pOutAddr + 2)) = ((LIMIT(ycomp + c_red) >> 3) << 11) | ((LIMIT( ycomp + c_green) >> 2) << 5) | ((LIMIT(ycomp + c_blue) >> 3));         
            }          
            pOutAddr += (_Context.BitsPerPixel >> 3) * 2;          
            pChrom++;
          }
          pLum += 64;
        }        
        pLum = pLum - 128 + 8;        
        refline += _Context.BytesPerLine;
      }
    }       
    pInBuffer +=  _Context.BlockSize;
  }
  return numberMCU;
}

/*********************************************************************
*
*       _Convert_422_ARGB
*/
static U32 _Convert_444_ARGB(U32 BlockIndex) {
  U32 numberMCU;
  U32 i,j, currentMCU, xRef,yRef;
  U32 refline;
  I32 ycomp, crcomp, cbcomp;
  I32 c_red, c_blue, c_green;
  U8 *pOutAddr;
  U8 *pChrom, *pLum;
  U8 * pInBuffer;
  U32  ImageSize;
  
  numberMCU = _Context.WorkBufferSize / _Context.BlockSize;
  currentMCU = BlockIndex;
  ImageSize  = _Context.BytesPerLine * _Context.ySize;
  pInBuffer  = _Context.pWorkBuffer;
  while(currentMCU < (numberMCU + BlockIndex)) {
    xRef = ((currentMCU * 8) / _Context.xSizeExtended) * 8;    
    yRef = ((currentMCU * 8) % _Context.xSizeExtended);    
    refline = _Context.BytesPerLine * xRef + ((_Context.BitsPerPixel >> 3)*yRef);
    currentMCU++;       
    pChrom = pInBuffer + 64; /* pChroma = pInBuffer + 4*64 */    
    pLum = pInBuffer;    
    for(i= 0; i < 8; i++) {
      if(refline < ImageSize) {
        pOutAddr = _Context.pOutBuffer + refline;
        for(j=0; j < 8; j++) {           
          cbcomp = (I32)(*pChrom);
          c_blue = (I32)(*(_aCBB + cbcomp));          
          crcomp = (I32)(*(pChrom + 64));
          c_red = (I32)(*(_aCRR + crcomp)); 
          c_green = ((I32)(*(_aCRG + crcomp)) + (I32)(*(_aCBG + cbcomp))) >> 16;                          
          if (_Context.ColorFormat == JPEG_ARGB8888) {
            ycomp = (I32)(*(pLum +j));          
            *(U32 *)pOutAddr = (LIMIT(ycomp + c_red) << 16)     | \
                               (LIMIT( ycomp + c_green) << 8) | \
                               (LIMIT(ycomp + c_blue));       
          } else {
            ycomp = (I32)(*(pLum +j));        
            *(U16 *)pOutAddr = ((LIMIT(ycomp + c_red) >> 3) << 11)     | \
                               ((LIMIT( ycomp + c_green) >> 2) << 5) | \
                               ((LIMIT(ycomp + c_blue) >> 3));      
          }           
          pOutAddr += (_Context.BitsPerPixel >> 3);
          pChrom++;
        }
        pLum += 8;
        refline += _Context.BytesPerLine;          
      }
    }    
    pInBuffer +=  _Context.BlockSize;
  }
  return numberMCU;
}

/*********************************************************************
*
*       _Convert_422_ARGB
*/
static U32 _Convert_Gray_ARGB(U32 BlockIndex) {
  U32 numberMCU;
  U32  currentMCU, xRef,yRef;
  U32 refline;
  U32 i,j, ySample;
  U8 *pOutAddr,  *pLum;
  U8 * pInBuffer;
  U32  ImageSize;
  
  numberMCU = _Context.WorkBufferSize / _Context.BlockSize;
  currentMCU = BlockIndex;
  ImageSize  = _Context.BytesPerLine * _Context.ySize;
  pInBuffer  = _Context.pWorkBuffer;
  while(currentMCU < (numberMCU + BlockIndex)) {
    xRef = ((currentMCU * 8) / _Context.xSizeExtended) * 8;    
    yRef = ((currentMCU * 8) % _Context.xSizeExtended);        
    refline = _Context.BytesPerLine * xRef + ((_Context.BitsPerPixel >> 3)*yRef);    
    currentMCU++;  
    pLum = pInBuffer;    
    for (i = 0; i < 8; i++) { 
      pOutAddr = _Context.pOutBuffer + refline;
      if (refline < ImageSize) {  
        for(j = 0; j < 8; j++) { 
          ySample =   (U32)(*pLum);
          if (_Context.ColorFormat == JPEG_ARGB8888) {
            *(U32 *)pOutAddr = ySample |  (ySample << 8) | (ySample << 16);
          } else {
            *(U16 *)pOutAddr = ((ySample >> 3) << 11) |  ((ySample >> 2) << 5) | (ySample >> 3);               
          }
          pOutAddr += (_Context.BitsPerPixel >> 3);
          pLum++;
        }
        refline += _Context.BytesPerLine;
      }
    }    
    pInBuffer +=  _Context.BlockSize;    
  }
  return numberMCU;
}

/*********************************************************************
*
*       HAL_JPEG_MspInit
*/
void HAL_JPEG_MspInit(JPEG_HandleTypeDef *hjpeg) {   
  //
  // Enable JPEG clock
  //
  __HAL_RCC_JPEG_CLK_ENABLE();
  //
  // Enable DMA clock
  //
  __HAL_RCC_DMA2_CLK_ENABLE();

  HAL_NVIC_SetPriority(JPEG_IRQn, 0x06, 0x0F);
  HAL_NVIC_EnableIRQ(JPEG_IRQn);
  //
  // Input DMA
  //
  // Set the parameters to be configured
  //
  hdmaIn.Init.Channel = DMA_CHANNEL_9;
  hdmaIn.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdmaIn.Init.PeriphInc = DMA_PINC_DISABLE;
  hdmaIn.Init.MemInc = DMA_MINC_ENABLE;
  hdmaIn.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdmaIn.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdmaIn.Init.Mode = DMA_NORMAL;
  hdmaIn.Init.Priority = DMA_PRIORITY_HIGH;
  hdmaIn.Init.FIFOMode = DMA_FIFOMODE_ENABLE;         
  hdmaIn.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdmaIn.Init.MemBurst = DMA_MBURST_INC4;
  hdmaIn.Init.PeriphBurst = DMA_PBURST_INC4;      
  hdmaIn.Instance = DMA2_Stream3;
  //
  // Associate the DMA handle
  //
  __HAL_LINKDMA(hjpeg, hdmain, hdmaIn);
  
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0x06, 0x0F);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);    
  //
  // DeInitialize the DMA Stream
  //
  HAL_DMA_DeInit(&hdmaIn);  
  //
  // Initialize the DMA stream
  //
  HAL_DMA_Init(&hdmaIn);
  //
  // Output DMA
  //
  // Set the parameters to be configured 
  //
  hdmaOut.Init.Channel = DMA_CHANNEL_9;
  hdmaOut.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdmaOut.Init.PeriphInc = DMA_PINC_DISABLE;
  hdmaOut.Init.MemInc = DMA_MINC_ENABLE;
  hdmaOut.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdmaOut.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdmaOut.Init.Mode = DMA_NORMAL;
  hdmaOut.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  hdmaOut.Init.FIFOMode = DMA_FIFOMODE_ENABLE;         
  hdmaOut.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdmaOut.Init.MemBurst = DMA_MBURST_INC4;
  hdmaOut.Init.PeriphBurst = DMA_PBURST_INC4;
  hdmaOut.Instance = DMA2_Stream4;
  //
  // DeInitialize the DMA Stream
  //
  HAL_DMA_DeInit(&hdmaOut);  
  //
  // Initialize the DMA stream
  //
  HAL_DMA_Init(&hdmaOut);
  //
  // Associate the DMA handle
  //
  __HAL_LINKDMA(hjpeg, hdmaout, hdmaOut);
  HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0x07, 0x0F);
  HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
  NVIC_SetPriority(CAN1_TX_IRQn, 0x0F);
  NVIC_EnableIRQ(CAN1_TX_IRQn);
  NVIC_SetPriority(CAN1_RX0_IRQn, 0x0F);
  NVIC_EnableIRQ(CAN1_RX0_IRQn);
}

/*********************************************************************
*
*       JPEG_IRQHandler
*/
void JPEG_IRQHandler(void);
void JPEG_IRQHandler(void) {
  HAL_JPEG_IRQHandler(&JPEG_Handle);
}

/*********************************************************************
*
*       DMA2_Stream3_IRQHandler
*/
void DMA2_Stream3_IRQHandler(void);
void DMA2_Stream3_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdmaIn);
}

/*********************************************************************
*
*       DMA2_Stream4_IRQHandler
*/
void DMA2_Stream4_IRQHandler(void);
void DMA2_Stream4_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdmaOut);
}

/*********************************************************************
*
*       CAN1_TX/RX_IRQHandler
*
*  This IRQHandler is used to call OS functions from a low latency
*  interrupt. If this IRQ is required for its original purpose
*  set up another one to signal the event.
*
*  Here it is required to signal a data ready event to the JPEG_Task
*  which is waiting to convert YCbCr values into RGB values.
*/
void CAN1_TX_IRQHandler(void);
void CAN1_TX_IRQHandler(void) {
  OS_EnterInterrupt();
  if (_Context.Error) {
    OS_SignalEvent(EVENT_ABORT, _pMainTask);
  } else {
    if (_Context.IRQFlag & EVENT_DATAREADY) {
      _Context.IRQFlag &= ~EVENT_DATAREADY;
      OS_SignalEvent(EVENT_DATAREADY, &JPEGTCB);
    }
    if (_Context.IRQFlag & EVENT_INFOREADY) {
      _Context.IRQFlag &= ~EVENT_INFOREADY;
      OS_SignalEvent(EVENT_INFOREADY, _pMainTask);
    }
  }  
  NVIC_ClearPendingIRQ(CAN1_TX_IRQn);
  OS_LeaveInterrupt();
}
void CAN1_RX0_IRQHandler(void);
void CAN1_RX0_IRQHandler(void) {
  OS_EnterInterrupt();
  if (_Context.Error) {
    OS_SignalEvent(EVENT_DESTROY, &JPEGTCB);
  } else {
    OS_SignalEvent(EVENT_GETDATA, &JPEGTCB);
  }
  NVIC_ClearPendingIRQ(CAN1_RX0_IRQn);
  OS_LeaveInterrupt();
}

/*********************************************************************
*
*       HAL_JPEG_InfoReadyCallback
*/
void HAL_JPEG_InfoReadyCallback(JPEG_HandleTypeDef *hjpeg, JPEG_ConfTypeDef *pInfo) {
  int MaxBlocksPerMCU;
  int PixelSize;
  int Extend;
  int ExtendFactor;
  U8 Factor_V;
  U8 Factor_H;
  int MCU_V;
  int MCU_H;
  int r;
  U32 BufferSize;

  r = 0;
  if ((pInfo->ImageHeight == 0) || (pInfo->ImageWidth == 0)) {
    r = 1;
  }
  Factor_V = ((hjpeg->Instance->CONFR4 >> 8)  & 0x0F) * 8;
  Factor_H = ((hjpeg->Instance->CONFR4 >> 12) & 0x0F) * 8;
  if (r == 0) {
    if (pInfo->ColorSpace == JPEG_GRAYSCALE_COLORSPACE) {
      _Context.pfConvert = _Convert_Gray_ARGB;
      MaxBlocksPerMCU    = 1;
      ExtendFactor       = 8;
    } else if (pInfo->ColorSpace == JPEG_YCBCR_COLORSPACE) {
      if (pInfo->ChromaSubsampling == JPEG_444_SUBSAMPLING) {
        _Context.pfConvert = _Convert_444_ARGB;
        MaxBlocksPerMCU    = 3;
        ExtendFactor       = 8;
      } else if (pInfo->ChromaSubsampling == JPEG_422_SUBSAMPLING) {
        if (Factor_V == 8) {
          _Context.pfConvert = _Convert_422_ARGB;
        } else {
          r = 1;
        }
        MaxBlocksPerMCU    = 4;
        ExtendFactor       = 16;
      } else if (pInfo->ChromaSubsampling == JPEG_420_SUBSAMPLING) {
        _Context.pfConvert = _Convert_420_ARGB;
        MaxBlocksPerMCU    = 6;
        ExtendFactor       = 16;
      }
    } else {
      r = 1;
    }
    if (r == 0) {
      if (_Context.hOutBuffer == 0) {
        switch (_Context.ColorFormat) {
        case JPEG_ARGB8888:
          _Context.BitsPerPixel = 32;
          PixelSize             = 4;
          break;
        case JPEG_RGB565:
          _Context.BitsPerPixel = 16;
          PixelSize             = 2;
          break;
        }
        MCU_H                  = pInfo->ImageWidth / Factor_H;
        MCU_H                  = (pInfo->ImageWidth % Factor_H) ? MCU_H + 1 : MCU_H;
        MCU_V                  = pInfo->ImageHeight / Factor_V;
        MCU_V                  = (pInfo->ImageHeight % Factor_V) ? MCU_V + 1 : MCU_V;
        _Context.TotalMCUs     = MCU_H * MCU_V;
        _Context.xSize         = pInfo->ImageWidth;
        _Context.ySize         = pInfo->ImageHeight;
        _Context.BlockSize     = MaxBlocksPerMCU * 8 * 8;  // A block has a size of 8x8 pixels
        _Context.BytesPerLine  = PixelSize * pInfo->ImageWidth;
        BufferSize             = (((pInfo->ImageWidth + (ExtendFactor - 1)) / ExtendFactor) * ExtendFactor) * ((pInfo->ImageHeight + (ExtendFactor - 1)) / ExtendFactor) * ExtendFactor * PixelSize;
        _Context.OutBufferSize = BufferSize;
        Extend                 = (pInfo->ImageWidth % ExtendFactor) ? ExtendFactor - (pInfo->ImageWidth % ExtendFactor) : 0;
        _Context.xSizeExtended = pInfo->ImageWidth + Extend;
      }
    }
  }
  if (r) {
    _Context.Error = r;
    HAL_JPEG_Abort(hjpeg);
    NVIC_SetPendingIRQ(CAN1_TX_IRQn);
  } else {
    _Context.IRQFlag |= EVENT_INFOREADY;
    NVIC_SetPendingIRQ(CAN1_TX_IRQn);
  }
}

/*********************************************************************
*
*       HAL_JPEG_GetDataCallback
*/
void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength) {
  GUI_USE_PARA(pDataOut);
  GUI_USE_PARA(OutDataLength);
  HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_OUTPUT);
  HAL_JPEG_ConfigOutputBuffer(hjpeg, (uint8_t *)_Context.pWorkBuffer, _Context.WorkBufferSize);  
  _Context.IRQFlag |= EVENT_DATAREADY;
  NVIC_SetPendingIRQ(CAN1_TX_IRQn);
}

/*********************************************************************
*
*       HAL_JPEG_GetDataCallback
*/
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbDecodedData) {
  if (NbDecodedData == _Context.NumBytesInBuffer) {
    HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_INPUT);
    NVIC_SetPendingIRQ(CAN1_RX0_IRQn);
  } else {
    HAL_JPEG_ConfigInputBuffer(&JPEG_Handle, _Context.pInBuffer + NbDecodedData, _Context.NumBytesInBuffer - NbDecodedData);
  }
}

/*********************************************************************
*
*       _Free
*/
static void _Free(void) {
  if (_Context.pWorkBuffer) {
    _Context.pfFree(_Context.pWorkBuffer);
    _Context.pWorkBuffer = NULL;
  } else {
    GUI_ALLOC_Free(_Context.hWorkBuffer);
    _Context.hWorkBuffer = 0;
  }
}

/*********************************************************************
*
*       _Init
*/
static int _Init(void) {
  int r;
  int Prio;

  r                  = 0;
  _Context.IndexMCU  = 0;
  if (_Context.pfFree && _Context.pfMalloc) {
    _Context.pWorkBuffer = _Context.pfMalloc(_Context.WorkBufferSize);
    if (_Context.pWorkBuffer == NULL) {
      r = 1;
    }
  } else {
    _Context.hWorkBuffer = GUI_ALLOC_AllocNoInit(_Context.WorkBufferSize);
    if (_Context.hWorkBuffer == NULL) {
      r = 1;
    }
  }
  if (r == 0) {
    _pMainTask = OS_GetTaskID();
    JPEG_Handle.Instance = JPEG;
    HAL_JPEG_Init(&JPEG_Handle);
    Prio = OS_GetPriority(_pMainTask) + 1;
    OS_CREATETASK(&JPEGTCB, "JPEG_Task", JPEG_Task, Prio, JPEGStack);
  }
  return r;
}

/*********************************************************************
*
*       _DeInit
*/
static void _DeInit(void) {
  _IsInitialized = 0;
  _Free();
  HAL_JPEG_DeInit(&JPEG_Handle);
  if (OS_IsTask(&JPEGTCB)) {
    OS_TerminateTask(&JPEGTCB);
  }
}

/*********************************************************************
*
*       _GetOutBuffer
*/
static int _GetOutBuffer(void) {
  int r = 0;
  //
  // Get a buffer which is able to hold the data for the whole JPEG in RGB format
  //
  if (_Context.hOutBuffer == 0) {
    if (_Context.Error == 0) {
      _Context.hOutBuffer = GUI_ALLOC_AllocNoInit(_Context.OutBufferSize);
      if (_Context.hOutBuffer == 0) {
        r = 1;
      } else {
        _Context.pOutBuffer = GUI_ALLOC_LockH(_Context.hOutBuffer);
        r = 0;
      }
    }
  }
  return r;
}

/*********************************************************************
*
*       _FreeOutuffer
*/
static void _FreeOutuffer(void) {
  //
  // Free output buffer
  //
  if (_Context.hOutBuffer) {
    GUI_ALLOC_UnlockH((void **)&_Context.pOutBuffer);
    GUI_ALLOC_Free(_Context.hOutBuffer);
    _Context.hOutBuffer = 0;
    _Context.pOutBuffer = NULL;
  }
}

/*********************************************************************
*
*       _DrawBitmap
*/
static void _DrawBitmap(int x, int y, void const * p, int xSize, int ySize, int BytesPerLine, int BitsPerPixel) {
  #if (GUI_WINSUPPORT)
    GUI_RECT r;
  #endif
  #if (GUI_WINSUPPORT)
    WM_ADDORG(x,y);
    r.x1 = (r.x0 = x) + xSize-1;
    r.y1 = (r.y0 = y) + ySize-1;
    WM_ITERATE_START(&r) {
  #endif
  LCD_DrawBitmap(x, y, xSize, ySize, 1, 1, BitsPerPixel, BytesPerLine, p, NULL);
  #if (GUI_WINSUPPORT)
    } WM_ITERATE_END();
  #endif
}

/*********************************************************************
*
*       JPEG_Task
*/
static void JPEG_Task(void) {
  OS_TASK_EVENT Event;
  int           BytesRead;
  
  Event = 0;
  while (1) {
    //
    // Wait for one of the events
    //
    Event |= OS_WaitEvent(EVENT_DATAREADY | EVENT_GETDATA | EVENT_DESTROY | EVENT_MEMREADY);
    //
    // Data are only allowed to be processed if the output memory is available
    //
    if ((Event & EVENT_DATAREADY) && (Event & EVENT_MEMREADY)) {
      Event &= ~EVENT_DATAREADY;
      //
      // Convert YCrCb values into RGB values
      //
      _Context.IndexMCU += _Context.pfConvert(_Context.IndexMCU);
      if((_Context.IndexMCU >= _Context.TotalMCUs) && (_Context.TotalMCUs != 0)) {
        //
        // Once finished, signalize the MainTask that the buffer is full and can be displayed
        //
        OS_SignalEvent(EVENT_READY, _pMainTask);
        //
        // Clear memory event for the next image
        //
        Event &= ~EVENT_MEMREADY;
        //
        // Wait for continue
        //
        OS_WaitEvent(EVENT_CONTINUE);
        //
        // If _StayAlive is 0 break here and kill the task
        //
        if (_StayAlive == 0) {
          break;
        }
      } else {
        //
        // If the whole jpeg is not processed yet resume output channel to get more decoded data
        //
        HAL_JPEG_Resume(&JPEG_Handle, JPEG_PAUSE_RESUME_OUTPUT);
      }
    }
    if (Event & EVENT_GETDATA) {
      //
      // Get data event gets singalized if the entire input data has been consumed and the decoder needs more
      //
      Event &= ~EVENT_GETDATA;
      BytesRead     = _Context.pfGetData(_Context.pVoid, (const U8 **)&_Context.pInBuffer, NUMBYTES_INBUFFER, _Context.Off);
      _Context.NumBytesInBuffer = BytesRead;
      _Context.Off += BytesRead;
      //
      // Configure input buffer and resume input channel
      //
      HAL_JPEG_ConfigInputBuffer(&JPEG_Handle, _Context.pInBuffer, BytesRead);
      HAL_JPEG_Resume(&JPEG_Handle, JPEG_PAUSE_RESUME_INPUT);
    }
    if (Event & EVENT_DESTROY) {
      break;
    }
  }
  _DeInit();
}

/*********************************************************************
*
*       JPEG_X_IncStayAlive
*
*  Function description
*    This function increments the stay alive variable.
*    That variable signalizes that a movie is running and the 
*    JPEG task must not deleted.
*/
void JPEG_X_IncStayAlive(void) {
  _StayAlive++;
}

/*********************************************************************
*
*       JPEG_X_DecStayAlive
*
*  Function description
*    This function decrements the stay alive variable.
*    If _StayAlive is 0 the JPEG task gets deleted.
*/
void JPEG_X_DecStayAlive(void) {
  if (_StayAlive == 0) {
    while(1);  // Error
  }
  _StayAlive--;
  if (_StayAlive == 0) {
    if (OS_IsTask(&JPEGTCB)) {
      OS_SignalEvent(EVENT_DESTROY, &JPEGTCB);
    }
  }
}

/*********************************************************************
*
*       _cbNotify
*/
void MOVIE_X_cbNotify(GUI_MOVIE_HANDLE hMovie, int Notification, U32 CurrentFrame) {
  GUI_USE_PARA(hMovie);
  GUI_USE_PARA(CurrentFrame);

  switch (Notification) {
  case GUI_MOVIE_NOTIFICATION_START:
    //
    // Increment stay alive
    //
    JPEG_X_IncStayAlive();
    break;
  case GUI_MOVIE_NOTIFICATION_STOP:
    //
    // Decrement stay alive
    //
    JPEG_X_DecStayAlive();
    break;
  case GUI_MOVIE_NOTIFICATION_PREDRAW:
    break;
  case GUI_MOVIE_NOTIFICATION_POSTDRAW:
    break;
  }
}

/*********************************************************************
*
*       JPEG_X_Init
*/
void JPEG_X_Init(JPEG_X_CONTEXT * pContext) {
  _Context                 = *pContext;
  _Context.ColorFormat     = JPEG_RGB565;
  _Context.pfMalloc        = malloc;       // Use malloc to get memory from internal RAM which is way faster than the external SDRAM
  _Context.pfFree          = free;
  _Context.WorkBufferSize  = 7680;
  GUI_JPEG_SetpfDrawEx(JPEG_X_Draw);
  GUI_MOVIE_SetpfNotify(MOVIE_X_cbNotify);
}

/*********************************************************************
*
*       JPEG_X_Draw
*/
int JPEG_X_Draw(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0) {
  OS_TASK_EVENT Event;  
  GUI_LOCK();
  
  _Context.xPos      = x0;
  _Context.yPos      = y0;
  _Context.Error     = 0;
  _Context.IRQFlag   = 0;
  _Context.IndexMCU  = 0;
  _Context.pfGetData = pfGetData;
  _Context.pVoid     = p;
  //
  // Initialize JPEG decoder and create task if necessary
  //
  if (_IsInitialized == 0) {
    _IsInitialized = 1;
    if (_Init()) {
      GUI_UNLOCK();
      return 1;
    }
  }
  _Context.Off              = _Context.pfGetData(_Context.pVoid, (const U8 **)&_Context.pInBuffer, NUMBYTES_INBUFFER, 0);
  _Context.NumBytesInBuffer = _Context.Off;
  //
  // Start decoding
  //
  HAL_JPEG_Decode_DMA(&JPEG_Handle, _Context.pInBuffer, _Context.Off, (U8 *)_Context.pWorkBuffer, _Context.WorkBufferSize);  
  //
  // Wait for info ready
  //
  Event = OS_WaitEventTimed(EVENT_INFOREADY | EVENT_ABORT, JPEG_TIMEOUT);
  if ((Event == 0) || (Event == EVENT_ABORT)) {
    _DeInit();    
    GUI_UNLOCK();
    return 1;
  } else {
    //
    // Get memory
    //
    if (_GetOutBuffer()) {
      _DeInit();    
      GUI_UNLOCK();
      return 1;
    }
  }
  //
  // Signal JPEG task memory is ready
  //
  OS_SignalEvent(EVENT_MEMREADY, &JPEGTCB);
  //
  // Wait till JPEG task is ready with converting
  //
  Event = OS_WaitEventTimed(EVENT_READY | EVENT_ABORT, JPEG_TIMEOUT);
  if ((Event == 0) || (Event == EVENT_ABORT)) {
    _DeInit();    
    _FreeOutuffer();
    GUI_UNLOCK();
    return 1;
  } else {
    //
    // Draw buffer
    //
    _DrawBitmap(_Context.xPos, _Context.yPos, (void const *)_Context.pOutBuffer, _Context.xSize, _Context.ySize, _Context.BytesPerLine, _Context.BitsPerPixel);
    HAL_JPEG_Abort(&JPEG_Handle);
    //
    // Tell JPEG task to continue
    //
    OS_SignalEvent(EVENT_CONTINUE, &JPEGTCB);
  }
  //
  // Free memory
  //
  _FreeOutuffer();
  GUI_UNLOCK();
  return _Context.Error;
}

/*************************** End of file ****************************/
