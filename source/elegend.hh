#ifndef __elegend_hh__
#define __elegend_hh__

#include "edrawing.hh"

class elegend : public edrawing
{
  private:
    double fLMargin = 0.19;
    double fRMargin = 0.155;
    double fBMargin = 0.17;
    double fTMargin = 0.18;
    double fWidthPerLengthLegend = 0.008;
    double fLegendHeightPerEntry = 0.04;
    int fFillStyleLegend = 0;
    int fBorderSizeLegend = 0;
    int fLegendAlign = 88;
    double fXOffset = 0.;
    double fYOffset = 0.;

  public:
    elegend(TObject *obj=nullptr, TString option="");
    elegend(TString option) : elegend((TObject *) nullptr, option) {}

    TLegend *getLegend() const { return (TLegend *) fObject; }

    void add(TObject *entry, TString name="", TString option="");

    void setLegendAlign(int val);
    void setLegendOffset(double xOffset, double yOffset);
    void findLegendSizeAndPosition();

    virtual void configureParameters();
    virtual void configureBasicOption();
    virtual void configureDrawOption();
    virtual void configureRange();
    virtual void configureAttributes();

    virtual void preDraw();

    ClassDef(elegend,0)
};

#endif
