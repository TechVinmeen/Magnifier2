#include "StdAfx.h"


#include "stdarx.h"
#include "resource.h"
#include "GsPreviewCtrl.h"
#include "Ac64bitHelpers.h"

BEGIN_MESSAGE_MAP(CGsPreviewCtrl, CStatic)
  //{{AFX_MSG_MAP(CGsPreviewCtrl)
  ON_WM_PAINT()
  ON_WM_NCHITTEST()

END_MESSAGE_MAP()

void CGsPreviewCtrl::OnPaint() 
{
  CPaintDC dc(this); 
  mpView->invalidate(); 
  mpView->update();
}

/*
void CGsPreviewCtrl::erasePreview()
{
  if (mpView)
    mpView->eraseAll();
  if (mpManager && mpModel) {  
    mpManager->destroyAutoCADModel (mpModel);
    mpModel = NULL;
  }
}
*/

void CGsPreviewCtrl::clearAll()
{
  if (mpView)  
  {
    mpView->eraseAll();
  }
  if (mpDevice)    
  {
    bool b = mpDevice->erase(mpView);
    RXASSERT(b);
  }

  if (mpGraphicsKernel)    
  {
    if (mpView)  
    {
      // free all of our temporary entities
      //freeTempEntities();

      mpGraphicsKernel->deleteView(mpView);
      mpView = NULL;
    }
    if (mpGhostModel) 
    {
      mpGraphicsKernel->deleteModel(mpGhostModel);
      mpGhostModel = NULL;
    }
    mpGraphicsKernel = NULL;
  }

  if (mpManager)    
  {
    if (mpModel) 
    {
      if (mbModelCreated)
        mpManager->destroyAutoCADModel(mpModel);
      mpModel = NULL;
    }

    if (mpDevice)    
    {
      mpManager->destroyAutoCADDevice(mpDevice);
      mpDevice = NULL;
    }
    mpManager = NULL;
  }    
}

void CGsPreviewCtrl::init(HMODULE hRes, bool bCreateModel)
{
  //Load our special cursors
  //mhPanCursor = LoadCursor(hRes,MAKEINTRESOURCE(IDI_PAN));
  //mhCrossCursor = LoadCursor(hRes,MAKEINTRESOURCE(IDI_CROSS));
  //mhOrbitCursor = LoadCursor(hRes,MAKEINTRESOURCE(IDI_ORBIT));
 // SetClassLongPtr(m_hWnd,GCLP_HCURSOR,(LONG_PTR)NULL);
  //Instantiate view, a device and a model object
  CRect rect;
  if (!mpManager) 
  {
    // get the AcGsManager object for a specified AutoCAD MDI Client CView
    mpManager = acgsGetGsManager();
    RXASSERT(mpManager);

	
	AcGsKernelDescriptor descriptor;
    descriptor.addRequirement(AcGsKernelDescriptor::k3DDrawing);
    descriptor.addRequirement(AcGsKernelDescriptor::k2DDrawing);
    descriptor.addRequirement(AcGsKernelDescriptor::k3DSelection);
	
    mpGraphicsKernel = AcGsManager::acquireGraphicsKernel(descriptor);
	RXASSERT(mpGraphicsKernel);

    
    mpDevice = mpManager->createAutoCADDevice(*mpGraphicsKernel, m_hWnd);
    RXASSERT(mpDevice);

    // get the size of the window that we are going to draw in
    GetClientRect( &rect);
    // make sure the gs device knows how big the window is
    mpDevice->onSize(rect.Width(), rect.Height());   
    // construct a simple view
    mpView = mpGraphicsKernel->createView();
    RXASSERT(mpView);
    if (bCreateModel)
    {
      RXASSERT(mpModel==NULL);
      // create an AcGsModel object with the AcGsModel::kMain RenderType 
      // (which is a hint to the graphics system that the geometry in this 
      // model should be rasterized into its main frame buffer). This 
      // AcGsModel is created with get and release functions that will open and close AcDb objects.
      mpModel = mpManager->createAutoCADModel(*mpGraphicsKernel);
      RXASSERT(mpModel);
      mbModelCreated = true;
    }
    //another model without open/close for the orbit gadget
    mpGhostModel = mpGraphicsKernel->createModel(AcGsModel::kDirect,0,0,0);
    mpView->add(&mOrbitGadget,mpGhostModel);
    mOrbitGadget.setGsView(mpView);
    mpDevice->add(mpView);
  }
}

LRESULT CGsPreviewCtrl::OnNcHitTest(CPoint point) 
{
  return HTTRANSPARENT;
}

