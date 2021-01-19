#include "KBGlobal.hh"
#include "TArrow.h"
#include "TLine.h"
#include "efunc.hh"
#include <iostream>
using namespace std;

ClassImp(efunc)

void efunc::configureDrawOption() {
  auto drawOption = fOptionString.getDrawOption();
  if (drawOption.IsNull()) fOptionString.setDrawOption("samel");

  if (fOptionString.findOption("gausInfo")) fDrawGausInfoDrawings = fOptionString.getValueB();
  if (fOptionString.findOption("gausLegend")) fSetLegendGausInfo = fOptionString.getValueB();
}

void efunc::configureRange() {
  auto f1 = (TF1 *) fObject;
  if (f1==nullptr)
    return;
  fX1Range = f1 -> GetXmin();
  fX2Range = f1 -> GetXmax();
  fY1Range = f1 -> GetMinimum();
  if (fY1Range<1.e-14) fY1Range = 0.;
  fY2Range = f1 -> GetMaximum();
}

void efunc::configureAttributes() {
  auto f1 = (TF1 *) fObject;
  if (f1==nullptr)
    return;
  f1 -> SetNpx(1000);
}

void efunc::draw()
{
  if (!fDrawGausInfoDrawings)
    edrawing::draw();
}
void efunc::postDraw()
{
  if (fDrawGausInfoDrawings)
    drawGausInfoDrawings();
}

void efunc::addToLegend(TLegend *legend, TString title, TString option)
{
  if (fObject!=nullptr)
    if (fAddToLegend) {
      if (title.IsNull()) title = fObject -> GetTitle();
      if (option.IsNull()) option = fOptionString.getDrawOption();
      if (option.Index("same")>=0) option.ReplaceAll("same","");
      //legend -> AddEntry(fObject, title.Data(), option.Data());
      legend -> AddEntry(fObject, fObject->GetName(), option.Data());
    }

  if (fSetLegendGausInfo) {
    legend -> AddEntry((TObject*)nullptr, Form("#mu = %f",((TF1 *)fObject)->GetParameter(1)), "");
    legend -> AddEntry((TObject*)nullptr, Form("#sigma = %f",((TF1 *)fObject)->GetParameter(2)), "");
  }
}

void efunc::drawGausInfoDrawings(int rangeMaxInSigma, vector<int> guidPointInSigma)
{
  //auto array = new TObjArray();

  auto f1 = (TF1 *) fObject;

  auto ampl = f1 -> GetParameter(0);
  auto mean = f1 -> GetParameter(1);
  auto sigm = f1 -> GetParameter(2);

  double xRange1, xRange2;
  f1 -> GetRange(xRange1, xRange2);

  auto f1Clone = (TF1 *) f1 -> Clone();
  f1Clone -> SetRange(mean-rangeMaxInSigma*sigm, mean+rangeMaxInSigma*sigm);
  f1Clone -> SetLineColor(f1->GetLineColor());
  f1Clone -> Draw("samel");
  //f1Clone -> SetDrawOption("samel");
  //array -> Add(f1Clone);

  auto yMax = f1Clone -> GetParameter(0);
  auto lrange = new TArrow(xRange1, yMax, xRange2, yMax, 0.01, "<>");
  lrange -> SetLineColor(kGray+1);
  lrange -> Draw("samel<>");
  //lrange -> SetDrawOption("samel");
  //array -> Add(lrange);

  for (auto gp : guidPointInSigma) {
    if (gp==0) {
      auto lc = new TLine(mean, 0, mean, ampl);
      lc -> SetLineColor(kGray);
      lc -> Draw("samel");
      //lc -> SetDrawOption("samel");
      //array -> Add(lc);
    }
    else {
      auto ac = f1Clone -> Eval(mean-gp*sigm);
      auto l1 = new TLine(mean-gp*sigm, 0, mean-gp*sigm, ac);
      auto l2 = new TLine(mean+gp*sigm, 0, mean+gp*sigm, ac);
      l1 -> SetLineColor(kGray);
      l2 -> SetLineColor(kGray);
      l1 -> SetLineStyle(2);
      l2 -> SetLineStyle(2);
      l1 -> Draw("samel");
      l2 -> Draw("samel");
      //l1 -> SetDrawOption("samel");
      //l2 -> SetDrawOption("samel");

      //array -> Add(l1);
      //array -> Add(l2);
    }
  }

  //return array;
}
