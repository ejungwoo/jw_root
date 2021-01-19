#include "ecanvas.hh"
#include "elegend.hh"
#include "efunc.hh"
#include "TGClient.h"
#include "TGWindow.h"
#include "THashList.h"
#include "TSystem.h"
#include "KBGlobal.hh"
#include <iostream>
using namespace std;

ClassImp(ecanvas)

ecanvas::ecanvas(const char *name)
: edrawing(name)
{
  init();
}

ecanvas::ecanvas(TPad *pad) : ecanvas(pad->GetName())//, 
{
  fObject = pad;
}

void ecanvas::init()
{
  edrawing::init();

  if (fEHistMain==nullptr) fEHistMain = new ehist();
  if (fDrawingArray ==nullptr) fDrawingArray = new TObjArray();
}

ehist *ecanvas::histSame(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, int ny, double y1, double y2, const char *opt)
{
  auto hist0 = new ehist(name, ttl, expr, sel, nx, x1, x2, ny, y1, y2, opt);
  addSame(hist0);
  return hist0;
}

ehist *ecanvas::histSame(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, const char *opt)
{
  auto hist0 = new ehist(name, ttl, expr, sel, nx, x1, x2, opt);
  addSame(hist0);
  return hist0;
}

ehist *ecanvas::histNext(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, int ny, double y1, double y2, const char *opt)
{
  auto hist0 = new ehist(name, ttl, expr, sel, nx, x1, x2, ny, y1, y2, opt);
  addNext(hist0);
  return hist0;
}

ehist *ecanvas::histNext(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, const char *opt)
{
  auto hist0 = new ehist(name, ttl, expr, sel, nx, x1, x2, opt);
  addNext(hist0);
  return hist0;
}

edrawing *ecanvas::makeDrawing(TObject *obj, TString option) {
  edrawing *drawing = nullptr;
  /**/ if (obj->InheritsFrom(TH1::Class()))     drawing = new ehist((TH1 *)obj, option);
  else if (obj->InheritsFrom(TF1::Class()))     drawing = new efunc((TF1 *)obj, option);
  //else if (obj->InheritsFrom(TGraph::Class()))  drawing = new egraph  (obj, option);
  else if (obj->InheritsFrom(TLegend::Class())) drawing = new elegend (obj, option);
  else                                          drawing = new edrawing(obj, option);
  return drawing;
}

edrawing *ecanvas::addDrawing(int idxPad, TObject *obj, TString option, bool isFrame)
{
  edrawing *drawing = nullptr;
  if (obj->InheritsFrom(edrawing::Class())) {
    drawing = (edrawing *) obj;
    drawing -> setOption(option);
  }
  else
    drawing = makeDrawing(obj,option);

       if (idxPad==-1) idxPad = fCurrentDaughterIndex; // same
  else if (idxPad==-2) idxPad = fCurrentDaughterIndex + 1; // next
  else if (idxPad==-9) { // same all

    if (fDaughterECanvasArray==nullptr) {
      addDrawing(0, drawing, option);
    }
    else {
      auto numDaughters = fDaughterECanvasArray -> GetEntries();
      for (auto iBranch=1; iBranch<=numDaughters; ++iBranch)
        addDrawing(iBranch,drawing, option);
    }
    return drawing;
  }

  fCurrentDaughterIndex = idxPad;

  if (idxPad!=0) {
    if (fDaughterECanvasArray==nullptr)
      fDaughterECanvasArray = new TObjArray();
    auto ecvs = (ecanvas *) fDaughterECanvasArray -> At(idxPad);
    if (ecvs==nullptr) {
      ecvs = new ecanvas(fNameCurrent+"_cd"+idxPad);
      ecvs -> setParentCanvas(this);
      fDaughterECanvasArray -> AddAt(ecvs, idxPad);
      drawing -> setECanvas(this,idxPad);
    }
    ecvs -> addDrawing(0, drawing, option, isFrame);
  }
  else {
    option = option + "," + drawing -> getOption();
    applyOptions(option);
    fDrawingArray -> Add(drawing);
    fDrawingArray -> Sort();
    if (fParentECanvas==nullptr) drawing -> setECanvas(this,0);
  }

  return drawing;
}

TString ecanvas::configureNewName() {
  if (fObjStack.FindObject(fNameCurrent)!=nullptr)
    fNameCurrent = Form("%s_v%d",fName.Data(),fObjStack.GetEntriesFast());
  return fNameCurrent;
}

