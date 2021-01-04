#ifndef __eoption_hh__
#define __eoption_hh__

#include "TObjArray.h"
#include "TString.h"

/**
 * string class to handle option format
 *
 * default option format has two component __option__ and __value__:
 * ```
 * "option=value"
 * ```
 * (spacing is ignored)
 *
 * Two or more options should be comma(",") separated. ex)
 * ```
 * "option=value, n=1, x1=33.3, make, colz"
 * ```
 *
 * If no __value__ is given, the value is set to 1 which will return __true__ from getValueB().
 *
 * Last option is set to draw option.
 *
 * If option is found by findOption() method,
 * the default output of getOption() and getValue() is directed to found content.
*/

class eoption : public TObjArray
{
  private:
    TString fData;
    int fNumOptions = 0;
    int fFoundIndex = -1;

  private:
    void configureOptions();

  public: 
    eoption(const char *val="");

    bool    findOption(TString opt);
    TString getData()               const;
    int     getNumOptions()         const;
    TString getOption(int idx=-1)   const;
    TString getValue (int idx=-1)   const;
    bool    getValueB(int idx=-1)   const;
    int     getValueI(int idx=-1)   const;
    double  getValueD(int idx=-1)   const;
    TString getDrawOption()         const;

    void setDrawOption(TString opt);

    const char *print(bool printout=true) const;

    //void operator=(const char *val);

    ClassDef(eoption,0);
};

#endif
