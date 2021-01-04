#include "ecanvas.hh"
#include "elegend.hh"
#include "TGClient.h"
#include "TGWindow.h"
#include "KBGlobal.hh"
#include <iostream>
using namespace std;

ClassImp(ecanvas)

int ecanvas::fIndexCanvas = 0;

ecanvas::ecanvas(const char *name) {
  fName = name;
  if (fName.IsNull())
    fName = Form("ecanvas_%d",ecanvas::fIndexCanvas++);
  
  //auto man = emanager::manager();
  //fLMargin = man -> fLMargin;
  //fRMargin = man -> fRMargin;
  //fBMargin = man -> fBMargin;
  //fTMargin = man -> fTMargin;
}

//ecanvas::ecanvas(TPad *pad) : ecanvas(pad->GetName()), fPad((TPad*)pad)
ecanvas::ecanvas(TPad *pad) : ecanvas(pad->GetName())//, 
{
  fPad = (TPad*) pad;
  //fPad -> SetMargin(fLMargin, fRMargin, fBMargin, fTMargin);
  //fPad -> Update();
  //fPad -> Modified();
}

edrawing *ecanvas::makeDrawing(TObject *obj, TString option) {
  edrawing *drawing = nullptr;
  /**/ if (obj->InheritsFrom(TH1::Class()))     drawing = new ehist((TH1 *)obj, option);
  //else if (obj->InheritsFrom(TF1::Class()))     drawing = new efunc   (obj, option);
  //else if (obj->InheritsFrom(TGraph::Class()))  drawing = new egraph  (obj, option);
  //else if (obj->InheritsFrom(TLegend::Class())) drawing = new elegend (obj, option);
  else                                          drawing = new edrawing(obj, option);
  return drawing;
}

edrawing *ecanvas::addDrawing(int idxPad, TObject *obj, TString option, bool isFrame)
{
  edrawing *drawing = nullptr;
  if (obj->InheritsFrom(edrawing::Class()))
    drawing = (edrawing *) obj;
  else
    drawing = makeDrawing(obj,option);

  if (idxPad==-1) idxPad = fCurrentDaughterIndex; // same
  if (idxPad==-2) idxPad = fCurrentDaughterIndex + 1; // next

  drawing -> setPadIndex(idxPad);
  fCurrentDaughterIndex = idxPad;

  if (idxPad!=0) {
    if (fDaughterECanvasArray==nullptr)
      fDaughterECanvasArray = new TObjArray();
    auto ecvs = (ecanvas *) fDaughterECanvasArray -> At(idxPad);
    if (ecvs==nullptr) {
      ecvs = new ecanvas(fName+"_"+idxPad);
      ecvs -> setParentCanvas(this);
      fDaughterECanvasArray -> AddAt(ecvs, idxPad);
    }
    ecvs -> addDrawing(0, drawing, option, isFrame);
    fNumDaughters = fDaughterECanvasArray -> GetEntries();
  }
  else {
    findOptions(option);
    //if (isFrame || drawing->getSortIndex()==0)
    //  fFrame = drawing;
    //else
    {
      Add(drawing);
      Sort();
    }
  }

  return drawing;
}


TPad *ecanvas::draw(TPad *pad)
{
  // pad ==========================================================
  int numDaugthers = 0;
  if (fDaughterECanvasArray!=nullptr)
    numDaugthers = fDaughterECanvasArray -> GetEntries();

  int numDivX = 1;
  int numDivY = 1;
  int padSizeX = 100;
  int padSizeY = 100;

  getNextPadInfo(numDaugthers, numDivX, numDivY, padSizeX, padSizeY);

  if (pad!=nullptr)
    fPad = pad;
  else {
    if (fPad==nullptr) {
      int positionXY = 20*(ecanvas::fIndexCanvas-1);
      fPad = (TPad *) new TCanvas(fName, fName, positionXY, positionXY, padSizeX, padSizeY);
    }
    pad = fPad;
  }

  if (fDaughterECanvasArray!=nullptr)
  {
    dividePad(numDivX, numDivY);

    for (auto iBranch=1; iBranch<=numDaugthers; ++iBranch) {
      auto becvs = (ecanvas *) fDaughterECanvasArray -> At(iBranch);
      becvs -> setPad((TPad *)(fPad -> cd(iBranch)));
      becvs -> draw();
    }
    return fPad;
  }

  fPad -> cd();
  /**/ if (fGridX) fPad -> SetGridx();
  else if (fGridY) fPad -> SetGridy();

  /**/ if (fLogX) fPad -> SetLogx();
  else if (fLogY) fPad -> SetLogy();
  else if (fLogZ) fPad -> SetLogz();

  // frame ========================================================
  auto numDrawings = GetEntries();
  if (numDrawings!=1) {
    getFrame();
    kb_debug << fName << endl;
    fFrame -> draw();
  }

  // main drawings ================================================
  auto legend = new TLegend();
  for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
    auto drawing = (edrawing *) At(idrawing);
    drawing -> addToLegend(legend);
    drawing -> draw();
  }

  // legend =======================================================
  auto drawlegend = new elegend(legend);
  drawlegend -> setLegendAlign(fLegendAlign);
  drawlegend -> draw();

  return fPad;
}

