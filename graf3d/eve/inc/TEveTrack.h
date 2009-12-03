// @(#)root/eve:$Id$
// Authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TEveTrack
#define ROOT_TEveTrack

#include <vector>

#include "TEveVector.h"
#include "TEveElement.h"
#include "TEveLine.h"

#include "TPolyMarker3D.h"
#include "TMarker.h"

class TEveTrackPropagator;
class TEveTrackList;

class TEveRecTrack;
class TEveMCTrack;
class TParticle;

class TEveTrack : public TEveLine
{
   friend class TEveTrackPropagator;
   friend class TEveTrackList;
   friend class TEveTrackGL;

private:
   TEveTrack& operator=(const TEveTrack&); // Not implemented

public:
   typedef std::vector<TEvePathMark>    vPathMark_t;
   typedef vPathMark_t::iterator        vPathMark_i;
   typedef vPathMark_t::const_iterator  vPathMark_ci;

   enum EBreakProjectedTracks_e { kBPTDefault, kBPTAlways, kBPTNever };

protected:
   TEveVector         fV;          // Starting vertex
   TEveVector         fP;          // Starting momentum
   TEveVector         fPEnd;       // Momentum at the last point of extrapolation
   Double_t           fBeta;       // Relativistic beta factor
   Int_t              fPdg;        // PDG code
   Int_t              fCharge;     // Charge in units of e0
   Int_t              fLabel;      // Simulation label
   Int_t              fIndex;      // Reconstruction index
   Int_t              fStatus;     // Status-word, user-defined.
   Bool_t             fLockPoints; // Lock points that are currently in - do nothing in MakeTrack().
   vPathMark_t        fPathMarks;  // TEveVector of known points along the track

   TEveTrackPropagator *fPropagator;   // Pointer to shared render-style

   UChar_t            fBreakProjectedTracks;  // How to handle breaks during projection - track specific settings.

   static Bool_t      fgDefaultBreakProjectedTracks; // How to handle breaks during projection - global setting.

public:
   TEveTrack();
   TEveTrack(TParticle* t, Int_t label, TEveTrackPropagator* prop=0);
   TEveTrack(TEveMCTrack*  t, TEveTrackPropagator* prop=0);
   TEveTrack(TEveRecTrack* t, TEveTrackPropagator* prop=0);
   TEveTrack(const TEveTrack& t);
   virtual ~TEveTrack();

   virtual void SetStdTitle();

   virtual void SetTrackParams(const TEveTrack& t);
   virtual void SetPathMarks  (const TEveTrack& t);

   virtual void MakeTrack(Bool_t recurse=kTRUE);

   TEveTrackPropagator* GetPropagator() const  { return fPropagator; }
   void SetPropagator(TEveTrackPropagator* prop);
   void SetAttLineAttMarker(TEveTrackList* tl);

   const TEveVector& GetVertex()      const { return fV;    }
   const TEveVector& GetMomentum()    const { return fP;    }
   const TEveVector& GetEndMomentum() const { return fPEnd; }

   Int_t GetPdg()    const    { return fPdg;    }
   void  SetPdg(Int_t pdg)    { fPdg = pdg;     }
   Int_t GetCharge() const    { return fCharge; }
   void  SetCharge(Int_t chg) { fCharge = chg;  }
   Int_t GetLabel()  const    { return fLabel;  }
   void  SetLabel(Int_t lbl)  { fLabel = lbl;   }
   Int_t GetIndex()  const    { return fIndex;  }
   void  SetIndex(Int_t idx)  { fIndex = idx;   }
   Int_t GetStatus()  const   { return fStatus; }
   void  SetStatus(Int_t idx) { fStatus = idx;  }

   void  AddPathMark(const TEvePathMark& pm) { fPathMarks.push_back(pm); }
   void  SortPathMarksByTime();
         vPathMark_t& RefPathMarks()       { return fPathMarks; }
   const vPathMark_t& RefPathMarks() const { return fPathMarks; }

   void  PrintPathMarks(); // *MENU*

   void   SetLockPoints(Bool_t l) { fLockPoints = l;    }
   Bool_t GetLockPoints()   const { return fLockPoints; }

   //-------------------------------------------------------------------

   virtual void SecSelected(TEveTrack*); // *SIGNAL*

   virtual const TGPicture* GetListTreeIcon(Bool_t open=kFALSE);

   virtual void CopyVizParams(const TEveElement* el);
   virtual void WriteVizParams(ostream& out, const TString& var);

   virtual TClass* ProjectedClass(const TEveProjection* p) const;

   Bool_t  ShouldBreakTrack() const;

   UChar_t GetBreakProjectedTracks() const     { return fBreakProjectedTracks; }
   void    SetBreakProjectedTracks(UChar_t bt) { fBreakProjectedTracks = bt;   }

