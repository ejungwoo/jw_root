#include "KBGlobal.hh"
#include "ehist.hh"
#include <iostream>
using namespace std;

ClassImp(ehist)

ehist::ehist(TH1 *hist, const char *option) : edrawing(hist, option)
{
  fName = hist -> GetName();
  fExpression = fName;
  fTitle = hist -> GetTitle();
}

ehist::ehist(const char *name, const char *title, const char *expression, const char *selection, ebinning binnx, ebinning binny, const char *option)
: edrawing(name, title, (TObject*) nullptr, option),
  fExpression(expression), fSelection(selection), fBinnx(binnx), fBinny(binny)
{
  if (fExpression.IsNull()) fExpression = fName;
}

void ehist::configureParameters() {
  fName = fObject -> GetName();
  fTitle = fObject -> GetTitle();
}

void ehist::configureDrawOption() {
  fDrawOption = fOptionString.getDrawOption();
  if (fOptionString.findOption("setStat")) fSetStats = fOptionString.getValueB();
  if (fDrawOption.IsNull()) fDrawOption = "hist";
  fDrawOption += " same";
}

void ehist::configureRange() {
  auto hist = (TH1 *) fObject;
  fX1Range = fBinnx.getMin();
  fX2Range = fBinnx.getMax();
  fY1Range = fBinny.getMin();
  fY2Range = fBinny.getMax();
}

void ehist::configureAttributes() {
  auto hist = (TH1 *) fObject;
  auto xaxis = hist -> GetXaxis();
  auto yaxis = hist -> GetYaxis();
  auto zaxis = hist -> GetZaxis();
  hist  -> SetLabelOffset (fXLabelOffset,"X");
  hist  -> SetLabelOffset (fYLabelOffset,"Y");
  hist  -> SetLabelOffset (fZLabelOffset,"Z");
  xaxis -> SetTitleOffset (fXTitleOffset);
  yaxis -> SetTitleOffset (fYTitleOffset); 
  zaxis -> SetTitleOffset (fZTitleOffset); 
  xaxis -> SetTitleSize   (fXTitleSize  ); 
  yaxis -> SetTitleSize   (fYTitleSize  ); 
  zaxis -> SetTitleSize   (fZTitleSize  ); 
  xaxis -> SetLabelSize   (fXLabelSize  ); 
  yaxis -> SetLabelSize   (fYLabelSize  ); 
  zaxis -> SetLabelSize   (fZLabelSize  ); 
}

TString ehist::getNewHistName() {
  TString histName = fName;
  TString histNameTest;
  int copyNo = 0;
  while (1) {
    if (copyNo==0) histNameTest = histName;
    else           histNameTest = histName+"_v"+copyNo;
    auto histFound = (TH1 *) fHistArray.FindObject(histNameTest);
    if (histFound==nullptr)
      break;
    copyNo++;
  }
  histName = histNameTest;
  return histName;
}

void ehist::actionBeforeDraw()
{
  if (fObject==nullptr)
    make();

  auto hist = (TH1 *) fObject;
  hist -> SetStats(fSetStats);
}

TH1 *ehist::make(TTree *tree)
{
  TH1 *hist = nullptr;
  auto name = getNewHistName();

  if (fBinnx.isNull()) fBinnx.make(tree,fName);

  if (fBinny.isNull()) hist = new TH1D(name,fTitle,fBinnx.getN(),fBinnx.getMin(),fBinnx.getMax());
  else                 hist = new TH2D(name,fTitle,fBinnx.getN(),fBinnx.getMin(),fBinnx.getMax(),fBinny.getN(),fBinny.getMin(),fBinny.getMax());

  if (tree!=nullptr) {
    tree -> Project(name,fExpression,fSelection);
    if (fBinny.isNull())
      fBinnx.set(1, hist->GetMinimum(), hist->GetMaximum());
  }

  fHistArray.Add(hist);
  fObject = hist;

  init();

  return hist;
}

const char *ehist::print(bool printout) const
{
  TString val = fName;
  if (!fTitle.IsNull()) val = val+"  "+GetTitle();
  val = val+"  "+fBinnx.print(false);
  if (!fBinny.isNull()) val = val+"  "+fBinny.print(false);

  if (printout) cout_info << val << endl;
  return val;
}
