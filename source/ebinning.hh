#ifndef __ebinning_hh__
#define __ebinning_hh__

#include "TH1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TTree.h"

class ebinning
{
  private:
    int fN = 0; ///< number of bins
    double fMin = 0; ///< lower bound
    double fMax = 0; ///< upper bound
    double fW = 0; ///< ebinning space width
    double fValue = 0; ///< value will be set after iteration using next(), back(), nextb()
    int fIdx = 0; ///< index for iteration

  public:
    ebinning(int n=-1, double min=0, double max=0, double w=-1);
    ebinning(ebinning const & binn) : fN(binn.fN), fMin(binn.fMin), fMax(binn.fMax), fW((fMax-fMin)/fN) {}
    ebinning(TH1 *hist, int i=1);
    ebinning(TGraph *graph, int i=1);
    ~ebinning() {}

     void init()                       { if (fW>0&&fN<1) setW(fN); else if (fN>0&&fW<1) setN(fW); }

     bool isNull()               const { if (fN<1||(fMin==0&&fMax==0)) return true; return false; }

      int getN()                 const { return fN; }
   double getMin()               const { return fMin; }
   double getMax()               const { return fMax; }
   double getW()                 const { return fW; }
   double getValue()             const { return fValue; }
      int getIdx()               const { return fIdx; }
      int getBin (double val)    const { return int((val-fMin)/fW); }
      int findBin(double val)    const { return getBin(val); }

   double getFullWidth()         const { return .5*(fMax + fMin); }
   double getFullCenter()        const { return (fMax - fMin); }

     void set    (double n, double min, double max);

     void setN   (double n)            { fN = n; fW = (fMax-fMin)/fN; }
     void setW   (double w)            { fW = w; fN = int((fMax-fMin)/fW); }
     void setMin (double min)          { fMin = min; fW = (fMax-fMin)/fN; }
     void setMax (double max)          { fMax = max; fW = (fMax-fMin)/fN; }

     void resetB (bool includeOUFlow)  { fIdx = (includeOUFlow?-1:0); }
   double nextB  (bool includeOUFlow)  {
     if ((includeOUFlow && fIdx>fN) || (fIdx>fN-1))
       return 0;
     fValue = fMin + (fIdx++) * fW + .5 * fW;
     return 1;
   }

   double lowEdge (int bin= 1) const { return fMin+(bin-1)*(fMax-fMin)/fN; }
   double highEdge(int bin=-1) const { if (bin==-1) bin=fN; return fMin+(bin)*(fMax-fMin)/fN; }
   double getCenter (int bin)  const { return (fMin + (bin-.5)*fW); }

   TString print(bool pout=1) const;
   void operator=(const ebinning binn);

   void make(TTree *tree, const char* branchName);
};

#endif
