#ifndef __efunc_hh__
#define __efunc_hh__

#include "edrawing.hh"
#include "ebinning.hh"

#include "TF1.h"

#include <vector>
using namespace std;

class tfitGaus;

class efunc : public edrawing
{
  private:
    bool fDrawGausInfoDrawings = false;
    bool fSetLegendGausInfo = false;

  public:
    efunc(TObject *obj, TString option) : edrawing(obj, option) {}
    efunc(const char *name, const char *title="", TString option="")
    : edrawing(name, title, (TObject *) nullptr, option) {}

    // edraw scope. MUST BE IMPLEMENTED FROM INHERITED CLASSES
    virtual void configureParameters() {}
    //virtual void configureBasicOption();
    virtual void configureDrawOption();
    virtual void configureRange();
    virtual void configureAttributes();

    // ecanvas scope
    virtual void postDraw();
    virtual void draw();
    void addToLegend(TLegend *legend, TString title="", TString option="");

    void setFunction(TF1 *func) { fObject = func; init(); }
    TF1 *getFunction() const  { return (TF1 *) fObject; }

    /// create drawings with guide lines at x = [guideCS[]] * sigma.
    void drawGausInfoDrawings(int rangeMaxInSigma=4, vector<int> guidPointInSigma={0,1});

    ClassDef(efunc,0)
};

#endif
