// @(#)root/geom:$Id$
// Author: Rene Brun   26/12/02

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// Media are used to store properties related to tracking and which are useful
// only when using geometry with a particle transport MC package (via VMC). One
// may define several tracking media for a given material. The media ID are user
// defined values that are not used by the geometry package. In case geometry
// is used via VMC (in GEANT) these numbers are overwritten, so one can only
// rely on these values after gMC->FinishGeometry() is called.
// The media parameters are inspired from GEANT3 and the values defined make sense
// in context of GEANT (3 but also 4) or FLUKA interfaces.
////////////////////////////////////////////////////////////////////////////////

#include "Riostream.h"
#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TList.h"

ClassImp(TGeoMedium)

//-----------------------------------------------------------------------------
TGeoMedium::TGeoMedium()
{
// Default constructor
   fId      = 0;
   for (Int_t i=0; i<20; i++) fParams[i] = 0.;
   fMaterial= 0;
}

//-----------------------------------------------------------------------------
TGeoMedium::TGeoMedium(const char *name, Int_t numed, const TGeoMaterial *mat, Double_t *params)
             :TNamed(name,"")
{
// constructor
   fName = fName.Strip();
   fId    = numed;
   for (Int_t i=0; i<20; i++) fParams[i] = 0.;
   fMaterial = (TGeoMaterial*)mat;
   for (Int_t i=0;i<10;i++) {
      if (params) fParams[i] = params[i];
      else        fParams[i] = 0;
   }
   gGeoManager->GetListOfMedia()->Add(this);
}

//-----------------------------------------------------------------------------
TGeoMedium::TGeoMedium(const char *name, Int_t numed, Int_t imat, Int_t isvol, Int_t ifield,
              Double_t fieldm, Double_t tmaxfd, Double_t stemax, Double_t deemax, Double_t epsil, Double_t stmin)
             :TNamed(name,"")
{
// constructor
   fName = fName.Strip();
   fId    = numed;
   for (Int_t i=0; i<20; i++) fParams[i] = 0.;
   TIter next (gGeoManager->GetListOfMaterials());
   TGeoMaterial *mat;
   while ((mat = (TGeoMaterial*)next())) {
      if (mat->GetUniqueID() == (UInt_t)imat) break;
   }
   if (!mat || (mat->GetUniqueID() != (UInt_t)imat)) {
      fMaterial = 0;
      Error("TGeoMedium", "%s, material number %d does not exist",name,imat);
      return;
   }
   fMaterial = (TGeoMaterial*)mat;
   fParams[0] = isvol;
   fParams[1] = ifield;
   fParams[2] = fieldm;
   fParams[3] = tmaxfd;
   fParams[4] = stemax;
   fParams[5] = deemax;
   fParams[6] = epsil;
   fParams[7] = stmin;
   gGeoManager->GetListOfMedia()->Add(this);
}

//-----------------------------------------------------------------------------
TGeoMedium::TGeoMedium(const TGeoMedium& gm) : 
  TNamed(gm),
  fId(gm.fId),
  fMaterial(gm.fMaterial)
{
   //copy constructor
   for(Int_t i=0; i<20; i++) fParams[i]=gm.fParams[i];
}
 
//-----------------------------------------------------------------------------
TGeoMedium& TGeoMedium::operator=(const TGeoMedium& gm) 
{
   //assignment operator
   if(this!=&gm) {
      TNamed::operator=(gm);
      fId=gm.fId;
      for(Int_t i=0; i<20; i++) fParams[i]=gm.fParams[i];
      fMaterial=gm.fMaterial;
   } 
   return *this;
}
 
//-----------------------------------------------------------------------------
TGeoMedium::~TGeoMedium()
{
// Destructor
}

//_____________________________________________________________________________
char *TGeoMedium::GetPointerName() const
{
// Provide a pointer name containing uid.
   static TString name;
   name = TString::Format("pMed%d", GetUniqueID());
   return (char*)name.Data();
}    

//_____________________________________________________________________________
void TGeoMedium::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   if (TestBit(TGeoMedium::kMedSavePrimitive)) return;
   fMaterial->SavePrimitive(out,option);
   out << "// Medium: " << GetName() << endl;
   out << "   numed   = " << fId << ";  // medium number" << endl;
   out << "   par[0]  = " << fParams[0] << "; // isvol" << endl;
   out << "   par[1]  = " << fParams[1] << "; // ifield" << endl;
   out << "   par[2]  = " << fParams[2] << "; // fieldm" << endl;
   out << "   par[3]  = " << fParams[3] << "; // tmaxfd" << endl;
   out << "   par[4]  = " << fParams[4] << "; // stemax" << endl;
   out << "   par[5]  = " << fParams[5] << "; // deemax" << endl;
   out << "   par[6]  = " << fParams[6] << "; // epsil" << endl;
   out << "   par[7]  = " << fParams[7] << "; // stmin" << endl;
   
   out << "   " << GetPointerName() << " = new TGeoMedium(\"" << GetName() << "\", numed," << fMaterial->GetPointerName() << ", par);" << endl;
   SetBit(TGeoMedium::kMedSavePrimitive);
}