void ecanvas::draw(TPad *pad)
{
  // pad ==========================================================
  int numDaughters = 0;
  if (fDaughterECanvasArray!=nullptr)
    numDaughters = fDaughterECanvasArray -> GetEntries();

  int numDivX = 1;
  int numDivY = 1;
  int padSizeX = 100;
  int padSizeY = 100;

  auto padO = getPad();

  if (pad!=nullptr)
    padO = pad;
  else {
    if (padO==nullptr)
    {
      getNextPadInfo(numDaughters, numDivX, numDivY, padSizeX, padSizeY);
      int positionXY = 20*(fObjStack.GetEntriesFast());
      configureNewName();
      padO = (TPad *) new TCanvas(fNameCurrent, fNameCurrent, positionXY, positionXY, padSizeX, padSizeY);
    }
    fObject = padO;
    pad = padO;
  }

  if (fDaughterECanvasArray!=nullptr)
  {
    kr_info(0) << "Drawing ecanvas " << fNameCurrent << " containing " << numDaughters << " pads." << endl;
    dividePad(numDivX, numDivY);

    for (auto iBranch=1; iBranch<=numDaughters; ++iBranch) {
      auto becvs = (ecanvas *) fDaughterECanvasArray -> At(iBranch);
      becvs -> setPad((TPad *)(padO -> cd(iBranch)));
      becvs -> draw();
    }
  }
  else {
    padO -> cd();
    applyPadMargin(padO);

    if (fGridX) padO -> SetGridx();
    if (fGridY) padO -> SetGridy();

    if (fLogX) padO -> SetLogx();
    if (fLogY) padO -> SetLogy();
    if (fLogZ) padO -> SetLogz();

    auto numDrawings = fDrawingArray -> GetEntriesFast();

    bool sumAll2DHistograms = true;

    // preDraw ========================================================
    for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
      auto drawing = (edrawing *) fDrawingArray -> At(idrawing);
      drawing -> preDraw();
      if (drawing -> InheritsFrom(ehist::Class())) {
        if (!((ehist *) drawing) -> histIs2D())
          sumAll2DHistograms = false;
      }
    }

    fEHistMain -> clear();

    // frame ========================================================
    if (numDrawings!=1) {
      if (!sumAll2DHistograms) {
        //getFrame() -> draw();
      }
    }


    // legend =======================================================
    auto legend = new elegend(fOptionString.getData());
    legend -> setECanvas(this,0);

    // main drawings ================================================
    for (bool drawHist2D : {1,0})
    {
      for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
        auto drawing = (edrawing *) fDrawingArray -> At(idrawing);
        if (drawing->isNull())
          continue;

        bool isHist2D = false;
        if (drawing -> InheritsFrom(ehist::Class()))
          isHist2D = ((ehist *) drawing) -> histIs2D();

        if (drawHist2D && isHist2D) {
          auto drawHist = (ehist *) drawing;
          if (fEHistMain->isNull()) { fEHistMain -> setDrawing(drawHist); } // -> getHist()); }
          else                      { fEHistMain -> addDrawing(drawHist); } // -> getHist()); }
        }
        else if (!drawHist2D && !isHist2D) {
          //kb_debug << drawing -> GetName() << " " << drawing -> getOption() << endl;
          drawing -> draw();
          legend -> add(drawing);
        }
      }

      if (sumAll2DHistograms && drawHist2D && !fEHistMain->isNull()) {
        fEHistMain -> preDraw();
        fEHistMain -> draw();
      }
    }

    legend -> preDraw();
    legend -> draw();

    // postDraw ========================================================
    for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
      auto drawing = (edrawing *) fDrawingArray -> At(idrawing);
      drawing -> postDraw();
    }
    legend -> postDraw();
  }

  fObjStack.Add(padO);
}

void ecanvas::draw(TTree *tree)
{
  make(tree);
  draw();
}

void ecanvas::setTag(const char *tag)
{
  fNameCurrent = fName + "_" + tag;

  auto numDrawings = fDrawingArray -> GetEntriesFast();
  for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
    auto drawing = (edrawing *) fDrawingArray -> At(idrawing);
    drawing -> setTag(tag);
  }

  if (fDaughterECanvasArray==nullptr)
    return;

  auto numDaughters = fDaughterECanvasArray -> GetEntries();
  for (auto iBranch=1; iBranch<=numDaughters; ++iBranch) {
    auto becvs = (ecanvas *) fDaughterECanvasArray -> At(iBranch);

    numDrawings = becvs -> getNumDrawings();
    for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
      auto drawing = (edrawing *) becvs -> getDrawing(idrawing);
      drawing -> setTag(tag);
    }
  }
}

