#ifndef __ehist_hh__
#define __ehist_hh__

#include "efunc.hh"
#include "edrawing.hh"
#include "ebinning.hh"

#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCut.h"
#include "TF1.h"
#include "TTask.h"

class tfitGaus;

class ehist : public edrawing
{
  protected:
    TString   fExpression;
    TCut      fSelection;
    ebinning  fBinnx;
    ebinning  fBinny;
    efunc     *fEFit = nullptr;

    bool fHistIs2D = false;

    bool fUserXRangeIsSet = false;
    bool fUserYRangeIsSet = false;
    bool fUserZRangeIsSet = false;
    double fUserXMax = 0;
    double fUserXMin = 0;
    double fUserYMax = 0;
    double fUserYMin = 0;
    double fUserZMax = 0;
    double fUserZMin = 0;

    double fXTitleOffset = 1.3;
    double fYTitleOffset = 1.7;
    double fZTitleOffset = -0.2;
    double fXLabelOffset = 0.005;
    double fYLabelOffset = 0.005;
    double fZLabelOffset = 0;
    double fXTitleSize   = 0.05;
    double fYTitleSize   = 0.05;
    double fZTitleSize   = 0.05;
    double fXLabelSize   = 0.04;
    double fYLabelSize   = 0.04;
    double fZLabelSize   = 0.03;

    bool fSetStats = true;

  public:
    ehist() : edrawing() {}
    ehist(TH1 *hist, const char *opt);
    ehist(ehist *hist1, ehist *hist2, const char *opt);

    ehist(const char *name, const char *ttl, const char *expr, const char *sel, ebinning binnx, ebinning binny, const char *opt);

    ehist(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, const char *opt="") 
    : ehist(name, ttl, expr, sel, ebinning(nx,x1,x2), ebinning(), opt) {}

    ehist(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, int ny, double y1, double y2, const char *opt="") 
    : ehist(name, ttl, expr, sel, ebinning(nx,x1,x2), ebinning(ny,y1,y2), opt) {}

    ehist(const char *name, const char *ttl, int nx, double x1, double x2, const char *opt="") 
    : ehist(name, ttl, name, "", ebinning(nx,x1,x2), ebinning(), opt) {}

    ehist(const char *name, const char *ttl, int nx, double x1, double x2, int ny, double y1, double y2, const char *opt="") 
    : ehist(name, ttl, name, "", ebinning(nx,x1,x2), ebinning(ny,y1,y2), opt) {}

    void init();

    // edraw scope. MUST BE IMPLEMENTED FROM INHERITED CLASSES
    virtual void configureParameters();
    //virtual void configureBasicOption();
    virtual void configureDrawOption();
    virtual void configureRange();
    virtual void configureAttributes();

    virtual void setECanvas(ecanvas *ecvs, int idxPad);

    virtual const char *print(bool printout=true) const;

    // ecanvas scope
    virtual void preDraw();

    TH1 *getHist() const { return (TH1 *) fObject; }

    void setFit(TF1 *fit) { fEFit -> setFunction(fit); }

    TString  getName()       const { return fName; }
    TString  getExpression() const { return fExpression; }
    TCut     getSelection()  const { return fSelection; }
    TString  getTitle()      const { return fTitle; }
    ebinning getBinnx()      const { return fBinnx; }
    ebinning getBinny()      const { return fBinny; }

    void getTitles(TString &ttlm, TString &ttlx, TString &ttly, TString &ttlz);

    void setHistIs2D(bool val) { fHistIs2D = val; }
    bool histIs2D() { return fHistIs2D; }

    void setName       (TString  val) { fName = val; }
    void setExpression (TString  val) { fExpression = val; }
    void setSelection  (TCut     val) { fSelection = val; }
    void setTitle      (TString  val) { fTitle = val; }
    void setBinnx      (ebinning val) { fBinnx = val; }
    void setBinny      (ebinning val) { fBinny = val; }

    void setDrawing(TObject *obj);
    void addDrawing(TObject *obj);

    void setTitle      (TString  val, int ttlIndex);
    void setMainTitle  (TString  val) { setTitle(val, 0); }
    void setXTitle     (TString  val) { setTitle(val, 1); }
    void setYTitle     (TString  val) { setTitle(val, 2); }
    void setZTitle     (TString  val) { setTitle(val, 3); }

    void make(TTree *tree=nullptr);

    ehist *copy(TString name) const;

    void fixPar(const char *name, double val);
    void fixPar(const char *name, const char *val);

    void addTask(TString taskName, double par=0, TString topt="", TString dopt="");

    ClassDef(ehist,0)
};

class tfitGaus : public TTask
{
  private:
    ehist *fEHist = nullptr;
    double fFitRangeInSigma = 2.5;
    TString fFitOption = "RQ0";

  public:
    tfitGaus(ehist *hist, double fitRangeInSigma=0, TString fitOption="")
    : TTask("tfitGaus","fit histogram sith gaussian") {
      if (fitRangeInSigma!=0) fFitRangeInSigma = fitRangeInSigma;
      if (fitOption!="") fFitOption = fitOption;
      fEHist = hist;
    }

    void Exec(Option_t *) { fit(fEHist->getHist()); }

    void fit(TH1 *hist, double fitRangeInSigma=0, TString fitOption="");
};

#endif
