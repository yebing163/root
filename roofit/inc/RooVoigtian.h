/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: RooVoigtian.rdl,v 1.4 2004/04/05 22:38:35 wverkerke Exp $
 * Authors:                                                                  *
 *   TS, Thomas Schietinger, SLAC,          schieti@slac.stanford.edu        *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_VOIGTIAN
#define ROO_VOIGTIAN

#include "RooFitCore/RooAbsPdf.hh"
#include "RooFitCore/RooRealProxy.hh"

class RooRealVar;

class RooVoigtian : public RooAbsPdf {
public:
  RooVoigtian(const char *name, const char *title,
	      RooAbsReal& _x, RooAbsReal& _mean, 
              RooAbsReal& _width, RooAbsReal& _sigma,
              Bool_t doFast = kFALSE);
  RooVoigtian(const RooVoigtian& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new RooVoigtian(*this,newname); }
  inline virtual ~RooVoigtian() { }

// These methods allow the user to select the fast evaluation
// of the complex error function using look-up tables
// (default is the "slow" CERNlib algorithm)

  inline void selectFastAlgorithm()    { _doFast = kTRUE;  }
  inline void selectDefaultAlgorithm() { _doFast = kFALSE; }

protected:

  RooRealProxy x ;
  RooRealProxy mean ;
  RooRealProxy width ;
  RooRealProxy sigma ;

  Double_t evaluate() const ;

private:

  Double_t _invRootPi;
  Bool_t _doFast;
  ClassDef(RooVoigtian,0) // Voigtian PDF
};

#endif