void ecanvas::setPar(const char *name, const char *val)
{
  auto numDrawings = fDrawingArray -> GetEntriesFast();
  for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
    auto drawing = (edrawing *) fDrawingArray -> At(idrawing);
    drawing -> setPar(name, val);
  }

  if (fDaughterECanvasArray==nullptr)
    return;

  auto numDaughters = fDaughterECanvasArray -> GetEntries();
  for (auto iBranch=1; iBranch<=numDaughters; ++iBranch) {
    auto becvs = (ecanvas *) fDaughterECanvasArray -> At(iBranch);

    numDrawings = becvs -> getNumDrawings();
    for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
      auto drawing = (edrawing *) becvs -> getDrawing(idrawing);
      drawing -> setPar(name, val);
    }
  }
}

void ecanvas::setPar(const char *name, double val)
{
  auto numDrawings = fDrawingArray -> GetEntriesFast();
  for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
    auto drawing = (edrawing *) fDrawingArray -> At(idrawing);
    drawing -> setPar(name, val);
  }

  if (fDaughterECanvasArray==nullptr)
    return;

  auto numDaughters = fDaughterECanvasArray -> GetEntries();
  for (auto iBranch=1; iBranch<=numDaughters; ++iBranch) {
    auto becvs = (ecanvas *) fDaughterECanvasArray -> At(iBranch);

    numDrawings = becvs -> getNumDrawings();
    for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
      auto drawing = (edrawing *) becvs -> getDrawing(idrawing);
      drawing -> setPar(name, val);
    }
  }
}

void ecanvas::make(TTree *tree)
{
  auto numDrawings = fDrawingArray -> GetEntriesFast();
  for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
    auto drawing = (edrawing *) fDrawingArray -> At(idrawing);
    drawing -> make(tree);
  }

  if (fDaughterECanvasArray==nullptr)
    return;

  auto numDaughters = fDaughterECanvasArray -> GetEntries();
  for (auto iBranch=1; iBranch<=numDaughters; ++iBranch) {
    auto becvs = (ecanvas *) fDaughterECanvasArray -> At(iBranch);

    numDrawings = becvs -> getNumDrawings();
    for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
      auto drawing = (edrawing *) becvs -> getDrawing(idrawing);
      drawing -> make();
      drawing -> make(tree);
    }
  }
}

void ecanvas::setPadSizeFull(int dx, int dy, bool fixSize) {
  fPadSizeSingleX = -dx;
  fPadSizeSingleY = -dy;
  fFixPadSize = fixSize;
}

void ecanvas::setPadSizeSingle(int dx, int dy, bool fixSize) {
  fPadSizeSingleX = (dx<=0) ? 10000 : dx;
  fPadSizeSingleY = (dy<=0) ? 10000 : dy;
  fFixPadSize = fixSize;
}

void ecanvas::setPadSizeMax(int xmax, int ymax, bool fixSize) {
  fPadSizeMaxX = (xmax<=0) ? ymax/(5./6.) : xmax;
  fPadSizeMaxY = (ymax<=0) ? xmax*(5./6.) : ymax;
  fFixPadSize = fixSize;
}

void ecanvas::setPad(TPad *pad) {
  fObject = pad;
}

//void ecanvas::setPadMargin(int option) { }

void ecanvas::setPadMargin(double lMargin, double rMargin, double bMargin, double tMargin) {
  fLMargin = lMargin;
  fRMargin = rMargin;
  fBMargin = bMargin;
  fTMargin = tMargin;
}

void ecanvas::applyPadMargin(TPad *pad) {
  if (pad==nullptr) pad = (TPad *) fObject;
  if (pad!=nullptr) {
    pad -> SetMargin(fLMargin, fRMargin, fBMargin, fTMargin);
    pad -> Modified();
    pad -> Update();
  }
}

