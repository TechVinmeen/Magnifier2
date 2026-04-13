//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2021 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////


#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

#include "sampdialog.h"
#include "Monitor.h"
#include "adsdef.h"
extern CSampDialog* gpDlg;

Acad::ErrorStatus
AsdkInputPointMonitor::monitorInputPoint(const AcEdInputPoint& input, AcEdInputPointMonitorResult& output)
{

    //acutPrintf( _T("\n") ); 
    //acutPrintf( _T("Computed point:          %.2f, %.2f, %.2f\n"), input.computedPoint()[X], input.computedPoint()[Y], input.computedPoint()[Z] );

    struct resbuf rb;
    acedDwgPoint ptIn;
    CPoint ptOut;
    ptIn[0] = input.computedPoint()[X];
    ptIn[1] = input.computedPoint()[Y];
    ptIn[2] = input.computedPoint()[Z];

    if (acedGetVar(_T("CVPORT"), &rb) == RTNORM)
    {
    acedCoordFromWorldToPixel(rb.resval.rint, ptIn, ptOut);
   // acutPrintf(_T("PIXEL point:          %d, %d\n"), ptOut.x, ptOut.y);
    POINT p;
    if (GetCursorPos(&p))
    {
        ptOut.x = p.x;
        ptOut.y = p.y;
        //cursor position now in p.x and p.y
    }
    MoveWindow(ptOut);
    SetViewTo(gpDlg->mPreviewCtrl.mpView, acdbHostApplicationServices()->workingDatabase(), gpDlg->m_viewMatrix, ptIn);
    
    }

    return Acad::eOk;
}



