#ifndef __ecanvas_hh__
#define __ecanvas_hh__

#include "TObjArray.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TTree.h"

#include "ehist.hh"
#include "edrawing.hh"

class ecanvas : public edrawing
{
  private:
    int        fCurrentDaughterIndex = 0;
    TObjArray *fDaughterECanvasArray = nullptr;
    TObjArray *fDrawingArray = nullptr;
    ehist     *fEHistMain = nullptr;

    bool fGridX = 0;
    bool fGridY = 0;
    bool fLogX = 0;
    bool fLogY = 0;
    bool fLogZ = 0;

    bool fSetStats = false;

    double fLMargin = 0.19;
    double fRMargin = 0.155;
    double fBMargin = 0.17;
    double fTMargin = 0.10;

    int  fPadSizeSingleX = 600;
    int  fPadSizeSingleY = 500;
    int  fPadSizeMaxX = 10000;
    int  fPadSizeMaxY = 10000;
    bool fFixPadSize = false;

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

    void init();

    ehist *histNext(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, int ny, double y1, double y2, const char *opt="");
    ehist *histNext(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, const char *opt="");

    ehist *histSame(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, int ny, double y1, double y2, const char *opt="");
    ehist *histSame(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, const char *opt="");

    edrawing *addDrawing (int idxPad, TObject *obj, TString option="", bool isFrame=false);
    edrawing *addFrame   (int idxPad, TObject *obj, TString option="") { return addDrawing(idxPad, obj, option, true); }
    edrawing *add        (int idxPad, TObject *obj, TString option="") { return addDrawing(idxPad, obj, option, false); }
    edrawing *addFrame   (TObject *obj, TString option="") { return addFrame( 0,obj,    option); }
    edrawing *add        (TObject *obj, TString option="") { return add     ( 0,obj,    option); }
    edrawing *addSame    (TObject *obj, TString option="") { return add     (-1,obj,    option); }
    edrawing *addNext    (TObject *obj, TString option="") { return add     (-2,obj,    option); }
    edrawing *addToAll   (TObject *obj, TString option="") { return add     (-9,obj,    option); }

    virtual void setTag(const char *tag);

    void make(TTree *tree=nullptr);

    void draw(TPad *pad=nullptr);
    void draw(TTree *tree);

    virtual void setPar(const char *name, double val);
    virtual void setPar(const char *name, const char *val);

  public:
    void setParentCanvas(ecanvas *ecvs) { fParentECanvas = ecvs; }
    ecanvas *setParentCanvas() { return fParentECanvas; }
    TPad *getPad() { return (TPad *) fObject; }

  public:
    //void setPadMargin(int option);
    void setPadMargin(double lMargin, double rMargin, double bMargin, double tMargin);
    void applyPadMargin(TPad *pad=nullptr);

    void setPadSizeFull(int dx, int dy, bool fixSize=false);
    void setPadSizeSingle(int dx, int dy=0, bool fixSize=false);
    void setPadSizeMax(int xmax, int ymax=0, bool fixSize=false);

    void setPad(TPad *pad);
    void getNextPadInfo(int numPads, int &numDivX, int &numDivY, int &padSizeX, int &padSizeY);
    void dividePad(int numDivX, int numDivY, double xMargin=0.001, double yMargin=0.001);
    edrawing *getFrame();

    virtual const char *print(bool printout=true) const;
    void printDrawings() const;

    edrawing *getDrawing(int i) const { return (edrawing *) fDrawingArray -> At(i); }
    int getNumDrawings() const { return fDrawingArray -> GetEntriesFast(); }

  private:
    edrawing *makeDrawing (TObject *obj, TString option);

    void applyOptions(TString option);
    void setRMarginForLegend(double dxLegend);
    void setTMarginForLegend(double dyLegend);

    TString configureNewName();

    ClassDef(ecanvas,0)
};

#endif
