#include "KBGlobal.hh"
#include "eoption.hh"
#include "TNamed.h"
#include "TObjString.h"
#include <iostream>
using namespace std;

ClassImp(eoption);

eoption::eoption(const char *val) : fData(val)
{
  configureOptions();
}

void eoption::configureOptions()
{
  TString tval = fData;
  tval.ReplaceAll(" ","");
  if (tval.Sizeof()>1&&tval[tval.Sizeof()-2]==',') tval += " ";

  tval.ReplaceAll(":",",");
  auto tokenizedArray = tval.Tokenize(",");
  auto numOptions = tokenizedArray -> GetEntriesFast();

  TNamed *drawOption = nullptr;

  if (numOptions>0) {
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
      if (indexEq<0||option=="draw") {
        if (drawOption==nullptr)
          drawOption = new TNamed("draw",value.Data());
        else
          drawOption -> SetTitle(drawOption->GetTitle()+value);
        continue;
      }

      TNamed *searchOption = nullptr;
      for (int i=0; i<GetEntriesFast(); ++i) {
        searchOption = (TNamed *) fCont[i];
        if (searchOption && 0==strcmp(option, searchOption->GetName())) {
          searchOption -> SetTitle(value);
          break;
        }
        searchOption = nullptr;
      }

      if (searchOption==nullptr) Add(new TNamed(option,value));
    }
    if (drawOption!=nullptr) Add(drawOption);
  }
  /*
  else {
    if (numOptions==0&&!tval.IsNull()) {
      auto indexEq = tval.Index("=");
      TString option = tval;
      TString value = "";

      if (indexEq>=0) {
        option = tval(0,indexEq);
        value = tval(indexEq+1,tval.Sizeof()-indexEq-2);
      }

      auto optionSet = new TNamed(option,value);
      Add(optionSet);
    }
  }
  */
}

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
//TString eoption::getDrawOption()    const { if (Last()==nullptr) return ""; return (Last() -> GetTitle()); }
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
