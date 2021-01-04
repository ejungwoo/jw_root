#include "KBGlobal.hh"
#include "estring.hh"
#include "TObjString.h"
#include <iostream>
using namespace std;

estring::estring(const char *val) : TString(val)
{
  configureOptions(val);
}

void estring::configureOptions(const char *val)
{
  TString tval = val;
  tval.ReplaceAll(" ","");
  if (tval.Sizeof()>1&&tval[tval.Sizeof()-2]==',') tval += " ";

  fOptionArray = tval.Tokenize(",");
  fNumOptions = fOptionArray -> GetEntries();

  if (fValueArray==nullptr)
    fValueArray = new TObjArray();

  for (auto i=0; i<fNumOptions; ++i)
  {
    auto optOString = (TObjString*) fOptionArray->At(i);
    TString optString = optOString -> GetString();

    auto valOString = new TObjString();

    auto indexEq = optString.Index("=");
    TString option = optString;
    TString value = "";

    if (indexEq>=0) {
      option = optString(0,indexEq);
      value = optString(indexEq+1,optString.Sizeof()-indexEq-2);
    }

    optOString -> SetString(option);
    valOString -> SetString(value);

    fValueArray -> AddAt(valOString, i);
  }
}

bool estring::findOption(TString opt)
{
  Int_t nobjects = fOptionArray -> GetEntriesFast();
  for (Int_t i=0; i<nobjects; ++i) {
    TObject *obj = (*fOptionArray)[i];
    if (obj && 0==strcmp(opt, obj->GetName())) {
      fIndexFound = i;
      return true;
    }
  }

  return false;
}

int     estring::getNumOptions()    const { return fNumOptions; }
TString estring::getOption(int idx) const { if (idx<0) idx=fIndexFound; return ((TObjString*) fOptionArray->At(idx)) -> GetString(); }
TString estring::getValue (int idx) const { if (idx<0) idx=fIndexFound; if (idx<0) return "0"; return ((TObjString*) fValueArray -> At(idx)) -> GetString(); }
bool    estring::getValueB(int idx) const { if (idx<0) idx=fIndexFound; auto val = getValue(idx); return ((val=="false"||val=="0")?false:true); }
int     estring::getValueI(int idx) const { if (idx<0) idx=fIndexFound; return getValue(idx).Atoi(); }
double  estring::getValueD(int idx) const { if (idx<0) idx=fIndexFound; return getValue(idx).Atof(); }
TString estring::getDrawOption()    const { return ((fNumOptions==0)?"":getOption(fNumOptions-1)); }
void    estring::setDrawOption(TString opt) { ((TObjString*) fOptionArray->At(fNumOptions-1)) -> SetString(opt); }

const char *estring::print(bool printout) const
{
  TString val;
  val += fNumOptions;
  val += ")  ";

  for (auto i=0; i<fNumOptions; ++i)
  {
    auto option = ((TObjString*) fOptionArray->At(i)) -> GetString();
    auto value = ((TObjString*) fValueArray->At(i)) -> GetString();
    if (value.IsNull())
      val += option+",  ";
    else
      val += option+"="+value+",  ";
  }

  if (printout) cout_info << val << endl;
  return val;
}
