//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2021 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////

#include <dbapserv.h>
#define CUR_DB acdbHostApplicationServices()->workingDatabase()

//helper functions:

BOOL endDlg();
BOOL startDlg();


void initApp();
void unloadApp();

extern "C" AcRx::AppRetCode acrxEntryPoint ( AcRx::AppMsgCode msg, void* );

