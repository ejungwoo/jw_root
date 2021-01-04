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
  if (fOptionString.findOption("setstat")) fSetStats = fOptionString.getValueB();
  if (fDrawOption.IsNull()) fDrawOption = "hist";
  fDrawOption += " same";
}

void ehist::configureRange() {
  auto hist = (TH1 *) fObject;
  if (hist==nullptr)
    return;
  fX1Range = fBinnx.getMin();
  fX2Range = fBinnx.getMax();
  fY1Range = fBinny.getMin();
  fY2Range = fBinny.getMax();
}

void ehist::configureAttributes() {
  auto hist = (TH1 *) fObject;
  if (hist==nullptr)
    return;

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

  hist -> GetXaxis() -> SetNdivisions(506);
  hist -> GetYaxis() -> SetNdivisions(506);
  hist -> GetZaxis() -> SetNdivisions(506);
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

void ehist::setTitle(TString val, int ttlIndex)
{
  TString ttl0 = fTitle;
  TString ttls[4] = {"","","",""};

  if (ttl0.Index(";")<0)
    ttls[1] = ttl0;
  else {
    auto idxsc = 0;
    for (auto i=0; i<4; ++i) {
      idxsc = ttl0.Index(";");
      if (idxsc<0) {
        ttls[i] = ttl0;
        break;
      }
      ttls[i] = ttl0(0,idxsc);
      ttl0 = ttl0(idxsc+1,ttl0.Sizeof());
    }
  }

  ttls[ttlIndex] = val;
  fTitle = ttls[0] + ";" + ttls[1] + ";" + ttls[2] + ";" + ttls[3];
}

TH1 *ehist::make(TTree *tree)
{
  TH1 *hist = nullptr;
  auto name = getNewHistName();

  if (fBinnx.isNull()) fBinnx.make(tree,fName);

  if (fBinny.isNull()) hist = new TH1D(name,fTitle,fBinnx.getN(),fBinnx.getMin(),fBinnx.getMax());
  else                 hist = new TH2D(name,fTitle,fBinnx.getN(),fBinnx.getMin(),fBinnx.getMax(),fBinny.getN(),fBinny.getMin(),fBinny.getMax());

  if (tree!=nullptr) {
    TString exprFinal = fExpression;
    for (auto i=0; i<10; ++i) {
      TString parString = Form("[%d]",i);
      if (exprFinal.Index(parString)>0) {
        exprFinal.ReplaceAll(parString,Form("%f",fParameters[i]));
      }
    }
    tree -> Project(name,exprFinal,fSelection);
    if (fBinny.isNull())
      fBinny.set(1, hist->GetMinimum(), hist->GetMaximum());
  }

  fHistArray.Add(hist);
  fObject = hist;

  init();

  return hist;
}

const char *ehist::print(bool printout) const
{
  TString val = fName+",  "+fTitle+",  "+fOptionString.getData();
  val = val+",  "+fBinnx.print(0);
  if (!fBinny.isNull())
    val = val+",  "+fBinny.print(0);

  if (printout) cout_info << val << endl;
  return val;
}
