#include "KBGlobal.hh"
#include "elegend.hh"
#include "TPad.h"
#include "TLegendEntry.h"
#include <iostream>
using namespace std;

ClassImp(elegend)

elegend::elegend(TObject *obj, TString option)
: edrawing(obj, option)
{
  if (fObject==nullptr)
    fObject = new TLegend();
}

void elegend::add(TObject *entry, TString name, TString option)
{
  auto legend = (TLegend *) fObject;
  legend -> AddEntry(entry, name, option);
}

void elegend::setLegendAlign(int val) { fLegendAlign = val; }
void elegend::setLegendOffset(double xOffset, double yOffset) { fXOffset = xOffset; fYOffset = yOffset; }

void elegend::configureParameters() {
  //auto man = emanager::manager();
  //fLMargin = man -> fLMargin;
  //fRMargin = man -> fRMargin;
  //fBMargin = man -> fBMargin;
  //fTMargin = man -> fTMargin;
  //fWidthPerLengthLegend = man -> fWidthPerLengthLegend;
  //fLegendHeightPerEntry = man -> fLegendHeightPerEntry;
}

void elegend::configureBasicOption() {}
void elegend::configureDrawOption() {}
void elegend::configureRange() { fFindRange = false; }

void elegend::configureAttributes() {
  auto legend = (TLegend *) fObject;
  legend -> SetX1(0.60);
  legend -> SetX2(1.-fRMargin);
  legend -> SetY1(0.7);
  legend -> SetY2(1.-fTMargin);
  legend -> SetFillStyle(0);
}
void elegend::actionBeforeDraw() { findLegendSizeAndPosition(); }

void elegend::findLegendSizeAndPosition()
{
  TPad *cvs = nullptr;
  //if (fParentECanvas!=nullptr)
    //cvs = fParentECanvas -> getCanvas();

  auto legend = (TLegend *) fObject;

  auto x1Frame = 0. + (cvs==nullptr?fLMargin:cvs->GetLeftMargin());
  auto x2Frame = 1. - (cvs==nullptr?fRMargin:cvs->GetRightMargin());
  auto y1Frame = 0. + (cvs==nullptr?fBMargin:cvs->GetBottomMargin());
  auto y2Frame = 1. - (cvs==nullptr?fTMargin:cvs->GetTopMargin());
  auto xUnit = x2Frame - x1Frame;
  auto yUnit = y2Frame - y1Frame;

  auto lTextMax = 0;
  TIter next_entry(legend->GetListOfPrimitives());
  while (TLegendEntry *le=(TLegendEntry*)next_entry()) {
    auto lText = TString(le->GetLabel()).Length();
    if (lTextMax<lText) lTextMax = lText;
  }
  auto dxLegend = lTextMax * fWidthPerLengthLegend + 0.18 * xUnit;
  auto dyLegend = legend -> GetNRows() * fLegendHeightPerEntry;

  int xAlign = fLegendAlign/10;
  int yAlign = fLegendAlign - 10*xAlign;

  double x1Legend = x2Frame;
  double y1Legend = y2Frame;
  if (xAlign!=9) x1Legend = (1-xAlign/8.)*x1Frame + (xAlign/8.)*(x2Frame-dxLegend);
  //else if (fParentECanvas!=nullptr) fParentECanvas -> setRMarginForLegend(dxLegend);
  if (yAlign!=9) y1Legend = (1-yAlign/8.)*y1Frame + (yAlign/8.)*(y2Frame-dyLegend);
  //else if (fParentECanvas!=nullptr) fParentECanvas -> setTMarginForLegend(dyLegend);
  double x2Legend = x1Legend + dxLegend;
  double y2Legend = y1Legend + dyLegend;

  legend -> SetX1(x1Legend + fXOffset*xUnit);
  legend -> SetX2(x2Legend + fXOffset*xUnit);
  legend -> SetY1(y1Legend + fYOffset*yUnit);
  legend -> SetY2(y2Legend + fYOffset*yUnit);
  legend -> SetFillStyle(fFillStyleLegend);
  legend -> SetBorderSize(fBorderSizeLegend);

  //kb_debug << x1Legend + fXOffset*xUnit << " " << x2Legend + fXOffset*xUnit << " " << y1Legend + fYOffset*yUnit << " " << y2Legend + fYOffset*yUnit
  //  << " " << fFillStyleLegend << " " << fBorderSizeLegend << endl;
}
