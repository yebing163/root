/*****************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: RooFitCore
 *    File: $Id: RooAbsPdf.rdl,v 1.5 2001/05/10 00:16:06 verkerke Exp $
 * Authors:
 *   DK, David Kirkby, Stanford University, kirkby@hep.stanford.edu
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu
 * History:
 *   07-Mar-2001 WV Created initial version
 *
 * Copyright (C) 2001 University of California
 *****************************************************************************/
#ifndef ROO_ABS_PDF
#define ROO_ABS_PDF

#include "RooFitCore/RooAbsReal.hh"
#include "RooFitCore/RooRealIntegral.hh"
class RooArgSet ;
class TPaveText;
class TH1F;
class TH2F;

class RooAbsPdf : public RooAbsReal {
public:

  // Constructors, assignment etc
  inline RooAbsPdf() { }
  RooAbsPdf(const char *name, const char *title, const char *unit= "") ;
  RooAbsPdf(const char *name, const char *title, Double_t minVal, Double_t maxVal, const char *unit= "") ;
  RooAbsPdf(const RooAbsPdf& other, const char* name=0);
  virtual ~RooAbsPdf();

  // Data set dependent accessors (normalization & dependent/parameter interpretation)

  // PDF-specific plotting & display
  TH1F *Scan(RooDataSet* data, RooRealVar &param, Int_t bins= 0) { return 0 ; } 
  TH1F *Scan(RooDataSet& data, RooRealVar &param, Int_t bins= 0) { return 0 ; } 
  TH2F *PlotContours(RooRealVar& var1, RooRealVar& var2,
		     Double_t n1= 1, Double_t n2= 2, Double_t n3= 0) { return 0 ; } 
  TPaveText *Parameters(const char *label= "", Int_t sigDigits = 2,
			Option_t *options = "NELU", Double_t xmin=0.65,
                        Double_t xmax= 0.99,Double_t ymax=0.95) { return 0 ; } 

  // Interactions with a dataset  
  Int_t fitTo(RooDataSet& data, Option_t *options = "", Double_t *minValue= 0) ;
  Int_t fitTo(TH1F* hist, Option_t *options = "", Double_t *minValue= 0) { return 0 ; }
  Double_t nLogLikelihood(const RooDataSet* dset, Bool_t extended=kFALSE) const ;
  void attachDataSet(const RooDataSet* set) ;

  // Function evaluation support
  virtual Bool_t traceEvalHook(Double_t value) const ;  
  Double_t getVal(const RooDataSet* dset=0) const ;
  Double_t getLogVal(const RooDataSet* dset=0) const ;
  void resetErrorCounters(Int_t resetValue=10) ;
  void setTraceCounter(Int_t value) ;

  // Support for extended maximum likelihood, switched off by default
  virtual Bool_t canBeExtended() const { return kFALSE ; } 
  virtual Double_t expectedEvents() const { return 0 ; } 

private:

  // This forces definition copy ctor in derived classes 
  RooAbsPdf(const RooAbsPdf& other);

protected:

  virtual Double_t extendedTerm(UInt_t observedEvents) const ;

  mutable Double_t _rawValue ;
  mutable RooRealIntegral* _norm   ; // Normalization integral
  mutable RooDataSet* _lastDataSet ; // Data set for which normalization integral is valid

  mutable Int_t _errorCount ;        // Number of errors remaining to print
  mutable Int_t _traceCount ;        // Number of traces remaining to print
  mutable Int_t _negCount ;          // Number of negative probablities remaining to print

  ClassDef(RooAbsPdf,1) // a real-valued variable and its value
};

#endif
