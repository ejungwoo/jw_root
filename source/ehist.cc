#include "KBGlobal.hh"
#include "ehist.hh"
#include "ecanvas.hh"
#include <iostream>
using namespace std;

ClassImp(ehist)

ehist::ehist(TH1 *hist, const char *option) : edrawing(hist, option)
{
}

ehist::ehist(const char *name, const char *title, const char *expression, const char *selection, ebinning binnx, ebinning binny, const char *option)
: edrawing(name, title, (TObject*) nullptr, option), fExpression(expression), fSelection(selection), fBinnx(binnx), fBinny(binny)
{
}

ehist::ehist(ehist *hist1, ehist *hist2, const char *option)
: edrawing((TObject*) nullptr, option)
{
  auto hxName  = hist1 -> getName();
  auto hxExpr  = hist1 -> getExpression();
  auto hxSel   = hist1 -> getSelection();
  auto hxBinnx = hist1 -> getBinnx();
  auto hxBinny = hist1 -> getBinny();

  auto hyName  = hist2 -> getName();
  auto hyExpr  = hist2 -> getExpression();
  auto hySel   = hist2 -> getSelection();
  auto hyBinnx = hist2 -> getBinnx();
  auto hyBinny = hist2 -> getBinny();

  TString dumm, ttlm, ttlx, ttly;
  hist1 -> getTitles(ttlm, ttlx, dumm, dumm);
  hist2 -> getTitles(dumm, ttly, dumm, dumm);

  fName = hxName + "_VS_" + hyName;
  fExpression = hyExpr + ":" + hxExpr;
  fSelection = hxSel;
  fTitle = ttlm + ";" + ttlx + ";" + ttly;
  fBinnx = hxBinnx;
  fBinny = hyBinnx;

  fHistIs2D = true;
}

void ehist::init()
{
  edrawing::init();

  if (fObject!=nullptr) {
    auto hist = (TH1 *) fObject;
    if (fName.IsNull()) fName = hist -> GetName();
    if (fTitle.IsNull()) fTitle = hist -> GetTitle();
    fBinnx = ebinning(hist,1);
    if (hist->InheritsFrom(TH2::Class()))
      fBinny = ebinning(hist,2);
    //if (fBinny.getN()>1) fHistIs2D = true;
  }
}

void ehist::configureParameters() {
}

void ehist::configureDrawOption()
{
  if (fOptionString.findOption("stats")) fSetStats = fOptionString.getValueB();

  if (fOptionString.findOption("xmax")) { fUserXRangeIsSet = true; fUserXMax = fOptionString.getValueD(); }
  if (fOptionString.findOption("xmin")) { fUserXRangeIsSet = true; fUserXMin = fOptionString.getValueD(); }
  if (fOptionString.findOption("ymax")) { fUserYRangeIsSet = true; fUserYMax = fOptionString.getValueD(); }
  if (fOptionString.findOption("ymin")) { fUserYRangeIsSet = true; fUserYMin = fOptionString.getValueD(); }
  if (fOptionString.findOption("zmax")) { fUserZRangeIsSet = true; fUserZMax = fOptionString.getValueD(); }
  if (fOptionString.findOption("zmin")) { fUserZRangeIsSet = true; fUserZMin = fOptionString.getValueD(); }

  auto drawOption = fOptionString.getDrawOption();

  if (fHistIs2D) {
    if (drawOption.IsNull())
      fOptionString.addDrawOption("colz");
  }

  else {
    if (drawOption.IsNull()) {
      if (drawOption.IsNull())
        fOptionString.setDrawOption("hist");
    }
    fOptionString.addDrawOption("same");
  }
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

void ehist::preDraw()
{
  if (fObject==nullptr)
    make();

  fTaskList -> CleanTasks();
  fTaskList -> ExecuteTasks("");

  auto hist = (TH1 *) fObject;
  hist -> SetStats(fSetStats);

  if (fUserXRangeIsSet) hist -> GetXaxis() -> SetRangeUser(fUserXMin, fUserXMax);
  if (fUserYRangeIsSet) hist -> GetYaxis() -> SetRangeUser(fUserYMin, fUserYMax);
  if (fUserZRangeIsSet) hist -> GetZaxis() -> SetRangeUser(fUserZMin, fUserZMax);
}

void ehist::getTitles(TString &ttlm, TString &ttlx, TString &ttly, TString &ttlz)
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

  ttlm = ttls[0];
  ttlx = ttls[1];
  ttly = ttls[2];
  ttlz = ttls[3];
}

