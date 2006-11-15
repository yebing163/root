// @(#)root/tmva $Id: TSpline2.h,v 1.10 2006/10/10 17:43:52 andreas.hoecker Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TSpline2                                                              *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Quadratic spline class; uses quadrax function for interpolation           *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Xavier Prudent  <prudent@lapp.in2p3.fr>  - LAPP, France                   *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-KP Heidelberg, Germany     *
 *      Kai Voss        <Kai.Voss@cern.ch>       - U. of Victoria, Canada         *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland,                                                        * 
 *      U. of Victoria, Canada,                                                   * 
 *      MPI-KP Heidelberg, Germany,                                               * 
 *      LAPP, Annecy, France                                                      *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#ifndef ROOT_TMVA_TSpline2
#define ROOT_TMVA_TSpline2

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSpline2                                                             //
//                                                                      //
// Quadratic interpolation class (using quadrax)                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TSpline.h"

namespace TMVA {

   class TSpline2 : public TSpline {

   public:
  
      TSpline2( TString title, TGraph* theGraph );
      virtual ~TSpline2( void );

      virtual  Double_t Eval( Double_t x ) const;

      // dummy implementations
      virtual void BuildCoeff( void );
      virtual void GetKnot( Int_t i, Double_t& x, Double_t& y ) const;

   private:

      TGraph *fGraph;   // graph that is splined
      Double_t Quadrax( const Float_t dm, const Float_t dm1,
                        const Float_t dm2,const  Float_t dm3,
                        const Float_t cos1, const Float_t cos2, 
                        const Float_t cos3 ) const;
  
      ClassDef(TSpline2,0) //Quadratic interpolation class (using quadrax)
         ;
   };

} // namespace TMVA

#endif 


