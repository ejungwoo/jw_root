#include "KBGlobal.hh"
#include "eoption.hh"
#include "TObjString.h"
#include <iostream>
using namespace std;

eoption::eoption(const char *val) : fData(val)
{
  configureOptions();
}

void eoption::configureOptions()
{
  TString tval = fData;
  tval.ReplaceAll(" ","");
  if (tval.Sizeof()>1&&tval[tval.Sizeof()-2]==',') tval += " ";

  auto tokenizedArray = tval.Tokenize(",");
  auto numOptions = GetEntriesFast();

  TNamed *drawOption;
  TNamed *optionSet;
  for (auto i=0; i<numOptions; ++i)
  {
    auto optOString = (TObjString*) tokenizedArray->At(i);
    TString optString = optOString -> GetString();

    auto indexEq = optString.Index("=");
    TString option = optString;
    TString value = "";

    if (indexEq>=0) {
      option = optString(0,indexEq);
      value = optString(indexEq+1,optString.Sizeof()-indexEq-2);
    }

    if (i==numOptions-1||option=="draw") drawOption = new TNamed(option,value);

    optionSet = new TNamed(option,value)
    Add(optionSet);
  }
}

bool eoption::findOption(TString opt)
{
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

int     eoption::getNumOptions()    const { return GetEntriesFast(); }
TString eoption::getOption(int idx) const { if (idx<0) idx = fFoundIndex; return ((TObjString*) At(idx)) -> GetName(); }
TString eoption::getValue (int idx) const { if (idx<0) idx = fFoundIndex; if (idx<0) return "0"; return ((TObjString*) At(idx)) -> GetTitle(); }
bool    eoption::getValueB(int idx) const { if (idx<0) idx = fFoundIndex; auto val = getValue(idx); return ((val=="false"||val=="0")?false:true); }
int     eoption::getValueI(int idx) const { if (idx<0) idx = fFoundIndex; return getValue(idx).Atoi(); }
double  eoption::getValueD(int idx) const { if (idx<0) idx = fFoundIndex; return getValue(idx).Atof(); }
TString eoption::getDrawOption()    const { if (Last()==nullptr) return 0; return (Last() -> GetName()); }
void    eoption::setDrawOption(TString opt) { ((TObjString*) At(fNumOptions-1)) -> SetName(opt); }

const char *eoption::print(bool printout) const
{
  TString val;
  val += fNumOptions;
  val += ")  ";

  for (auto i=0; i<fNumOptions; ++i)
  {
    auto option = ((TObjString*) At(i)) -> GetName();
    auto value = ((TObjString*) At(i)) -> GetTitle();
    if (value.IsNull())
      val += option+",  ";
    else
      val += option+"="+value+",  ";
  }

  if (printout) cout_info << val << endl;
  return val;
}