void ehist::setDrawing(TObject *obj)
{
  if (obj->InheritsFrom(edrawing::Class())) {
    auto drawing = (edrawing *) obj;
    fObject = drawing -> getObject();
    setOption(drawing -> getOption());
  }
  else {
    fObject = obj;
  }

  init();
}

void ehist::addDrawing(TObject *obj)
{
  auto hist0 = (TH1 *) fObject;
  if (obj->InheritsFrom(edrawing::Class())) {
    auto drawing = (edrawing *) obj;
    hist0 -> Add((TH1 *) (drawing -> getObject()));
  }
  else {
    hist0 -> Add((TH1 *) obj);
  }
}

void ehist::setTitle(TString val, int ttlIndex)
{
  TString ttlm, ttlx, ttly, ttlz;
  getTitles(ttlm, ttlx, ttly, ttlz);

       if (ttlIndex==0) ttlm = val;
  else if (ttlIndex==1) ttlx = val;
  else if (ttlIndex==2) ttly = val;
  else if (ttlIndex==3) ttlz = val;

  fTitle = ttlm + ";" + ttlx + ";" + ttly + ";" + ttlz;
}

void ehist::make(TTree *tree)
{
  auto rm0 = [](TString vstring) {
    auto posdot = vstring.Index(".");
    auto poslast = vstring.Sizeof()-2;
    auto lstring = TString(vstring(poslast));
    if (posdot>=0&&posdot<poslast)
      while (lstring=="0") {
        vstring.Remove(poslast);
        poslast = vstring.Sizeof()-2;
        lstring = TString(vstring(poslast));
      }
    return vstring;
  };

  TH1 *hist = nullptr;
  auto nameFinal = configureNewName();

  if (fBinnx.isNull()) fBinnx.make(tree,fExpression);

  fHistIs2D = fBinny.isNull() ? false : true;

  TString expFinal = fExpression;
  TString selFinal = fSelection.GetTitle();
  TString ttlFinal = fTitle;
  auto numPar = fParameters.GetEntriesFast();
  for (auto j : {0,0})
    for (auto i=0; i<numPar; ++i) {
      if (fParameters[i]->InheritsFrom(TNamed::Class())) {
        auto par = (TNamed *) fParameters[i];
        const char *parName = par -> GetName();
        const char *parVal = par -> GetTitle();
        TString parString = Form("[%s]",parName);
        if (expFinal.Index(parString)>=0) expFinal.ReplaceAll(parString,Form("%s",parVal));
        if (selFinal.Index(parString)>=0) selFinal.ReplaceAll(parString,Form("%s",parVal));
        if (ttlFinal.Index(parString)>=0) ttlFinal.ReplaceAll(parString,Form("%s",parVal));
      }
      else {
        auto par = (TParameter<Double_t> *) fParameters[i];
        const char *parName = par -> GetName();
        TString parVal = rm0(Form("%f",par->GetVal()));
        TString parString = Form("[%s]",parName);
        if (expFinal.Index(parString)>=0) expFinal.ReplaceAll(parString,parVal);
        if (selFinal.Index(parString)>=0) selFinal.ReplaceAll(parString,parVal);
        if (ttlFinal.Index(parString)>=0) ttlFinal.ReplaceAll(parString,parVal);
      }
    }

  if (fHistIs2D) hist = new TH2D(nameFinal,ttlFinal,fBinnx.getN(),fBinnx.getMin(),fBinnx.getMax(),fBinny.getN(),fBinny.getMin(),fBinny.getMax());
  else           hist = new TH1D(nameFinal,ttlFinal,fBinnx.getN(),fBinnx.getMin(),fBinnx.getMax());

  if (tree!=nullptr)
  {
    auto entries = tree -> Project(nameFinal,expFinal,selFinal);
    kr_info(0) << "(\"" << nameFinal << "\", \"" << ttlFinal << "\", \"" << expFinal << "\", \"" << selFinal << ", " << fBinnx.print(0) << ", " << fBinny.print(0) << "); // >> " << entries << endl;
    if (!fHistIs2D)
      fBinny.set(0, hist->GetMinimum(), hist->GetMaximum());
  }

  fObjStack.Add(hist);
  fObject = hist;

  init();
}

