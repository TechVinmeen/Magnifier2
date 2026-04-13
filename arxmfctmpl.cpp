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
#include "StdAfx.h"

#include <Windows.h>
#include "aced.h"
#include "dbmain.h"
#include "acdbabb.h"
#include "adeskabb.h"
#include "rxregsvc.h"
#include "acgi.h"
#include "acdocman.h"
#include "dbapserv.h"

#include "Monitor.h"
#include "docmonitor.h"
#include "filter.h"

#include "tchar.h"

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

extern CSampDialog* gpDlg;

/////////////////////////////////////////////////////////////////////////////
// Define the sole extension module object.
AC_IMPLEMENT_EXTENSION_MODULE(modelessDll);


void cmdAddMonitor();
void cmdRemoveMonitor();
void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void startCursorMonitor();
void stopCursorMonitor();

AsdkInputPointMonitor myMonitor;
static UINT_PTR timerID = 0;

//DocumentMonitor* gDocMonitor = nullptr;


/////////////////////////////////////////////////////////////////////////////
//
// Rx interface
//
/////////////////////////////////////////////////////////////////////////////

void initApp()
{
	acedRegCmds->addCommand(_T("AsdkInputPoint"), _T("AddMonitor"), _T("ADDMON"), ACRX_CMD_TRANSPARENT | ACRX_CMD_NO_UNDO_MARKER, cmdAddMonitor);
	acedRegCmds->addCommand(_T("AsdkInputPoint"), _T("RemoveMonitor"), _T("REMMON"), ACRX_CMD_TRANSPARENT  | ACRX_CMD_NO_UNDO_MARKER, cmdRemoveMonitor);	
}


void unloadApp()
{
	//delete gDocMonitor; // Clean up the monitor
	//gDocMonitor = nullptr;
	cmdRemoveMonitor();
	acedRegCmds->removeGroup(_T("AsdkInputPoint"));
}

void cmdAddMonitor()
{
	curDoc()->inputPointManager()->addPointMonitor(&myMonitor);
	startDlg();
}

void cmdRemoveMonitor()
{
	curDoc()->inputPointManager()->removePointMonitor(&myMonitor);
	endDlg();
}



/////////////////////////////////////////////////////////////////////////////
//
// Entry points
//
/////////////////////////////////////////////////////////////////////////////

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
        modelessDll.AttachInstance(hInstance);
	else if (dwReason == DLL_PROCESS_DETACH)
        modelessDll.DetachInstance();  
	
	return 1;   // ok
}


extern "C" AcRx::AppRetCode 
acrxEntryPoint(AcRx::AppMsgCode msg, void* p)
{
	switch( msg ) 
	{
	case AcRx::kInitAppMsg: 
		acrxDynamicLinker->unlockApplication(p);
		acrxRegisterAppMDIAware(p);
		acrxDynamicLinker->registerAppMDIAware(p);
		initApp(); 
		startCursorMonitor();
		break;
	case AcRx::kUnloadDwgMsg:

		break;
	case AcRx::kUnloadAppMsg: 

		unloadApp(); 
		break;
	default:
		break;
	}
	return AcRx::kRetOK;
}



void startCursorMonitor() {
	if (timerID == 0) {
		// Set a timer with a 500ms interval
		timerID = SetTimer(nullptr, 0, 500, (TIMERPROC)TimerProc);
		if (timerID) {
			acutPrintf(_T("Cursor monitor started.\n"));
		}
		else {
			acutPrintf(_T("Failed to start cursor monitor.\n"));
		}
	}
}


void stopCursorMonitor() {
	if (timerID != 0) {
		KillTimer(nullptr, timerID);
		timerID = 0;
		acutPrintf(_T("Cursor monitor stopped.\n"));
	}
}
void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	return;
	if (!gpDlg) return;
	static HCURSOR lastCursor = nullptr;

	HCURSOR hcursor = GetCursor();
	if (hcursor == 0) {
		gpDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		gpDlg->ShowWindow(SW_HIDE);
	}
}