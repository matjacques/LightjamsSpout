

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Nov 14 17:54:10 2015
 */
/* Compiler settings for LightjamsSpout.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IComponentRegistrar,0xa817e7a2,0x43fa,0x11d0,0x9e,0x44,0x00,0xaa,0x00,0xb6,0x77,0x0a);


MIDL_DEFINE_GUID(IID, IID_ILightjamsSpoutReceiver,0x83A7DAD1,0x813D,0x4B00,0xB8,0x49,0x94,0x6E,0xEF,0x9E,0x35,0xBC);


MIDL_DEFINE_GUID(IID, LIBID_LightjamsSpoutLib,0xE68A5E84,0x367B,0x46F2,0xBB,0x45,0xEE,0xC2,0xE6,0xC0,0xD6,0x2B);


MIDL_DEFINE_GUID(CLSID, CLSID_CompReg,0xA99139BB,0xC23B,0x41FC,0xB7,0xD5,0xBE,0x16,0xD5,0x64,0xFF,0xC2);


MIDL_DEFINE_GUID(CLSID, CLSID_LightjamsSpoutReceiver,0x9DE4149A,0x24CF,0x4B37,0x87,0xEE,0x39,0x90,0x05,0xBC,0x89,0x97);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



