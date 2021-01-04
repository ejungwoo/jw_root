#ifndef __ecanvas_hh__
#define __ecanvas_hh__

#include "TObjArray.h"
#include "TCanvas.h"
#include "TPad.h"

#include "ehist.hh"
#include "edrawing.hh"

class ecanvas : public TObjArray
{
  private:
    ecanvas *fParentECanvas = nullptr;
    int fCurrentDaughterIndex = 0;
    int fNumDaughters = 0;
    TObjArray *fDaughterECanvasArray = nullptr;
    TPad *fPad = nullptr;
    edrawing *fFrame = nullptr;

    int fLegendAlign = 88;
    bool fGridX = 0;
    bool fGridY = 0;
    bool fLogX = 0;
    bool fLogY = 0;
    bool fLogZ = 0;
    double fLMargin = 0.19;
    double fRMargin = 0.155;
    double fBMargin = 0.17;
    double fTMargin = 0.18;

    int fNumDivXY[81][2] = {
      {1,1},{1,1},{2,1},{3,1},
      {2,2},{3,2},{3,2},{4,2},
      {4,2},{3,3},{5,2},{4,3},
      {4,3},{5,3},{5,3},{5,3},
      {4,4},{6,3},{6,3},{5,4},{5,4},
      {6,4},{6,4},{6,4},{6,4},
      {6,5},{6,5},{6,5},{6,5},{6,5},{6,5},
      {7,5},{7,5},{7,5},{7,5},{7,5},
      {7,6},{7,6},{7,6},{7,6},{7,6},{7,6},{7,6},
      {8,6},{8,6},{8,6},{8,6},{8,6},{8,6},
      {9,6},{9,6},{9,6},{9,6},{9,6},{9,6},
      {9,7},{9,7},{9,7},{9,7},{9,7},{9,7},{9,7},{9,7},{9,7},
      {9,8},{9,8},{9,8},{9,8},{9,8},{9,8},{9,8},{9,8},{9,8},
      {10,8},{10,8},{10,8},{10,8},{10,8},{10,8},{10,8},{10,8}
    };

  public:
    ecanvas(const char *name="");
    ecanvas(TPad *pad);

    edrawing *addDrawing (int idxPad, TObject *obj, TString option="", bool isFrame=false);

    edrawing *addFrame   (int idxPad, TObject *obj, TString option="") { return addDrawing(idxPad, obj, option, true); }
    edrawing *add        (int idxPad, TObject *obj, TString option="") { return addDrawing(idxPad, obj, option, false); }

    edrawing *addFrame   (TObject *obj, TString option="") { return addFrame( 0,obj,    option); }
    edrawing *add        (TObject *obj, TString option="") { return add     ( 0,obj,    option); }
    edrawing *addsame    (TObject *obj, TString option="") { return add     (-1,obj,    option); }
    edrawing *addnext    (TObject *obj, TString option="") { return add     (-2,obj,    option); }

    TPad *draw(TPad *pad=nullptr);

  public:
    void setParentCanvas(ecanvas *ecvs) { fParentECanvas = ecvs; }
    ecanvas *setParentCanvas() { return fParentECanvas; }
    TPad *getCanvas() { return fPad; }

  public:
    void setPad(TPad *pad);
    void setPadMargin(TPad *pad=nullptr);
    void getNextPadInfo(int numPads, int &numDivX, int &numDivY, int &padSizeX, int &padSizeY);
    void dividePad(int numDivX, int numDivY, double xMargin=0.001, double yMargin=0.001);
    edrawing *getFrame();

    virtual const char *print(bool printout=true) const;

  private:
    edrawing *makeDrawing (TObject *obj, TString option);

    void findOptions(TString option);
    void setRMarginForLegend(double dxLegend);
    void setTMarginForLegend(double dyLegend);

    static int fIndexCanvas;

    ClassDef(ecanvas,0)
};

#endif
