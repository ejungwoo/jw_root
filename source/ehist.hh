#ifndef __ehist_hh__
#define __ehist_hh__

#include "edrawing.hh"
#include "ebinning.hh"

#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCut.h"

class ehist : public edrawing
{
  protected:
    TString   fExpression;
    TCut      fSelection;
    ebinning  fBinnx;
    ebinning  fBinny;
    TObjArray fHistArray;

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

  private:
    TString getNewHistName();

  public:
    ehist(TH1 *hist, const char *option);
    ehist(const char *name, const char *ttl, const char *expr, const char *sel, ebinning binnx, ebinning binny, const char *opt);
    ehist(const char *name, const char *ttl, const char *expr, const char *sel, int nx, double x1, double x2, int ny=0, double y1=0, double y2=0, const char *opt="")
    : ehist(name, ttl, expr, sel, ebinning(nx,x1,x2), ebinning(ny,y1,y2), opt) {}
    ehist(const char *name, const char *ttl, int nx=0, double x1=0, double x2=0, int ny=0, double y1=0, double y2=0)
    : ehist(name, ttl, name, "", ebinning(nx,x1,x2), ebinning(ny,y1,y2), "") {}

    // edraw scope. MUST BE IMPLEMENTED FROM INHERITED CLASSES
    virtual void configureParameters();
    //virtual void configureBasicOption();
    virtual void configureDrawOption();
    virtual void configureRange();
    virtual void configureAttributes();

    virtual const char *print(bool printout=true) const;

    // ecanvas scope
    virtual void actionBeforeDraw();
    virtual void actionAfterDraw() {}

    TString  getName()       const { return fName; }
    TString  getExpression() const { return fExpression; }
    TCut     getCut()        const { return fSelection; }
    TString  getTitle()      const { return fTitle; }
    ebinning getBinnx()      const { return fBinnx; }
    ebinning getBinny()      const { return fBinny; }

    void setName       (TString  val) { fName = val; }
    void setExpression (TString  val) { fExpression = val; }
    void setCut        (TCut     val) { fSelection = val; }
    void setTitle      (TString  val) { fTitle = val; }
    void setBinnx      (ebinning val) { fBinnx = val; }
    void setBinny      (ebinning val) { fBinny = val; }

    TH1 *make(TTree *tree=nullptr);
};

#endif
