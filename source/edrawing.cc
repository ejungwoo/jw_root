#include "KBGlobal.hh"
#include <iostream>
using namespace std;
#include "edrawing.hh"

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
}

void edrawing::init() {
  configureParameters();
  configureBasicOption();
  configureDrawOption();
  if (fFindRange) configureRange();
  if (fApplyAttributes) configureAttributes();
}

void edrawing::setOption(TString option) { fOptionString = estring(option); init(); }
void edrawing::addOption(TString option) { fOptionString = estring(option); init(); }

void edrawing::configureBasicOption() {
  /**/ if (fOptionString.findOption("frame")) fSortIndex = 0;
  else if (fOptionString.findOption("rank")) fSortIndex = fOptionString.getValueI();
  else fSortIndex = 1;

  if (fOptionString.findOption("legend")) fAddToLegend     = fOptionString.getValueB();
  if (fOptionString.findOption("range") ) fFindRange       = fOptionString.getValueB();
  if (fOptionString.findOption("att")   ) fApplyAttributes = fOptionString.getValueB();

  fDrawOption = fOptionString.getDrawOption();
}

const char *edrawing::print(bool printout) const {
  TString val = fName+"  "+fTitle;
  if (printout) cout_info << val << endl;
  return val;
}

void edrawing::draw()
{
  actionBeforeDraw();
  fObject -> Draw(fDrawOption);
  actionAfterDraw();
}

void edrawing::addToLegend(TLegend *legend, TString title, TString option)
{
  if (fAddToLegend) {
    if (title.IsNull()) title = fObject -> GetTitle();
    if (option.IsNull()) option = fDrawOption.Data();
    if (option.Index("same")>=0) option.ReplaceAll("same","");
    legend -> AddEntry(fObject, title.Data(), option.Data());
  }
}

// TObject scope
Int_t edrawing::Compare(TObject *obj2) const
{
  auto drawing2 = (edrawing *) obj2;
  auto sidx2 = drawing2 -> getSortIndex();
  if (fSortIndex < sidx2) return -1;
  if (fSortIndex > sidx2) return 1;
  return 0;
}
