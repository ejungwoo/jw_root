#include "KBGlobal.hh"
#include "eoption.hh"
#include "TNamed.h"
#include "TObjString.h"
#include <iostream>
using namespace std;

ClassImp(eoption);

eoption::eoption(const char *val) : fData(val)
{
  configureOptions(val);
}

void eoption::configureOptions(TString val)
{
  TString tval = val;
  tval.ReplaceAll(" ","");
  if (tval.Sizeof()>1&&tval[tval.Sizeof()-2]==',') tval += " ";

  tval.ReplaceAll(":",",");
  auto tokenizedArray = tval.Tokenize(",");
  auto numOptions = tokenizedArray -> GetEntriesFast();

  TNamed *drawOption = (TNamed *) FindObject("draw");

  for (auto i=0; i<numOptions; ++i)
  {
    TString optString = ((TObjString*) tokenizedArray->At(i)) -> GetString();
    auto indexEq = optString.Index("=");
    TString option = optString;
    TString value;
    if (indexEq>=0) {
      option = optString(0,indexEq);
      value = optString(indexEq+1,optString.Sizeof()-indexEq-2);
    }

    // drawing option
    if (indexEq<0||option=="draw") {
      if (drawOption==nullptr)
        drawOption = new TNamed("draw",value.Data());
      else {
        TString valueDraw = drawOption->GetTitle();
        if (valueDraw.Index(value)<0)
          drawOption -> SetTitle(valueDraw+value);
      }
      continue;
    }

    TNamed *searchOption = (TNamed *) FindObject(option);
    if (searchOption==nullptr) Add(new TNamed(option,value));
    else searchOption -> SetTitle(value);
  }
  if (drawOption!=nullptr) Add(drawOption);

  fNumOptions = GetEntriesFast();
  for (Int_t i=0; i<fNumOptions; ++i)
  {
    auto obj = (TNamed *) fCont[i];
    if (i==0) fData = Form("%s=%s",obj->GetName(),obj->GetTitle());
    else      fData = fData + Form(",%s=%s",obj->GetName(),obj->GetTitle());
  }
}

void eoption::addOption(TString val) { configureOptions(val); }

bool eoption::findOption(TString opt)
{
  fFoundIndex = -1;
  auto numOptions = GetEntriesFast();
  for (Int_t i=0; i<numOptions; ++i)
  {
    TObject *obj = fCont[i];
    if (obj && 0==strcmp(opt, obj->GetName())) {
      fFoundIndex = i;
      return true;
    }
  }

  return false;
}

TString eoption::getData()          const { return fData; }
int     eoption::getNumOptions()    const { return GetEntriesFast(); }
TString eoption::getOption(int idx) const { if (idx<0) idx = fFoundIndex; return ((TObjString*) At(idx)) -> GetName(); }
TString eoption::getValue (int idx) const { if (idx<0) idx = fFoundIndex; return ((TObjString*) At(idx)) -> GetTitle(); }
bool    eoption::getValueB(int idx) const { if (idx<0) idx = fFoundIndex; auto val = getValue(idx); return ((val=="false"||val=="0")?false:true); }
int     eoption::getValueI(int idx) const { if (idx<0) idx = fFoundIndex; return getValue(idx).Atoi(); }
double  eoption::getValueD(int idx) const { if (idx<0) idx = fFoundIndex; return getValue(idx).Atof(); }
TString eoption::getDrawOption()    const { if (GetEntriesFast()>0 && strcmp(Last()->GetName(),"draw")==0) return Last()->GetTitle(); return ""; }
void    eoption::setDrawOption(TString opt) { ((TNamed *) ((TObjString*) At(fNumOptions-1))) -> SetName(opt); }

const char *eoption::print(bool printout) const
{
  TString val;
  val += fNumOptions;
  val += ")  ";

  for (auto i=0; i<fNumOptions; ++i)
  {
    TString option = ((TObjString*) At(i)) -> GetName();
    TString value = ((TObjString*) At(i)) -> GetTitle();
    if (value.IsNull())
      val += option+",  ";
    else
      val += option+"="+value+",  ";
  }

  if (printout) cout_info << val << endl;
  return val;
}
