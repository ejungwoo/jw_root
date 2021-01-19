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

void eoption::clear()
{
  fData = "";
  Clear();
}

const char *eoption::print(bool printout) const
{
  TString val;
  val += GetEntriesFast();
  val += ")  ";

  for (auto i=0; i<GetEntriesFast(); ++i)
  {
    TString option = ((TNamed *) At(i)) -> GetName();
    TString value = ((TNamed *) At(i)) -> GetTitle();
    if (value.IsNull())
      val += option+",  ";
    else
      val += option+"="+value+",  ";
  }

  if (printout) cout_info << val << endl;
  return val;
}

void eoption::configureOptions(TString val)
{
  TString tval = val;
  tval.ReplaceAll(" ","");
  if (tval.Sizeof()>1&&tval[tval.Sizeof()-2]==',') tval += " ";

  tval.ReplaceAll(":",",");
  auto tokenizedArray = tval.Tokenize(",");
  auto numOptions = tokenizedArray -> GetEntriesFast();

  for (auto i=0; i<numOptions; ++i)
  {
    TString optString = ((TObjString*) tokenizedArray->At(i)) -> GetString();
    auto indexEq = optString.Index("=");
    TString option = "draw";
    TString value = optString;
    if (indexEq>=0) {
      option = optString(0,indexEq);
      value = optString(indexEq+1,optString.Sizeof()-indexEq-2);
    }

    TNamed *searchOption = (TNamed *) FindObject(option);
    if (searchOption==nullptr) Add(new TNamed(option,value));
    else searchOption -> SetTitle(value);
  }

  for (Int_t i=0; i<GetEntriesFast(); ++i)
  {
    auto obj = (TNamed *) fCont[i];
    if (i==0) fData = Form("%s=%s",obj->GetName(),obj->GetTitle());
    else      fData = fData + Form(",%s=%s",obj->GetName(),obj->GetTitle());
  }
}

void eoption::setOption(TString val) { configureOptions(val); }

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

TString eoption::getData()              const { return fData; }
int     eoption::getNumOptions()        const { return GetEntriesFast(); }
TString eoption::getOption(int idx)     const { if (idx<0) idx = fFoundIndex; return ((TNamed *) At(idx)) -> GetName(); }
TString eoption::getValue (TString opt) const { auto obj = (TNamed *) FindObject(opt); if (obj==nullptr) return ""; return obj -> GetTitle(); }
bool    eoption::getValueB(TString opt) const { auto obj = (TNamed *) FindObject(opt); if (obj==nullptr) return 0; TString val = obj -> GetTitle(); return ((val=="false"||val=="0")?false:true); }
int     eoption::getValueI(TString opt) const { auto obj = (TNamed *) FindObject(opt); if (obj==nullptr) return 0; return TString(obj->GetTitle()).Atoi(); }
double  eoption::getValueD(TString opt) const { auto obj = (TNamed *) FindObject(opt); if (obj==nullptr) return 0; return TString(obj->GetTitle()).Atof(); }
TString eoption::getValue (int idx)     const { if (idx<0) idx = fFoundIndex; return ((TNamed *) At(idx)) -> GetTitle(); }
bool    eoption::getValueB(int idx)     const { if (idx<0) idx = fFoundIndex; auto val = getValue(idx); return ((val=="false"||val=="0")?false:true); }
int     eoption::getValueI(int idx)     const { if (idx<0) idx = fFoundIndex; return getValue(idx).Atoi(); }
double  eoption::getValueD(int idx)     const { if (idx<0) idx = fFoundIndex; return getValue(idx).Atof(); }
TString eoption::getDrawOption()        const { if (GetEntriesFast()>0 && strcmp(Last()->GetName(),"draw")==0) return Last()->GetTitle(); return ""; }

void eoption::addDrawOption(TString opt) {
  if (getDrawOption().Index(opt)<0)
    setOption(TString("draw=")+getDrawOption()+opt);
}
void eoption::setDrawOption(TString opt) { setOption(TString("draw=")+opt); }
