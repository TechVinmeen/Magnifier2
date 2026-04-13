#include "Docmonitor.h"


    DocumentMonitor::DocumentMonitor() {
        acDocManager->addReactor(this); // Register the reactor
    }

    DocumentMonitor::~DocumentMonitor() {
        acDocManager->removeReactor(this); // Unregister the reactor
    }

    // This method is called when a document is about to be deactivated
    void DocumentMonitor::documentToBeDeactivated(AcApDocument* pDoc)  {
        //acutPrintf( pDoc->fileName());
        cmdRemoveMonitor();
    }
    
    /*
    This method is called when a document is about to be deactivated
    void DocumentMonitor::documentActivated(AcApDocument* pDoc) {
        acedAlert(pDoc->fileName());
        cmdAddMonitor();
    }
    */