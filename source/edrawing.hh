#ifndef __edrawing_hh__
#define __edrawing_hh__

#include "TNamed.h"
#include "TLegend.h"
#include "TObject.h"

#include "eoption.hh"

class edrawing : public TNamed
{
  protected:
    TObject *fObject;
    eoption fOptionString;
    int fPadIndex = -1;
    int fSortIndex = 0;
    bool fRanged = true;
    double fX1Range = -999;
    double fX2Range = -999;
    double fY1Range = -999;
    double fY2Range = -999;
    TString fDrawOption;

    bool fAddToLegend = true;
    bool fFindRange = true;
    bool fApplyAttributes = true;

  public:
    /// for option, see setOption()
    edrawing(TObject *obj, TString option="");
    edrawing(const char *name, const char *title, TObject *obj, TString option="");
    void init();

    /// set options : rank, legend, range, att
    /// @param[in] option following \ref eoption option format
    ///   - frame : the object is set as frame
    ///   - rank=value : the object is set as (value)th rank drawing. rank=0 is frame. default rank is 1
    ///   - legend=1/0 : add to legend
    ///   - range=1/0 : find range
    ///   - att=1/0 : set automatic attribute to the object
    void setOption(TString option);
    void addOption(TString option);
    void setPadIndex(int idx) { if (fPadIndex<0) fPadIndex = idx; }
    void setSortIndex(int idx) { fSortIndex = idx; }

    TObject *getObject() const { return fObject; }
    TString getOption() const { return fOptionString.getData(); }
    int getPadIndex() const { return fPadIndex; }
    int getSortIndex() const { return fSortIndex; }
    double getX1Range() const { return fX1Range; }
    double getX2Range() const { return fX2Range; }
    double getY1Range() const { return fY1Range; }
    double getY2Range() const { return fY2Range; }
    bool getAddToLegend() const { return fAddToLegend; }
    bool getFindRange() const { return fFindRange; }
    bool getApplyAttributes() const { return fApplyAttributes; }

    // edraw scope. MUST BE IMPLEMENTED FROM INHERITED CLASSES

    /// Will be called from init(). Configure parameters.
    virtual void configureParameters() {}
    /// Will be called from init(). Configure options from fOptionString.
    virtual void configureBasicOption();
    virtual void configureDrawOption() {}
    virtual void configureRange() {}
    virtual void configureAttributes() {}

    // ecanvas scope
    virtual void actionBeforeDraw() {}
    virtual void actionAfterDraw() {}

    virtual const char *print(bool printout=true) const;
    void draw();
    virtual void addToLegend(TLegend *legend, TString title="", TString option="");

    // TObject scope
    virtual void Print(Option_t *) const { print(true); }
    virtual Bool_t IsSortable()    const { return true; }
    virtual Int_t Compare(TObject *obj2) const;

    ClassDef(edrawing,0);
};

#endif
