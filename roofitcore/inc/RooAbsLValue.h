/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id: RooAbsLValue.rdl,v 1.9 2005/02/14 20:44:18 wverkerke Exp $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_ABS_LVALUE
#define ROO_ABS_LVALUE

#include <iostream>
#include "Rtypes.h"


class RooAbsLValue {
public:

  // Constructors, cloning and assignment
  RooAbsLValue() ;
  virtual ~RooAbsLValue();

  virtual void setBin(Int_t ibin) = 0 ;
  virtual Int_t getBin() const = 0 ;
  virtual Int_t numBins() const = 0 ;
  virtual Double_t getBinWidth(Int_t i) const = 0 ;

  virtual void randomize() = 0 ;

protected:

  ClassDef(RooAbsLValue,1) // Abstract variable
};

#endif
