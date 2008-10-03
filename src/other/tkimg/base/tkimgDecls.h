/*
 * tkimgDecls.h --
 *
 *	Declarations of functions in the platform independent public TKIMG API.
 *
 */

#ifndef _TKIMGDECLS
#define _TKIMGDECLS

/*
 * WARNING: The contents of this file is automatically generated by the
 * genStubs.tcl script. Any modifications to the function declarations
 * below should be made in the tkimg.decls script.
 */

#include <tk.h>

/* !BEGIN!: Do not edit below this line. */

/*
 * Exported function declarations:
 */

/* 0 */
EXTERN Tcl_Channel	tkimg_OpenFileChannel _ANSI_ARGS_((
				Tcl_Interp * interp, CONST char * fileName,
				int permissions));
/* 1 */
EXTERN int		tkimg_ReadInit _ANSI_ARGS_((Tcl_Obj * data, int c,
				tkimg_MFile * handle));
/* 2 */
EXTERN void		tkimg_WriteInit _ANSI_ARGS_((Tcl_DString * buffer,
				tkimg_MFile * handle));
/* 3 */
EXTERN int		tkimg_Getc _ANSI_ARGS_((tkimg_MFile * handle));
/* 4 */
EXTERN int		tkimg_Read _ANSI_ARGS_((tkimg_MFile * handle,
				char * dst, int count));
/* 5 */
EXTERN int		tkimg_Putc _ANSI_ARGS_((int c, tkimg_MFile * handle));
/* 6 */
EXTERN int		tkimg_Write _ANSI_ARGS_((tkimg_MFile * handle,
				CONST char * src, int count));
/* 7 */
EXTERN void		tkimg_ReadBuffer _ANSI_ARGS_((int onOff));
/* Slot 8 is reserved */
/* Slot 9 is reserved */
/* 10 */
EXTERN int		tkimg_PhotoPutBlock _ANSI_ARGS_((
				Tk_PhotoHandle handle,
				Tk_PhotoImageBlock * blockPtr, int x, int y,
				int width, int height));
/* Slot 11 is reserved */
/* Slot 12 is reserved */
/* Slot 13 is reserved */
/* Slot 14 is reserved */
/* Slot 15 is reserved */
/* Slot 16 is reserved */
/* Slot 17 is reserved */
/* Slot 18 is reserved */
/* Slot 19 is reserved */
/* 20 */
EXTERN void		tkimg_FixChanMatchProc _ANSI_ARGS_((
				Tcl_Interp ** interp, Tcl_Channel * chan,
				CONST char ** file, Tcl_Obj ** format,
				int ** width, int ** height));
/* 21 */
EXTERN void		tkimg_FixObjMatchProc _ANSI_ARGS_((
				Tcl_Interp ** interp, Tcl_Obj ** data,
				Tcl_Obj ** format, int ** width,
				int ** height));
/* 22 */
EXTERN void		tkimg_FixStringWriteProc _ANSI_ARGS_((
				Tcl_DString * data, Tcl_Interp ** interp,
				Tcl_DString ** dataPtr, Tcl_Obj ** format,
				Tk_PhotoImageBlock ** blockPtr));
/* Slot 23 is reserved */
/* Slot 24 is reserved */
/* Slot 25 is reserved */
/* Slot 26 is reserved */
/* Slot 27 is reserved */
/* Slot 28 is reserved */
/* Slot 29 is reserved */
/* 30 */
EXTERN char*		tkimg_GetStringFromObj _ANSI_ARGS_((Tcl_Obj * objPtr,
				int * lengthPtr));
/* 31 */
EXTERN char*		tkimg_GetByteArrayFromObj _ANSI_ARGS_((
				Tcl_Obj * objPtr, int * lengthPtr));
/* 32 */
EXTERN int		tkimg_ListObjGetElements _ANSI_ARGS_((
				Tcl_Interp * interp, Tcl_Obj * objPtr,
				int * argc, Tcl_Obj *** argv));

