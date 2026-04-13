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
#ifndef _CTESTDIALOG_H
#define _CTESTDIALOG_H

#include "resource.h"
#include "stdafx.h"
#include "GsPreviewCtrl.h"	// Added by ClassView

class CSampDialog : public CDialog
{
public:
	CSampDialog( CWnd* pParent = NULL );
	enum { IDD = ID_SAMPDIALOG };
	CGsPreviewCtrl mPreviewCtrl;
	AcDbDatabase* mCurrentDwg;
	Acad::ErrorStatus InitDrawingControl(AcDbDatabase* pDb, const TCHAR* space = ACDB_MODEL_SPACE);
	AcGeMatrix3d m_viewMatrix;

	// - take into account previous temporary entities
	AcDbExtents m_tempExt;
protected:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

BOOL startDlg();
BOOL endDlg();
void MoveWindow(CPoint newPosition);

#endif // _CTESTDIALOG_H
