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
//
#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif


extern CSampDialog* gpDlg;
extern bool gbDisplayDialog;

#include "AdskDMgr.h"
#include <cinttypes>


class CDocInfo
{
public:
    CDocInfo();
    bool undoBtnAvailable;
    bool undoCmd;
};

CDocInfo::CDocInfo(){this->undoBtnAvailable=false; this->undoCmd=false;}

AsdkDataManager<CDocInfo> DocDataMgr; //MDI safe
#define docState DocDataMgr.docData()

//
//	Attach the database reactor if one isn't attached already.  
//	Then, start up the dialog.
//
//
void attachDbReactor(AcDbDatabase* pDb)
{
    if(gbDisplayDialog)
    {
        AcDbDatabase* pWkDb = acdbHostApplicationServices()->workingDatabase();
        assert(pWkDb == pDb);
        AcDbDatabase* pCurDb = curDoc()->database();
        assert(pCurDb == pDb);
        acDocManager->lockDocument(curDoc(), AcAp::kWrite);
    
        gpDbReactor = new CDbModReactor();
        pWkDb->addReactor(gpDbReactor);
        acutPrintf(_T("\nAttached CDbModReactor to the current database.\n"));     
        
        acDocManager->unlockDocument(curDoc());
        acedPostCommandPrompt();
    }
    else
        endDlg();
}
//
//
//	Detach the database reactor if the db is deactivated.  
//  Close the dialog if requested by the user.
//
//
void detachDbReactor(AcDbDatabase* pDb)
{
    acDocManager->lockDocument(curDoc(), AcAp::kWrite);
    if(gpDbReactor) 
    {
        pDb->removeReactor(gpDbReactor);
        delete gpDbReactor;
        gpDbReactor = NULL;
        acutPrintf(_T("\nDetached DbReactor from the current database.\n"));
    }
    acDocManager->unlockDocument(curDoc());

    acedPostCommandPrompt();
}


void CDocReactor::documentActivated(AcApDocument* pDoc)
{
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
        attachDbReactor(pDoc->database());
#ifndef NDEBUG
        acutPrintf(_T("\ndocumentActiveated: %s.\n"), pDoc->fileName());
        acedPostCommandPrompt();
#endif
    }
}
//
// We need to attach the database reactor to the newly created
// database. We need to delete the previous database reactor as well.
//

void CDocReactor::documentCreated(AcApDocument* pDoc)
{
#ifndef NDEBUG
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
        acutPrintf(_T("\ndocumentCreated: %s.\n"), pDoc->fileName());
        acedPostCommandPrompt();
    }
#endif

}

//
// This deactivation includes a non-Acad window is activated 
// so the Acad window is losing focus.
//
void CDocReactor::documentToBeDeactivated(AcApDocument* pDoc)
{
    acedAlert(_T("OKTBD"));
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
#ifndef NDEBUG
        acutPrintf(_T("\ndocumentToBeDeactiveated: %s."), pDoc->fileName());
        acedPostCommandPrompt();
#endif
        detachDbReactor(pDoc->database());
    }

}
//
// Obviously, we need to delete the database reactor associated
// with the destroyed doc.
//
void CDocReactor::documentToBeDestroyed(AcApDocument* pDoc)
{
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
#ifndef NDEBUG
        acutPrintf(_T("\ndocumentToBeDestroyed: %s.\n"), pDoc->fileName());
        acedPostCommandPrompt();
#endif
        detachDbReactor(pDoc->database());
    }

}
//
// This one can not be used because it gives the historically 
// to be activated document prior to the current action.
//
void CDocReactor::documentToBeActivated(AcApDocument* pDoc)
{
#ifndef NDEBUG
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
        acutPrintf(_T("\ndocumentToBeActivated: %s."), pDoc->fileName());
        acedPostCommandPrompt();
    }
#endif
}


