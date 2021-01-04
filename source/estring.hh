#ifndef __estring_hh__
#define __estring_hh__

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

class estring : public TString
{
  private:
    int fNumOptions = 0;
    TObjArray *fOptionArray = nullptr;
    TObjArray *fValueArray = nullptr;
    int fIndexFound = -1;

  private:
    void configureOptions(const char *val);

  public: 
    estring(const char *val="");

    bool    findOption(TString opt);
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
};

#endif
