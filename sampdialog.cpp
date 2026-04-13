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
#include "sampdialog.h"
//
#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

// global per app
CSampDialog* gpDlg = NULL;
bool gbDisplayDialog = false;
extern CAcExtensionModule modelessDll;

/////////////////////////////////////////////////////////////////////////////////
//
//
//
CSampDialog::CSampDialog (CWnd* pParent) : CDialog(CSampDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSampDialog)
	 
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
//	When AutoCAD asks your dialog if it is willing to give up input
//	focus, return true to keep the focus and false to give it up.
//
#ifndef WM_ACAD_MFC_BASE
#define WM_ACAD_MFC_BASE        (1000)
#endif

#ifndef WM_ACAD_KEEPFOCUS
#define WM_ACAD_KEEPFOCUS       (WM_ACAD_MFC_BASE + 1)
#endif

BEGIN_MESSAGE_MAP(CSampDialog, CDialog)
	ON_WM_CLOSE()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

LRESULT CSampDialog::OnNcHitTest(CPoint point)
{
	if (mPreviewCtrl.mpView)
	{
		//SetViewTo(mPreviewCtrl.mpView, acdbHostApplicationServices()->workingDatabase(), m_viewMatrix);
	}
	return HTTRANSPARENT;
}

BOOL CSampDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	//acutPrintf(_T("CBlockViewDlg::OnInitDialog"));
	if (!mPreviewCtrl.SubclassDlgItem(IDC_STATIC, this))
		return FALSE;

	// now initialise the drawing control with the current working database
	InitDrawingControl(acdbHostApplicationServices()->workingDatabase());
	// initialise the visual style/view mode combo
	//InitVisualStyleCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////////////////////////////////////////////////////////////////
// takes a drawing and updates the GsView with it
Acad::ErrorStatus CSampDialog::InitDrawingControl(AcDbDatabase* pDb, const TCHAR* space)
{
	// have we got a valid drawing
	if (pDb == NULL)
		return Acad::eNullBlockName;

	// save the passed drawing so that we can use it later in various controls such as zoom extents
	mCurrentDwg = pDb;

	// initialize values
	m_tempExt.minPoint().set(1e20, 1e20, 1e20);
	m_tempExt.maxPoint().set(-1e20, -1e20, -1e20);

	AcDbBlockTableRecordPointer spaceRec(space, pDb, AcDb::kForRead);
	// if ok
	if (spaceRec.openStatus() == Acad::eOk)
	{
		//acutPrintf(_T("CBlockViewDlg::InitDrawingControl"));
		// initialize the preview control
		mPreviewCtrl.init(modelessDll.ModuleResourceInstance(), true);
		mPreviewCtrl.SetFocus();
		AcDbObjectId currentVsId;
		// set the view to the *Active* AutoCAD view
		currentVsId = SetViewTo(mPreviewCtrl.mpView, pDb, m_viewMatrix,NULL);
		// tell the view to display this space
		mPreviewCtrl.view()->add(spaceRec, mPreviewCtrl.model());
		// finally, tell the AcGsView what style to display in
		mPreviewCtrl.mpView->setVisualStyle(currentVsId);
	}
	return spaceRec.openStatus();
}

AcDbObjectId SetViewTo(AcGsView* pView, AcDbDatabase* pDb, AcGeMatrix3d& viewMat, const acedDwgPoint ptDwgpoint )
{
	// we are going to set the view to the current view of the drawing
	// The overall approach is to calculate the extents of the database in the coordinate system of the view
	// Calculate the extents in WCS
	

	AcGePoint3d extMax = pDb->extmax();
	AcGePoint3d extMin = pDb->extmin();

	// initialize it with sensible numbers - even if there is no entity
	if (extMin.distanceTo(extMax) > 1e20)
	{
		extMin.set(0, 0, 0);
		extMax.set(100, 100, 100);
	}

	// get the view port information - see parameter list
	ads_real height = 0.0, width = 0.0, viewTwist = 0.0;
	AcGePoint3d targetView;
	AcGeVector3d viewDir;
	AcDbObjectId currentVsId;

GetActiveViewPortInfo(pDb, height, width, targetView, viewDir, viewTwist, /*currentVsId,*/ true);

	if (ptDwgpoint != NULL) {//NRG changes the target if the call to this function came from mousemove ;-)
		targetView.x = ptDwgpoint[0];
		targetView.y = ptDwgpoint[1];
		targetView.z = ptDwgpoint[2];
	}
	CPoint ptIn;
	acedDwgPoint ptOut;
	acedCoordFromPixelToWorld(ptIn, ptOut);
	// we are only interested in the directions of the view, not the sizes, so we normalise. 
	viewDir = viewDir.normal();

	//**********************************************
	// Our view coordinate space consists of z direction 
	// get a perp vector off the z direction
	// Make sure its normalised
	AcGeVector3d viewXDir = viewDir.perpVector().normal();
	// correct the x angle by applying the twist
	viewXDir = viewXDir.rotateBy(viewTwist, -viewDir);
	// now we can work out y, this is of course perp to the x and z directions. No need to normalise this, 
	// as we know that x and z are of unit length, and perpendicular, so their cross product must be on unit length
	AcGeVector3d viewYDir = viewDir.crossProduct(viewXDir);

	// find a nice point around which to transform the view. We'll use the same point as the center of the view.
	AcGePoint3d boxCenter = extMin + 0.5 * (extMax - extMin);

	//**********************************************
	// create a transform from WCS to View space
	// this represents the transformation from WCS to the view space. (Actually not quite since 
	// we are keeping the fixed point as the center of the box for convenience )
	
	viewMat = AcGeMatrix3d::alignCoordSys(boxCenter, AcGeVector3d::kXAxis, AcGeVector3d::kYAxis, AcGeVector3d::kZAxis,
		boxCenter, viewXDir, viewYDir, viewDir).inverse();

	AcDbExtents wcsExtents(extMin, extMax);
	// now we have the view Extents
	AcDbExtents viewExtents = wcsExtents;
	// transforms the extents in WCS->view space
	viewExtents.transformBy(viewMat);

	//**********************************************
	// get the extents of the AutoCAD view
	/*
	double xMax = fabs(viewExtents.maxPoint().x - viewExtents.minPoint().x);
	double yMax = fabs(viewExtents.maxPoint().y - viewExtents.minPoint().y);
	*/
	//**********************************************
	// setup the view
	AcGePoint3d eye = targetView + viewDir;

	// upvector				
	pView->setView(eye, targetView, viewYDir, /*xMax*/ width/20.0, height/20.0 /*yMax*/);
	// update the gsView
	refreshView(pView);

	return currentVsId;
}

