// @(#)root/base:$Name:  $:$Id: TContextMenu.cxx,v 1.2 2001/05/24 16:23:52 brun Exp $
// Author: Nenad Buncic   08/02/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TContextMenu                                                         //
//                                                                      //
// This class provides an interface to  context sensitive popup menus.  //
// These menus pop up when the user hits the right mouse button, and    //
// are destroyed when the menu pops downs.                              //
//                                                                      //
// Context Menus are automatically generated by ROOT using the          //
// following convention: if the string // *MENU* is found in the        //
// comment field of a member function. This function will be added to   //
// the list of items in the menu.                                       //
//                                                                      //
// The picture below shows a canvas with a pop-up menu.                 //
//                                                                      //
//Begin_Html <img src="gif/hsumMenu.gif"> End_Html                      //
//                                                                      //
// The picture below shows a canvas with a pop-up menu and a dialog box.//
//                                                                      //
//Begin_Html <img src="gif/hsumDialog.gif"> End_Html                    //
//////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TContextMenu.h"
#include "TVirtualPad.h"
#include "TGuiFactory.h"
#include "TMethod.h"
#include "TMethodArg.h"
#include "TGlobal.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TToggle.h"


ClassImp(TContextMenu)


//______________________________________________________________________________
TContextMenu::TContextMenu(const char *name, const char *title)
             : TNamed(name, title)
{
   // Create a context menu.

   fSelectedObject = 0;
   fSelectedMethod = 0;
   fBrowser        = 0;
   fSelectedPad    = 0;
   fSelectedCanvas = 0;

   fContextMenuImp = gGuiFactory->CreateContextMenuImp(this, name, title);
}

//______________________________________________________________________________
TContextMenu::~TContextMenu()
{
   // Destroy a context menu.

   delete fContextMenuImp;

   fSelectedMethod = 0;
   fSelectedObject = 0;
   fContextMenuImp = 0;
}

//______________________________________________________________________________
void TContextMenu::Action(TObject *object, TMethod *method)
{
   // Action to be performed when this menu item is selected.
   // If the selected method requires arguments we popup an
   // automatically generated dialog, otherwise the method is
   // directly executed.

   if (method) {
      SetMethod( method );

      if (method->GetListOfMethodArgs()->First())
          fContextMenuImp->Dialog(object, method);
      else {
#ifndef WIN32
          Execute(object, method, "");
#else
#ifdef GDK_WIN32
          Execute(object, method, "");
#else
          // It is a workaround of the "Dead lock under Windows
          char *cmd = Form("((TContextMenu *)0x%lx)->Execute((TObject *)0x%lx,"
                           "(TMethod *)0x%lx,(TObjArray *)0);",
                           (Long_t)this,(Long_t)object,(Long_t)method);
          //Printf("%s", cmd);
          gROOT->ProcessLine(cmd);
          //Execute( object, method, (TObjArray *)NULL );
#endif
#endif
      }
   }
}

//______________________________________________________________________________
void TContextMenu::Action(TObject *object, TToggle *toggle)
{
   // Action to be performed when this toggle menu item is selected.

   if (object && toggle) {
      TVirtualPad *savedPad = 0;

      gROOT->SetSelectedPrimitive(object);
      if (fSelectedPad) {
         savedPad = (TVirtualPad *) gPad;
         if (savedPad) fSelectedPad->cd();
      }

      gROOT->SetFromPopUp(kTRUE);
      toggle->Toggle();
      if (fSelectedCanvas && fSelectedCanvas->GetPadSave()->TestBit(kNotDeleted))
         fSelectedCanvas->GetPadSave()->Modified();
      if (fSelectedPad && fSelectedPad->TestBit(kNotDeleted))
         fSelectedPad->Modified();
      gROOT->SetFromPopUp(kFALSE);

      if (fSelectedPad && savedPad) {
         if (savedPad->TestBit(kNotDeleted)) savedPad->cd();
      }

      if (fSelectedCanvas && fSelectedCanvas->TestBit(kNotDeleted))
         fSelectedCanvas->Update();
      if (fSelectedCanvas && fSelectedCanvas->GetPadSave()->TestBit(kNotDeleted))
         fSelectedCanvas->GetPadSave()->Update();
   }
}

//______________________________________________________________________________
Char_t *TContextMenu::CreateArgumentTitle(TMethodArg *argument)
{
   // Create string describing argument (for use in dialog box).

   static Char_t argTitle[128];

   if (argument) {
      sprintf(argTitle, "(%s)  %s", argument->GetTitle(), argument->GetName());
      if (argument->GetDefault() && *(argument->GetDefault())) {
         strcat(argTitle, "  [default: ");
         strcat(argTitle, argument->GetDefault());
         strcat(argTitle, "]");
      }
   } else
      *argTitle = 0;

   return argTitle;
}