ehist *ehist::copy(TString name) const
{
  ehist *chist;
  if (isNull()) {
    chist = new ehist(name, fTitle, fExpression, fSelection, fBinnx, fBinny, fOptionString.getData());
  }
  else {
    chist = new ehist((TH1 *) fObject->Clone(), fOptionString.getData());
    chist -> setBinnx(fBinnx);
    chist -> setBinny(fBinny);
    chist -> setName(name);
  }

  auto numPar = fParameters.GetEntriesFast();
  for (auto i=0; i<numPar; ++i) {
    if (fParameters[i]->InheritsFrom(TNamed::Class())) {
      auto par = (TNamed *) fParameters[i];
      const char *parName = par -> GetName();
      const char *parVal = par -> GetTitle();
      chist -> setPar(parName,parVal);
    }
    else {
      auto par = (TParameter<Double_t> *) fParameters[i];
      const char *parName = par -> GetName();
      double parVal = par -> GetVal();
      chist -> setPar(parName,parVal);
    }
  }

  chist -> init();
  chist -> setHistIs2D(fHistIs2D);

  return chist;
}

void ehist::fixPar(const char *name, double val) {
  TString expFinal = fExpression;
  TString selFinal = fSelection.GetTitle();
  TString ttlFinal = fTitle;
  TString parString = Form("[%s]",name);
  if (expFinal.Index(parString)>=0) expFinal.ReplaceAll(parString,Form("%f",val));
  if (selFinal.Index(parString)>=0) selFinal.ReplaceAll(parString,Form("%f",val));
  if (ttlFinal.Index(parString)>=0) ttlFinal.ReplaceAll(parString,Form("%f",val));
  fExpression = expFinal;
  fSelection = selFinal;
  fTitle = ttlFinal;
}

void ehist::fixPar(const char *name, const char *val) {
  TString expFinal = fExpression;
  TString selFinal = fSelection.GetTitle();
  TString ttlFinal = fTitle;
  TString parString = Form("[%s]",name);
  if (expFinal.Index(parString)>=0) expFinal.ReplaceAll(parString,Form("%s",val));
  if (selFinal.Index(parString)>=0) selFinal.ReplaceAll(parString,Form("%s",val));
  if (ttlFinal.Index(parString)>=0) ttlFinal.ReplaceAll(parString,Form("%s",val));
  fExpression = expFinal;
  fSelection = selFinal;
  fTitle = ttlFinal;
}

