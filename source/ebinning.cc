#include "KBGlobal.hh"
#include "ebinning.hh"
#include "TAxis.h"
#include <iostream>
using namespace std;

ebinning::ebinning(int n, double min, double max, double w) : fN(n), fMin(min), fMax(max), fW(w) { init(); }

ebinning::ebinning(TH1 *hist, int i)
{
  TAxis *axis;
       if (i==2) axis = hist -> GetYaxis();
  else if (i==3) axis = hist -> GetZaxis();
  else           axis = hist -> GetXaxis();

  fN = axis -> GetNbins();
  fMin = axis -> GetBinLowEdge(1);
  fMax = axis -> GetBinUpEdge(fN);
  fW = (fMax-fMin)/fN;
}

ebinning::ebinning(TGraph *graph, int i)
{
  fN = graph -> GetN();
  double x1,x2,y1,y2;
  graph -> ComputeRange(x1,y1,x2,y2);
  double xe1 = (x2-x1)/(fN-1)/2.;
  double xe2 = (x2-x1)/(fN-1)/2.;
  double ye1 = (y2-y1)/(fN-1)/2.;
  double ye2 = (y2-y1)/(fN-1)/2.;
  if (graph->InheritsFrom(TGraphErrors::Class())) {
    xe1 = graph -> GetErrorX(0);
    xe2 = graph -> GetErrorX(fN-1);
    ye1 = graph -> GetErrorY(0);
    ye2 = graph -> GetErrorY(fN-1);
  }
  if (i==2) { fMin = y1 - ye1; fMax = y2 + ye2; }
  else      { fMin = x1 - xe1; fMax = x2 + xe2; }
  fW = (fMax-fMin)/fN;
}

void ebinning::init()
{
       if (fW>0&&fN<1) setW(fW);
  else if (fN>0&&fW<1) setN(fN);
}


void ebinning::set(double n, double min, double max)
{
  fN = n;
  fMin = min;
  fMax = max;
  fW = (fMax-fMin)/fN;
}

TString ebinning::print(bool pout) const
{
  TString line = Form("(%d,%f,%f;%f)",fN,fMin,fMax,fW);
  if (pout)
    cout_info << line << endl;
  return line;
}

void ebinning::operator=(const ebinning binn)
{
  fN = binn.getN();
  fMin = binn.getMin();
  fMax = binn.getMax();
  fW = binn.getW();
}

void ebinning::make(TTree *tree, const char* branchName)
{
  if (fN<1) fN = 100;
  if (tree==nullptr) {
    fMin = 0;
    fMax = 1;
    return;
  }
  auto min = tree -> GetMinimum(branchName);
  auto max = tree -> GetMaximum(branchName);
  if (0) {
    auto dmm = (max - min) / 10.;
    fMax = max + dmm;
    fMin = min - dmm;
  }
  else {
    fMax = max;
    fMin = min;
  }
  fW = (fMax-fMin)/fN;
}