void refreshView(AcGsView* pView)
{
	if (pView != NULL)
	{
		pView->invalidate();
		pView->update();
	}
}

//***************************************************************************************
// get the view port information - see parameter list
bool GetActiveViewPortInfo(AcDbDatabase* pDb, ads_real& height, ads_real& width,
	AcGePoint3d& target, AcGeVector3d& viewDir,
	ads_real& viewTwist,
	//AcDbObjectId& currentVsId,
	bool getViewCenter)
{
	// if not ok
	if (pDb == NULL)
		return false;

	// get the current document associated with the PDb database and set it current

	AcApDocument* pDoc = acDocManager->document(pDb);
	acDocManager->setCurDocument(pDoc);


	// make sure the active view port is uptodate
	acedVports2VportTableRecords();

	// open the view port records
	AcDbViewportTablePointer pVTable(pDb->viewportTableId(), AcDb::kForRead);
	// if we opened them ok
	if (pVTable.openStatus() == Acad::eOk)
	{
		AcDbViewportTableRecord* pViewPortRec = NULL;
		Acad::ErrorStatus es = pVTable->getAt(_T("*Active"), pViewPortRec, AcDb::kForRead);
		if (es == Acad::eOk)
		{
			// get the height of the view
			height = pViewPortRec->height();
			// get the width
			width = pViewPortRec->width();
			// if the user wants the center of the viewport used
			if (getViewCenter == true)
			{
				struct resbuf rb;
				memset(&rb, 0, sizeof(struct resbuf));
				// get the system var VIEWCTR
				acedGetVar(_T("VIEWCTR"), &rb);
				// set that as the target
				target = AcGePoint3d(rb.resval.rpoint[X], rb.resval.rpoint[Y], rb.resval.rpoint[Z]);
			}
			// we want the viewport's camera target setting
			else
			{
				// get the target of the view
				target = pViewPortRec->target();


				
			}	
			AcGePoint3d targetPoint;
			POINT lpPoint;
			acedDwgPoint dwgpoint;
			//GetCursorPos(&lpPoint);
			if (GetCursorPos(&lpPoint)==TRUE)
			{
				CPoint cpoint(lpPoint.x, lpPoint.y);
				//acedAlert(_T("hahaha"));
				acedCoordFromPixelToWorld(cpoint, dwgpoint);
				targetPoint.x = dwgpoint[0];
				targetPoint.y = dwgpoint[1];
				targetPoint.z = dwgpoint[2];
				target = targetPoint;
				};
			// 
			// get the view direction
			viewDir = pViewPortRec->viewDirection();
			// get the view twist of the viewport
			viewTwist = pViewPortRec->viewTwist();
			// return the current Visual Style
			//currentVsId = pViewPortRec->visualStyle();
		}
		// close after use
		pViewPortRec->close();
	}


	// now restore the original document
	acDocManager->setCurDocument(acDocManager->mdiActiveDocument());

	return (true);
}


BOOL startDlg()
{
	BOOL b = TRUE;
	if (!gpDlg)
	{
		CAcModuleResourceOverride resOverride;
		HWND hwnd1 = adsw_acadMainWnd();
		HWND hwnd2 = acedGetAcadFrame()->m_hWnd;
		//acutPrintf(_T("THE CLIENT HWND IS:           %10x\n"), hwnd2);


		gpDlg = new CSampDialog(NULL);

		b = gpDlg->Create(ID_SAMPDIALOG);

	}

	return false;
}

BOOL endDlg()
{
	if (!gpDlg)
		return TRUE;

	BOOL b = gpDlg->DestroyWindow();
	if (b)
		gpDlg = NULL;
	return b;
}

void MoveWindow(CPoint newPosition)
{
	if (gpDlg)
	{
		CRect dlgRect;
		gpDlg->GetWindowRect(dlgRect);
		//gpDlg->MoveWindow(newPosition.x - (dlgRect.Width() / 2) , newPosition.y + (dlgRect.Height() / 2), dlgRect.Width(), dlgRect.Height());
		gpDlg->SetWindowPos( NULL, newPosition.x - (dlgRect.Width() / 2), newPosition.y - (dlgRect.Height() / 2), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		//SetViewTo(mPreviewCtrl.mpView, pDb, m_viewMatrix);
	}
	return;
}