typedef struct TkimgStubs {
    int magic;
    struct TkimgStubHooks *hooks;

    Tcl_Channel (*tkimg_OpenFileChannel) _ANSI_ARGS_((Tcl_Interp * interp, CONST char * fileName, int permissions)); /* 0 */
    int (*tkimg_ReadInit) _ANSI_ARGS_((Tcl_Obj * data, int c, tkimg_MFile * handle)); /* 1 */
    void (*tkimg_WriteInit) _ANSI_ARGS_((Tcl_DString * buffer, tkimg_MFile * handle)); /* 2 */
    int (*tkimg_Getc) _ANSI_ARGS_((tkimg_MFile * handle)); /* 3 */
    int (*tkimg_Read) _ANSI_ARGS_((tkimg_MFile * handle, char * dst, int count)); /* 4 */
    int (*tkimg_Putc) _ANSI_ARGS_((int c, tkimg_MFile * handle)); /* 5 */
    int (*tkimg_Write) _ANSI_ARGS_((tkimg_MFile * handle, CONST char * src, int count)); /* 6 */
    void (*tkimg_ReadBuffer) _ANSI_ARGS_((int onOff)); /* 7 */
    void *reserved8;
    void *reserved9;
    int (*tkimg_PhotoPutBlock) _ANSI_ARGS_((Tk_PhotoHandle handle, Tk_PhotoImageBlock * blockPtr, int x, int y, int width, int height)); /* 10 */
    void *reserved11;
    void *reserved12;
    void *reserved13;
    void *reserved14;
    void *reserved15;
    void *reserved16;
    void *reserved17;
    void *reserved18;
    void *reserved19;
    void (*tkimg_FixChanMatchProc) _ANSI_ARGS_((Tcl_Interp ** interp, Tcl_Channel * chan, CONST char ** file, Tcl_Obj ** format, int ** width, int ** height)); /* 20 */
    void (*tkimg_FixObjMatchProc) _ANSI_ARGS_((Tcl_Interp ** interp, Tcl_Obj ** data, Tcl_Obj ** format, int ** width, int ** height)); /* 21 */
    void (*tkimg_FixStringWriteProc) _ANSI_ARGS_((Tcl_DString * data, Tcl_Interp ** interp, Tcl_DString ** dataPtr, Tcl_Obj ** format, Tk_PhotoImageBlock ** blockPtr)); /* 22 */
    void *reserved23;
    void *reserved24;
    void *reserved25;
    void *reserved26;
    void *reserved27;
    void *reserved28;
    void *reserved29;
    char* (*tkimg_GetStringFromObj) _ANSI_ARGS_((Tcl_Obj * objPtr, int * lengthPtr)); /* 30 */
    char* (*tkimg_GetByteArrayFromObj) _ANSI_ARGS_((Tcl_Obj * objPtr, int * lengthPtr)); /* 31 */
    int (*tkimg_ListObjGetElements) _ANSI_ARGS_((Tcl_Interp * interp, Tcl_Obj * objPtr, int * argc, Tcl_Obj *** argv)); /* 32 */
} TkimgStubs;

#ifdef __cplusplus
extern "C" {
#endif
extern TkimgStubs *tkimgStubsPtr;
#ifdef __cplusplus
}
#endif

#if defined(USE_TKIMG_STUBS) && !defined(USE_TKIMG_STUB_PROCS)

/*
 * Inline function declarations:
 */

#ifndef tkimg_OpenFileChannel
#define tkimg_OpenFileChannel \
	(tkimgStubsPtr->tkimg_OpenFileChannel) /* 0 */
#endif
#ifndef tkimg_ReadInit
#define tkimg_ReadInit \
	(tkimgStubsPtr->tkimg_ReadInit) /* 1 */
#endif
#ifndef tkimg_WriteInit
#define tkimg_WriteInit \
	(tkimgStubsPtr->tkimg_WriteInit) /* 2 */
#endif
#ifndef tkimg_Getc
#define tkimg_Getc \
	(tkimgStubsPtr->tkimg_Getc) /* 3 */
#endif
#ifndef tkimg_Read
#define tkimg_Read \
	(tkimgStubsPtr->tkimg_Read) /* 4 */
#endif
#ifndef tkimg_Putc
#define tkimg_Putc \
	(tkimgStubsPtr->tkimg_Putc) /* 5 */
#endif
#ifndef tkimg_Write
#define tkimg_Write \
	(tkimgStubsPtr->tkimg_Write) /* 6 */
#endif
#ifndef tkimg_ReadBuffer
#define tkimg_ReadBuffer \
	(tkimgStubsPtr->tkimg_ReadBuffer) /* 7 */
#endif
/* Slot 8 is reserved */
/* Slot 9 is reserved */
#ifndef tkimg_PhotoPutBlock
#define tkimg_PhotoPutBlock \
	(tkimgStubsPtr->tkimg_PhotoPutBlock) /* 10 */
#endif
/* Slot 11 is reserved */
/* Slot 12 is reserved */
/* Slot 13 is reserved */
/* Slot 14 is reserved */
/* Slot 15 is reserved */
/* Slot 16 is reserved */
/* Slot 17 is reserved */
/* Slot 18 is reserved */
/* Slot 19 is reserved */
#ifndef tkimg_FixChanMatchProc
#define tkimg_FixChanMatchProc \
	(tkimgStubsPtr->tkimg_FixChanMatchProc) /* 20 */
#endif
#ifndef tkimg_FixObjMatchProc
#define tkimg_FixObjMatchProc \
	(tkimgStubsPtr->tkimg_FixObjMatchProc) /* 21 */
#endif
#ifndef tkimg_FixStringWriteProc
#define tkimg_FixStringWriteProc \
	(tkimgStubsPtr->tkimg_FixStringWriteProc) /* 22 */
#endif
/* Slot 23 is reserved */
/* Slot 24 is reserved */
/* Slot 25 is reserved */
/* Slot 26 is reserved */
/* Slot 27 is reserved */
/* Slot 28 is reserved */
/* Slot 29 is reserved */
#ifndef tkimg_GetStringFromObj
#define tkimg_GetStringFromObj \
	(tkimgStubsPtr->tkimg_GetStringFromObj) /* 30 */
#endif
#ifndef tkimg_GetByteArrayFromObj
#define tkimg_GetByteArrayFromObj \
	(tkimgStubsPtr->tkimg_GetByteArrayFromObj) /* 31 */
#endif
#ifndef tkimg_ListObjGetElements
#define tkimg_ListObjGetElements \
	(tkimgStubsPtr->tkimg_ListObjGetElements) /* 32 */
#endif

#endif /* defined(USE_TKIMG_STUBS) && !defined(USE_TKIMG_STUB_PROCS) */

/* !END!: Do not edit above this line. */

#endif /* _TKIMGDECLS */
