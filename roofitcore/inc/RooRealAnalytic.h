/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id$
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2002, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_REAL_ANALYTIC
#define ROO_REAL_ANALYTIC

#include "RooFitCore/RooRealBinding.hh"

class RooRealAnalytic : public RooRealBinding {
public:
  inline RooRealAnalytic(const RooAbsReal &func, const RooArgSet &vars, Int_t code, const RooArgSet* normSet=0) :
    RooRealBinding(func,vars,normSet), _code(code) { }
  inline virtual ~RooRealAnalytic() { }

  virtual Double_t operator()(const Double_t xvector[]) const;

protected:
  Int_t _code;

  ClassDef(RooRealAnalytic,0) // RooAbsFunc decorator
};

#endif

