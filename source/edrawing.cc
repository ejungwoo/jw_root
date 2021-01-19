#include "ecanvas.hh"
#include "edrawing.hh"

#include "TSystem.h"

ClassImp(edrawing)

edrawing::edrawing(TObject *obj, TString option)
: fObject(obj), fOptionString(option)
{
  init();
}

edrawing::edrawing(const char *name, const char *title, TObject *obj, TString option)
: TNamed(name, title), fObject(obj), fOptionString(option)
{
  init();
  fNameCurrent = fName;
}

void edrawing::init()
{
  if (fName.IsNull()) {
    if (fObject!=nullptr) {
      SetName(fObject->GetName());
      if (fTitle.IsNull())
        SetTitle(fName);
    }
    if (fNameCurrent.IsNull())
      fNameCurrent = fName;
  }

  if (fTaskList==nullptr)
    fTaskList = new TTask(fName,"");

  configureParameters();
  configureBasicOption();
  configureDrawOption();
  if (fFindRange) configureRange();
  if (fApplyAttributes) configureAttributes();
}

void edrawing::clear()
{
  fObject = nullptr;
  fSortIndex = 0;

  fOptionString.clear();
}

TString edrawing::configureNewName() {
  if (fObjStack.FindObject(fNameCurrent)!=nullptr)
    fNameCurrent = Form("%s_v%d",fName.Data(),fObjStack.GetEntriesFast());
  return fNameCurrent;
}

void edrawing::setTag(const char *tag)
{
  fNameCurrent = fName + "_" + tag;
}

void edrawing::setOption(TString option)
{
  fOptionString.setOption(option);
  configureBasicOption();
  configureDrawOption();
}

void edrawing::configureBasicOption() {
  /**/ if (fOptionString.findOption("frame")) fSortIndex = 0;
  else if (fOptionString.findOption("sort")) fSortIndex = fOptionString.getValueI();
  else fSortIndex = 1;

  if (fOptionString.findOption("legend")) fAddToLegend     = fOptionString.getValueB();
  if (fOptionString.findOption("range") ) fFindRange       = fOptionString.getValueB();
  if (fOptionString.findOption("att")   ) fApplyAttributes = fOptionString.getValueB();

  if (fOptionString.findOption("legendttl")) fLegendTitle = fOptionString.getValue();

  fOptionString.getDrawOption();
}

void edrawing::setECanvas(ecanvas *ecvs, int idxPad) {
  fParentECanvas = ecvs;
  fECanvasPadIndex = idxPad;
}

const char *edrawing::print(bool printout) const {
  TString val = fName+"  "+fTitle+"  "+fOptionString.getData();
  if (printout) cout_info << val << endl;
  return val;
}

void edrawing::draw()
{
  if (fObject!=nullptr) {
    kb_debug << fNameCurrent << " " << fOptionString.getData() << endl;
    fObject -> Draw(fOptionString.getDrawOption());
  }
}

void edrawing::addToLegend(TLegend *legend, TString title, TString option)
{
  if (fObject==nullptr)
    return;
  if (fAddToLegend) {
    //if (title.IsNull()) title = fObject -> GetTitle();
    if (title.IsNull()) title = fLegendTitle;
    if (option.IsNull()) option = fOptionString.getDrawOption();
    if (option.Index("same")>=0) option.ReplaceAll("same","");
    if (option.Index("hist")>=0) option.ReplaceAll("hist","l");
    legend -> AddEntry(fObject, title.Data(), option.Data());
  }
}

// TObject scope
Int_t edrawing::Compare(const TObject *obj2) const
{
  auto drawing2 = (edrawing *) obj2;
  auto sidx2 = drawing2 -> getSortIndex();
  if (fSortIndex < sidx2) return -1;
  if (fSortIndex > sidx2) return 1;
  return 0;
}

void edrawing::setPar(const char *name, double val)
{
  auto par = (TParameter<Double_t> *) fParameters.FindObject(name);
  if (par==nullptr) {
    par = new TParameter<Double_t>(name, val);
    fParameters.Add(par);
  } else
    par -> SetVal(val);
}

void edrawing::setPar(const char *name, const char *val)
{
  auto par = (TNamed *) fParameters.FindObject(name);
  if (par==nullptr) {
    par = new TNamed(name, val);
    fParameters.Add(par);
  } else
    par -> SetTitle(val);
}

void edrawing::save(const char *format, const char *dirName)
{
  TString dirName0 = dirName;
  if(fObject==nullptr)
    return;

  TString fullName = fNameCurrent  + "." + format;
  gSystem -> Exec(TString("mkdir -p ")+dirName0);
  if (!dirName0.EndsWith("/"))
    dirName0 += "/";

  fObject -> SaveAs(dirName0 + fullName);
}
