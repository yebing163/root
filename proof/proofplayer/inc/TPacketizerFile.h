// @(#)root/proofplayer:$Id$
// Author: G. Ganis 2009

/*************************************************************************
 * Copyright (C) 1995-2002, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TPacketizerFile
#define ROOT_TPacketizerFile

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPacketizerFile                                                      //
//                                                                      //
// This packetizer generates packets which conatin a single file path   //
// to be used in process. Used for tasks generating files, like in      //
// PROOF bench.                                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TVirtualPacketizer
#include "TVirtualPacketizer.h"
#endif
#ifndef ROOT_TMap
#include "TMap.h"
#endif


class TMessage;
class TList;
class TStopwatch;

class TPacketizerFile : public TVirtualPacketizer {

public:              // This is always needed
   class TSlaveStat;
   class TIterObj;

private:
   TMap       *fFiles;           // Files to be produced/processed per node
   TList      *fNotAssigned;     // List of files not assigned to a specific node
   TList      *fIters;           // Iterators on the file lists per node
   Long64_t    fAssigned;        // no. files processed or being processed.

   TStopwatch *fStopwatch;       // For measuring the start time of each packet

   TPacketizerFile();
   // : fFiles(0), fIters(0), fAssigned(0), fStopwatch(0) { }
   TPacketizerFile(const TPacketizerFile&);     // no implementation, will generate
   void operator=(const TPacketizerFile&);  // error on accidental usage

public:
   TPacketizerFile(TList *workers, Long64_t, TList *input, TProofProgressStatus *st = 0);
   virtual ~TPacketizerFile();

   TDSetElement *GetNextPacket(TSlave *wrk, TMessage *r);

   Double_t      GetCurrentTime();

   Float_t       GetCurrentRate(Bool_t &all);
   Int_t         GetActiveWorkers() { return -1; }

   ClassDef(TPacketizerFile,0)  //Generate work packets for parallel processing
};

//-------------------------------------------------------------------------------

#endif