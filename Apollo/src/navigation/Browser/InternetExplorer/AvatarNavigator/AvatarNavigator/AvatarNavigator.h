

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Dec 15 17:50:13 2009
 */
/* Compiler settings for .\AvatarNavigator.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __AvatarNavigator_h__
#define __AvatarNavigator_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAvatarNavigatorBHO_FWD_DEFINED__
#define __IAvatarNavigatorBHO_FWD_DEFINED__
typedef interface IAvatarNavigatorBHO IAvatarNavigatorBHO;
#endif 	/* __IAvatarNavigatorBHO_FWD_DEFINED__ */


#ifndef __AvatarNavigatorBHO_FWD_DEFINED__
#define __AvatarNavigatorBHO_FWD_DEFINED__

#ifdef __cplusplus
typedef class AvatarNavigatorBHO AvatarNavigatorBHO;
#else
typedef struct AvatarNavigatorBHO AvatarNavigatorBHO;
#endif /* __cplusplus */

#endif 	/* __AvatarNavigatorBHO_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IAvatarNavigatorBHO_INTERFACE_DEFINED__
#define __IAvatarNavigatorBHO_INTERFACE_DEFINED__

/* interface IAvatarNavigatorBHO */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IAvatarNavigatorBHO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7DE94C3-5524-4EBF-8843-9BA20D2292C4")
    IAvatarNavigatorBHO : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAvatarNavigatorBHOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAvatarNavigatorBHO * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAvatarNavigatorBHO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAvatarNavigatorBHO * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAvatarNavigatorBHO * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAvatarNavigatorBHO * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAvatarNavigatorBHO * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAvatarNavigatorBHO * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IAvatarNavigatorBHOVtbl;

    interface IAvatarNavigatorBHO
    {
        CONST_VTBL struct IAvatarNavigatorBHOVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAvatarNavigatorBHO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAvatarNavigatorBHO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAvatarNavigatorBHO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAvatarNavigatorBHO_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAvatarNavigatorBHO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAvatarNavigatorBHO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAvatarNavigatorBHO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAvatarNavigatorBHO_INTERFACE_DEFINED__ */



#ifndef __AvatarNavigatorLib_LIBRARY_DEFINED__
#define __AvatarNavigatorLib_LIBRARY_DEFINED__

/* library AvatarNavigatorLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_AvatarNavigatorLib;

EXTERN_C const CLSID CLSID_AvatarNavigatorBHO;

#ifdef __cplusplus

class DECLSPEC_UUID("4B2C4F08-7B67-4A7D-8DF6-EC64F046C6EC")
AvatarNavigatorBHO;
#endif
#endif /* __AvatarNavigatorLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