//______________________________________________________________________________
Char_t *TContextMenu::CreateDialogTitle( TObject *object, TMethod *method )
{
   // Create title for dialog box retrieving argument values.

   static Char_t methodTitle[128];

   if (object && method)
      sprintf(methodTitle, "%s::%s", object->ClassName(), method->GetName());
   else
      *methodTitle = 0;

   return methodTitle;
}

//______________________________________________________________________________
Char_t *TContextMenu::CreatePopupTitle(TObject *object)
{
   // Create title for popup menu.

   static Char_t popupTitle[128];

   if (object) {
      if (!*(object->GetName()) || !strcmp(object->GetName(), object->ClassName())) {
         TGlobal *global = (TGlobal *) gROOT->GetGlobal(object);
         if (global && *(global->GetName()))
            sprintf(popupTitle, "  %s::%s  ", object->ClassName(), global->GetName());
         else
            sprintf(popupTitle, "  %s  ", object->ClassName());
      } else
         sprintf(popupTitle, "  %s::%s  ", object->ClassName(), object->GetName());
   } else
      *popupTitle = 0;

   return popupTitle;
}

//______________________________________________________________________________
void TContextMenu::Execute( TObject *object, TMethod *method, const Char_t *params )
{
   // Execute method with specified arguments for specified object.

   if (object && method) {
      TVirtualPad *savedPad = 0;

      gROOT->SetSelectedPrimitive(object);
      if (fSelectedPad) {
         savedPad = (TVirtualPad *) gPad;
         if (savedPad) fSelectedPad->cd();
      }

      gROOT->SetFromPopUp(kTRUE);
//      if (fSelectedCanvas) fSelectedCanvas->GetPadSave()->cd();
      object->Execute((char *) method->GetName(), params);
      if (fSelectedCanvas && fSelectedCanvas->GetPadSave()->TestBit(kNotDeleted))
         fSelectedCanvas->GetPadSave()->Modified();
      if (fSelectedPad && fSelectedPad->TestBit(kNotDeleted))
         fSelectedPad->Modified();
      gROOT->SetFromPopUp( kFALSE );

      if (fSelectedPad && savedPad) {
//         fSelectedPad->Modified();
         if (savedPad->TestBit(kNotDeleted)) savedPad->cd();
      }

      if (fSelectedCanvas && fSelectedCanvas->TestBit(kNotDeleted))
         fSelectedCanvas->Update();
      if (fSelectedCanvas && fSelectedCanvas->GetPadSave()->TestBit(kNotDeleted))
         fSelectedCanvas->GetPadSave()->Update();
   }
}

//______________________________________________________________________________
void TContextMenu::Execute( TObject *object, TMethod *method, TObjArray *params )
{
   // Execute method with specified arguments for specified object.

   if (object && method) {
      TVirtualPad *savedPad = 0;

      gROOT->SetSelectedPrimitive(object);
      if (fSelectedPad) {
         savedPad = (TVirtualPad *) gPad;
         if (savedPad) fSelectedPad->cd();
      }

      gROOT->SetFromPopUp(kTRUE);
      if (fSelectedCanvas) fSelectedCanvas->GetPadSave()->cd();
      object->Execute(method, params);
      if (fSelectedCanvas && fSelectedCanvas->GetPadSave()->TestBit(kNotDeleted))
         fSelectedCanvas->GetPadSave()->Modified();
      gROOT->SetFromPopUp( kFALSE );

      if (fSelectedPad && savedPad) {
//         fSelectedPad->Modified();
         if (savedPad->TestBit(kNotDeleted)) savedPad->cd();
      }

      if (fSelectedCanvas && fSelectedCanvas->TestBit(kNotDeleted))
         fSelectedCanvas->Update();
   }
}

//______________________________________________________________________________
void TContextMenu::Popup( Int_t x, Int_t y, TObject *obj, TVirtualPad *c, TVirtualPad *p )
{
   // Popup context menu at given location in canvas c and pad p for selected
   // object.

   SetBrowser(0);
   SetObject(obj);
   SetCanvas(c);
   SetPad(p);

   DisplayPopUp(x,y);
}

//______________________________________________________________________________
void TContextMenu::Popup( Int_t x, Int_t y, TObject *obj, TBrowser *b)
{
   // Popup context menu at given location in browser b for selected object.

   SetBrowser(b);
   SetObject(obj);
   SetCanvas(0);
   SetPad(0);

   DisplayPopUp(x,y);
}