void ecanvas::setPad(TPad *pad) {
  fPad = pad;
}

void ecanvas::setPadMargin(TPad *pad) {
  if (pad==nullptr) pad = fPad;
  if (pad!=nullptr) {
    pad -> SetMargin(fLMargin, fRMargin, fBMargin, fTMargin);
    pad -> Modified();
    pad -> Update();
  }
}

void ecanvas::getNextPadInfo(int numPads, int &numDivX, int &numDivY, int &padSizeX, int &padSizeY)
{
  padSizeX = 600;
  padSizeY = 500;
  numDivX = fNumDivXY[numPads][0];
  numDivY = fNumDivXY[numPads][1];

  Int_t dummyX, dummryY;
  UInt_t mornitorSizeX=2000, mornitorSizeY=1500;
  auto cvsdummy = new TCanvas("cvsdummy");
  gVirtualX -> GetWindowSize(gClient->GetRoot()->GetId(),dummyX,dummryY,mornitorSizeX,mornitorSizeY);
  delete cvsdummy;

  auto padSizeXScaled = padSizeX*numDivX;
  while (padSizeXScaled>mornitorSizeX) padSizeXScaled -= 10;
  padSizeY = padSizeY * (double(padSizeXScaled)/padSizeX);
  padSizeX = padSizeXScaled;

  auto padSizeYScaled = padSizeY*numDivY;
  while (padSizeYScaled>mornitorSizeY) padSizeYScaled -= 10;
  padSizeX = padSizeX * (double(padSizeYScaled)/padSizeY);
  padSizeY = padSizeYScaled;
}

void ecanvas::dividePad(int numDivX, int numDivY, double xMargin, double yMargin)
{
  fPad -> Divide(numDivX,numDivY,xMargin,yMargin);
  for (auto iBranch=1; iBranch<=(numDivX*numDivY); ++iBranch) {
    auto pad = (TPad *)(fPad -> cd(iBranch));
    setPadMargin(pad);
  }
}

edrawing *ecanvas::getFrame()
{
  if (fFrame==nullptr) {
    double y1Frame = DBL_MAX;
    double y2Frame = -DBL_MAX;
    double x1Frame = DBL_MAX;
    double x2Frame = -DBL_MAX;
    auto numDrawings = GetEntries();
    for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
      auto drawing = (edrawing *) At(idrawing);
      if (drawing->getFindRange()) {
        if (x1Frame > drawing -> getX1Range()) x1Frame = drawing -> getX1Range();
        if (x2Frame < drawing -> getX2Range()) x2Frame = drawing -> getX2Range();
        if (y1Frame > drawing -> getY1Range()) y1Frame = drawing -> getY1Range();
        if (y2Frame < drawing -> getY2Range()) y2Frame = drawing -> getY2Range();
      }
    }
    if (numDrawings==0) {
      y1Frame = 0; y2Frame = 1;
      x1Frame = 0; x2Frame = 1;
    }
    fFrame = new ehist(TString(fName)+"_frame","",200,x1Frame,x2Frame,200,y1Frame,y2Frame);
  }

  return fFrame;
}

const char *ecanvas::print(bool printout) const
{
  TString val = Form("ecanvas with %d daughter pads, # of drawings: (",fNumDaughters);

  auto numBranches = 0;
  if (fDaughterECanvasArray!=nullptr)
    numBranches = fDaughterECanvasArray -> GetEntries();

  if (numBranches==0)
    val += Form("%d)", this->GetEntries());
  else
    for (auto iBranch=0; iBranch<numBranches; ++iBranch) {
      auto ecvs2 = (ecanvas*) fDaughterECanvasArray -> At(iBranch);
      if (iBranch==numBranches-1) val += Form("%d)",  ecvs2->GetEntries());
      else                        val += Form("%d, ", ecvs2->GetEntries());
    }

  if (printout) cout_info << val << endl;
  return val;
}

void ecanvas::findOptions(TString option)
{
  auto optionManager = estring(option);

  /**/ if (optionManager.findOption("legendrt")) fLegendAlign = 88;
  else if (optionManager.findOption("legendlt")) fLegendAlign = 8;
  else if (optionManager.findOption("legendlb")) fLegendAlign = 0;
  else if (optionManager.findOption("legendrb")) fLegendAlign = 80;
  else if (optionManager.findOption("legendrrt")) fLegendAlign = 98;
  else if (optionManager.findOption("legendrrb")) fLegendAlign = 90;

  /**/ if (optionManager.findOption("gridx")) fGridX = true;
  else if (optionManager.findOption("gridy")) fGridY = true;

  /**/ if (optionManager.findOption("logx")) fLogX = true;
  else if (optionManager.findOption("logy")) fLogY = true;
  else if (optionManager.findOption("logz")) fLogZ = true;
}

void ecanvas::setRMarginForLegend(double dxLegend) { fRMargin = dxLegend; setPadMargin(); }
void ecanvas::setTMarginForLegend(double dyLegend) { fTMargin = dyLegend; setPadMargin(); }
