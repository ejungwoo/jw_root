#ifndef __edrawing_hh__
#define __edrawing_hh__

#include "TNamed.h"
#include "TLegend.h"
#include "TObject.h"
#include "TTree.h"
#include "TParameter.h"
#include "TTask.h"

class ecanvas;
#include "eoption.hh"

#include "KBGlobal.hh"
#include <iostream>
using namespace std;

class edrawing : public TNamed
{
  protected:
    TString fNameCurrent;
    TString fLegendTitle;
    TObject *fObject = nullptr;
    int fSortIndex = 0;

    eoption fOptionString;
    bool fAddToLegend = false;
    bool fFindRange = true;
    bool fApplyAttributes = true;

    double fX1Range = -999;
    double fX2Range = -999;
    double fY1Range = -999;
    double fY2Range = -999;

    ecanvas *fParentECanvas = nullptr;
    int fECanvasPadIndex = 0;

    TObjArray fObjStack;
    TObjArray fParameters;

    TTask *fTaskList = nullptr;

  public:
    edrawing() {}
    edrawing(TObject *obj, TString option="");
    edrawing(const char *name, const char *title="", TObject *obj=nullptr, TString option="");

    virtual void init();
    virtual void clear();

    virtual TString configureNewName();
    virtual void setTag(const char *tag);
            void setName(const char *name) { fNameCurrent = name; }

    bool isNull() const { if (fObject==nullptr) return true; return false; }

    /// set options : rank, legend, range, att
    /// @param[in] option following \ref eoption option format
    ///   - frame : the object is set as frame
    ///   - sort=value : the object is sorted as (value)th drawing. default sort is 1.
    ///   - legend=1/0 : add to legend
    ///   - range=1/0 : find range
    ///   - att=1/0 : set automatic attribute to the object
    void setOption(TString option);
    void clearOption(TString option="") { fOptionString.clear(); }

    void setSortIndex(int idx) { fSortIndex = idx; }

    virtual void setDrawing(TObject *obj) { fObject = obj; init(); }
    virtual void addDrawing(TObject *obj) {}

    TString  getCurrentName()     const { return fNameCurrent; }
    TObject *getObject()          const { return fObject; }
    TString  getOption()          const { return fOptionString.getData(); }
    ecanvas *getParentECanvas()   const { return fParentECanvas; }
    int      getPadIndex()        const { return fECanvasPadIndex; }
    int      getSortIndex()       const { return fSortIndex; }
    double   getX1Range()         const { return fX1Range; }
    double   getX2Range()         const { return fX2Range; }
    double   getY1Range()         const { return fY1Range; }
    double   getY2Range()         const { return fY2Range; }
    bool     getAddToLegend()     const { return fAddToLegend; }
    bool     getFindRange()       const { return fFindRange; }
    bool     getApplyAttributes() const { return fApplyAttributes; }

    // edraw scope. MUST BE IMPLEMENTED FROM INHERITED CLASSES
    virtual void configureParameters() {} ///< Will be called from init(). Configure parameters.
    virtual void configureBasicOption();  ///< Will be called from init(). Configure options from fOptionString.
    virtual void configureDrawOption() {}
    virtual void configureRange() {}
    virtual void configureAttributes() {}

    // ecanvas scope
    virtual void setECanvas(ecanvas *ecvs, int idxPad);
    virtual const char *print(bool printout=true) const;
    virtual void draw();
    virtual void preDraw() {}
    virtual void postDraw() {}
    virtual void addToLegend(TLegend *legend, TString title="", TString option="");

    // TObject scope
    virtual void Print(Option_t *) const { print(true); }
    Bool_t IsSortable() const { return true; }
    virtual Int_t Compare(const TObject *obj2) const;

    // make
    virtual void make(TTree *tree=nullptr) {}
    virtual void save(const char *format="root", const char *dirName="data");

    virtual void setPar(const char *name, double val);
    virtual void setPar(const char *name, const char *val);
    virtual void fixPar(const char *name, double val) {}
    virtual void fixPar(const char *name, const char *val) {}

    virtual void addTask(TTask *task) { fTaskList -> Add(task); }
    virtual void addTask(TString taskName, double par=0, TString topt="", TString dopt="") {}

    ClassDef(edrawing,0);
};

#endif