void ecanvas::getNextPadInfo(int numPads, int &numDivX, int &numDivY, int &padSizeX, int &padSizeY)
{
  numDivX = fNumDivXY[numPads][0];
  numDivY = fNumDivXY[numPads][1];

  auto padSizeFullX = fPadSizeSingleX*numDivX;
  auto padSizeFullY = fPadSizeSingleY*numDivY;

  if (fPadSizeSingleX<0) {
    padSizeFullX = -fPadSizeSingleX;
    padSizeFullY = -fPadSizeSingleY;
    if (fFixPadSize)
      return;
  }

  Int_t dummyX, dummryY;
  UInt_t mornitorSizeX=2000, mornitorSizeY=1500;
  auto cvsdummy = new TCanvas("cvsdummy");
  // TODO @todo below doesn't work correctly when two or more displays are used! The mornitor width get summed up???
  gVirtualX -> GetWindowSize(gVirtualX->GetDefaultRootWindow(),dummyX,dummryY,mornitorSizeX,mornitorSizeY);
  //gVirtualX -> GetWindowSize(gClient->GetRoot()->GetId(),dummyX,dummryY,mornitorSizeX,mornitorSizeY);
  delete cvsdummy;
  if (mornitorSizeX>fPadSizeMaxX) mornitorSizeX = fPadSizeMaxX;
  if (mornitorSizeY>fPadSizeMaxY) mornitorSizeY = fPadSizeMaxY;

  auto padSizeScaledX = padSizeFullX;
  if (!fFixPadSize) while (padSizeScaledX>mornitorSizeX) padSizeScaledX -= 10;

  auto padSizeScaledY = double(padSizeScaledX) * padSizeFullY / padSizeFullX;
  if (!fFixPadSize) while (padSizeScaledY>mornitorSizeY) padSizeScaledY -= 10;

  padSizeX = padSizeScaledX;
  padSizeY = padSizeScaledY;
}

void ecanvas::dividePad(int numDivX, int numDivY, double xMargin, double yMargin)
{
  getPad() -> Divide(numDivX,numDivY,xMargin,yMargin);
  for (auto iBranch=1; iBranch<=(numDivX*numDivY); ++iBranch) {
    auto pad = (TPad *)(getPad() -> cd(iBranch));
  }
}

edrawing *ecanvas::getFrame()
{
  {
    double y1Frame = DBL_MAX;
    double y2Frame = -DBL_MAX;
    double x1Frame = DBL_MAX;
    double x2Frame = -DBL_MAX;
    auto numDrawings = fDrawingArray -> GetEntriesFast();
    for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
      auto drawing = (edrawing *) fDrawingArray -> At(idrawing);
      if (drawing->isNull())
        continue;
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
    TString frameOption;
    if (fSetStats==false)
      frameOption = "stats=0,draw=hist";
    else
      frameOption = "draw=hist";

    fEHistMain = new ehist(fNameCurrent+"_frame","",200,x1Frame,x2Frame,200,y1Frame,y2Frame,frameOption);
  }

  fEHistMain -> make();
  return fEHistMain;
}

const char *ecanvas::print(bool printout) const
{
  TString val = Form("ecanvas with %d daughter pads, # of drawings: (",fDaughterECanvasArray -> GetEntries());

  auto numBranches = 0;
  if (fDaughterECanvasArray!=nullptr)
    numBranches = fDaughterECanvasArray -> GetEntries();

  if (numBranches==0)
    val += Form("%d)", fDrawingArray -> GetEntriesFast());
  else {
    for (auto iBranch=1; iBranch<=numBranches; ++iBranch) {
      auto ecvs2 = (ecanvas*) fDaughterECanvasArray -> At(iBranch);
      if (iBranch==numBranches-1) val += Form("%d)",  ecvs2 -> getNumDrawings());
      else                        val += Form("%d, ", ecvs2 -> getNumDrawings());
    }
  }

  if (printout)
    kr_info(0) << val << endl;

  return val;
}

void ecanvas::printDrawings() const
{
  auto numDrawings = fDrawingArray -> GetEntriesFast();
  for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
    auto drawing = (edrawing *) fDrawingArray -> At(idrawing);
    drawing -> print();
  }

  if (fDaughterECanvasArray==nullptr)
    return;

  auto numDaughters = fDaughterECanvasArray -> GetEntries();
  for (auto iBranch=1; iBranch<=numDaughters; ++iBranch) {
    auto becvs = (ecanvas *) fDaughterECanvasArray -> At(iBranch);

    numDrawings = becvs -> getNumDrawings();
    for (auto idrawing=0; idrawing<numDrawings; ++idrawing) {
      auto drawing = (edrawing *) becvs -> getDrawing(idrawing);
      drawing -> print();
    }
  }
}

void ecanvas::applyOptions(TString option)
{
  auto opts = eoption(option);

  if (opts.findOption("gridx")) fGridX = opts.getValueB();
  if (opts.findOption("gridy")) fGridY = opts.getValueB();

  if (opts.findOption("logx")) fLogX = opts.getValueB();
  if (opts.findOption("logy")) fLogY = opts.getValueB();
  if (opts.findOption("logz")) fLogZ = opts.getValueB();

  if (fOptionString.findOption("stats")) fSetStats = fOptionString.getValueB();
}

void ecanvas::setRMarginForLegend(double dxLegend) { fRMargin = dxLegend; applyPadMargin(); }
void ecanvas::setTMarginForLegend(double dyLegend) { fTMargin = dyLegend; applyPadMargin(); }
