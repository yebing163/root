// @(#)root/eg:$Name:  $:$Id: TVirtualMCDecayer.h,v 1.2 2006/04/24 07:56:53 brun Exp $
// Author: Andreas Morsch  13/04/2002
   
#ifndef ROOT_TVirtualMCDecayer
#define ROOT_TVirtualMCDecayer
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// Abstract base class for particle decays.
// Clients are the transport code and the primary particle generators

#include "TObject.h"
class TClonesArray;
class TLorentzVector;

class TVirtualMCDecayer : public TObject {
public:
//
   TVirtualMCDecayer() {;}
   virtual ~TVirtualMCDecayer(){;}
   virtual void    Init()                                     =0;
   virtual void    Decay(Int_t idpart, TLorentzVector* p)     =0;
   virtual Int_t   ImportParticles(TClonesArray *particles)   =0;
   virtual void    SetForceDecay(Int_t type)                  =0;
   virtual void    ForceDecay()                               =0;
   virtual Float_t GetPartialBranchingRatio(Int_t ipart)      =0;
   virtual Float_t GetLifetime(Int_t kf)                      =0;
   virtual void    ReadDecayTable()                           =0;
   ClassDef(TVirtualMCDecayer,1) // Particle Decayer Base Class
};
#endif //ROOT_TVirtualMCDecayer