void ehist::setECanvas(ecanvas *ecvs, int idxPad) {
  fParentECanvas = ecvs;
  fECanvasPadIndex = idxPad;
  if (fEFit!=nullptr)
    if (fEFit -> getParentECanvas()==nullptr)
      ecvs -> add(idxPad, fEFit);
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

void ehist::addTask(TString taskName, double par, TString taskOption, TString drawOption)
{
  if (taskName=="tfitGaus")
  {
    auto task = new tfitGaus(this, par, taskOption);
    fTaskList -> Add(task);

    if (drawOption.IsNull())
      drawOption = "addtol=1,draw=samel";

    drawOption = drawOption += Form(",sort=%d",fSortIndex+1);
    if (fEFit==nullptr)
      fEFit = new efunc((TF1 *) nullptr, drawOption);
  }
}

void tfitGaus::fit(TH1 *hist, double fitRangeInSigma, TString fitOption)
{
  if (hist==nullptr)
    return;

  if (fitRangeInSigma!=0) fFitRangeInSigma = fitRangeInSigma;
  if (fitOption!="") fFitOption = fitOption;

  auto binAtYMax = hist -> GetMaximumBin();
  auto yMax = hist -> GetBinContent(binAtYMax);
  auto xMean = hist -> GetXaxis()->GetBinCenter(binAtYMax);
  auto xSigma = hist -> GetStdDev();
  auto gaus = new TF1(Form("%s_fitGaus",hist->GetName()),"gaus(0)",xMean-xSigma*fFitRangeInSigma,xMean+xSigma*fFitRangeInSigma);
  gaus -> SetParameters(yMax,xMean,xSigma);

  auto nBins = hist -> GetXaxis() -> GetNbins();
  auto histRange1 = hist -> GetXaxis() -> GetBinLowEdge(1);
  auto histRange2 = hist -> GetXaxis() -> GetBinUpEdge(nBins);

  auto checkFitIsOutOfRange = [gaus, &histRange1, &histRange2]()
  {
    bool fitIsOutOfRange = false;
    double fitRange1, fitRange2;
    gaus -> GetRange(fitRange1, fitRange2);
    double fitRangeCut = (fitRange2 - fitRange1)/3.;
    if (fitRange1 < histRange1) {
           if (fitRange2 < histRange1)               { fitIsOutOfRange = true; }
      else if (fitRange2 - histRange1 < fitRangeCut) { fitIsOutOfRange = true; }
      else                                           { fitRange1 = histRange1; }
    } else if (fitRange2 > histRange2) {
           if (fitRange1 > histRange2)               { fitIsOutOfRange = true; }
      else if (histRange2 - fitRange1 < fitRangeCut) { fitIsOutOfRange = true; }
      else                                           { fitRange2 = histRange2; }
    }
    gaus -> SetRange(fitRange1, fitRange2);
    if (fitIsOutOfRange) {
      cout_warning <<" gaus-range:("<<fitRange1<<","<<fitRange2<<") out of hist-range:("<<histRange1<<","<<histRange2<<")"<< endl;
      gaus -> SetParameters(0,0,0);
    }
    return fitIsOutOfRange;
  };

  if (checkFitIsOutOfRange()) return;

  hist -> Fit(gaus, fFitOption);
  xMean = gaus -> GetParameter(1);
  xSigma = gaus -> GetParameter(2);
  gaus -> SetRange(xMean-fFitRangeInSigma*xSigma,xMean+fFitRangeInSigma*xSigma);

  if (checkFitIsOutOfRange()) return;

  int fitIterationCount = 1;
  int fitIterationMax = 20;
  double sigmaResolution = 0.1;
  double xSigma2 = 0.;
  while (fitIterationCount<fitIterationMax && TMath::Abs(xSigma-xSigma2)/xSigma>sigmaResolution)
  {
    xSigma2 = xSigma;
    gaus -> SetRange(xMean-fFitRangeInSigma*xSigma2,xMean+fFitRangeInSigma*xSigma2);

    if (checkFitIsOutOfRange()) return;

    hist -> Fit(gaus, fFitOption);
    xMean = gaus -> GetParameter(1);
    xSigma = gaus -> GetParameter(2);

    ++fitIterationCount;
  }

  kr_info(0) <<"fitGaus("<<fEHist->getCurrentName()<<","<<fFitRangeInSigma<<","<<fFitOption<<")"
    <<" -> [a:"<<gaus->GetParameter(0)<<", m:"<<xMean<<", s:"<<xSigma<<"]"
    <<" ("<<fitIterationCount<<")"<<std::endl;

  fEHist -> setFit(gaus);
}
