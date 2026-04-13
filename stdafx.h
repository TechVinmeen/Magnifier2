//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2021 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
//
// Check if the build is DEBUG version and it's intended
// to be used with Non-DEBUG AutoCAD.
// In this case, for MFC header files, we need to undefine
// _DEBUG symbol
// Read this project readme.txt for more detail
//#define AC_FULL_DEBUG
#if defined( _DEBUG) && !defined(AC_FULL_DEBUG)
	#pragma message("Building debug modeless.arx to be used with release AutoCAD")
	#define _DEBUG_THIS_ONLY
	#undef _DEBUG
#endif

#ifndef WINVER
#define WINVER 0x601
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


// Turn on the _DEBUG symbol if it was defined, before including
// non-MFC header files.
//
#ifdef _DEBUG_THIS_ONLY
	#define _DEBUG
	#undef _DEBUG_THIS_ONLY
#endif


#include <dbapserv.h>
#include <rxregsvc.h>
#include <AcExtensionModule.h>
#include <stdio.h>
#include <stdarg.h>
#include <aced.h>
#include <adslib.h>
#include <dbapserv.h>
#include <rxmfcapi.h>
#include <adeskabb.h>
#include <axlock.h>
#include <acdocman.h>

// RX Includes
//NRG-Start
#include "rxdebug.h"
#include "acdb.h"               // acdb definitions

#include "aced.h"               // aced stuff
#include "adslib.h"             // RXADS definitions
#include "acdocman.h"           // MDI document manager
#include "rxmfcapi.h"           // ObjectARX MFC support
#include "AcExtensionModule.h"  // Utility class for extension dlls
#include "gs.h"
#include "acgs.h"
#include "acgsmanager.h"
#include "dbobjptr.h"
//*NRG
#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#define _DEBUG_WAS_DEFINED
#pragma message ("     Compiling MFC / STL / ATL header files in release mode.")
#undef _DEBUG
#endif
#include <aced.h>
#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG
#endif


#include "dbReactor.h"
//#include "docReactor.h"

#include "SampDialog.h"

#define CUR_DB acdbHostApplicationServices()->workingDatabase()


//helper functions:

BOOL endDlg();
BOOL startDlg();



void initApp();
void unloadApp();

bool GetActiveViewPortInfo(AcDbDatabase* pDb, ads_real& height, ads_real& width, AcGePoint3d& target, AcGeVector3d& viewDir, ads_real& viewTwist, bool getViewCenter);
void refreshView(AcGsView* pView);
AcDbObjectId SetViewTo(AcGsView* pView, AcDbDatabase* pDb, AcGeMatrix3d& viewMat, const acedDwgPoint ptDwgpoint);


extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* p);

