#include "tchar.h"
#include "aced.h"
#include "acdocman.h"

extern void cmdAddMonitor(); // VERY BAD CODING
extern void cmdRemoveMonitor();

class DocumentMonitor : public AcApDocManagerReactor {
public:
    DocumentMonitor();

    ~DocumentMonitor();

   virtual void documentToBeDeactivated(AcApDocument* pDoc);
  // virtual void documentActivated(AcApDocument* pActivatedDoc);
};