   static Bool_t GetDefaultBreakProjectedTracks()          { return fgDefaultBreakProjectedTracks; }
   static void   SetDefaultBreakProjectedTracks(Bool_t bt) { fgDefaultBreakProjectedTracks = bt;   }

   ClassDef(TEveTrack, 1); // Track with given vertex, momentum and optional referece-points (path-marks) along its path.
};

/******************************************************************************/
// TEveTrackList
/******************************************************************************/

class TEveTrackList : public TEveElementList,
		      public TAttMarker,
		      public TAttLine
{
   friend class TEveTrackListEditor;

private:
   TEveTrackList(const TEveTrackList&);            // Not implemented
   TEveTrackList& operator=(const TEveTrackList&); // Not implemented

protected:
   TEveTrackPropagator* fPropagator;   // Basic track rendering parameters, not enforced to elements.

   Bool_t               fRecurse;    // Recurse when propagating marker/line/etc attributes to tracks.

   Bool_t               fRnrLine;    // Render track as line.
   Bool_t               fRnrPoints;  // Render track as points.

   Float_t              fMinPt;      // Minimum track pT for display selection.
   Float_t              fMaxPt;      // Maximum track pT for display selection.
   Float_t              fLimPt;      // Highest track pT in the container.
   Float_t              fMinP;       // Minimum track p for display selection.
   Float_t              fMaxP;       // Maximum track p for display selection.
   Float_t              fLimP;       // Highest track p in the container.

   void    FindMomentumLimits(TEveElement* el, Bool_t recurse=kTRUE);
   Float_t RoundMomentumLimit(Float_t x);
   void    SanitizeMinMaxCuts();

public:
   TEveTrackList(TEveTrackPropagator* prop=0);
   TEveTrackList(const char* name, TEveTrackPropagator* prop=0);
   virtual ~TEveTrackList();

   void  MakeTracks(Bool_t recurse=kTRUE);
   void  FindMomentumLimits(Bool_t recurse=kTRUE);

   void  SetPropagator(TEveTrackPropagator* prop);
   TEveTrackPropagator* GetPropagator() { return fPropagator; }

   Bool_t GetRecurse() const   { return fRecurse; }
   void   SetRecurse(Bool_t x) { fRecurse = x; }

   //--------------------------------

   virtual void   SetMainColor(Color_t c);
   virtual void   SetLineColor(Color_t c) { SetMainColor(c); }
   virtual void   SetLineColor(Color_t c, TEveElement* el);
   virtual void   SetLineWidth(Width_t w);
   virtual void   SetLineWidth(Width_t w, TEveElement* el);
   virtual void   SetLineStyle(Style_t s);
   virtual void   SetLineStyle(Style_t s, TEveElement* el);

   virtual void   SetMarkerColor(Color_t c);
   virtual void   SetMarkerColor(Color_t c, TEveElement* el);
   virtual void   SetMarkerSize(Size_t s);
   virtual void   SetMarkerSize(Size_t s, TEveElement* el);
   virtual void   SetMarkerStyle(Style_t s);
   virtual void   SetMarkerStyle(Style_t s, TEveElement* el);

   void   SetRnrLine(Bool_t rnr);
   void   SetRnrLine(Bool_t rnr, TEveElement* el);
   Bool_t GetRnrLine() const { return fRnrLine; }

   void   SetRnrPoints(Bool_t r);
   void   SetRnrPoints(Bool_t r, TEveElement* el);
   Bool_t GetRnrPoints() const { return fRnrPoints; }

   void SelectByPt(Float_t min_pt, Float_t max_pt);
   void SelectByPt(Float_t min_pt, Float_t max_pt, TEveElement* el);
   void SelectByP (Float_t min_p,  Float_t max_p);
   void SelectByP (Float_t min_p,  Float_t max_p,  TEveElement* el);

   Float_t GetMinPt() const { return fMinPt; }
   Float_t GetMaxPt() const { return fMaxPt; }
   Float_t GetLimPt() const { return fLimPt; }
   Float_t GetMinP()  const { return fMinP;  }
   Float_t GetMaxP()  const { return fMaxP;  }
   Float_t GetLimP()  const { return fLimP;  }

   //-------------------------------------------------------------------

   TEveTrack* FindTrackByLabel(Int_t label); // *MENU*
   TEveTrack* FindTrackByIndex(Int_t index); // *MENU*

   virtual void CopyVizParams(const TEveElement* el);
   virtual void WriteVizParams(ostream& out, const TString& var);

   virtual TClass* ProjectedClass(const TEveProjection* p) const;

   ClassDef(TEveTrackList, 1); // A list of tracks supporting change of common attributes and selection based on track parameters.
};

#endif
