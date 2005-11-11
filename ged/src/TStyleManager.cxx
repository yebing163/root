// @(#)root/ged:$Name:  $:$Id: TStyleManager.cxx,v 1.0 2005/09/08
// Author: Denis Favre-Miville   08/09/05

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TStyleManager                                                       //
//                                                                      //
//  This class provides a Graphical User Interface to manage styles     //
//       in ROOT. It allows the user to edit styles, import / export    //
//       them to macros, apply a style on the selected object or on     //
//       all canvases, change gStyle.                                   //
//                                                                      //
//  The Style Manager interface is composed of two parts:               //
//  - the top level interface that manages a list of styles;            //
//  - the style editor, which deals with the current style settings.    //
//                                                                      //
//Begin_Html
/*
<img src="gif/StyleManager.gif">
*/
//End_Html
//                                                                      //
// The combo box 'Available Styles' contains the list of available      //
// styles for the current ROOT session and shows the currently          //
// selected one. The field on the right shows the setting of the gStyle.//
// You can set the global variable gStyle to the selected style by      //
// clicking on the button in the middle.                                //
// The group frame 'Apply on' displays information for the currently    //
// selected canvas and object in the ROOT session. This selection might //
// be changed by clicking on another object with the middle mouse       //
// button. You have a choice to apply a style on the selected object or //
// on all available canvases.                                           //
// WARNING: You cannot undo the changes after applying the style! If    //
// you are not sure of that action, it may be better to see a preview   //
// of what you are going to apply.                                      //
// If the check button 'Preview' is selected, a preview of the selected //
// canvas according to the selected style will be shown. The selection  //
// of the next check button 'Run Time Preview' will apply updates of    //
// the preview any time a value of the selected style is changed. For   //
// drawings that take a time it is better to disable this option.       //
//                                                                      //
// Create a new style:                                                  //
// A new style can be created via the Style menu/New... or the toolbar. //
// A clone of the selected style will be used as a base of the new      //
// style. All its values can be modified via the style editor later.    //
// The dialog that appears will ask for the name and description of the //
// new style.                                                           //
//                                                                      //
// Import a style (from a macro):                                       //
// A style macro can be imported at any time. The new imported style in //
// the ROOT session will become the selected one.                       //
//                                                                      //
// Import a style (from a canvas):                                      //
// You can do that selecting the Style menu/Import from.../Canvas or    //
// the corresponding Tool bar button. A new style will be created in the//
// ROOT session and will become the selected one. This style is a clone //
// of the gStyle with modified values as they are set in the currently  //
// selected canvas. You can import a style from any canvas and apply it //
// later on some objects.                                               //
//                                                                      //
// Export a style (in a C++ macro file):                                //
// To store a style longer than for the current ROOT session you can    //
// save it in a C++ macro file. This can be done via the menu or the    //
// tool bar button. There is a naming convention for the style macros:  //
// the name must be 'Style_*.C', where * can be replaced by anything    //
// you want.                                                            //
//                                                                      //
// Delete a style:                                                      //
// The selected style can be deleted from the list when you use the     //
// Style menu/Delete or the corresponding tool bar button. The selected //
// style is removed from the list of all available styles for the       //
// current ROOT session. WARRNING: it will be lost if you didn't saved  //
// it in a C++ macro file before its deletion. Also, you cannot delete  //
// the selected style if it is set to gStyle. A message 'Can not delete //
// gStyle' will be displayed on the CINT prompt.                        //
//                                                                      //
//Begin_Html
/*
<img src="gif/StyleEditor.gif">
*/
//End_Html
//                                                                      //
// Editor's buttons:                                                    //
// Open / close the style editor:                                       //
// The button 'Edit >>' opens the style editor and its label changes to //
// 'Close <<'. For all details of what can be changed and how please see//
// the provided Help.                                                   //
//                                                                      //
// Reset a style (to a previously saved state):                         //
// When the editor is opened, the 'Reset' button allows you to reset    //
// the values of the selected style for editing. Doing that you cancel  //
// all changes made since the last time you saved that style in a macro.//
// If the selected style is one of the five ROOT styles (Plain, Bold,   //
// Video, Pub or  Default), it will be recreated.                       //
//                                                                      //
// Update the preview:                                                  //
// The button 'Update Preview' is available when a preview is shown and //
// the run time option is not selected. This button allows you to       //
// refresh the preview any time you want to see how the style you edit  //
// looks like.                                                          //
//                                                                      //
// Help button:                                                         //
// Provides a help of the currently selected tab.                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TStyleManager.h"
#include "TStyleDialog.h"
#include "TStylePreview.h"
#include "HelpSMText.h"

#include <TCanvas.h>
#include <TColor.h>
#include <TG3DLine.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGColorSelect.h>
#include <TGComboBox.h>
#include <TGedMarkerSelect.h>
#include <TGedPatternSelect.h>
#include <TGFileDialog.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGLayout.h>
#include <TGMenu.h>
#include <TGMsgBox.h>
#include <TGNumberEntry.h>
#include <TGResourcePool.h>
#include <TGStatusBar.h>
#include <TGTab.h>
#include <TGToolBar.h>
#include <TRootHelpDialog.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TVirtualPad.h>

TStyleManager *TStyleManager::fgStyleManager = 0;

ClassImp(TStyleManager)

enum EStyleManagerWid {
   kMenuNew,
   kMenuDelete,
   kMenuRename,
   kMenuImportCanvas,
   kMenuImportMacro,
   kMenuExport,
   kMenuExit,
   kMenuHelp,
   kMenuHelpEditor,
   kMenuHelpGeneral,
   kMenuHelpCanvas,
   kMenuHelpPad,
   kMenuHelpHistos,
   kMenuHelpAxis,
   kMenuHelpTitle,
   kMenuHelpStats,
   kMenuHelpPSPDF,

   kToolbarNew,
   kToolbarDelete,
   kToolbarImportCanvas,
   kToolbarImportMacro,
   kToolbarExport,
   kToolbarHelp,

   kTopStylesList,
   kTopPreview,
   kTopPreviewRealTime,
   kTopMakeDefault,
   kTopCurStyle,
   kTopCurPad,
   kTopCurObj,
   kTopApplyOnAll,
   kTopApplyOnSel,
   kTopApplyOnBut,
   kTopMoreLess,

   kEditButHelp,
   kEditButUpPrev,
   kEditButReset,

   kGeneralFillColor,
   kGeneralFillStyle,
   kGeneralHatchesLineWidth,
   kGeneralHatchesSpacing,
   kGeneralTextColor,
   kGeneralTextSize,
   kGeneralTextSizeInPixels,
   kGeneralTextFont,
   kGeneralTextAlign,
   kGeneralTextAngle,
   kGeneralLineColor,
   kGeneralLineWidth,
   kGeneralLineStyle,
   kGeneralLineStyleEdit,
   kGeneralMarkerColor,
   kGeneralMarkerStyle,
   kGeneralMarkerSize,
   kGeneralScreenFactor,

   kCanvasColor,
   kCanvasDefX,
   kCanvasDefY,
   kCanvasDefW,
   kCanvasDefH,
   kCanvasBorderModeSunken,
   kCanvasBorderModeNone,
   kCanvasBorderModeRaised,
   kCanvasBorderSize,
   kCanvasOptDateBool,
   kCanvasAttDateTextColor,
   kCanvasAttDateTextSize,
   kCanvasAttDateTextSizeInPixels,
   kCanvasOptDateFormat,
   kCanvasAttDateTextFont,
   kCanvasAttDateTextAngle,
   kCanvasAttDateTextAlign,
   kCanvasDateX,
   kCanvasDateY,

   kPadLeftMargin,
   kPadRightMargin,
   kPadTopMargin,
   kPadBottomMargin,
   kPadBorderModeSunken,
   kPadBorderModeNone,
   kPadBorderModeRaised,
   kPadBorderSize,
   kPadColor,
   kPadTickX,
   kPadTickY,
   kPadGridX,
   kPadGridY,
   kPadGridColor,
   kPadGridWidth,
   kPadGridStyle,

   kHistFillColor,
   kHistFillStyle,
   kHistLineColor,
   kHistLineWidth,
   kHistLineStyle,
   kHistBarWidth,
   kHistBarOffset,
   kHistMinimumZero,
   kHistPaintTextFormat,
   kHistNumberContours,
   kHistLegoInnerR,

   kFrameFillColor,
   kFrameFillStyle,
   kFrameLineColor,
   kFrameLineWidth,
   kFrameLineStyle,
   kFramePaletteEdit,
   kFrameBorderModeSunken,
   kFrameBorderModeNone,
   kFrameBorderModeRaised,
   kFrameBorderSize,

   kGraphsFuncColor,
   kGraphsFuncWidth,
   kGraphsFuncStyle,
   kGraphsDrawBorder,
   kGraphsEndErrorSize,
   kGraphsErrorX,

   kAxisTimeOffsetDate,
   kAxisTimeOffsetTime,
   kAxisStripDecimals,
   kAxisApplyOnXYZ,

   kAxisXTitleSize,
   kAxisXTitleSizeInPixels,
   kAxisXTitleColor,
   kAxisXTitleOffset,
   kAxisXTitleFont,
   kAxisXLabelSize,
   kAxisXLabelSizeInPixels,
   kAxisXLabelColor,
   kAxisXLabelOffset,
   kAxisXLabelFont,
   kAxisXAxisColor,
   kAxisXTickLength,
   kAxisOptLogx,
   kAxisXNdivMain,
   kAxisXNdivSub,
   kAxisXNdivSubSub,
   kAxisXNdivisionsOptimize,

   kAxisYTitleSize,
   kAxisYTitleSizeInPixels,
   kAxisYTitleColor,
   kAxisYTitleOffset,
   kAxisYTitleFont,
   kAxisYLabelSize,
   kAxisYLabelSizeInPixels,
   kAxisYLabelColor,
   kAxisYLabelOffset,
   kAxisYLabelFont,
   kAxisYAxisColor,
   kAxisYTickLength,
   kAxisOptLogy,
   kAxisYNdivMain,
   kAxisYNdivSub,
   kAxisYNdivSubSub,
   kAxisYNdivisionsOptimize,

   kAxisZTitleSize,
   kAxisZTitleSizeInPixels,
   kAxisZTitleColor,
   kAxisZTitleOffset,
   kAxisZTitleFont,
   kAxisZLabelSize,
   kAxisZLabelSizeInPixels,
   kAxisZLabelColor,
   kAxisZLabelOffset,
   kAxisZLabelFont,
   kAxisZAxisColor,
   kAxisZTickLength,
   kAxisOptLogz,
   kAxisZNdivMain,
   kAxisZNdivSub,
   kAxisZNdivSubSub,
   kAxisZNdivisionsOptimize,

   kTitleOptTitle,
   kTitleFillColor,
   kTitleStyle,
   kTitleTextColor,
   kTitleFontSize,
   kTitleFontSizeInPixels,
   kTitleFont,
   kTitleAlign,
   kTitleBorderSize,
   kTitleX,
   kTitleY,
   kTitleW,
   kTitleH,
   kTitleLegendBorderSize,

   kStatColor,
   kStatStyle,
   kStatTextColor,
   kStatFontSize,
   kStatFontSizeInPixels,
   kStatFont,
   kStatX,
   kStatY,
   kStatW,
   kStatH,
   kStatBorderSize,
   kStatOptStatName,
   kStatOptStatEntries,
   kStatOptStatOverflow,
   kStatOptStatMean,
   kStatOptStatUnderflow,
   kStatOptStatRMS,
   kStatOptStatSkewness,
   kStatOptStatIntegral,
   kStatOptStatKurtosis,
   kStatOptStatErrors,
   kStatFormat,
   kStatOptFitValues,
   kStatOptFitErrors,
   kStatOptFitProbability,
   kStatOptFitChi,
   kStatFitFormat,

   kPSPDFHeaderPS,
   kPSPDFTitlePS,
   kPSPDFColorModelPS,
   kPSPDFColorModelPSRGB,
   kPSPDFColorModelPSCMYK,
   kPSPDFLineScalePS,
   kPSPDFPaperSizePredef,
   kPSPDFPaperSizeX,
   kPSPDFPaperSizeY
};

const char *kFiletypes[] = { "ROOT macros", "Style_*.C",
                               0,             0 };

//______________________________________________________________________________
TStyleManager::TStyleManager(const TGWindow *p) : TGMainFrame(p)
{
   // Constructor. Create the main window of the style manager.

   SetWindowName("Style Manager");
   SetCleanup(kNoCleanup);

   // Initialization: no selected style, no preview, no signal/slots,
   //                 no selected object, no current macro file.
   fCurSelStyle = 0;
   fCurMacro = 0;
   fCurPad = 0;
   fCurObj = 0;
   fPreviewWindow = 0;
   fRealTimePreview = kFALSE;
   fCurTabNum = 0;
   fCurTabAxisNum = 0;
   fMoreAndNotLess = kTRUE;
   fSigSlotConnected = kFALSE;
   fStyleChanged = kFALSE;

   // Create the trash lists to have an effective deletion of every object.
   fTrashListLayout = new TList();
   fTrashListFrame = new TList();

   // To avoid to create a lot a copies of the often used layouts.
   fLayoutExpandX = new TGLayoutHints(kLHintsExpandX);
   fTrashListLayout->Add(fLayoutExpandX);
   fLayoutExpandXMargin = new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5);
   fTrashListLayout->Add(fLayoutExpandXMargin);
   fLayoutExpandXY = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   fTrashListLayout->Add(fLayoutExpandXY);
   fLayoutExpandXYMargin = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);
   fTrashListLayout->Add(fLayoutExpandXYMargin);
   fLayoutExpandXCenterYMargin = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 0, 0, 3, 3);
   fTrashListLayout->Add(fLayoutExpandXCenterYMargin);

   // Build the graphical interface.
   AddMenus(this);
   AddToolbar(this);
   AddTopLevelInterface(this);
   AddEdition(this);

   // Add status bar.
   fStatusBar = new TGStatusBar(this, 50, 10, kVerticalFrame);
   Int_t parts[] = { 20, 30, 50 };
   fStatusBar->SetParts(parts, 3);
   fStatusBar->Draw3DCorner(kFALSE);
   AddFrame(fStatusBar, fLayoutExpandX);

   // Initialize the layout algorithm and map the main frame.
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();

   //  Ensure the editor will be visible (not out of the screen's range) when
   // the user will press the 'More' button, if he didn't move the window.
   Int_t x, y;
   UInt_t w, h;
   gVirtualX->GetWindowSize(GetId(), x, y, w, h);
   fSMWidth = w;
   fSMHeight = h;
   if (fSMWidth < 467) fSMWidth = 467;
   if (fSMHeight < 708) fSMHeight = 708;
   Window_t win;
   gVirtualX->TranslateCoordinates(GetId(), GetParent()->GetId(), 0, 0, x, y, win);
   x -= 6;
   y -= 21;
   MoveResize(x, TMath::Max(TMath::Min(y, (Int_t) (gClient->GetDisplayHeight() - h)), 0), w, h);

   // Only the top level interface is shown, at the begining.
   DoMoreLess();

   // Connect all widgets (excluding editor).
   ConnectAll();

   Init();
}

//______________________________________________________________________________
TStyleManager::~TStyleManager()
{
   // Destructor.

   // Disconnect all widgets
   DisconnectAll();
   DisconnectEditor(fCurTabNum);

   if (fPreviewWindow) {
      DoPreviewClosed();
      delete fPreviewWindow;
   }

   //  Delete every graphical data member,
   // excluding fPreviewWindow and fLayout[..].
   gClient->FreePicture(fToolBarNewPic);
   gClient->FreePicture(fToolBarDeletePic);
   gClient->FreePicture(fToolBarImportCanvasPic);
   gClient->FreePicture(fToolBarImportMacroPic);
   gClient->FreePicture(fToolBarExportPic);
   gClient->FreePicture(fToolBarHelpPic);
   gClient->FreePicture(fMakeDefaultPic);

   delete fImportCascade;
   delete fMenuStyle;
   delete fMenuHelp;
   delete fMenuBar;

   delete fToolBar;
   delete fToolBarNew;
   delete fToolBarDelete;
   delete fToolBarImportCanvas;
   delete fToolBarImportMacro;
   delete fToolBarExport;
   delete fToolBarHelp;
   delete fHorizontal3DLine;

   delete fListLabel;
   delete fListComboBox;
   delete fCurMacro;
   delete fCurStylabel;
   delete fCurStyle;
   delete fCurPadLabel;
   delete fCurPadTextEntry;
   delete fCurObjLabel;
   delete fCurObjTextEntry;
   delete fPreviewButton;
   delete fPreviewRealTime;
   delete fMakeDefault;

   delete fApplyOnGroup;
   delete fApplyOnAll;
   delete fApplyOnSel;
   delete fApplyOnButton;
   delete fMoreLess;

   delete fFillColor;
   delete fFillStyle;
   delete fHatchesLineWidth;
   delete fHatchesSpacing;
   delete fTextColor;
   delete fTextSize;
   delete fTextSizeInPixels;
   delete fTextFont;
   delete fTextAlign;
   delete fTextAngle;
   delete fLineColor;
   delete fLineWidth;
   delete fLineStyle;
   delete fLineStyleEdit;
   delete fMarkerColor;
   delete fMarkerStyle;
   delete fMarkerSize;
   delete fScreenFactor;
   delete fCanvasColor;
   delete fCanvasDefX;
   delete fCanvasDefY;
   delete fCanvasDefW;
   delete fCanvasDefH;
   delete fCanvasBorderMode;
   delete fCanvasBorderSize;
   delete fOptDateBool;
   delete fAttDateTextColor;
   delete fAttDateTextSize;
   delete fAttDateTextSizeInPixels;
   delete fOptDateFormat;
   delete fAttDateTextFont;
   delete fAttDateTextAngle;
   delete fAttDateTextAlign;
   delete fDateX;
   delete fDateY;
   delete fPadLeftMargin;
   delete fPadRightMargin;
   delete fPadTopMargin;
   delete fPadBottomMargin;
   delete fPadBorderMode;
   delete fPadBorderSize;
   delete fPadColor;
   delete fPadTickX;
   delete fPadTickY;
   delete fPadGridX;
   delete fPadGridY;
   delete fGridColor;
   delete fGridWidth;
   delete fGridStyle;
   delete fHistFillColor;
   delete fHistFillStyle;
   delete fHistLineColor;
   delete fHistLineWidth;
   delete fHistLineStyle;
   delete fBarWidth;
   delete fBarOffset;
   delete fHistMinimumZero;
   delete fPaintTextFormat;
   delete fNumberContours;
   delete fLegoInnerR;
   delete fFrameFillColor;
   delete fFrameFillStyle;
   delete fFrameLineColor;
   delete fFrameLineWidth;
   delete fFrameLineStyle;
   delete fPaletteEdit;
   delete fFrameBorderMode;
   delete fFrameBorderSize;
   delete fFuncColor;
   delete fFuncWidth;
   delete fFuncStyle;
   delete fDrawBorder;
   delete fEndErrorSize;
   delete fErrorX;
   delete fTimeOffsetDate;
   delete fTimeOffsetTime;
   delete fStripDecimals;
   delete fApplyOnXYZ;
   delete fXTitleSize;
   delete fXTitleSizeInPixels;
   delete fXTitleColor;
   delete fXTitleOffset;
   delete fXTitleFont;
   delete fXLabelSize;
   delete fXLabelSizeInPixels;
   delete fXLabelColor;
   delete fXLabelOffset;
   delete fXLabelFont;
   delete fXAxisColor;
   delete fXTickLength;
   delete fOptLogx;
   delete fXNdivMain;
   delete fXNdivSub;
   delete fXNdivSubSub;
   delete fXNdivisionsOptimize;
   delete fYTitleSize;
   delete fYTitleSizeInPixels;
   delete fYTitleColor;
   delete fYTitleOffset;
   delete fYTitleFont;
   delete fYLabelSize;
   delete fYLabelSizeInPixels;
   delete fYLabelColor;
   delete fYLabelOffset;
   delete fYLabelFont;
   delete fYAxisColor;
   delete fYTickLength;
   delete fOptLogy;
   delete fYNdivMain;
   delete fYNdivSub;
   delete fYNdivSubSub;
   delete fYNdivisionsOptimize;
   delete fZTitleSize;
   delete fZTitleSizeInPixels;
   delete fZTitleColor;
   delete fZTitleOffset;
   delete fZTitleFont;
   delete fZLabelSize;
   delete fZLabelSizeInPixels;
   delete fZLabelColor;
   delete fZLabelOffset;
   delete fZLabelFont;
   delete fZAxisColor;
   delete fZTickLength;
   delete fOptLogz;
   delete fZNdivMain;
   delete fZNdivSub;
   delete fZNdivSubSub;
   delete fZNdivisionsOptimize;
   delete fOptTitle;
   delete fTitleColor;
   delete fTitleStyle;
   delete fTitleTextColor;
   delete fTitleFontSize;
   delete fTitleFontSizeInPixels;
   delete fTitleFont;
   delete fTitleAlign;
   delete fTitleBorderSizeLabel;
   delete fTitleBorderSize;
   delete fTitleX;
   delete fTitleY;
   delete fTitleW;
   delete fTitleH;
   delete fLegendBorderSizeLabel;
   delete fLegendBorderSize;
   delete fStatColor;
   delete fStatStyle;
   delete fStatTextColor;
   delete fStatFontSize;
   delete fStatFontSizeInPixels;
   delete fStatFont;
   delete fStatX;
   delete fStatY;
   delete fStatW;
   delete fStatH;
   delete fStatBorderSizeLabel;
   delete fStatBorderSize;
   delete fOptStatName;
   delete fOptStatEntries;
   delete fOptStatOverflow;
   delete fOptStatMean;
   delete fOptStatUnderflow;
   delete fOptStatRMS;
   delete fOptStatSkewness;
   delete fOptStatIntegral;
   delete fOptStatKurtosis;
   delete fOptStatErrors;
   delete fStatFormatLabel;
   delete fStatFormat;
   delete fOptFitValues;
   delete fOptFitErrors;
   delete fOptFitProbability;
   delete fOptFitChi;
   delete fFitFormatLabel;
   delete fFitFormat;
   delete fHeaderPS;
   delete fTitlePS;
   delete fColorModelPS;
   delete fColorModelPSRGB;
   delete fColorModelPSCMYK;
   delete fLineScalePS;
   delete fPaperSizePredef;
   delete fPaperSizeX;
   delete fPaperSizeY;
   delete fEditionHelp;
   delete fEditionUpdatePreview;
   delete fEditionReset;
   delete fEditionButtonFrame;
   delete fHistosTab;
   delete fAxisTab;
   delete fEditionTab;
   delete fEditionFrame;

   delete fStatusBar;

   // Delete the temporary frames and layout.
   TObject *obj1;
   TObject *obj2;

   obj1 = fTrashListFrame->First();
   while (obj1) {
      obj2 = fTrashListFrame->After(obj1);
      fTrashListFrame->Remove(obj1);
      delete obj1;
      obj1 = obj2;
   }
   delete fTrashListFrame;

   obj1 = fTrashListLayout->First();
   while (obj1) {
      obj2 = fTrashListLayout->After(obj1);
      fTrashListLayout->Remove(obj1);
      delete obj1;
      obj1 = obj2;
   }
   delete fTrashListLayout;

   fgStyleManager = 0;
}

//______________________________________________________________________________
void TStyleManager::Init()
{
   // Set up the interface. Called by the ctor or by the 'Show' method.

   // Build the list of available styles and select gStyle.
   BuildList(gStyle);

   // Show the current object.
   if ((gROOT->GetSelectedPad()) && (gROOT->GetSelectedPad()->GetCanvas())) {
      DoSelectCanvas(gROOT->GetSelectedPad()->GetCanvas(),
                     gROOT->GetSelectedPad()->GetCanvas(), kButton2Down);
   } else {
      DoSelectNoCanvas();
   }
}

//______________________________________________________________________________
void TStyleManager::Hide()
{
   // Called to hide the style manager.

   if (fgStyleManager) {
      fgStyleManager->UnmapWindow();
   }
}

//______________________________________________________________________________
void TStyleManager::Show()
{
   // Called to show the style manager. Static method.

   if (fgStyleManager) {
      fgStyleManager->Init();
      if (!fgStyleManager->IsMapped()) {
         fgStyleManager->MapWindow();
      }
   } else {
      TStyleManager::GetSM() = new TStyleManager(gClient->GetRoot());
   }
}

//______________________________________________________________________________
void TStyleManager::Terminate()
{
   //  Called to delete the style manager. Called when the ROOT session is
   // closed via a canvas' menu.

   delete fgStyleManager;
   fgStyleManager = 0;
}

//______________________________________________________________________________
void TStyleManager::AddMenus(TGCompositeFrame *p)
{
   // Add the menu bar to the frame 'p'.

   fMenuBar = new TGMenuBar(p);

   fMenuStyle = new TGPopupMenu(gClient->GetRoot());
   fMenuStyle->Associate(this);
   fMenuStyle->AddEntry("&New...", kMenuNew);
   fMenuStyle->AddEntry("&Delete", kMenuDelete);
   fMenuStyle->AddSeparator();
   fMenuStyle->AddEntry("&Rename...", kMenuRename);
   fMenuStyle->AddSeparator();
   fImportCascade = new TGPopupMenu(gClient->GetRoot());
   fImportCascade->Associate(this);
   fImportCascade->AddEntry("&Macro...", kMenuImportMacro);
   fImportCascade->AddEntry("&Canvas...", kMenuImportCanvas);
   fMenuStyle->AddPopup("&Import From...", fImportCascade);

   fMenuStyle->AddEntry("&Export...", kMenuExport);
   fMenuStyle->AddSeparator();
   fMenuStyle->AddEntry("&Close", kMenuExit);
   TGLayoutHints *layout1 = new TGLayoutHints(kLHintsNormal);
   fTrashListLayout->Add(layout1);
   fMenuBar->AddPopup("&Style", fMenuStyle, layout1);

   fMenuHelp = new TGPopupMenu(gClient->GetRoot());
   fMenuHelp->Associate(this);
   fMenuHelp->AddEntry("Top &level", kMenuHelp);
   fMenuHelp->AddSeparator();
   fMenuHelp->AddEntry("&General", kMenuHelpGeneral);
   fMenuHelp->AddEntry("&Canvas", kMenuHelpCanvas);
   fMenuHelp->AddEntry("Pa&d", kMenuHelpPad);
   fMenuHelp->AddEntry("&Histograms", kMenuHelpHistos);
   fMenuHelp->AddEntry("&Axis", kMenuHelpAxis);
   fMenuHelp->AddEntry("&Title", kMenuHelpTitle);
   fMenuHelp->AddEntry("&Stats", kMenuHelpStats);
   fMenuHelp->AddEntry("&PS / PDF", kMenuHelpPSPDF);
   TGLayoutHints *layout2 = new TGLayoutHints(kLHintsRight);
   fTrashListLayout->Add(layout2);
   fMenuBar->AddPopup("&Help", fMenuHelp, layout2);

   p->AddFrame(fMenuBar, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::DoNew()
{
   // Create a new style. Called via the menu bar or the tool bar.

   // Open a message box to allow the user to create a new style.
   new TStyleDialog(this, fCurSelStyle, 1, 0);

   // Create the list of available styles, and select:
   //    - the new style, if it has been created (Ok).
   //    - the previous selected style, if no style has been created (Cancel).
   if (fLastChoice) BuildList();
               else BuildList(fCurSelStyle);
}

//______________________________________________________________________________
void TStyleManager::DoDelete()
{
   //  Delete the current selected style from the ROOT session.
   // Called via the menu or the tool bar.

   // Protection: the user is NOT allowed to delete gStyle.
   // As a consequence, there is always at least one style in the ROOT session.
   if (fCurSelStyle == gStyle) {
      printf("Can not delete gStyle.\n");
      return;
   }

   delete fCurSelStyle;
   fCurSelStyle = 0;

   BuildList(gStyle);
}

//______________________________________________________________________________
void TStyleManager::DoRename()
{
   // Rename the current selected style. Called via the menu bar.

   new TStyleDialog(this, fCurSelStyle, 2, 0);

   // Create the list of styles and select the previous selected style.
   BuildList(fCurSelStyle);
}

//______________________________________________________________________________
void TStyleManager::DoExport()
{
   //  Save the current selected style in a C++ macro file. Called via the menu
   // or the tool bar.

   // Create an associated macro and propose a pertinent name to the user.
   CreateMacro();
   char* newName = new char[(10 + strlen(fCurSelStyle->GetName())) * sizeof(char)];
   sprintf(newName, "Style_%s.C", fCurSelStyle->GetName());

   //  Protection: The user isn't allowed to export a style if the output
   // file name isn't based on the "Style_*.C" mask, without spaces.
   char* tmpFileName;
   const char* tmpBaseName;
   do {
      fCurMacro->fFilename = StrDup(newName);

      // Open a dialog to ask the user to choose an output file.
      new TGFileDialog(gClient->GetRoot(), this, kFDSave, fCurMacro);
      tmpFileName = fCurMacro->fFilename;
      if (tmpFileName) tmpBaseName = gSystem->BaseName(tmpFileName);
                  else tmpBaseName = 0;
   } while (tmpBaseName && (strstr(tmpBaseName, "Style_") != tmpBaseName)
                        && (strstr(tmpBaseName, " ") != 0));

   delete [] newName;

   if (tmpBaseName != 0) {
      // Export the style.
      fCurSelStyle->SaveSource(gSystem->UnixPathName(tmpFileName));
      fCurMacro->fFilename = StrDup(tmpBaseName);
      fStyleChanged = kFALSE;
   }

   UpdateStatusBar();
}

//______________________________________________________________________________
void TStyleManager::DoExit()
{
   // Close the style manager. Called via the menu bar.

//   SendCloseMessage();   // Doesn't delete the StyleManager. Hides it.
   delete this;
}

//______________________________________________________________________________
void TStyleManager::DoHelp(Int_t i)
{
   // Open an help window. Called via the menu bar or the tool bar.

   TRootHelpDialog *hd;
   switch (i) {
      case 0:
         hd = new TRootHelpDialog(this, "Help on General Tab", 600, 400);
         hd->SetText(gHelpSMGeneral);
         break;
      case 1:
         hd = new TRootHelpDialog(this, "Help on Canvas Tab", 600, 400);
         hd->SetText(gHelpSMCanvas);
         break;
      case 2:
         hd = new TRootHelpDialog(this, "Help on Pad Tab", 600, 400);
         hd->SetText(gHelpSMPad);
         break;
      case 3:
         hd = new TRootHelpDialog(this, "Help on Histograms Tab", 600, 400);
         hd->SetText(gHelpSMHistos);
         break;
      case 4:
         hd = new TRootHelpDialog(this, "Help on Axis Tab", 600, 400);
         hd->SetText(gHelpSMAxis);
         break;
      case 5:
         hd = new TRootHelpDialog(this, "Help on Title Tab", 600, 400);
         hd->SetText(gHelpSMTitle);
         break;
      case 6:
         hd = new TRootHelpDialog(this, "Help on Stats Tab", 600, 400);
         hd->SetText(gHelpSMStats);
         break;
      case 7:
         hd = new TRootHelpDialog(this, "Help on PS / PDF Tab", 600, 400);
         hd->SetText(gHelpSMPSPDF);
         break;
      default:
         hd = new TRootHelpDialog(this, "Help on Top Level", 600, 400);
         hd->SetText(gHelpSMTopLevel);
   }
   hd->Popup();
}

//______________________________________________________________________________
void TStyleManager::DoImportCanvas()
{
   //  Create a new style (a copy of gStyle) and import the properties of the
   // current canvas inside.

   if ((!fCurPad) || (!fCurObj)) return;

   new TStyleDialog(this, gStyle, 3, fCurPad);

   // Create the list of available style, and select:
   //    - the new style, if it has been created
   //    - the previous selected style, if no style has been created (Cancel)
   if (fLastChoice) {
      BuildList();

      // Auto export of the canvas' style.
      CreateMacro();
      char* newName = new char[(10 + strlen(fCurSelStyle->GetName())) * sizeof(char)];
      sprintf(newName, "Style_%s.C", fCurSelStyle->GetName());
      fCurMacro->fFilename = StrDup(newName);
      delete [] newName;
      fCurSelStyle->SaveSource(gSystem->UnixPathName(fCurMacro->fFilename));
   } else {
      BuildList(fCurSelStyle);
   }
}

//______________________________________________________________________________
void TStyleManager::CreateMacro()
{
   // Create a TGFileInfo concerning a macro, if it doesn't exist already.

   if (fCurMacro) delete fCurMacro;
   fCurMacro = new TGFileInfo();
   TString dir(".");
   fCurMacro->fFileTypes = kFiletypes;
   fCurMacro->fIniDir    = StrDup(dir);
   fCurMacro->fFilename  = 0;
}

//______________________________________________________________________________
void TStyleManager::AddToolbar(TGCompositeFrame *p)
{
   // Add the tool bar to the frame 'p'.

   TGLayoutHints *layout1 = new TGLayoutHints(kLHintsNormal, 3);
   fTrashListLayout->Add(layout1);
   TGLayoutHints *layout2 = new TGLayoutHints(kLHintsNormal, 6);
   fTrashListLayout->Add(layout2);

   fToolBar = new TGToolBar(p);
   fToolBarNewPic = gClient->GetPicture("sm_new.xpm");
   fToolBarNew = new TGPictureButton(fToolBar, fToolBarNewPic, kToolbarNew);
   fToolBarNew->Associate(this);
   fToolBar->AddFrame(fToolBarNew);

   fToolBarImportCanvasPic = gClient->GetPicture("sm_import_canvas.xpm");
   fToolBarImportCanvas = new TGPictureButton(fToolBar, fToolBarImportCanvasPic, kToolbarImportCanvas);
   fToolBarImportCanvas->Associate(this);
   fToolBar->AddFrame(fToolBarImportCanvas, layout2);

   fToolBarImportMacroPic = gClient->GetPicture("sm_import_macro.xpm");
   fToolBarImportMacro = new TGPictureButton(fToolBar, fToolBarImportMacroPic, kToolbarImportMacro);
   fToolBarImportMacro->Associate(this);
   fToolBar->AddFrame(fToolBarImportMacro);

   fToolBarExportPic = gClient->GetPicture("sm_export.xpm");
   fToolBarExport = new TGPictureButton(fToolBar, fToolBarExportPic, kToolbarExport);
   fToolBarExport->Associate(this);
   fToolBar->AddFrame(fToolBarExport, layout1);

   fToolBarDeletePic = gClient->GetPicture("sm_delete.xpm");
   fToolBarDelete = new TGPictureButton(fToolBar, fToolBarDeletePic, kToolbarDelete);
   fToolBarDelete->Associate(this);
   fToolBar->AddFrame(fToolBarDelete, layout2);

   fToolBarHelpPic = gClient->GetPicture("sm_help.xpm");
   fToolBarHelp = new TGPictureButton(fToolBar, fToolBarHelpPic, kToolbarHelp);
   fToolBarHelp->Associate(this);
   fToolBar->AddFrame(fToolBarHelp, layout2);

   p->AddFrame(fToolBar, fLayoutExpandX);
   fHorizontal3DLine = new TGHorizontal3DLine(p);
   p->AddFrame(fHorizontal3DLine, fLayoutExpandX);

   fToolBarNew->SetToolTipText("Create a new style");
   fToolBarDelete->SetToolTipText("Delete the selected style");
   fToolBarImportCanvas->SetToolTipText("Import a style from selected canvas");
   fToolBarImportMacro->SetToolTipText("Import a style from a macro");
   fToolBarExport->SetToolTipText("Export the selected style into a macro");
   fToolBarHelp->SetToolTipText("Help about the top level interface");
}

//______________________________________________________________________________
void TStyleManager::AddTopLevelInterface(TGCompositeFrame *cf)
{
   //  Add the top level interface to the frame 'cf'. This part of the
   // interface will provide all enable functionalities, excluding the
   // edition of styles.

   TGLayoutHints *layout1 = new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2);
   fTrashListLayout->Add(layout1);
   TGLayoutHints *layout2 = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 15);
   fTrashListLayout->Add(layout2);
   TGLayoutHints *layout3 = new TGLayoutHints(kLHintsNormal, 0, 0, 18);
   fTrashListLayout->Add(layout3);
   TGLayoutHints *layout4 = new TGLayoutHints(kLHintsNormal, 10, 10);
   fTrashListLayout->Add(layout4);
   TGLayoutHints *layout5 = new TGLayoutHints(kLHintsExpandX, 125);
   fTrashListLayout->Add(layout5);
   TGLayoutHints *layout6 = new TGLayoutHints(kLHintsNormal, 0, 10, 3);
   fTrashListLayout->Add(layout6);
   TGLayoutHints *layout7 = new TGLayoutHints(kLHintsNormal, 0, 16, 3);
   fTrashListLayout->Add(layout7);
   TGLayoutHints *layout8 = new TGLayoutHints(kLHintsExpandX, 0, 0, 10);
   fTrashListLayout->Add(layout8);
   TGLayoutHints *layout9 = new TGLayoutHints(kLHintsNormal, -15, 0, -5, -10);
   fTrashListLayout->Add(layout9);
   TGLayoutHints *layout10 = new TGLayoutHints(kLHintsNormal, 15, 0, -5, -10);
   fTrashListLayout->Add(layout10);
   TGLayoutHints *layout11 = new TGLayoutHints(kLHintsExpandX, 0, 0, 15);
   fTrashListLayout->Add(layout11);
   TGLayoutHints *layout12 = new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 5);
   fTrashListLayout->Add(layout12);
   TGLayoutHints *layout13 = new TGLayoutHints(kLHintsExpandX, 20, 0, 7);
   fTrashListLayout->Add(layout13);

   TGVerticalFrame *topLevel = new TGVerticalFrame(cf);
   fTrashListFrame->AddFirst(topLevel);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(topLevel);
   fTrashListFrame->AddFirst(h1);
   TGVerticalFrame *v11 = new TGVerticalFrame(h1);
   fTrashListFrame->AddFirst(v11);
   fListLabel = new TGLabel(v11, "Available Styles:");
   v11->AddFrame(fListLabel);
   fListComboBox = new TGComboBox(v11, kTopStylesList);
   fListComboBox->Associate(this);
   fListComboBox->Resize(200, 22);
   v11->AddFrame(fListComboBox, layout1);
   h1->AddFrame(v11, fLayoutExpandX);
   TGVerticalFrame *v12 = new TGVerticalFrame(h1);
   fTrashListFrame->AddFirst(v12);
   fMakeDefaultPic = gClient->GetPicture("arrow_right2.xpm");
   fMakeDefault = new TGPictureButton(v12, fMakeDefaultPic, kTopMakeDefault);
   fMakeDefault->Associate(this);
   fMakeDefault->Resize(40, 22);
   v12->AddFrame(fMakeDefault, layout3);
   h1->AddFrame(v12, layout4);
   TGVerticalFrame *v13 = new TGVerticalFrame(h1);
   fTrashListFrame->AddFirst(v13);
   fCurStylabel = new TGLabel(v13, "gStyle is set to:");
   v13->AddFrame(fCurStylabel);
   fCurStyle = new TGTextEntry(v13, "", kTopCurStyle);
   fCurStyle->Associate(this);
   fCurStyle->SetEnabled(kFALSE);
   v13->AddFrame(fCurStyle, layout1);
   h1->AddFrame(v13, fLayoutExpandX);
   topLevel->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(topLevel);
   fTrashListFrame->AddFirst(h2);
   TGGroupFrame *gf = new TGGroupFrame(h2, "Apply on");
   fTrashListFrame->AddFirst(gf);
   TGVerticalFrame *vf = new TGVerticalFrame(gf);
   fTrashListFrame->AddFirst(vf);
   Pixel_t red;
   gClient->GetColorByName("#FF0000", red);
   TGHorizontalFrame *selCanvas = new TGHorizontalFrame(vf);
   fTrashListFrame->AddFirst(selCanvas);
   fCurPadLabel = new TGLabel(selCanvas, "Canvas:");
   selCanvas->AddFrame(fCurPadLabel, layout6);
   fCurPadTextEntry = new TGTextEntry(selCanvas, "", kTopCurPad);
   fCurPadTextEntry->SetEnabled(kFALSE);
   fCurPadTextEntry->SetTextColor(red, kFALSE);
   selCanvas->AddFrame(fCurPadTextEntry, fLayoutExpandX);
   vf->AddFrame(selCanvas, fLayoutExpandX);
   TGHorizontalFrame *selObject = new TGHorizontalFrame(vf);
   fTrashListFrame->AddFirst(selObject);
   fCurObjLabel = new TGLabel(selObject, "Object:");
   selObject->AddFrame(fCurObjLabel, layout7);
   fCurObjTextEntry = new TGTextEntry(selObject, "", kTopCurObj);
   fCurObjTextEntry->Associate(this);
   fCurObjTextEntry->SetEnabled(kFALSE);
   fCurObjTextEntry->SetTextColor(red, kFALSE);
   selObject->AddFrame(fCurObjTextEntry, fLayoutExpandX);
   vf->AddFrame(selObject, layout8);
   TGHorizontalFrame *h4 = new TGHorizontalFrame(vf);
   fTrashListFrame->AddFirst(h4);
   fApplyOnGroup = new TGHButtonGroup(h4);
   fApplyOnAll = new TGRadioButton(fApplyOnGroup, "All canvases",    kTopApplyOnAll);
   fApplyOnAll->Associate(this);
   fApplyOnSel = new TGRadioButton(fApplyOnGroup, "Selected object", kTopApplyOnSel);
   fApplyOnSel->Associate(this);
   fAllAndNotCurrent = kFALSE;
   fApplyOnGroup->SetButton(kTopApplyOnSel);
   fApplyOnGroup->Show();
   fApplyOnGroup->SetLayoutHints(layout9, fApplyOnAll);
   fApplyOnGroup->SetLayoutHints(layout10, fApplyOnSel);
   h4->AddFrame(fApplyOnGroup);
   fApplyOnButton = new TGTextButton(h4, "&Apply", kTopApplyOnBut);
   fApplyOnButton->Associate(this);
   fApplyOnButton->Resize(100, 22);
   h4->AddFrame(fApplyOnButton, layout13);
   vf->AddFrame(h4, fLayoutExpandX);
   gf->AddFrame(vf, layout11);
   h2->AddFrame(gf, layout12);
   topLevel->AddFrame(h2, fLayoutExpandX);

   TGHorizontalFrame *h3 = new TGHorizontalFrame(topLevel);
   fTrashListFrame->AddFirst(h3);
   fPreviewButton = new TGCheckButton(h3, "&Preview", kTopPreview);
   fPreviewButton->Associate(this);
   h3->AddFrame(fPreviewButton, layout6);
   fPreviewRealTime = new TGCheckButton(h3, "Run &Time Preview", kTopPreviewRealTime);
   fPreviewRealTime->Associate(this);
   fPreviewRealTime->SetEnabled(kFALSE);
   h3->AddFrame(fPreviewRealTime, layout6);
   fMoreLess = new TGTextButton(h3, "&Close <<", kTopMoreLess);
   fMoreLess->Associate(this);
   h3->AddFrame(fMoreLess, layout5);
   topLevel->AddFrame(h3, fLayoutExpandX);

   cf->AddFrame(topLevel, layout2);

   fApplyOnButton->SetToolTipText("Apply the selected style on the selected object");
   fPreviewButton->SetToolTipText("Show / Hide the preview window");
   fPreviewRealTime->SetToolTipText("Continuous / Asynchronous update of the preview");
}

//______________________________________________________________________________
void TStyleManager::BuildList(TStyle *style)
{
   //  Build the list of styles which will appear in the available styles
   // combo box. The new style to select is mentioned. If no style has
   // been specified, the last entry of the list is selected.

   // Empty the list.
   fListComboBox->RemoveEntries(1, fListComboBox->GetNumberOfEntries());

   // Build the list of all styles already created in the ROOT session.
   Int_t i = 1;
   Int_t styleID = 0;
   TStyle *tmpStyle = (TStyle *) (gROOT->GetListOfStyles()->First());
   while (tmpStyle) {
      if (tmpStyle == style) styleID = i;
      fListComboBox->AddEntry(tmpStyle->GetName(), i++);
      tmpStyle = (TStyle *) (gROOT->GetListOfStyles()->After(tmpStyle));
   }

   // Select 'style' in the list of available styles.
   if (styleID == 0) styleID = i - 1;
   fListComboBox->Select(styleID);
   DoListSelect();
   fCurStyle->SetText(gStyle->GetName());
}

//______________________________________________________________________________
void TStyleManager::UpdateStatusBar()
{
   //  Update the content of the status bar: show the name of the current
   // selected style, its title and the macro from which it has been imported.

   fStatusBar->SetText(fCurSelStyle->GetName(), 0);
   fStatusBar->SetText(fCurSelStyle->GetTitle(), 2);

   if ((!strcmp(fCurSelStyle->GetName(), "Default"))
    || (!strcmp(fCurSelStyle->GetName(), "Plain"  ))
    || (!strcmp(fCurSelStyle->GetName(), "Bold"   ))
    || (!strcmp(fCurSelStyle->GetName(), "Video"  ))
    || (!strcmp(fCurSelStyle->GetName(), "Pub"    ))) {
      fStatusBar->SetText("ROOT style", 1);
   } else if (fStyleChanged) {
      fStatusBar->SetText("User Style _ Not Saved", 1);
   } else {
      fStatusBar->SetText("User Style", 1);
   }
}

//______________________________________________________________________________
void TStyleManager::UpdateEditor(Int_t tabNum)
{
   //  Update the values of every widget entry in the editor. The new values
   // are loaded from the current selected style.

   Double_t delta;
   Int_t year;
   Int_t month;
   Int_t day;
   Int_t oneYearInSecs;
   Int_t oneMonthInSecs;
   Int_t tmp;
   Int_t tmp2;
   switch (tabNum) {
      case 0: // GENERAL
         fFillColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetFillColor()));
         fFillStyle->SetPattern(fCurSelStyle->GetFillStyle());
         fHatchesLineWidth->Select(fCurSelStyle->GetHatchesLineWidth());
         fHatchesSpacing->SetNumber(fCurSelStyle->GetHatchesSpacing());
         fMarkerColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetMarkerColor()));
         fMarkerStyle->SetMarkerStyle(fCurSelStyle->GetMarkerStyle());
         fMarkerSize->Select((Int_t) fCurSelStyle->GetMarkerSize() * 5);
         fScreenFactor->SetNumber(fCurSelStyle->GetScreenFactor());
         fLineColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetLineColor()));
         fLineWidth->Select(fCurSelStyle->GetLineWidth());
         fLineStyle->Select(fCurSelStyle->GetLineStyle());
         // Nothing to do with fLineStyleEdit.
         fTextColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetTextColor()));
         if (fCurSelStyle->GetTextFont()%10 > 2) {
            fTextSizeInPixels->SetState(kButtonDown, kFALSE);
            ModTextSizeInPixels(kTRUE);
         } else {
            fTextSizeInPixels->SetState(kButtonUp, kFALSE);
            ModTextSizeInPixels(kFALSE);
         }
         fTextFont->Select(fCurSelStyle->GetTextFont()/10);
         fTextAlign->Select(fCurSelStyle->GetTextAlign());
         fTextAngle->SetNumber(fCurSelStyle->GetTextAngle());
         break;
      case 1: // CANVAS
         fCanvasColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetCanvasColor()));
         fCanvasDefX->SetIntNumber(fCurSelStyle->GetCanvasDefX());
         fCanvasDefY->SetIntNumber(fCurSelStyle->GetCanvasDefY());
         fCanvasDefW->SetIntNumber(fCurSelStyle->GetCanvasDefW());
         fCanvasDefH->SetIntNumber(fCurSelStyle->GetCanvasDefH());
         fCanvasBorderMode->SetButton(fCurSelStyle->GetCanvasBorderMode() + 1 + kCanvasBorderModeSunken);
         fCanvasBorderSize->Select(fCurSelStyle->GetCanvasBorderSize());
         fAttDateTextColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetAttDate()->GetTextColor()));
         if (fCurSelStyle->GetAttDate()->GetTextFont()%10 > 2) {
            fAttDateTextSizeInPixels->SetState(kButtonDown, kFALSE);
            ModAttDateTextSizeInPixels(kTRUE);
         } else {
            fAttDateTextSizeInPixels->SetState(kButtonUp, kFALSE);
            ModAttDateTextSizeInPixels(kFALSE);
         }
         fOptDateFormat->Select(fCurSelStyle->GetOptDate()/10 + 1);
         fAttDateTextFont->Select(fCurSelStyle->GetAttDate()->GetTextFont()/10);
         fAttDateTextAlign->Select(fCurSelStyle->GetAttDate()->GetTextAlign());
         fAttDateTextAngle->SetNumber(fCurSelStyle->GetAttDate()->GetTextAngle());
         fDateX->SetIntNumber((Int_t) (fCurSelStyle->GetDateX()*100 + 0.5));
         fDateY->SetIntNumber((Int_t) (fCurSelStyle->GetDateY()*100 + 0.5));

         if (fCurSelStyle->GetOptDate()) {
            fOptDateBool->SetState(kButtonDown, kFALSE);
            fAttDateTextColor->Enable();
            fAttDateTextSize->SetState(kTRUE);
            if (!fAttDateTextSizeInPixels->IsDown())
               fAttDateTextSizeInPixels->SetEnabled(kTRUE);
// TODO Just delete when ComboBox can be grayed
            //fOptDateFormat->SetState(kTRUE);
            //ModAttDateTextFont->SetState(kTRUE);
            //ModAttDateTextAlign->SetState(kTRUE);
            fAttDateTextAngle->SetState(kTRUE);
            fDateX->SetState(kTRUE);
            fDateY->SetState(kTRUE);
         } else {
            fOptDateBool->SetState(kButtonUp, kFALSE);
            fAttDateTextColor->Disable();
            fAttDateTextSize->SetState(kFALSE);
            fAttDateTextSizeInPixels->SetEnabled(kFALSE);
// TODO Just delete when ComboBox can be grayed
            //fOptDateFormat->SetState(kFALSE);
            //ModAttDateTextFont->SetState(kFALSE);
            //ModAttDateTextAlign->SetState(kFALSE);
            fAttDateTextAngle->SetState(kFALSE);
            fDateX->SetState(kFALSE);
            fDateY->SetState(kFALSE);
         }
         break;
      case 2: // PAD
         fPadTopMargin->SetIntNumber((Int_t) (fCurSelStyle->GetPadTopMargin() * 100 + 0.5));
         fPadBottomMargin->SetIntNumber((Int_t) (fCurSelStyle->GetPadBottomMargin() * 100 + 0.5));
         fPadLeftMargin->SetIntNumber((Int_t) (fCurSelStyle->GetPadLeftMargin() * 100 + 0.5));
         fPadRightMargin->SetIntNumber((Int_t) (fCurSelStyle->GetPadRightMargin() * 100 + 0.5));
         fPadBorderMode->SetButton(fCurSelStyle->GetPadBorderMode() + 1 + kPadBorderModeSunken);
         fPadBorderSize->Select(fCurSelStyle->GetPadBorderSize());
         fPadColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetPadColor()));
         if (fCurSelStyle->GetPadTickX()) 
            fPadTickX->SetState(kButtonDown, kFALSE);
         else 
            fPadTickX->SetState(kButtonUp, kFALSE);
         if (fCurSelStyle->GetPadTickY()) 
            fPadTickY->SetState(kButtonDown, kFALSE);
         else 
            fPadTickY->SetState(kButtonUp, kFALSE);
         if (fCurSelStyle->GetPadGridX()) 
            fPadGridX->SetState(kButtonDown, kFALSE);
         else 
            fPadGridX->SetState(kButtonUp, kFALSE);
         if (fCurSelStyle->GetPadGridY()) 
            fPadGridY->SetState(kButtonDown, kFALSE);
         else 
            fPadGridY->SetState(kButtonUp, kFALSE);
         fGridColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetGridColor()));
         fGridWidth->Select(fCurSelStyle->GetGridWidth());
         fGridStyle->Select(fCurSelStyle->GetGridStyle());
         break;
      case 3: // HISTOS
         fHistFillColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetHistFillColor()));
         fHistFillStyle->SetPattern(fCurSelStyle->GetHistFillStyle());
         fHistLineColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetHistLineColor()));
         fHistLineWidth->Select(fCurSelStyle->GetHistLineWidth());
         fHistLineStyle->Select(fCurSelStyle->GetHistLineStyle());
         fBarWidth->SetNumber(fCurSelStyle->GetBarWidth());
         fBarOffset->SetNumber(fCurSelStyle->GetBarOffset());
         if (fCurSelStyle->GetHistMinimumZero()) 
            fHistMinimumZero->SetState(kButtonDown, kFALSE);
         else 
            fHistMinimumZero->SetState(kButtonUp, kFALSE);
         fPaintTextFormat->SetText(fCurSelStyle->GetPaintTextFormat());
         fNumberContours->SetIntNumber(fCurSelStyle->GetNumberContours());
         fLegoInnerR->SetIntNumber((Int_t) (fCurSelStyle->GetLegoInnerR() * 100 + 0.5));
         fFrameFillColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetFrameFillColor()));
         fFrameFillStyle->SetPattern(fCurSelStyle->GetFrameFillStyle());
         fFrameLineColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetFrameLineColor()));
         fFrameLineWidth->Select(fCurSelStyle->GetFrameLineWidth());
         fFrameLineStyle->Select(fCurSelStyle->GetFrameLineStyle());
         // Nothing to do with fPaletteEdit;
         fFrameBorderMode->SetButton(fCurSelStyle->GetFrameBorderMode() + 1 + kFrameBorderModeSunken);
         fFrameBorderSize->Select(fCurSelStyle->GetFrameBorderSize());
         fFuncColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetFuncColor()));
         fFuncWidth->Select(fCurSelStyle->GetFuncWidth());
         fFuncStyle->Select(fCurSelStyle->GetFuncStyle());
         if (fCurSelStyle->GetDrawBorder()) 
            fDrawBorder->SetState(kButtonDown, kFALSE);
         else 
            fDrawBorder->SetState(kButtonUp, kFALSE);
         fEndErrorSize->SetNumber(fCurSelStyle->GetEndErrorSize());
         fErrorX->SetIntNumber((Int_t) (fCurSelStyle->GetErrorX() * 100 + 0.5));
         break;
      case 4: // AXIS
         delta = fCurSelStyle->GetTimeOffset() - 788918400;
         year = 1995;
         month = 1;
         day = 1;
         while (delta < 0) {
            year--;
            if (year % 4) oneYearInSecs = 3600 * 24 * 365;
            else oneYearInSecs = 3600 * 24 * 366;
            delta += oneYearInSecs;
         }
         oneYearInSecs = 3600 * 24 * 365;             // because 365 days in 1995.
         while (delta >= oneYearInSecs) {
            if (year % 4) oneYearInSecs = 3600 * 24 * 365;
            else oneYearInSecs = 3600 * 24 * 366;
            delta -= oneYearInSecs;
            year++;
         }
         oneMonthInSecs = 3600 * 24 * 31;       // because 31 days in January.
         while (delta >= oneMonthInSecs) {
            month++;
            delta -= oneMonthInSecs;
            switch (month) {
               case 2:
                  if (year % 4) oneMonthInSecs = 3600 * 24 * 28;
                  else oneMonthInSecs = 3600 * 24 * 29;
                  break;
               case 3: case 5: case 7: case 8: case 10: case 12:
                  oneMonthInSecs = 3600 * 24 * 31;
                  break;
               default:
                  oneMonthInSecs = 3600 * 24 * 30;
            }
         }
         day = (Int_t) delta / (3600 * 24) + 1;
         delta = ((Int_t) delta) % (3600 * 24);
         fTimeOffsetDate->SetNumber(year*10000 + month*100 + day);
         fTimeOffsetTime->SetNumber(delta);

         if (fCurSelStyle->GetStripDecimals()) 
            fStripDecimals->SetState(kButtonUp, kFALSE);
         else 
            fStripDecimals->SetState(kButtonDown, kFALSE);
         fXTitleSize->SetNumber(fCurSelStyle->GetTitleSize("X"));
         if (fCurSelStyle->GetTitleFont("X")%10 > 2) {
            fXTitleSizeInPixels->SetState(kButtonDown, kFALSE);
            ModXTitleSizeInPixels(kTRUE);
         } else {
            fXTitleSizeInPixels->SetState(kButtonUp, kFALSE);
            ModXTitleSizeInPixels(kFALSE);
         }
         fXTitleColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetTitleColor("X")));
         fXTitleOffset->SetNumber(fCurSelStyle->GetTitleOffset("X"));
         fXTitleFont->Select(fCurSelStyle->GetTitleFont("X")/10);
         fXLabelSize->SetNumber(fCurSelStyle->GetLabelSize("X"));
         if (fCurSelStyle->GetLabelFont("X")%10 > 2) {
            fXLabelSizeInPixels->SetState(kButtonDown, kFALSE);
            ModXLabelSizeInPixels(kTRUE);
         } else {
            fXLabelSizeInPixels->SetState(kButtonUp, kFALSE);
            ModXLabelSizeInPixels(kFALSE);
         }
         fXLabelColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetLabelColor("X")));
         fXLabelOffset->SetNumber(fCurSelStyle->GetLabelOffset("X"));
         fXLabelFont->Select(fCurSelStyle->GetLabelFont("X")/10);
         fXAxisColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetAxisColor("X")));
         fXTickLength->SetNumber(fCurSelStyle->GetTickLength("X"));
         if (fCurSelStyle->GetOptLogx()) 
            fOptLogx->SetState(kButtonDown, kFALSE);
         else 
            fOptLogx->SetState(kButtonUp, kFALSE);
         fXNdivMain->SetIntNumber(TMath::Abs(fCurSelStyle->GetNdivisions("X")) % 100);
         fXNdivSub->SetIntNumber((TMath::Abs(fCurSelStyle->GetNdivisions("X")) % 10000)/100);
         fXNdivSubSub->SetIntNumber((TMath::Abs(fCurSelStyle->GetNdivisions("X")) % 1000000)/10000);
         if (fCurSelStyle->GetNdivisions("X") > 0) 
            fXNdivisionsOptimize->SetState(kButtonDown, kFALSE);
         else 
            fXNdivisionsOptimize->SetState(kButtonUp, kFALSE);
         fYTitleSize->SetNumber(fCurSelStyle->GetTitleSize("Y"));
         if (fCurSelStyle->GetTitleFont("Y")%10 > 2) {
            fYTitleSizeInPixels->SetState(kButtonDown, kFALSE);
            ModYTitleSizeInPixels(kTRUE);
         } else {
            fYTitleSizeInPixels->SetState(kButtonUp, kFALSE);
            ModYTitleSizeInPixels(kFALSE);
         }
         fYTitleColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetTitleColor("Y")));
         fYTitleOffset->SetNumber(fCurSelStyle->GetTitleOffset("Y"));
         fYTitleFont->Select(fCurSelStyle->GetTitleFont("Y")/10);
         fYLabelSize->SetNumber(fCurSelStyle->GetLabelSize("Y"));
         if (fCurSelStyle->GetLabelFont("Y")%10 > 2) {
            fYLabelSizeInPixels->SetState(kButtonDown, kFALSE);
            ModYLabelSizeInPixels(kTRUE);
         } else {
            fYLabelSizeInPixels->SetState(kButtonUp, kFALSE);
            ModYLabelSizeInPixels(kFALSE);
         }
         fYLabelColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetLabelColor("Y")));
         fYLabelOffset->SetNumber(fCurSelStyle->GetLabelOffset("Y"));
         fYLabelFont->Select(fCurSelStyle->GetLabelFont("Y")/10);
         fYAxisColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetAxisColor("Y")));
         fYTickLength->SetNumber(fCurSelStyle->GetTickLength("Y"));
         if (fCurSelStyle->GetOptLogy()) 
            fOptLogy->SetState(kButtonDown, kFALSE);
         else  
            fOptLogy->SetState(kButtonUp, kFALSE);
         fYNdivMain->SetIntNumber(TMath::Abs(fCurSelStyle->GetNdivisions("Y")) % 100);
         fYNdivSub->SetIntNumber((TMath::Abs(fCurSelStyle->GetNdivisions("Y")) % 10000)/100);
         fYNdivSubSub->SetIntNumber((TMath::Abs(fCurSelStyle->GetNdivisions("Y")) % 1000000)/10000);
         if (fCurSelStyle->GetNdivisions("Y") > 0) 
            fYNdivisionsOptimize->SetState(kButtonDown, kFALSE);
         else 
            fYNdivisionsOptimize->SetState(kButtonUp, kFALSE);
         fZTitleSize->SetNumber(fCurSelStyle->GetTitleSize("Z"));
         if (fCurSelStyle->GetTitleFont("Z")%10 > 2) {
            fZTitleSizeInPixels->SetState(kButtonDown, kFALSE);
            ModZTitleSizeInPixels(kTRUE);
         } else {
            fZTitleSizeInPixels->SetState(kButtonUp, kFALSE);
            ModZTitleSizeInPixels(kFALSE);
         }
         fZTitleColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetTitleColor("Z")));
         fZTitleOffset->SetNumber(fCurSelStyle->GetTitleOffset("Z"));
         fZTitleFont->Select(fCurSelStyle->GetTitleFont("Z")/10);
         fZLabelSize->SetNumber(fCurSelStyle->GetLabelSize("Z"));
         if (fCurSelStyle->GetLabelFont("Z")%10 > 2) {
            fZLabelSizeInPixels->SetState(kButtonDown, kFALSE);
            ModZLabelSizeInPixels(kTRUE);
         } else {
            fZLabelSizeInPixels->SetState(kButtonUp, kFALSE);
            ModZLabelSizeInPixels(kFALSE);
         }
         fZLabelColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetLabelColor("Z")));
         fZLabelOffset->SetNumber(fCurSelStyle->GetLabelOffset("Z"));
         fZLabelFont->Select(fCurSelStyle->GetLabelFont("Z")/10);
         fZAxisColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetAxisColor("Z")));
         fZTickLength->SetNumber(fCurSelStyle->GetTickLength("Z"));
         
         if (fCurSelStyle->GetOptLogz()) 
            fOptLogz->SetState(kButtonDown, kFALSE);
         else 
            fOptLogz->SetState(kButtonUp, kFALSE);
         
         fZNdivMain->SetIntNumber(TMath::Abs(fCurSelStyle->GetNdivisions("Z")) % 100);
         fZNdivSub->SetIntNumber((TMath::Abs(fCurSelStyle->GetNdivisions("Z")) % 10000)/100);
         fZNdivSubSub->SetIntNumber((TMath::Abs(fCurSelStyle->GetNdivisions("Z")) % 1000000)/10000);
         if (fCurSelStyle->GetNdivisions("Z") > 0) 
            fZNdivisionsOptimize->SetState(kButtonDown, kFALSE);
         else 
            fZNdivisionsOptimize->SetState(kButtonUp, kFALSE);
         break;
      case 5: // TITLES
         fTitleColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetTitleFillColor()));
         fTitleStyle->SetPattern(fCurSelStyle->GetTitleStyle());
         fTitleTextColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetTitleTextColor()));
         fTitleFontSize->SetNumber(fCurSelStyle->GetTitleFontSize());
         if (fCurSelStyle->GetTitleFont()%10 > 2) {
            fTitleFontSizeInPixels->SetState(kButtonDown, kFALSE);
            ModTitleFontSizeInPixels(kTRUE);
         } else {
            fTitleFontSizeInPixels->SetState(kButtonUp, kFALSE);
            ModTitleFontSizeInPixels(kFALSE);
         }
         fTitleFont->Select(fCurSelStyle->GetTitleFont()/10);
         fTitleAlign->Select(fCurSelStyle->GetTitleAlign());
         fTitleBorderSize->Select(fCurSelStyle->GetTitleBorderSize());
         fLegendBorderSize->Select(fCurSelStyle->GetLegendBorderSize());
         fTitleX->SetIntNumber((Int_t) (fCurSelStyle->GetTitleX() * 100 + 0.5));
         fTitleY->SetIntNumber((Int_t) (fCurSelStyle->GetTitleY() * 100 + 0.5));
         fTitleW->SetIntNumber((Int_t) (fCurSelStyle->GetTitleW() * 100 + 0.5));
         fTitleH->SetIntNumber((Int_t) (fCurSelStyle->GetTitleH() * 100 + 0.5));

         if (fCurSelStyle->GetOptTitle()) {
            fOptTitle->SetState(kButtonDown, kFALSE);
            fTitleColor->Enable();
            fTitleStyle->Enable();
            fTitleTextColor->Enable();
            fTitleFontSize->SetState(kTRUE);
            if (!fTitleFontSizeInPixels->IsDown())
               fTitleFontSizeInPixels->SetEnabled(kTRUE);
// TODO Just delete when ComboBox can be grayed
            //fTitleFont->SetState(kTRUE);
            //fTitleAlign->SetState(kTRUE);
            //fTitleBorderSize->SetState(kTRUE);
            //fLegendBorderSize->SetState(kTRUE);
            fTitleX->SetState(kTRUE);
            fTitleY->SetState(kTRUE);
            fTitleW->SetState(kTRUE);
            fTitleH->SetState(kTRUE);
         } else {
            fOptTitle->SetState(kButtonUp, kFALSE);
            fTitleColor->Disable();
            fTitleStyle->Disable();
            fTitleTextColor->Disable();
            fTitleFontSize->SetState(kFALSE);
            fTitleFontSizeInPixels->SetEnabled(kFALSE);
// TODO Just delete when ComboBox can be grayed
            //fTitleFont->SetState(kFALSE);
            //fTitleAlign->SetState(kFALSE);
            //fTitleBorderSize->SetState(kFALSE);
            //fLegendBorderSize->SetState(kFALSE);
            fTitleX->SetState(kFALSE);
            fTitleY->SetState(kFALSE);
            fTitleW->SetState(kFALSE);
            fTitleH->SetState(kFALSE);
         }
         break;
      case 6: // STATS
         fStatColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetStatColor()));
         fStatStyle->SetPattern(fCurSelStyle->GetStatStyle());
         fStatTextColor->SetColor(TColor::Number2Pixel(fCurSelStyle->GetStatTextColor()));
         fStatFont->Select(fCurSelStyle->GetStatFont()/10);
         if (fCurSelStyle->GetStatFont()%10 > 2) {
            fStatFontSizeInPixels->SetState(kButtonDown, kFALSE);
            ModStatFontSizeInPixels(kTRUE);
         } else {
            fStatFontSizeInPixels->SetState(kButtonUp, kFALSE);
            ModStatFontSizeInPixels(kFALSE);
         }
         fStatFontSize->SetNumber(fCurSelStyle->GetStatFontSize());
         
         fStatX->SetNumber(fCurSelStyle->GetStatX());
         fStatY->SetNumber(fCurSelStyle->GetStatY());
         fStatW->SetNumber(fCurSelStyle->GetStatW());
         fStatH->SetNumber(fCurSelStyle->GetStatH());
         fStatBorderSize->Select(fCurSelStyle->GetStatBorderSize());
         tmp = fCurSelStyle->GetOptStat();
         
         if (tmp % 10) fOptStatName->SetState(kButtonDown, kFALSE);
         else fOptStatName->SetState(kButtonUp, kFALSE);
         
         if ((tmp/10) % 10) fOptStatEntries->SetState(kButtonDown, kFALSE);
         else fOptStatEntries->SetState(kButtonUp, kFALSE);
         
         if ((tmp/100) % 10) fOptStatMean->SetState(kButtonDown, kFALSE);
         else fOptStatMean->SetState(kButtonUp, kFALSE);
         
         if ((tmp/1000) % 10) fOptStatRMS->SetState(kButtonDown, kFALSE);
         else fOptStatRMS->SetState(kButtonUp, kFALSE);
         
         if ((tmp/10000) % 10) fOptStatUnderflow->SetState(kButtonDown, kFALSE);
         else fOptStatUnderflow->SetState(kButtonUp, kFALSE);
         
         if ((tmp/100000) % 10) fOptStatOverflow->SetState(kButtonDown, kFALSE);
         else fOptStatOverflow->SetState(kButtonUp, kFALSE);
         
         if ((tmp/1000000) % 10) fOptStatIntegral->SetState(kButtonDown, kFALSE);
         else fOptStatIntegral->SetState(kButtonUp, kFALSE);
         
         if ((tmp/10000000) % 10) fOptStatSkewness->SetState(kButtonDown, kFALSE);
         else fOptStatSkewness->SetState(kButtonUp, kFALSE);
         
         if ((tmp/100000000) % 10) fOptStatKurtosis->SetState(kButtonDown, kFALSE);
         else fOptStatKurtosis->SetState(kButtonUp, kFALSE);
         
         if ((((tmp/100) % 10) == 2) || (((tmp/1000) % 10) == 2) || 
             (((tmp/10000000) % 10) == 2) || (((tmp/100000000) % 10) == 2))
            fOptStatErrors->SetState(kButtonDown, kFALSE);   
         else  fOptStatErrors->SetState(kButtonUp, kFALSE);
         
         fStatFormat->SetText(fCurSelStyle->GetStatFormat());
         tmp2 = fCurSelStyle->GetOptFit();
         
         if (tmp2 % 10) fOptFitValues->SetState(kButtonDown, kFALSE);
         else fOptFitValues->SetState(kButtonUp, kFALSE);
         
         if ((tmp2/10) % 10) {
            fOptFitErrors->SetState(kButtonDown, kFALSE);
            fOptFitValues->SetState(kButtonDown, kFALSE);
         } else
            fOptFitErrors->SetState(kButtonUp, kFALSE);
            
         if ((tmp2/100) % 10) fOptFitChi->SetState(kButtonDown, kFALSE);
         else fOptFitChi->SetState(kButtonUp, kFALSE);
         
         if ((tmp2/1000) % 10) fOptFitProbability->SetState(kButtonDown, kFALSE);
         else fOptFitProbability->SetState(kButtonUp, kFALSE);
         
         fFitFormat->SetText(fCurSelStyle->GetFitFormat());
         break;
         
      case 7: // PS / PDF
         fHeaderPS->SetText(fCurSelStyle->GetHeaderPS());
         fTitlePS->SetText(fCurSelStyle->GetTitlePS());
         fColorModelPS->SetButton(fCurSelStyle->GetColorModelPS() + kPSPDFColorModelPSRGB);
         fLineScalePS->SetNumber(fCurSelStyle->GetLineScalePS());
         Float_t papSizeX;
         Float_t papSizeY;
         fCurSelStyle->GetPaperSize(papSizeX, papSizeY);
         if ((papSizeX == 20) && (papSizeY == 26)) {
            fPaperSizePredef->Select(3);
            fPaperSizeEnCm = kTRUE;
            fPaperSizeX->SetNumber(papSizeX);
            fPaperSizeY->SetNumber(papSizeY);
         } else if ((papSizeX == 20) && (papSizeY == 24)) {
            fPaperSizePredef->Select(4);
            fPaperSizeEnCm = kFALSE;
            fPaperSizeX->SetNumber(papSizeX * 0.394);
            fPaperSizeY->SetNumber(papSizeY * 0.394);
         } else {
            fPaperSizePredef->Select(1);
            fPaperSizeEnCm = kTRUE;
            fPaperSizeX->SetNumber(papSizeX);
            fPaperSizeY->SetNumber(papSizeY);
         }
         break;
   }
}

//______________________________________________________________________________
void TStyleManager::ConnectAll()
{
   // Connect every entry in the top level interface to the slot.

   Connect("CloseWindow()", "TStyleManager", this, "CloseWindow()");
   fMenuStyle->Connect("Activated(Int_t)", "TStyleManager", this, "DoMenu(Int_t)");
   fMenuHelp->Connect("Activated(Int_t)", "TStyleManager", this, "DoMenu(Int_t)");
   fToolBarNew->Connect("Clicked()", "TStyleManager", this, Form("DoMenu(Int_t=%d)", kMenuNew));
   fToolBarDelete->Connect("Clicked()", "TStyleManager", this, Form("DoMenu(Int_t=%d)", kMenuDelete));
   fToolBarImportCanvas->Connect("Clicked()", "TStyleManager", this, Form("DoMenu(Int_t=%d)", kMenuImportCanvas));
   fToolBarImportMacro->Connect("Clicked()", "TStyleManager", this, Form("DoMenu(Int_t=%d)", kMenuImportMacro));
   fToolBarExport->Connect("Clicked()", "TStyleManager", this, Form("DoMenu(Int_t=%d)", kMenuExport));
   fToolBarHelp->Connect("Clicked()", "TStyleManager", this, Form("DoMenu(Int_t=%d)", kMenuHelp));
   fListComboBox->Connect("Selected(Int_t)", "TStyleManager", this, "DoListSelect()");
   fPreviewButton->Connect("Toggled(Bool_t)", "TStyleManager", this, "DoPreview(Bool_t)");
   fPreviewRealTime->Connect("Toggled(Bool_t)", "TStyleManager", this, "DoRealTime(Bool_t)");
   fMakeDefault->Connect("Clicked()", "TStyleManager", this, "DoMakeDefault()");
   fApplyOnGroup->Connect("Clicked(Int_t)", "TStyleManager", this, "DoApplyOnSelect(Int_t)");
   fApplyOnButton->Connect("Clicked()", "TStyleManager", this, "DoApplyOn()");
   fMoreLess->Connect("Clicked()", "TStyleManager", this, "DoMoreLess()");

   fEditionHelp->Connect("Clicked()", "TStyleManager", this, Form("DoMenu(Int_t=%d)", kMenuHelpEditor));
   fEditionUpdatePreview->Connect("Clicked()", "TStyleManager", this, "DoEditionUpdatePreview()");
   fEditionReset->Connect("Clicked()", "TStyleManager", this, "DoImportMacro(Int_t=kFALSE)");
   fEditionTab->Connect("Selected(Int_t)", "TStyleManager", this, "DoChangeTab(Int_t)");
   fAxisTab->Connect("Selected(Int_t)", "TStyleManager", this, "DoChangeAxisTab(Int_t)");

   // Connect signals emited when the current pad changed.
   TQObject::Connect("TCanvas", "Selected(TVirtualPad *, TObject *, Int_t)", "TStyleManager",
      this, "DoSelectCanvas(TVirtualPad *, TObject *, Int_t)");
   TQObject::Connect("TCanvas", "Closed()", "TStyleManager", this, "DoSelectNoCanvas()");
}

//______________________________________________________________________________
void TStyleManager::DisconnectAll()
{
   // Disconnect every entry in the top level interface of the slot.

   Disconnect("CloseWindow()");
   fMenuStyle->Disconnect("Activated(Int_t)");
   fMenuHelp->Disconnect("Activated(Int_t)");
   fToolBarNew->Disconnect("Clicked()");
   fToolBarDelete->Disconnect("Clicked()");
   fToolBarImportCanvas->Disconnect("Clicked()");
   fToolBarImportMacro->Disconnect("Clicked()");
   fToolBarExport->Disconnect("Clicked()");
   fToolBarHelp->Disconnect("Clicked()");
   fListComboBox->Disconnect("Selected(Int_t)");
   fPreviewButton->Disconnect("Toggled(Bool_t)");
   fMakeDefault->Disconnect("Clicked()");
   fApplyOnGroup->Disconnect("Clicked(Int_t)");
   fApplyOnButton->Disconnect("Clicked()");
   fMoreLess->Disconnect("Clicked()");

   fEditionHelp->Disconnect("Clicked()");
   fEditionUpdatePreview->Disconnect("Clicked()");
   fEditionReset->Disconnect("Clicked()");
   fEditionTab->Disconnect("Selected(Int_t)");

   TQObject::Disconnect("TCanvas", "Selected(TVirtualPad *, TObject *, Int_t)");
   TQObject::Disconnect("TCanvas", "Closed()");
}

//______________________________________________________________________________
void TStyleManager::ConnectEditor(Int_t tabNum)
{
   // Connect every widget entry of the editor to its specific slot.

   if (fSigSlotConnected) return;
   fSigSlotConnected = kTRUE;

   switch (tabNum) {
      case 0: // GENERAL
         fFillColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModFillColor()");
         fFillStyle->Connect("PatternSelected(Style_t)", "TStyleManager", this, "ModFillStyle()");
         fHatchesLineWidth->Connect("Selected(Int_t)", "TStyleManager", this, "ModHatchesLineWidth()");
         fHatchesSpacing->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModHatchesSpacing()");
         fMarkerColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModMarkerColor()");
         fMarkerStyle->Connect("MarkerSelected(Style_t)", "TStyleManager", this, "ModMarkerStyle()");
         fMarkerSize->Connect("Selected(Int_t)", "TStyleManager", this, "ModMarkerSize()");
         fScreenFactor->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModScreenFactor()");
         fLineColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModLineColor()");
         fLineWidth->Connect("Selected(Int_t)", "TStyleManager", this, "ModLineWidth()");
         fLineStyle->Connect("Selected(Int_t)", "TStyleManager", this, "ModLineStyle()");
         fLineStyleEdit->Connect("Clicked()", "TStyleManager", this, "ModLineStyleEdit()");
         fTextColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModTextColor()");
         fTextSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModTextSize()");
         fTextSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModTextSizeInPixels(Bool_t)");
         fTextFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModTextFont()");
         fTextAlign->Connect("Selected(Int_t)", "TStyleManager", this, "ModTextAlign()");
         fTextAngle->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModTextAngle()");
         break;
      case 1: // CANVAS
         fCanvasColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModCanvasColor()");
         fCanvasDefX->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModCanvasDefX()");
         fCanvasDefY->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModCanvasDefY()");
         fCanvasDefW->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModCanvasDefW()");
         fCanvasDefH->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModCanvasDefH()");
         fCanvasBorderMode->Connect("Clicked(Int_t)", "TStyleManager", this, "ModCanvasBorderMode()");
         fCanvasBorderSize->Connect("Selected(Int_t)", "TStyleManager", this, "ModCanvasBorderSize()");
         fOptDateBool->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptDateBool()");
         fAttDateTextColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModAttDateTextColor()");
         fAttDateTextSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModAttDateTextSize()");
         fAttDateTextSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModAttDateTextSizeInPixels(Bool_t)");
         fOptDateFormat->Connect("Selected(Int_t)", "TStyleManager", this, "ModOptDateFormat()");
         fAttDateTextFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModAttDateTextFont()");
         fAttDateTextAngle->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModAttDateTextAngle()");
         fAttDateTextAlign->Connect("Selected(Int_t)", "TStyleManager", this, "ModAttDateTextAlign()");
         fDateX->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModDateX()");
         fDateY->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModDateY()");
         break;
      case 2: // PAD
         fPadTopMargin->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModPadTopMargin()");
         fPadBottomMargin->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModPadBottomMargin()");
         fPadLeftMargin->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModPadLeftMargin()");
         fPadRightMargin->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModPadRightMargin()");
         fPadBorderMode->Connect("Clicked(Int_t)", "TStyleManager", this, "ModPadBorderMode()");
         fPadBorderSize->Connect("Selected(Int_t)", "TStyleManager", this, "ModPadBorderSize()");
         fPadColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModPadColor()");
         fPadTickX->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModPadTickX()");
         fPadTickY->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModPadTickY()");
         fPadGridX->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModPadGridX()");
         fPadGridY->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModPadGridY()");
         fGridColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModGridColor()");
         fGridWidth->Connect("Selected(Int_t)", "TStyleManager", this, "ModGridWidth()");
         fGridStyle->Connect("Selected(Int_t)", "TStyleManager", this, "ModGridStyle()");
         break;
      case 3: // HISTOS
         fHistFillColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModHistFillColor()");
         fHistFillStyle->Connect("PatternSelected(Style_t)", "TStyleManager", this, "ModHistFillStyle()");
         fHistLineColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModHistLineColor()");
         fHistLineWidth->Connect("Selected(Int_t)", "TStyleManager", this, "ModHistLineWidth()");
         fHistLineStyle->Connect("Selected(Int_t)", "TStyleManager", this, "ModHistLineStyle()");
         fBarWidth->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModBarWidth()");
         fBarOffset->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModBarOffset()");
         fHistMinimumZero->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModHistMinimumZero()");
         fPaintTextFormat->Connect("TextChanged(const char *)", "TStyleManager", this, "ModPaintTextFormat()");
         fNumberContours->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModNumberContours()");
         fLegoInnerR->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModLegoInnerR()");
         fFrameFillColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModFrameFillColor()");
         fFrameFillStyle->Connect("PatternSelected(Style_t)", "TStyleManager", this, "ModFrameFillStyle()");
         fFrameLineColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModFrameLineColor()");
         fFrameLineWidth->Connect("Selected(Int_t)", "TStyleManager", this, "ModFrameLineWidth()");
         fFrameLineStyle->Connect("Selected(Int_t)", "TStyleManager", this, "ModFrameLineStyle()");
         fPaletteEdit->Connect("Clicked()", "TStyleManager", this, "ModPaletteEdit()");
         fFrameBorderMode->Connect("Clicked(Int_t)", "TStyleManager", this, "ModFrameBorderMode()");
         fFrameBorderSize->Connect("Selected(Int_t)", "TStyleManager", this, "ModFrameBorderSize()");
         fFuncColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModFuncColor()");
         fFuncWidth->Connect("Selected(Int_t)", "TStyleManager", this, "ModFuncWidth()");
         fFuncStyle->Connect("Selected(Int_t)", "TStyleManager", this, "ModFuncStyle()");
         fDrawBorder->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModDrawBorder()");
         fEndErrorSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModEndErrorSize()");
         fErrorX->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModErrorX()");
         break;
      case 4: // AXIS
         fTimeOffsetDate->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModTimeOffset()");
         fTimeOffsetTime->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModTimeOffset()");
         fStripDecimals->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModStripDecimals()");
         fApplyOnXYZ->Connect("Clicked()", "TStyleManager", this, "ModApplyOnXYZ()");
         fXTitleSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModXTitleSize()");
         fXTitleSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModXTitleSizeInPixels(Bool_t)");
         fXTitleColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModXTitleColor()");
         fXTitleOffset->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModXTitleOffset()");
         fXTitleFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModXTitleFont()");
         fXLabelSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModXLabelSize()");
         fXLabelSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModXLabelSizeInPixels(Bool_t)");
         fXLabelColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModXLabelColor()");
         fXLabelOffset->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModXLabelOffset()");
         fXLabelFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModXLabelFont()");
         fXAxisColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModXAxisColor()");
         fXTickLength->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModXTickLength()");
         fOptLogx->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptLogx()");
         fXNdivMain->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModXNdivisions()");
         fXNdivSub->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModXNdivisions()");
         fXNdivSubSub->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModXNdivisions()");
         fXNdivisionsOptimize->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModXNdivisions()");
         fYTitleSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModYTitleSize()");
         fYTitleSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModYTitleSizeInPixels(Bool_t)");
         fYTitleColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModYTitleColor()");
         fYTitleOffset->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModYTitleOffset()");
         fYTitleFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModYTitleFont()");
         fYLabelSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModYLabelSize()");
         fYLabelSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModYLabelSizeInPixels(Bool_t)");
         fYLabelColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModYLabelColor()");
         fYLabelOffset->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModYLabelOffset()");
         fYLabelFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModYLabelFont()");
         fYAxisColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModYAxisColor()");
         fYTickLength->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModYTickLength()");
         fOptLogy->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptLogy()");
         fYNdivMain->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModYNdivisions()");
         fYNdivSub->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModYNdivisions()");
         fYNdivSubSub->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModYNdivisions()");
         fYNdivisionsOptimize->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModYNdivisions()");
         fZTitleSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModZTitleSize()");
         fZTitleSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModZTitleSizeInPixels(Bool_t)");
         fZTitleColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModZTitleColor()");
         fZTitleOffset->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModZTitleOffset()");
         fZTitleFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModZTitleFont()");
         fZLabelSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModZLabelSize()");
         fZLabelSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModZLabelSizeInPixels(Bool_t)");
         fZLabelColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModZLabelColor()");
         fZLabelOffset->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModZLabelOffset()");
         fZLabelFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModZLabelFont()");
         fZAxisColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModZAxisColor()");
         fZTickLength->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModZTickLength()");
         fOptLogz->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptLogz()");
         fZNdivMain->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModZNdivisions()");
         fZNdivSub->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModZNdivisions()");
         fZNdivSubSub->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModZNdivisions()");
         fZNdivisionsOptimize->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModZNdivisions()");
         break;
      case 5: // TITLES
         fOptTitle->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptTitle()");
         fTitleColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModTitleFillColor()");
         fTitleStyle->Connect("PatternSelected(Style_t)", "TStyleManager", this, "ModTitleStyle()");
         fTitleTextColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModTitleTextColor()");
         fTitleFontSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModTitleFontSize()");
         fTitleFontSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModTitleFontSizeInPixels(Bool_t)");
         fTitleFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModTitleFont()");
         fTitleAlign->Connect("Selected(Int_t)", "TStyleManager", this, "ModTitleAlign()");
         fTitleBorderSize->Connect("Selected(Int_t)", "TStyleManager", this, "ModTitleBorderSize()");
         fTitleX->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModTitleX()");
         fTitleY->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModTitleY()");
         fTitleW->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModTitleW()");
         fTitleH->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModTitleH()");
         fLegendBorderSize->Connect("Selected(Int_t)", "TStyleManager", this, "ModLegendBorderSize()");
         break;
      case 6: // STATS
         fStatColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModStatColor(Pixel_t)");
         fStatStyle->Connect("PatternSelected(Style_t)", "TStyleManager", this, "ModStatStyle(Style_t)");
         fStatTextColor->Connect("ColorSelected(Pixel_t)", "TStyleManager", this, "ModStatTextColor(Pixel_t)");
         fStatFontSize->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModStatFontSize()");
         fStatFontSizeInPixels->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModStatFontSizeInPixels(Bool_t)");
         fStatFont->Connect("Selected(Int_t)", "TStyleManager", this, "ModStatFont()");
         fStatX->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModStatX()");
         fStatY->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModStatY()");
         fStatW->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModStatW()");
         fStatH->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModStatH()");
         fStatBorderSize->Connect("Selected(Int_t)", "TStyleManager", this, "ModStatBorderSize()");
         fOptStatName->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fOptStatEntries->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fOptStatOverflow->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fOptStatMean->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fOptStatUnderflow->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fOptStatRMS->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fOptStatSkewness->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fOptStatIntegral->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fOptStatKurtosis->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fOptStatErrors->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptStat()");
         fStatFormat->Connect("TextChanged(const char *)", "TStyleManager", this, "ModStatFormat(const char *)");
         fOptFitValues->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptFit()");
         fOptFitErrors->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptFit()");
         fOptFitProbability->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptFit()");
         fOptFitChi->Connect("Toggled(Bool_t)", "TStyleManager", this, "ModOptFit()");
         fFitFormat->Connect("TextChanged(const char *)", "TStyleManager", this, "ModFitFormat(const char *)");
         break;
      case 7: // PS / PDF
         fHeaderPS->Connect("TextChanged(const char *)", "TStyleManager", this, "ModHeaderPS()");
         fTitlePS->Connect("TextChanged(const char *)", "TStyleManager", this, "ModTitlePS()");
         fColorModelPS->Connect("Clicked(Int_t)", "TStyleManager", this, "ModColorModelPS()");
         fLineScalePS->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModLineScalePS()");
         fPaperSizePredef->Connect("Selected(Int_t)", "TStyleManager", this, "ModPaperSizePredef()");
         fPaperSizeX->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModPaperSizeXY()");
         fPaperSizeY->Connect("ValueSet(Long_t)", "TStyleManager", this, "ModPaperSizeXY()");
         break;
   }
}

//______________________________________________________________________________
void TStyleManager::DisconnectEditor(Int_t tabNum)
{
   //  Disconnect every widget entry of the editor from its slot. Must be
   // called before UpdateEditor() to avoid recursive calls.

   if (!fSigSlotConnected) return;
   fSigSlotConnected = kFALSE;

   switch (tabNum) {
      case 0: // GENERAL
         fFillColor->Disconnect("ColorSelected(Pixel_t)");
         fFillStyle->Disconnect("PatternSelected(Style_t)");
         fHatchesLineWidth->Disconnect("Selected(Int_t)");
         fHatchesSpacing->Disconnect("ValueSet(Long_t)");
         fMarkerColor->Disconnect("ColorSelected(Pixel_t)");
         fMarkerStyle->Disconnect("MarkerSelected(Style_t)");
         fMarkerSize->Disconnect("Selected(Int_t)");
         fScreenFactor->Disconnect("ValueSet(Long_t)");
         fLineColor->Disconnect("ColorSelected(Pixel_t)");
         fLineWidth->Disconnect("Selected(Int_t)");
         fLineStyle->Disconnect("Selected(Int_t)");
         fLineStyleEdit->Disconnect("Clicked()");
         fTextColor->Disconnect("ColorSelected(Pixel_t)");
         fTextSize->Disconnect("ValueSet(Long_t)");
         fTextFont->Disconnect("Selected(Int_t)");
         fTextAlign->Disconnect("Selected(Int_t)");
         fTextAngle->Disconnect("ValueSet(Long_t)");
         break;
      case 1: // CANVAS
         fCanvasColor->Disconnect("ColorSelected(Pixel_t)");
         fCanvasDefX->Disconnect("ValueSet(Long_t)");
         fCanvasDefY->Disconnect("ValueSet(Long_t)");
         fCanvasDefW->Disconnect("ValueSet(Long_t)");
         fCanvasDefH->Disconnect("ValueSet(Long_t)");
         fCanvasBorderMode->Disconnect("Clicked(Int_t)");
         fCanvasBorderSize->Disconnect("Selected(Int_t)");
         fOptDateBool->Disconnect("Toggled(Bool_t)");
         fAttDateTextColor->Disconnect("ColorSelected(Pixel_t)");
         fAttDateTextSize->Disconnect("ValueSet(Long_t)");
         fOptDateFormat->Disconnect("Selected(Int_t)");
         fAttDateTextFont->Disconnect("Selected(Int_t)");
         fAttDateTextAngle->Disconnect("ValueSet(Long_t)");
         fAttDateTextAlign->Disconnect("Selected(Int_t)");
         fDateX->Disconnect("ValueSet(Long_t)");
         fDateY->Disconnect("ValueSet(Long_t)");
         break;
      case 2: // PAD
         fPadTopMargin->Disconnect("ValueSet(Long_t)");
         fPadBottomMargin->Disconnect("ValueSet(Long_t)");
         fPadLeftMargin->Disconnect("ValueSet(Long_t)");
         fPadRightMargin->Disconnect("ValueSet(Long_t)");
         fPadBorderMode->Disconnect("Clicked(Int_t)");
         fPadBorderSize->Disconnect("Selected(Int_t)");
         fPadColor->Disconnect("ColorSelected(Pixel_t)");
         fPadTickX->Disconnect("Toggled(Bool_t)");
         fPadTickY->Disconnect("Toggled(Bool_t)");
         fPadGridX->Disconnect("Toggled(Bool_t)");
         fPadGridY->Disconnect("Toggled(Bool_t)");
         fGridColor->Disconnect("ColorSelected(Pixel_t)");
         fGridWidth->Disconnect("Selected(Int_t)");
         fGridStyle->Disconnect("Selected(Int_t)");
         break;
      case 3: // HISTOS
         fHistFillColor->Disconnect("ColorSelected(Pixel_t)");
         fHistFillStyle->Disconnect("PatternSelected(Style_t)");
         fHistLineColor->Disconnect("ColorSelected(Pixel_t)");
         fHistLineWidth->Disconnect("Selected(Int_t)");
         fHistLineStyle->Disconnect("Selected(Int_t)");
         fBarWidth->Disconnect("ValueSet(Long_t)");
         fBarOffset->Disconnect("ValueSet(Long_t)");
         fHistMinimumZero->Disconnect("Toggled(Bool_t)");
         fPaintTextFormat->Disconnect("TextChanged(const char *)");
         fNumberContours->Disconnect("ValueSet(Long_t)");
         fLegoInnerR->Disconnect("ValueSet(Long_t)");
         fFrameFillColor->Disconnect("ColorSelected(Pixel_t)");
         fFrameFillStyle->Disconnect("PatternSelected(Style_t)");
         fFrameLineColor->Disconnect("ColorSelected(Pixel_t)");
         fFrameLineWidth->Disconnect("Selected(Int_t)");
         fFrameLineStyle->Disconnect("Selected(Int_t)");
         fPaletteEdit->Disconnect("Clicked()");
         fFrameBorderMode->Disconnect("Clicked(Int_t)");
         fFrameBorderSize->Disconnect("Selected(Int_t)");
         fFuncColor->Disconnect("ColorSelected(Pixel_t)");
         fFuncWidth->Disconnect("Selected(Int_t)");
         fFuncStyle->Disconnect("Selected(Int_t)");
         fDrawBorder->Disconnect("Toggled(Bool_t)");
         fEndErrorSize->Disconnect("ValueSet(Long_t)");
         fErrorX->Disconnect("ValueSet(Long_t)");
         break;
      case 4: // AXIS
         fTimeOffsetDate->Disconnect("ValueSet(Long_t)");
         fTimeOffsetTime->Disconnect("ValueSet(Long_t)");
         fStripDecimals->Disconnect("Toggled(Bool_t)");
         fApplyOnXYZ->Disconnect("Clicked()");
         fXTitleSize->Disconnect("ValueSet(Long_t)");
         fXTitleColor->Disconnect("ColorSelected(Pixel_t)");
         fXTitleOffset->Disconnect("ValueSet(Long_t)");
         fXTitleFont->Disconnect("Selected(Int_t)");
         fXLabelSize->Disconnect("ValueSet(Long_t)");
         fXLabelColor->Disconnect("ColorSelected(Pixel_t)");
         fXLabelOffset->Disconnect("ValueSet(Long_t)");
         fXLabelFont->Disconnect("Selected(Int_t)");
         fXAxisColor->Disconnect("ColorSelected(Pixel_t)");
         fXTickLength->Disconnect("ValueSet(Long_t)");
         fOptLogx->Disconnect("Toggled(Bool_t)");
         fXNdivMain->Disconnect("ValueSet(Long_t)");
         fXNdivSub->Disconnect("ValueSet(Long_t)");
         fXNdivSubSub->Disconnect("ValueSet(Long_t)");
         fXNdivisionsOptimize->Disconnect("Toggled(Bool_t)");
         fYTitleSize->Disconnect("ValueSet(Long_t)");
         fYTitleColor->Disconnect("ColorSelected(Pixel_t)");
         fYTitleOffset->Disconnect("ValueSet(Long_t)");
         fYTitleFont->Disconnect("Selected(Int_t)");
         fYLabelSize->Disconnect("ValueSet(Long_t)");
         fYLabelColor->Disconnect("ColorSelected(Pixel_t)");
         fYLabelOffset->Disconnect("ValueSet(Long_t)");
         fYLabelFont->Disconnect("Selected(Int_t)");
         fYAxisColor->Disconnect("ColorSelected(Pixel_t)");
         fYTickLength->Disconnect("ValueSet(Long_t)");
         fOptLogy->Disconnect("Toggled(Bool_t)");
         fYNdivMain->Disconnect("ValueSet(Long_t)");
         fYNdivSub->Disconnect("ValueSet(Long_t)");
         fYNdivSubSub->Disconnect("ValueSet(Long_t)");
         fYNdivisionsOptimize->Disconnect("Toggled(Bool_t)");
         fZTitleSize->Disconnect("ValueSet(Long_t)");
         fZTitleColor->Disconnect("ColorSelected(Pixel_t)");
         fZTitleOffset->Disconnect("ValueSet(Long_t)");
         fZTitleFont->Disconnect("Selected(Int_t)");
         fZLabelSize->Disconnect("ValueSet(Long_t)");
         fZLabelColor->Disconnect("ColorSelected(Pixel_t)");
         fZLabelOffset->Disconnect("ValueSet(Long_t)");
         fZLabelFont->Disconnect("Selected(Int_t)");
         fZAxisColor->Disconnect("ColorSelected(Pixel_t)");
         fZTickLength->Disconnect("ValueSet(Long_t)");
         fOptLogz->Disconnect("Toggled(Bool_t)");
         fZNdivMain->Disconnect("ValueSet(Long_t)");
         fZNdivSub->Disconnect("ValueSet(Long_t)");
         fZNdivSubSub->Disconnect("ValueSet(Long_t)");
         fZNdivisionsOptimize->Disconnect("Toggled(Bool_t)");
         break;
      case 5: // TITLES
         fOptTitle->Disconnect("Toggled(Bool_t)");
         fTitleColor->Disconnect("ColorSelected(Pixel_t)");
         fTitleStyle->Disconnect("PatternSelected(Style_t)");
         fTitleTextColor->Disconnect("ColorSelected(Pixel_t)");
         fTitleFontSize->Disconnect("ValueSet(Long_t)");
         fTitleFont->Disconnect("Selected(Int_t)");
         fTitleAlign->Disconnect("Selected(Int_t)");
         fTitleBorderSize->Disconnect("Selected(Int_t)");
         fTitleX->Disconnect("ValueSet(Long_t)");
         fTitleY->Disconnect("ValueSet(Long_t)");
         fTitleW->Disconnect("ValueSet(Long_t)");
         fTitleH->Disconnect("ValueSet(Long_t)");
         fLegendBorderSize->Disconnect("Selected(Int_t)");
         break;
      case 6: // STATS
         fStatColor->Disconnect("ColorSelected(Pixel_t)");
         fStatStyle->Disconnect("PatternSelected(Style_t)");
         fStatTextColor->Disconnect("ColorSelected(Pixel_t)");
         fStatFontSize->Disconnect("ValueSet(Long_t)");
         fStatFont->Disconnect("Selected(Int_t)");
         fStatX->Disconnect("ValueSet(Long_t)");
         fStatY->Disconnect("ValueSet(Long_t)");
         fStatW->Disconnect("ValueSet(Long_t)");
         fStatH->Disconnect("ValueSet(Long_t)");
         fStatBorderSize->Disconnect("Selected(Int_t)");
         fOptStatName->Disconnect("Toggled(Bool_t)");
         fOptStatEntries->Disconnect("Toggled(Bool_t)");
         fOptStatOverflow->Disconnect("Toggled(Bool_t)");
         fOptStatMean->Disconnect("Toggled(Bool_t)");
         fOptStatUnderflow->Disconnect("Toggled(Bool_t)");
         fOptStatRMS->Disconnect("Toggled(Bool_t)");
         fOptStatSkewness->Disconnect("Toggled(Bool_t)");
         fOptStatIntegral->Disconnect("Toggled(Bool_t)");
         fOptStatKurtosis->Disconnect("Toggled(Bool_t)");
         fOptStatErrors->Disconnect("Toggled(Bool_t)");
         fStatFormat->Disconnect("TextChanged(const char *)");
         fOptFitValues->Disconnect("Toggled(Bool_t)");
         fOptFitErrors->Disconnect("Toggled(Bool_t)");
         fOptFitProbability->Disconnect("Toggled(Bool_t)");
         fOptFitChi->Disconnect("Toggled(Bool_t)");
         fFitFormat->Disconnect("TextChanged(const char *)");
         break;
      case 7: // PS / PDF
         fHeaderPS->Disconnect("TextChanged(const char *)");
         fTitlePS->Disconnect("TextChanged(const char *)");
         fColorModelPS->Disconnect("Clicked(Int_t)");
         fLineScalePS->Disconnect("ValueSet(Long_t)");
         fPaperSizePredef->Disconnect("Selected(Int_t)");
         fPaperSizeX->Disconnect("ValueSet(Long_t)");
         fPaperSizeY->Disconnect("ValueSet(Long_t)");
         break;
   }
}

//______________________________________________________________________________
void TStyleManager::DoEditor()
{
   //  Called each time something is changed in the style editor. Thanks to
   // this method, we can know if the style differs from the original style.

   fStyleChanged = kTRUE;

   // Update the status bar.
   UpdateStatusBar();

   // Update the preview if the real time mode is selected.
   if (fRealTimePreview)
      DoEditionUpdatePreview();
}

//______________________________________________________________________________
void TStyleManager::AddEdition(TGCompositeFrame *p)
{
   //  Add the editor to the frame 'p'. It contains the tabs allowing the user
   // to modify every data member of the current TStyle object.

   TGLayoutHints *layout1 = new TGLayoutHints(kLHintsExpandX, 8, 8, 5, 5);
   fTrashListLayout->Add(layout1);
   TGLayoutHints *layout2 = new TGLayoutHints(kLHintsExpandX, 10, 10);
   fTrashListLayout->Add(layout2);

   fEditionFrame = new TGVerticalFrame(p);
   fEditionFrame->ChangeOptions(kRaisedFrame);

   fEditionTab = new TGTab(fEditionFrame, 200, 150);
   fEditionTab->Associate(this);
   CreateTabGeneral(fEditionTab->AddTab("General"));
   CreateTabCanvas(fEditionTab->AddTab("Canvas"));
   CreateTabPad(fEditionTab->AddTab("Pad"));
   CreateTabHistos(fEditionTab->AddTab("Histograms"));
   CreateTabAxis(fEditionTab->AddTab("Axis"));
   CreateTabTitle(fEditionTab->AddTab("Title"));
   CreateTabStats(fEditionTab->AddTab("Stats"));
   CreateTabPsPdf(fEditionTab->AddTab("PS / PDF"));
   fEditionFrame->AddFrame(fEditionTab, layout1);

   fEditionButtonFrame = new TGHorizontalFrame(fEditionFrame);
   fEditionHelp = new TGTextButton(fEditionButtonFrame, "He&lp", kEditButHelp);
   fEditionHelp->Associate(this);
   fEditionButtonFrame->AddFrame(fEditionHelp, layout1);
   fEditionUpdatePreview = new TGTextButton(fEditionButtonFrame, "&Update Preview", kEditButUpPrev);
   fEditionUpdatePreview->Associate(this);
   fEditionUpdatePreview->SetEnabled(kFALSE);
   fEditionButtonFrame->AddFrame(fEditionUpdatePreview, layout1);
   fEditionReset = new TGTextButton(fEditionButtonFrame, "&Reset", kEditButReset);
   fEditionReset->Associate(this);
   fEditionButtonFrame->AddFrame(fEditionReset, layout1);
   fEditionFrame->AddFrame(fEditionButtonFrame, layout1);

   p->AddFrame(fEditionFrame, layout1);

   fEditionHelp->SetToolTipText("Help about the current tab");
   fEditionUpdatePreview->SetToolTipText("Force the refresh of the preview window");
   fEditionReset->SetToolTipText("Reset the selected style");
}

//______________________________________________________________________________
void TStyleManager::CreateTabGeneral(TGCompositeFrame *tab)
{
   // Add the tab 'General' to the editor.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 10, 21, 5, 5);
   fTrashListLayout->Add(layout);

   TGHorizontalFrame *h1 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h1);
   AddGeneralFill(h1);
   AddGeneralLine(h1);
   tab->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h2);
   AddGeneralText(h2);
   TGVerticalFrame *v = new TGVerticalFrame(h2);
   fTrashListFrame->AddFirst(v);
   AddGeneralMarker(v);
   TGHorizontalFrame *h3 = new TGHorizontalFrame(v);
   fTrashListFrame->AddFirst(h3);
   fScreenFactor = AddNumberEntry(h3, 0, 0, 0, kGeneralScreenFactor,
                        "Screen factor:", 0, 6, TGNumberFormat::kNESRealOne,
                        TGNumberFormat::kNEAPositive,
                        TGNumberFormat::kNELLimitMinMax, 0.2, 5);
   v->AddFrame(h3, layout);
   h2->AddFrame(v, fLayoutExpandXY);
   tab->AddFrame(h2, fLayoutExpandX);

   fScreenFactor->GetNumberEntry()->SetToolTipText("Coefficient for different screen's resolutions");
}

//______________________________________________________________________________
void TStyleManager::AddGeneralFill(TGCompositeFrame *f)
{
   // Add the 'Fill' group frame to the 'General' tab.

   TGLayoutHints *layout2 = new TGLayoutHints(kLHintsExpandX, 5, 0, 5, 5);
   fTrashListLayout->Add(layout2);

   TGGroupFrame *gf = new TGGroupFrame(f, "Fill");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fFillColor = AddColorEntry(h1, kGeneralFillColor);
   fFillStyle = AddFillStyleEntry(h1, kGeneralFillStyle);
   gf->AddFrame(h1, fLayoutExpandX);
   AddTitle(gf, "Hatchings");
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fHatchesLineWidth = AddLineWidthEntry(h2, kGeneralHatchesLineWidth);
   fHatchesSpacing = AddNumberEntry(h2, 0, 5, 0, kGeneralHatchesSpacing,
                        "", 0, 5, TGNumberFormat::kNESRealOne,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0.1, 5);
   gf->AddFrame(h2, layout2);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fFillColor->SetToolTipText("General fill color");
//   fFillStyle->SetToolTipText("General fill pattern");
   fHatchesSpacing->GetNumberEntry()->SetToolTipText("Spacing between the hatching's lines");
}

//______________________________________________________________________________
void TStyleManager::AddGeneralLine(TGCompositeFrame *f)
{
   // Add the 'Line' group frame to the 'General' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Line");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fLineColor = AddColorEntry(h, kGeneralLineColor);
   fLineWidth = AddLineWidthEntry(h, kGeneralLineWidth);
   gf->AddFrame(h, fLayoutExpandX);
   fLineStyle = AddLineStyleEntry(gf, kGeneralLineStyle);
   fLineStyleEdit = AddTextButton(gf, "Lines' Style Editor...", kGeneralLineStyleEdit);
   fLineStyleEdit->SetEnabled(kFALSE);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fLineColor->SetToolTipText("General line color");
}

//______________________________________________________________________________
void TStyleManager::AddGeneralText(TGCompositeFrame *f)
{
   // Add the 'Text' group frame to the 'General' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Text");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fTextColor = AddColorEntry(h1, kGeneralTextColor);
   fTextFont = AddFontTypeEntry(h1, kGeneralTextFont);
   gf->AddFrame(h1, fLayoutExpandX);
   fTextAlign = AddTextAlignEntry(gf, kGeneralTextAlign);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fTextSizeInPixels = AddCheckButton(h2, "Pixels", kGeneralTextSizeInPixels);
   fTextSize = AddNumberEntry(h2, 21, 10, 0, kGeneralTextSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   fTextAngle = AddNumberEntry(gf, 0, 0, 0, kGeneralTextAngle, "Angle:",
                        0, 5, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, -180, 180);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fTextColor->SetToolTipText("General text color");
   fTextSizeInPixels->SetToolTipText("Set the text size in pixels if selected, otherwise - in % of pad.");
   fTextSize->GetNumberEntry()->SetToolTipText("General text size (in pixels or in % of pad)");
   fTextAngle->GetNumberEntry()->SetToolTipText("General text angle");
}

//______________________________________________________________________________
void TStyleManager::AddGeneralMarker(TGCompositeFrame *f)
{
   // Add the 'Marker' group frame to the 'General' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Marker");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fMarkerColor = AddColorEntry(h, kGeneralMarkerColor);
   fMarkerStyle = AddMarkerStyleEntry(h, kGeneralMarkerStyle);
   fMarkerSize = AddMarkerSizeEntry(h, kGeneralMarkerSize);
   gf->AddFrame(h, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fMarkerColor->SetToolTipText("Marker color");
//   fMarkerStyle->SetToolTipText("Marker shape");
}

//______________________________________________________________________________
void TStyleManager::CreateTabCanvas(TGCompositeFrame *tab)
{
   // Add the tab 'Canvas' to the editor.

   TGHorizontalFrame *h = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h);
   TGVerticalFrame *v1 = new TGVerticalFrame(h);
   fTrashListFrame->AddFirst(v1);
   AddCanvasFill(v1);
   AddCanvasGeometry(v1);
   AddCanvasBorder(v1);
   h->AddFrame(v1, fLayoutExpandXY);
   TGVerticalFrame *v2 = new TGVerticalFrame(h);
   fTrashListFrame->AddFirst(v2);
   AddCanvasDate(v2);
   h->AddFrame(v2, fLayoutExpandXY);
   tab->AddFrame(h, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::AddCanvasFill(TGCompositeFrame *f)
{
   // Add the 'Fill' group frame to the 'Canvas' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Fill");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fCanvasColor = AddColorEntry(h, kCanvasColor);
   gf->AddFrame(h, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fCanvasColor->SetToolTipText("Color used to fill canvases");
}

//______________________________________________________________________________
void TStyleManager::AddCanvasGeometry(TGCompositeFrame *f)
{
   // Add the 'Geometry' group frame to the 'Canvas' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Geometry");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fCanvasDefX = AddNumberEntry(h1, 0, 9, 0, kCanvasDefX, "X:",
                        0, 5, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAPositive,
                        TGNumberFormat::kNELLimitMinMax, 0, 5000);
   fCanvasDefY = AddNumberEntry(h1, 7, 8, 0, kCanvasDefY, "Y:",
                        0, 5, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAPositive,
                        TGNumberFormat::kNELLimitMinMax, 0, 5000);
   gf->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fCanvasDefW = AddNumberEntry(h2, 0, 7, 0, kCanvasDefW, "W:",
                        0, 5, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAPositive,
                        TGNumberFormat::kNELLimitMinMax, 0, 5000);
   fCanvasDefH = AddNumberEntry(h2, 7, 8, 0, kCanvasDefH, "H:",
                        0, 5, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAPositive,
                        TGNumberFormat::kNELLimitMinMax, 0, 5000);
   gf->AddFrame(h2, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

   fCanvasDefX->GetNumberEntry()->SetToolTipText("Canvases' default abscissa");
   fCanvasDefY->GetNumberEntry()->SetToolTipText("Canvases' default ordinate");
   fCanvasDefW->GetNumberEntry()->SetToolTipText("Canvases' default width");
   fCanvasDefH->GetNumberEntry()->SetToolTipText("Canvases' default height");
}

//______________________________________________________________________________
void TStyleManager::AddCanvasBorder(TGCompositeFrame *f)
{
   // Add the 'Border' group frame to the 'Canvas' tab.

   fCanvasBorderMode = AddBorderModeEntry(f, kCanvasBorderModeSunken, kCanvasBorderModeNone, kCanvasBorderModeRaised);
   fCanvasBorderSize = AddLineWidthEntry(fCanvasBorderMode, kCanvasBorderSize);
}

//______________________________________________________________________________
void TStyleManager::AddCanvasDate(TGCompositeFrame *f)
{
   // Add the 'Date' group frame to the 'Canvas' tab.

   TGLayoutHints *layout2 = new TGLayoutHints(kLHintsExpandX, 10);
   fTrashListLayout->Add(layout2);

   TGGroupFrame *gf = new TGGroupFrame(f, "Date");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fOptDateBool = AddCheckButton(h1, "Show", kCanvasOptDateBool, 23);
   fAttDateTextColor = AddColorEntry(h1, kCanvasAttDateTextColor);
   gf->AddFrame(h1, fLayoutExpandX);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fAttDateTextSizeInPixels = AddCheckButton(h2, "Pixels", kCanvasAttDateTextSizeInPixels);
   fAttDateTextSize = AddNumberEntry(h2, 22, 10, 0, kCanvasAttDateTextSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   fOptDateFormat = AddDateFormatEntry(gf, kCanvasOptDateFormat);
   fAttDateTextFont = AddFontTypeEntry(gf, kCanvasAttDateTextFont);
   fAttDateTextAlign = AddTextAlignEntry(gf, kCanvasAttDateTextAlign);
   fAttDateTextAngle = AddNumberEntry(gf, 0, 0, 0, kCanvasAttDateTextAngle,
                        "Angle:", 0, 6, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, -180, 180);
   AddTitle(gf, "Position");
   TGVerticalFrame *h3 = new TGVerticalFrame(gf);
   fTrashListFrame->AddFirst(h3);
   fDateX = AddNumberEntry(h3, 0, 0, 0, kCanvasDateX, "X (% of Pad):",
                        0, 6, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 100);
   fDateY = AddNumberEntry(h3, 0, 0, 0, kCanvasDateY, "Y (% of Pad):",
                        0, 6, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 100);
   gf->AddFrame(h3, layout2);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fOptDateBool->SetToolTipText("Show / Hide the date in canvases");
// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fAttDateTextColor->SetToolTipText("Color of the date text");
   fAttDateTextSizeInPixels->SetToolTipText("Set the date text size in pixels if selected, otherwise - in % of pad");
   fAttDateTextSize->GetNumberEntry()->SetToolTipText("Date text size (in pixels or in % of pad)");
   fAttDateTextAngle->GetNumberEntry()->SetToolTipText("Date text angle");
   fDateX->GetNumberEntry()->SetToolTipText("Date abscissa in percent of pad");
   fDateY->GetNumberEntry()->SetToolTipText("Date ordinate in percent of pad");
}

//______________________________________________________________________________
void TStyleManager::CreateTabPad(TGCompositeFrame *tab)
{
   // Add the tab 'Pad' to the editor.

   TGHorizontalFrame *h1 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h1);
   AddPadMargin(h1);
   TGVerticalFrame *v = new TGVerticalFrame(h1);
   fTrashListFrame->AddFirst(v);
   AddPadFill(v);
   AddPadTicks(v);
   h1->AddFrame(v, fLayoutExpandXY);
   tab->AddFrame(h1, fLayoutExpandX);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h2);
   AddPadBorder(h2);
   AddPadGrid(h2);
   tab->AddFrame(h2, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::AddPadMargin(TGCompositeFrame *f)
{
   // Add the 'Margin' group frame to the 'Pad' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Margin (% of Pad)");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fPadLeftMargin = AddNumberEntry(h1, 0, 5, 0, kPadLeftMargin, "Left:",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   fPadRightMargin = AddNumberEntry(h1, 0, 0, 0, kPadRightMargin, "Right:",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   gf->AddFrame(h1, fLayoutExpandXY);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fPadTopMargin = AddNumberEntry(h2, 0, 5, 0, kPadTopMargin, "Top:",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   fPadBottomMargin = AddNumberEntry(h2, 0, 0, 0, kPadBottomMargin, "Bottom:",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   gf->AddFrame(h2, fLayoutExpandXY);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fPadLeftMargin->GetNumberEntry()->SetToolTipText("Pads' left margin");
   fPadRightMargin->GetNumberEntry()->SetToolTipText("Pads' right margin");
   fPadTopMargin->GetNumberEntry()->SetToolTipText("Pads' top margin");
   fPadBottomMargin->GetNumberEntry()->SetToolTipText("Pads' bottom margin");
}

//______________________________________________________________________________
void TStyleManager::AddPadBorder(TGCompositeFrame *f)
{
   // Add the 'Border' group frame to the 'Pad' tab.

   fPadBorderMode = AddBorderModeEntry(f, kPadBorderModeSunken, kPadBorderModeNone, kPadBorderModeRaised);
   fPadBorderSize = AddLineWidthEntry(fPadBorderMode, kPadBorderSize);
}

//______________________________________________________________________________
void TStyleManager::AddPadFill(TGCompositeFrame *f)
{
   // Add the 'Fill' group frame to the 'Pad' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Fill");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fPadColor = AddColorEntry(h, kPadColor);
   gf->AddFrame(h, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fPadColor->SetToolTipText("Color used to fill pads");
}

//______________________________________________________________________________
void TStyleManager::AddPadTicks(TGCompositeFrame *f)
{
   // Add the 'Ticks' group frame to the 'Pad' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Ticks");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(h);
   fTrashListFrame->AddFirst(h1);
   fPadTickX = AddCheckButton(h1, "Along X", kPadTickX);
   h->AddFrame(h1, fLayoutExpandX);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(h);
   fTrashListFrame->AddFirst(h2);
   fPadTickY = AddCheckButton(h2, "Along Y", kPadTickY);
   h->AddFrame(h2, fLayoutExpandX);
   gf->AddFrame(h, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fPadTickX->SetToolTipText("Show / Hide the ticks along X");
   fPadTickY->SetToolTipText("Show / Hide the ticks along Y");
}

//______________________________________________________________________________
void TStyleManager::AddPadGrid(TGCompositeFrame *f)
{
   // Add the 'Grid' group frame to the 'Pad' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Grid");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   TGHorizontalFrame *h10 = new TGHorizontalFrame(h1);
   fTrashListFrame->AddFirst(h10);
   fPadGridX = AddCheckButton(h10, "Along X", kPadGridX);
   h1->AddFrame(h10, fLayoutExpandX);
   TGHorizontalFrame *h20 = new TGHorizontalFrame(h1);
   fTrashListFrame->AddFirst(h20);
   fPadGridY = AddCheckButton(h20, "Along Y", kPadGridY);
   h1->AddFrame(h20, fLayoutExpandX);
   gf->AddFrame(h1, fLayoutExpandX);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fGridColor = AddColorEntry(h2, kPadGridColor);
   fGridWidth = AddLineWidthEntry(h2, kPadGridWidth);
   gf->AddFrame(h2, fLayoutExpandX);
   fGridStyle = AddLineStyleEntry(gf, kPadGridStyle);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fGridColor->SetToolTipText("Line color for the grid");
   fPadGridX->SetToolTipText("Show / Hide the grid along X");
   fPadGridY->SetToolTipText("Show / Hide the grid along Y");
}

//______________________________________________________________________________
void TStyleManager::CreateTabHistos(TGCompositeFrame *tab)
{
   // Add the tab 'Histos' to the editor.

   fHistosTab = new TGTab(tab, 1, 1);
   fHistosTab->Associate(this);
   CreateTabHistosHistos(fHistosTab->AddTab("Histos"));
   CreateTabHistosFrames(fHistosTab->AddTab("Frames"));
   CreateTabHistosGraphs(fHistosTab->AddTab("Graphs"));
   tab->AddFrame(fHistosTab, fLayoutExpandXY);
}

//______________________________________________________________________________
void TStyleManager::CreateTabHistosHistos(TGCompositeFrame *tab)
{
   // Add the sub-tab 'Histos' to the tab 'Histos'.

   TGHorizontalFrame *h1 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h1);
   AddHistosHistosFill(h1);
   AddHistosHistosContours(h1);
   tab->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h2);
   AddHistosHistosLine(h2);
   AddHistosHistosAxis(h2);
   tab->AddFrame(h2, fLayoutExpandX);

   TGHorizontalFrame *h3 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h3);
   AddHistosHistosBar(h3);
   AddHistosHistosLegoInnerR(h3);
   tab->AddFrame(h3, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::AddHistosHistosFill(TGCompositeFrame *f)
{
   // Add the 'Fill' group frame to the 'Histos - Histos' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Fill");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fHistFillColor = AddColorEntry(h1, kHistFillColor);
   fHistFillStyle = AddFillStyleEntry(h1, kHistFillStyle);
   gf->AddFrame(h1, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fHistFillColor->SetToolTipText("Color used to fill histograms");
//   fHistFillStyle->SetToolTipText("Pattern used to fill histograms");
}

//______________________________________________________________________________
void TStyleManager::AddHistosHistosLine(TGCompositeFrame *f)
{
    // Add the 'Line' group frame to the 'Histos - Histos' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Line");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fHistLineColor = AddColorEntry(h, kHistLineColor);
   fHistLineWidth = AddLineWidthEntry(h, kHistLineWidth);
   gf->AddFrame(h, fLayoutExpandX);
   fHistLineStyle = AddLineStyleEntry(gf, kHistLineStyle);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fHistLineColor->SetToolTipText("Color used for histograms' lines");
}

//______________________________________________________________________________
void TStyleManager::AddHistosHistosBar(TGCompositeFrame *f)
{
   // Add the 'Bar' group frame to the 'Histos - Histos' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Bar");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fBarWidth = AddNumberEntry(h, 0, 5, 0, kHistBarWidth, "W:",
                        0, 5, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 5);
   fBarOffset = AddNumberEntry(h, 8, 5, 0, kHistBarOffset, "O:",
                        0, 5, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 5);
   gf->AddFrame(h, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fBarWidth->GetNumberEntry()->SetToolTipText("Width of bars");
   fBarOffset->GetNumberEntry()->SetToolTipText("Offset of bars");
}

//______________________________________________________________________________
void TStyleManager::AddHistosHistosContours(TGCompositeFrame *f)
{
   // Add the 'Contours' group frame to the 'Histos - Histos' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Contours");
   fTrashListFrame->AddFirst(gf);
   fNumberContours = AddNumberEntry(gf, 0, 0, 0, kHistNumberContours, "Number:",
                        0, 5, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fNumberContours->GetNumberEntry()->SetToolTipText("Number of level lines to draw");
}

//______________________________________________________________________________
void TStyleManager::AddHistosHistosAxis(TGCompositeFrame *f)
{
   // Add the 'Axis' group frame to the 'Histos - Histos' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Axis");
   fTrashListFrame->AddFirst(gf);
   fHistMinimumZero = AddCheckButton(gf, "Minimum zero", kHistMinimumZero);
   fPaintTextFormat = AddTextEntry(gf, "Paint format:", kHistPaintTextFormat);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fHistMinimumZero->SetToolTipText("Set to zero / Compute the minimum of axis range");
   fPaintTextFormat->SetToolTipText("Paint format of the axis labels in histograms");
}

//______________________________________________________________________________
void TStyleManager::AddHistosHistosLegoInnerR(TGCompositeFrame *f)
{
   // Add the '3D Cylindrical' group frame to the 'Histos - Histos' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "3D Cylindrical (%)");
   fTrashListFrame->AddFirst(gf);
   fLegoInnerR = AddNumberEntry(gf, 0, 0, 0, kHistLegoInnerR, "Inner radius:",
                        0, 5, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 100);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fLegoInnerR->GetNumberEntry()->SetToolTipText("Percent of radius allocated to the tube");
}

//______________________________________________________________________________
void TStyleManager::CreateTabHistosFrames(TGCompositeFrame *tab)
{
   // Add the sub-tab 'Frames' to the tab 'Histos'.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 10, 10, 0, 13);
   fTrashListLayout->Add(layout);

   TGHorizontalFrame *h1 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h1);

   TGVerticalFrame *v1 = new TGVerticalFrame(h1);
   fTrashListFrame->AddFirst(v1);
   AddHistosFramesFill(v1);
   AddHistosFramesLine(v1);
   h1->AddFrame(v1, fLayoutExpandXY);

   TGVerticalFrame *v2 = new TGVerticalFrame(h1);
   fTrashListFrame->AddFirst(v2);
   AddHistosFramesBorder(v2);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(v2);
   fTrashListFrame->AddFirst(h2);
   fPaletteEdit = AddTextButton(h2, "Palette Editor...", kFramePaletteEdit);
   fPaletteEdit->SetEnabled(kFALSE);
   v2->AddFrame(h2, layout);
   h1->AddFrame(v2, fLayoutExpandXY);

   tab->AddFrame(h1, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::AddHistosFramesFill(TGCompositeFrame *f)
{
   // Add the 'Fill' group frame to the 'Histos - Frames' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Fill");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fFrameFillColor = AddColorEntry(h1, kFrameFillColor);
   fFrameFillStyle = AddFillStyleEntry(h1, kFrameFillStyle);
   gf->AddFrame(h1, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fFrameFillColor->SetToolTipText("Color used to fill frames");
//   fFrameFillStyle->SetToolTipText("Pattern used to fill frames");
}

//______________________________________________________________________________
void TStyleManager::AddHistosFramesLine(TGCompositeFrame *f)
{
   // Add the 'Line' group frame to the 'Histos - Frames' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Line");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fFrameLineColor = AddColorEntry(h, kFrameLineColor);
   fFrameLineWidth = AddLineWidthEntry(h, kFrameLineWidth);
   gf->AddFrame(h, fLayoutExpandX);
   fFrameLineStyle = AddLineStyleEntry(gf, kFrameLineStyle);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fFrameLineColor->SetToolTipText("Color of lines in frames");
}

//______________________________________________________________________________
void TStyleManager::AddHistosFramesBorder(TGCompositeFrame *f)
{
   // Add the 'Border' group frame to the 'Histos - Frames' tab.

   fFrameBorderMode = AddBorderModeEntry(f, kFrameBorderModeSunken, kFrameBorderModeNone, kFrameBorderModeRaised);
   fFrameBorderSize = AddLineWidthEntry(fFrameBorderMode, kFrameBorderSize);
}

//______________________________________________________________________________
void TStyleManager::CreateTabHistosGraphs(TGCompositeFrame *tab)
{
   // Add the sub-tab 'Graphs' to the tab 'Histos'.

   TGHorizontalFrame *h = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h);
   AddHistosGraphsLine(h);
   AddHistosGraphsErrors(h);
   tab->AddFrame(h, fLayoutExpandX);
   AddHistosGraphsBorder(tab);
}

//______________________________________________________________________________
void TStyleManager::AddHistosGraphsLine(TGCompositeFrame *f)
{
   // Add the 'Line' group frame to the 'Histos - Graphs' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Line");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fFuncColor = AddColorEntry(h, kGraphsFuncColor);
   fFuncWidth = AddLineWidthEntry(h, kGraphsFuncWidth);
   gf->AddFrame(h, fLayoutExpandX);
   fFuncStyle = AddLineStyleEntry(gf, kGraphsFuncStyle);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fFuncColor->SetToolTipText("Color of curves in graphs");
}

//______________________________________________________________________________
void TStyleManager::AddHistosGraphsBorder(TGCompositeFrame *f)
{
   // Add the 'Draw Border' check button to the 'Histos - Graphs' tab.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 10, 21, 5, 5);
   fTrashListLayout->Add(layout);

   TGHorizontalFrame *h = new TGHorizontalFrame(f);
   fTrashListFrame->AddFirst(h);
   fDrawBorder = AddCheckButton(h, "Draw Border (for Filled Function)", kGraphsDrawBorder);
   f->AddFrame(h, layout);

   fDrawBorder->SetToolTipText("Show / Hide the border of filled functions");
}

//______________________________________________________________________________
void TStyleManager::AddHistosGraphsErrors(TGCompositeFrame *f)
{
   // Add the 'Errors' group frame to the 'Histos - Graphs' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Errors");
   fTrashListFrame->AddFirst(gf);
   fEndErrorSize = AddNumberEntry(gf, 0, 0, 0, kGraphsEndErrorSize,
                        "End error size:", 0, 4, TGNumberFormat::kNESRealOne,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 5);
   fErrorX = AddNumberEntry(gf, 0, 0, 0, kGraphsErrorX, "Error X (% of bin):",
                        0, 4, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 100);
   f->AddFrame(gf, fLayoutExpandXMargin);

   fEndErrorSize->GetNumberEntry()->SetToolTipText("Size of lines drawn at the end of error bars");
   fErrorX->GetNumberEntry()->SetToolTipText("Percent of the bin width to use for errors along X");
}

//______________________________________________________________________________
void TStyleManager::CreateTabAxis(TGCompositeFrame *tab)
{
   // Add the tab 'Axis' to the editor.

   TGLayoutHints *layout =
                  new TGLayoutHints(kLHintsNormal, 10, 13, 3);
   fTrashListLayout->Add(layout);

   TGHorizontalFrame *h = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h);

   TGVerticalFrame *h3 = new TGVerticalFrame(h);
   fTrashListFrame->AddFirst(h3);
   fStripDecimals = AddCheckButton(h3, "Decimal labels' part", kAxisStripDecimals, 0, 8);
   TGVerticalFrame *space = new TGVerticalFrame(h3);
   fTrashListFrame->AddFirst(space);
   h3->AddFrame(space, fLayoutExpandXY);
   fApplyOnXYZ = AddTextButton(h3, "Apply on XYZ", kAxisApplyOnXYZ);
   h->AddFrame(h3, layout);

   TGGroupFrame *gf = new TGGroupFrame(h, "Date/Time Offset");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fTimeOffsetDate = AddNumberEntry(h2, 0, 13, 10, kAxisTimeOffsetDate, "",
                        0, 10, TGNumberFormat::kNESDayMYear,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELNoLimits, 0, 0);
   fTimeOffsetTime = AddNumberEntry(h2, 0, 15, 0, kAxisTimeOffsetTime, "",
                        0, 8, TGNumberFormat::kNESHourMinSec,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELNoLimits, 0, 0);
   gf->AddFrame(h2, fLayoutExpandX);
   h->AddFrame(gf, fLayoutExpandXMargin);
   tab->AddFrame(h, fLayoutExpandX);

   fAxisTab = new TGTab(tab);
   fAxisTab->Associate(this);
   CreateTabAxisX(fAxisTab->AddTab("X axis"));
   CreateTabAxisY(fAxisTab->AddTab("Y axis"));
   CreateTabAxisZ(fAxisTab->AddTab("Z axis"));
   tab->AddFrame(fAxisTab, fLayoutExpandXY);

   fStripDecimals->SetToolTipText("Draw / Hide the decimal part of labels");
   fApplyOnXYZ->SetToolTipText("Apply settings of the currently selected axis on XYZ");
   fTimeOffsetDate->GetNumberEntry()->SetToolTipText("Date offset for axis (dd/mm/yyyy)");
   fTimeOffsetTime->GetNumberEntry()->SetToolTipText("Time offset for axis (hh/mm/ss)");
}

//______________________________________________________________________________
void TStyleManager::CreateTabAxisX(TGCompositeFrame *tab)
{
   // Add the sub-tab 'X Axis' to the tab 'Axis'.

   TGHorizontalFrame *h1 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h1);
   AddAxisXLine(h1);
   AddAxisXDivisions(h1);
   tab->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h2);
   AddAxisXTitle(h2);
   AddAxisXLabels(h2);
   tab->AddFrame(h2, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::AddAxisXLine(TGCompositeFrame *f)
{
   // Add the 'Line' group frame to the 'Axis - X Axis' tab.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 20);
   fTrashListLayout->Add(layout);

   TGGroupFrame *gf = new TGGroupFrame(f, "Line");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fXAxisColor = AddColorEntry(h, kAxisXAxisColor);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(h);
   fTrashListFrame->AddFirst(h2);
   fXTickLength = AddNumberEntry(h2, 3, 8, 0, kAxisXTickLength, "Ticks:",
                        0, 5, TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 5);
   h->AddFrame(h2, layout);
   gf->AddFrame(h, fLayoutExpandX);
   fOptLogx = AddCheckButton(gf, "Logarithmic scale", kAxisOptLogx);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fXAxisColor->SetToolTipText("Color of axis' line");
   fXTickLength->GetNumberEntry()->SetToolTipText("Set the ticks' length");
   fOptLogx->SetToolTipText("Draw logarithmic scale");
}

//______________________________________________________________________________
void TStyleManager::AddAxisXTitle(TGCompositeFrame *f)
{
   // Add the 'Title' group frame to the 'Axis - X Axis' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Title");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fXTitleColor = AddColorEntry(h1, kAxisXTitleColor);
   fXTitleFont = AddFontTypeEntry(h1, kAxisXTitleFont);
   gf->AddFrame(h1, fLayoutExpandX);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fXTitleSizeInPixels = AddCheckButton(h2, "Pixels", kAxisXTitleSizeInPixels);
   fXTitleSize = AddNumberEntry(h2, 21, 8, 0, kAxisXTitleSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   fXTitleOffset = AddNumberEntry(gf, 68, 8, 0, kAxisXTitleOffset, "Offset:",
                        0, 5, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 1);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fXTitleColor->SetToolTipText("Color of axis' title");
   fXTitleSizeInPixels->SetToolTipText("Set the title size in pixels if selected, otherwise - in % of pad");
   fXTitleSize->GetNumberEntry()->SetToolTipText("Title size (in pixels or in % of pad)");
   fXTitleOffset->GetNumberEntry()->SetToolTipText("Offset between axis and title");
}

//______________________________________________________________________________
void TStyleManager::AddAxisXDivisions(TGCompositeFrame *f)
{
   // Add the 'Divisions' group frame to the 'Axis - X Axis' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Divisions");
   fTrashListFrame->AddFirst(gf);

   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fXNdivSubSub = AddNumberEntry(h1, 0, 0, 0, kAxisXNdivSubSub, "",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   fXNdivSub = AddNumberEntry(h1, 0, 18, 0, kAxisXNdivSub, "",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   fXNdivMain = AddNumberEntry(h1, 0, 18, 0, kAxisXNdivMain, "",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   gf->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fXNdivisionsOptimize = AddCheckButton(h2, "Optimize", kAxisXNdivisionsOptimize);
   gf->AddFrame(h2, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

   fXNdivMain->GetNumberEntry()->SetToolTipText("Primary axis divisions");
   fXNdivSub->GetNumberEntry()->SetToolTipText("Secondary axis divisions");
   fXNdivSubSub->GetNumberEntry()->SetToolTipText("Tertiary axis divisions");
   fXNdivisionsOptimize->SetToolTipText("Optimize the number of axis divisions if selected");
}

//______________________________________________________________________________
void TStyleManager::AddAxisXLabels(TGCompositeFrame *f)
{
   // Add the 'Labels' group frame to the 'Axis - X Axis' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Labels");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fXLabelColor = AddColorEntry(h1, kAxisXLabelColor);
   fXLabelFont = AddFontTypeEntry(h1, kAxisXLabelFont);
   gf->AddFrame(h1, fLayoutExpandX);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fXLabelSizeInPixels = AddCheckButton(h2, "Pixels", kAxisXLabelSizeInPixels);
   fXLabelSize = AddNumberEntry(h2, 22, 8, 0, kAxisXLabelSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   fXLabelOffset = AddNumberEntry(gf, 69, 8, 0, kAxisXTitleOffset, "Offset:",
                        0, 5, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 1);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fXLabelColor->SetToolTipText("Color of axis' labels");
   fXLabelSizeInPixels->SetToolTipText("Set the labels size in pixels if selected, otherwise - in % of pad");
   fXLabelSize->GetNumberEntry()->SetToolTipText("Label size (in pixels or in % of pad)");
   fXLabelOffset->GetNumberEntry()->SetToolTipText("Offset between axis and labels");
}

//______________________________________________________________________________
void TStyleManager::CreateTabAxisY(TGCompositeFrame *tab)
{
   // Add the sub-tab 'Y Axis' to the tab 'Axis'.

   TGHorizontalFrame *h1 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h1);
   AddAxisYLine(h1);
   AddAxisYDivisions(h1);
   tab->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h2);
   AddAxisYTitle(h2);
   AddAxisYLabels(h2);
   tab->AddFrame(h2, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::AddAxisYLine(TGCompositeFrame *f)
{
   // Add the 'Line' group frame to the 'Axis - Y Axis' tab.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 20);
   fTrashListLayout->Add(layout);

   TGGroupFrame *gf = new TGGroupFrame(f, "Line");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fYAxisColor = AddColorEntry(h, kAxisYAxisColor);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(h);
   fTrashListFrame->AddFirst(h2);
   fYTickLength = AddNumberEntry(h2, 3, 8, 0, kAxisYTickLength, "Ticks:",
                        0, 5, TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 5);
   h->AddFrame(h2, layout);
   gf->AddFrame(h, fLayoutExpandX);
   fOptLogy = AddCheckButton(gf, "Logarithmic scale", kAxisOptLogy);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fYAxisColor->SetToolTipText("Color of axis' line");
   fYTickLength->GetNumberEntry()->SetToolTipText("Set the ticks' length");
   fOptLogy->SetToolTipText("Draw logarithmic scale");
}

//______________________________________________________________________________
void TStyleManager::AddAxisYTitle(TGCompositeFrame *f)
{
   // Add the 'Title' group frame to the 'Axis - Y Axis' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Title");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fYTitleColor = AddColorEntry(h1, kAxisYTitleColor);
   fYTitleFont = AddFontTypeEntry(h1, kAxisYTitleFont);
   gf->AddFrame(h1, fLayoutExpandX);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fYTitleSizeInPixels = AddCheckButton(h2, "Pixels", kAxisYTitleSizeInPixels);
   fYTitleSize = AddNumberEntry(h2, 21, 8, 0, kAxisYTitleSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   fYTitleOffset = AddNumberEntry(gf, 68, 8, 0, kAxisYTitleOffset, "Offset:",
                        0, 5, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 1);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fYTitleColor->SetToolTipText("Color of axis' title");
   fYTitleSizeInPixels->SetToolTipText("Set the title size in pixels if selected, otherwise - in % of pad");
   fYTitleSize->GetNumberEntry()->SetToolTipText("Title size (in pixels or in % of pad)");
   fYTitleOffset->GetNumberEntry()->SetToolTipText("Offset between axis and title");
}

//______________________________________________________________________________
void TStyleManager::AddAxisYDivisions(TGCompositeFrame *f)
{
   // Add the 'Divisions' group frame to the 'Axis - Y Axis' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Divisions");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fYNdivSubSub = AddNumberEntry(h1, 0, 0, 0, kAxisYNdivSubSub, "",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   fYNdivSub = AddNumberEntry(h1, 0, 18, 0, kAxisYNdivSub, "",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   fYNdivMain = AddNumberEntry(h1, 0, 18, 0, kAxisYNdivMain, "",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   gf->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fYNdivisionsOptimize = AddCheckButton(h2, "Optimize", kAxisYNdivisionsOptimize);
   gf->AddFrame(h2, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

   fYNdivMain->GetNumberEntry()->SetToolTipText("Primary axis divisions");
   fYNdivSub->GetNumberEntry()->SetToolTipText("Secondary axis divisions");
   fYNdivSubSub->GetNumberEntry()->SetToolTipText("Tertiary axis divisions");
   fYNdivisionsOptimize->SetToolTipText("Optimize the number of axis divisions");
}

//______________________________________________________________________________
void TStyleManager::AddAxisYLabels(TGCompositeFrame *f)
{
   // Add the 'Labels' group frame to the 'Axis - Y Axis' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Labels");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fYLabelColor = AddColorEntry(h1, kAxisYLabelColor);
   fYLabelFont = AddFontTypeEntry(h1, kAxisYLabelFont);
   gf->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fYLabelSizeInPixels = AddCheckButton(h2, "Pixels", kAxisYLabelSizeInPixels);
   fYLabelSize = AddNumberEntry(h2, 22, 8, 0, kAxisYLabelSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   fYLabelOffset = AddNumberEntry(gf, 69, 8, 0, kAxisYTitleOffset, "Offset:",
                        0, 5, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 1);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fYLabelColor->SetToolTipText("Color of axis' labels");
   fYLabelSizeInPixels->SetToolTipText("Set the labels size in pixels if selected, otherwise - in % of pad");
   fYLabelSize->GetNumberEntry()->SetToolTipText("Label size (in pixels or in % of pad)");
   fYLabelOffset->GetNumberEntry()->SetToolTipText("Offset between axis and labels");
}

//______________________________________________________________________________
void TStyleManager::CreateTabAxisZ(TGCompositeFrame *tab)
{
   // Add the sub-tab 'Z Axis' to the tab 'Axis'.

   TGHorizontalFrame *h1 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h1);
   AddAxisZLine(h1);
   AddAxisZDivisions(h1);
   tab->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h2);
   AddAxisZTitle(h2);
   AddAxisZLabels(h2);
   tab->AddFrame(h2, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::AddAxisZLine(TGCompositeFrame *f)
{
   // Add the 'Line' group frame to the 'Axis - Z Axis' tab.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 20);
   fTrashListLayout->Add(layout);

   TGGroupFrame *gf = new TGGroupFrame(f, "Line");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fZAxisColor = AddColorEntry(h, kAxisZAxisColor);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(h);
   fTrashListFrame->AddFirst(h2);
   fZTickLength = AddNumberEntry(h2, 3, 8, 0, kAxisZTickLength, "Ticks:",
                        0, 5, TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 5);
   h->AddFrame(h2, layout);
   gf->AddFrame(h, fLayoutExpandX);
   fOptLogz = AddCheckButton(gf, "Logarithmic scale", kAxisOptLogz);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fZAxisColor->SetToolTipText("Color of axis' line");
   fZTickLength->GetNumberEntry()->SetToolTipText("Set the ticks' length");
   fOptLogz->SetToolTipText("Draw logarithmic scale");
}

//______________________________________________________________________________
void TStyleManager::AddAxisZTitle(TGCompositeFrame *f)
{
   // Add the 'Title' group frame to the 'Axis - Z Axis' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Title");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fZTitleColor = AddColorEntry(h1, kAxisZTitleColor);
   fZTitleFont = AddFontTypeEntry(h1, kAxisZTitleFont);
   gf->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fZTitleSizeInPixels = AddCheckButton(h2, "Pixels", kAxisZTitleSizeInPixels);
   fZTitleSize = AddNumberEntry(h2, 21, 8, 0, kAxisZTitleSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   fZTitleOffset = AddNumberEntry(gf, 68, 8, 0, kAxisZTitleOffset, "Offset:",
                        0, 5, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 1);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fZTitleColor->SetToolTipText("Color of axis' title");
   fZTitleSizeInPixels->SetToolTipText("Set the title size in pixels if selected, otherwise - in % of pad");
   fZTitleSize->GetNumberEntry()->SetToolTipText("Title size (in pixels or in % of pad)");
   fZTitleOffset->GetNumberEntry()->SetToolTipText("Offset between axis and title");
}

//______________________________________________________________________________
void TStyleManager::AddAxisZDivisions(TGCompositeFrame *f)
{
   // Add the 'Divisions' group frame to the 'Axis - Z Axis' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Divisions");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fZNdivSubSub = AddNumberEntry(h1, 0, 0, 0, kAxisZNdivSubSub, "",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   fZNdivSub = AddNumberEntry(h1, 0, 18, 0, kAxisZNdivSub, "",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   fZNdivMain = AddNumberEntry(h1, 0, 18, 0, kAxisZNdivMain, "",
                        0, 3, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 99);
   gf->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fZNdivisionsOptimize = AddCheckButton(h2, "Optimize", kAxisZNdivisionsOptimize);
   gf->AddFrame(h2, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

   fZNdivMain->GetNumberEntry()->SetToolTipText("Primary axis divisions");
   fZNdivSub->GetNumberEntry()->SetToolTipText("Secondary axis divisions");
   fZNdivSubSub->GetNumberEntry()->SetToolTipText("Tertiary axis divisions");
   fZNdivisionsOptimize->SetToolTipText("Optimize the number of axis divisions");
}

//______________________________________________________________________________
void TStyleManager::AddAxisZLabels(TGCompositeFrame *f)
{
   // Add the 'Labels' group frame to the 'Axis - Z Axis' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Labels");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fZLabelColor = AddColorEntry(h1, kAxisZLabelColor);
   fZLabelFont = AddFontTypeEntry(h1, kAxisZLabelFont);
   gf->AddFrame(h1, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fZLabelSizeInPixels = AddCheckButton(h2, "Pixels", kAxisZLabelSizeInPixels);
   fZLabelSize = AddNumberEntry(h2, 22, 8, 0, kAxisZLabelSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   fZLabelOffset = AddNumberEntry(gf, 69, 8, 0, kAxisZTitleOffset, "Offset:",
                        0, 5, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 1);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fZLabelColor->SetToolTipText("Color of axis' labels");
   fZLabelSizeInPixels->SetToolTipText("Set the labels size in pixels if selected, otherwise - in % of pad");
   fZLabelSize->GetNumberEntry()->SetToolTipText("Label size (in pixels or in % of pad)");
   fZLabelOffset->GetNumberEntry()->SetToolTipText("Offset between axis and labels");
}

//______________________________________________________________________________
void TStyleManager::CreateTabTitle(TGCompositeFrame *tab)
{
   // Add the tab 'Title' to the editor.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 10, 20, 5, 5);
   fTrashListLayout->Add(layout);

   TGHorizontalFrame *h1 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h1);
   fOptTitle = AddCheckButton(h1, "Show title", kTitleOptTitle);
   tab->AddFrame(h1, layout);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h2);
   TGVerticalFrame *v1 = new TGVerticalFrame(h2);
   fTrashListFrame->AddFirst(v1);
   AddTitleFill(v1);
   AddTitleText(v1);
   h2->AddFrame(v1, fLayoutExpandXY);
   TGVerticalFrame *v2 = new TGVerticalFrame(h2);
   fTrashListFrame->AddFirst(v2);
   AddTitleBorderSize(v2);
   AddTitleGeometry(v2);
   h2->AddFrame(v2, fLayoutExpandXY);
   tab->AddFrame(h2, fLayoutExpandX);

   fOptTitle->SetToolTipText("Show / Hide the title pave");
}

//______________________________________________________________________________
void TStyleManager::AddTitleFill(TGCompositeFrame *f)
{
   // Add the 'Fill' group frame to the 'Title' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Fill");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fTitleColor = AddColorEntry(h1, kTitleFillColor);
   fTitleStyle = AddFillStyleEntry(h1, kTitleStyle);
   gf->AddFrame(h1, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fTitleColor->SetToolTipText("Color used to fill the title pave");
//   fTitleStyle->SetToolTipText("Pattern used to fill the title pave");
}

//______________________________________________________________________________
void TStyleManager::AddTitleBorderSize(TGCompositeFrame *f)
{
   // Add the 'Shadow' group frame to the 'Title' tab.

   TGLayoutHints *layout1 = new TGLayoutHints(kLHintsNormal, 0, 24, 6);
   fTrashListLayout->Add(layout1);
   TGLayoutHints *layout2 = new TGLayoutHints(kLHintsNormal, 0, 5, 6);
   fTrashListLayout->Add(layout2);
   TGLayoutHints *layout3 = new TGLayoutHints(kLHintsExpandX, 0, 0, 3, 3);
   fTrashListLayout->Add(layout3);

   TGGroupFrame *gf = new TGGroupFrame(f, "Shadow");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fTitleBorderSizeLabel = new TGLabel(h1, "Title's:");
   h1->AddFrame(fTitleBorderSizeLabel, layout1);
   fTitleBorderSize = AddLineWidthEntry(h1, kTitleBorderSize);
   gf->AddFrame(h1, layout3);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fLegendBorderSizeLabel = new TGLabel(h2, "Legend's:");
   h2->AddFrame(fLegendBorderSizeLabel, layout2);
   fLegendBorderSize = AddLineWidthEntry(h2, kTitleLegendBorderSize);
   gf->AddFrame(h2, layout3);
   f->AddFrame(gf, fLayoutExpandXMargin);
}

//______________________________________________________________________________
void TStyleManager::AddTitleText(TGCompositeFrame *f)
{
   // Add the 'Text' group frame to the 'Title' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Text");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fTitleTextColor = AddColorEntry(h1, kTitleTextColor);
   fTitleFont = AddFontTypeEntry(h1, kTitleFont);
   gf->AddFrame(h1, fLayoutExpandX);
   fTitleAlign = AddTextAlignEntry(gf, kTitleAlign);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fTitleFontSizeInPixels = AddCheckButton(h2, "Pixels", kTitleFontSizeInPixels);
   fTitleFontSize = AddNumberEntry(h2, 21, 10, 0, kTitleFontSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fTitleTextColor->SetToolTipText("Color of the title's text");
   fTitleFontSizeInPixels->SetToolTipText("Set the title's text size in pixels if selected, otherwise - in % of pad");
   fTitleFontSize->GetNumberEntry()->SetToolTipText("Title's text size (in pixels or in % of pad)");
}

//______________________________________________________________________________
void TStyleManager::AddTitleGeometry(TGCompositeFrame *f)
{
   // Add the 'Geometry' group frame to the 'Title' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Geometry (% of Pad)");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fTitleX = AddNumberEntry(h1, 0, 8, 0, kTitleX, "X:",
                        0, 4, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 100);
   fTitleY = AddNumberEntry(h1, 8, 8, 0, kTitleY, "Y:",
                           0, 4, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 100);
   gf->AddFrame(h1, fLayoutExpandXY);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fTitleW = AddNumberEntry(h2, 0, 6, 0, kTitleW, "W:",
                        0, 4, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 100);
   fTitleH = AddNumberEntry(h2, 8, 8, 0, kTitleH, "H:",
                        0, 4, TGNumberFormat::kNESInteger,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 100);
   gf->AddFrame(h2, fLayoutExpandXY);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fTitleX->GetNumberEntry()->SetToolTipText("Title' default abscissa");
   fTitleY->GetNumberEntry()->SetToolTipText("Title' default ordinate");
   fTitleW->GetNumberEntry()->SetToolTipText("Title' default width");
   fTitleH->GetNumberEntry()->SetToolTipText("Title' default height");
}

//______________________________________________________________________________
void TStyleManager::CreateTabStats(TGCompositeFrame *tab)
{
   // Add the tab 'Stats' to the editor.

   TGLayoutHints *layout1 = new TGLayoutHints(kLHintsNormal, 0, 5, 6);
   fTrashListLayout->Add(layout1);
   TGLayoutHints *layout2 = new TGLayoutHints(kLHintsExpandX, 10, 21, 5, 5);
   fTrashListLayout->Add(layout2);

   TGHorizontalFrame *h1 = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h1);

   TGVerticalFrame *v1 = new TGVerticalFrame(h1);
   fTrashListFrame->AddFirst(v1);
   AddStatsFill(v1);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(v1);
   fTrashListFrame->AddFirst(h2);
   fStatBorderSizeLabel = new TGLabel(h2, "Stats' shadow:");
   h2->AddFrame(fStatBorderSizeLabel, layout1);
   fStatBorderSize = AddLineWidthEntry(h2, kStatBorderSize);
   v1->AddFrame(h2, layout2);
   AddStatsText(v1);
   AddStatsGeometry(v1);
   h1->AddFrame(v1, fLayoutExpandXY);

   TGVerticalFrame *v2 = new TGVerticalFrame(h1);
   fTrashListFrame->AddFirst(v2);
   AddStatsStats(v2);
   AddStatsFit(v2);
   h1->AddFrame(v2, fLayoutExpandXY);

   tab->AddFrame(h1, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::AddStatsFill(TGCompositeFrame *f)
{
   // Add the 'Fill' group frame to the 'Stats' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Fill");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   fStatColor = AddColorEntry(h, kStatColor);
   fStatStyle = AddFillStyleEntry(h, kStatStyle);
   gf->AddFrame(h, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fStatColor->SetToolTipText("Color used to fill the stats pave");
//   fStatStyle->SetToolTipText("Pattern used to fill the stats pave");
}

//______________________________________________________________________________
void TStyleManager::AddStatsText(TGCompositeFrame *f)
{
   // Add the 'Text' group frame to the 'Stats' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Text");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fStatTextColor = AddColorEntry(h1, kStatTextColor);
   fStatFont = AddFontTypeEntry(h1, kStatFont);
   gf->AddFrame(h1, fLayoutExpandX);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fStatFontSizeInPixels = AddCheckButton(h2, "Pixels", kStatFontSizeInPixels);
   fStatFontSize = AddNumberEntry(h2, 21, 10, 0, kStatFontSize, "Size:", 0, 5,
                        TGNumberFormat::kNESRealThree,
                        TGNumberFormat::kNEAAnyNumber,
                        TGNumberFormat::kNELLimitMinMax, 0, 0.3);
   gf->AddFrame(h2, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXYMargin);

// TODO Delete the // when the selectColor and selectPattern tooltips are OK.
//   fStatTextColor->SetToolTipText("Color of the stats's text");
   fStatFontSizeInPixels->SetToolTipText("Set the stats's text size in pixels if selected, otherwise - in % of pad");
   fStatFontSize->GetNumberEntry()->SetToolTipText("Stats's text size (in pixels or in % of pad)");
}

//______________________________________________________________________________
void TStyleManager::AddStatsGeometry(TGCompositeFrame *f)
{
   // Add the 'Geometry' group frame to the 'Stats' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Geometry");
   fTrashListFrame->AddFirst(gf);

   TGHorizontalFrame *h1 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h1);
   fStatX = AddNumberEntry(h1, 0, 7, 0, kStatX, "X:",
                        0., 4, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEANonNegative,
                        TGNumberFormat::kNELLimitMinMax, 0., 1.);
   fStatY = AddNumberEntry(h1, 8, 7, 0, kStatY, "Y:",
                        0., 4, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEANonNegative,
                        TGNumberFormat::kNELLimitMinMax, 0., 1.);
   gf->AddFrame(h1, fLayoutExpandXY);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   fStatW = AddNumberEntry(h2, 0, 5, 0, kStatW, "W:",
                        0., 4, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEANonNegative,
                        TGNumberFormat::kNELLimitMinMax, 0., 1.);
   fStatH = AddNumberEntry(h2, 8, 7, 0, kStatH, "H:",
                        0., 4, TGNumberFormat::kNESRealTwo,
                        TGNumberFormat::kNEANonNegative,
                        TGNumberFormat::kNELLimitMinMax, 0., 1.);
   gf->AddFrame(h2, fLayoutExpandXY);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fStatX->GetNumberEntry()->SetToolTipText("X position of top right corner of stat box.");
   fStatY->GetNumberEntry()->SetToolTipText("Y position of top right corner of stat box.");
   fStatW->GetNumberEntry()->SetToolTipText("Width of stat box.");
   fStatH->GetNumberEntry()->SetToolTipText("Height of stat box.");
}

//______________________________________________________________________________
void TStyleManager::AddStatsStats(TGCompositeFrame *f)
{
   // Add the 'Stat Options' group frame to the 'Stats' tab.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsNormal, 0, 0, 5);
   fTrashListLayout->Add(layout);

   TGGroupFrame *gf = new TGGroupFrame(f, "Stat Options");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   TGVerticalFrame *v1 = new TGVerticalFrame(h);
   fTrashListFrame->AddFirst(v1);
   fOptStatName = AddCheckButton(v1, "Name", kStatOptStatName);
   fOptStatOverflow = AddCheckButton(v1, "Overflow", kStatOptStatOverflow);
   fOptStatUnderflow = AddCheckButton(v1, "Underflow", kStatOptStatUnderflow);
   fOptStatSkewness = AddCheckButton(v1, "Skewness", kStatOptStatSkewness);
   fOptStatKurtosis = AddCheckButton(v1, "Kurtosis", kStatOptStatKurtosis);
   h->AddFrame(v1, fLayoutExpandXY);
   TGVerticalFrame *v2 = new TGVerticalFrame(h);
   fTrashListFrame->AddFirst(v2);
   fOptStatEntries = AddCheckButton(v2, "Entries", kStatOptStatEntries);
   fOptStatMean = AddCheckButton(v2, "Mean", kStatOptStatMean);
   fOptStatRMS = AddCheckButton(v2, "RMS", kStatOptStatRMS);
   fOptStatIntegral = AddCheckButton(v2, "Integral", kStatOptStatIntegral);
   fOptStatErrors = AddCheckButton(v2, "Errors", kStatOptStatErrors);
   h->AddFrame(v2, fLayoutExpandXY);
   gf->AddFrame(h, fLayoutExpandX);

   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   TGHorizontalFrame *h3 = new TGHorizontalFrame(h2);
   fTrashListFrame->AddFirst(h3);
   fStatFormatLabel = new TGLabel(h3, "Paint format:");
   h3->AddFrame(fStatFormatLabel, layout);
   h2->AddFrame(h3, fLayoutExpandX);
   fStatFormat = AddTextEntry(h2, "", kStatFormat);
   gf->AddFrame(h2, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXYMargin);

   fOptStatName->SetToolTipText("Show / Hide the histogram name");
   fOptStatOverflow->SetToolTipText("Show / Hide the number of overflows");
   fOptStatUnderflow->SetToolTipText("Show / Hide the number of underflows");
   fOptStatSkewness->SetToolTipText("Show / Hide the skewness");
   fOptStatKurtosis->SetToolTipText("Show / Hide the kurtosis");
   fOptStatEntries->SetToolTipText("Show / Hide the number of entries");
   fOptStatMean->SetToolTipText("Show / Hide the mean value");
   fOptStatRMS->SetToolTipText("Show / Hide root-mean-square (RMS)");
   fOptStatIntegral->SetToolTipText("Show / Hide the integral of bins");
   fOptStatErrors->SetToolTipText("Show / Hide the errors");
   fStatFormat->SetToolTipText("Paint format of stat options");
}

//______________________________________________________________________________
void TStyleManager::AddStatsFit(TGCompositeFrame *f)
{
   // Add the 'Fit Options' group frame to the 'Stats' tab.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsNormal, 0, 0, 5);
   fTrashListLayout->Add(layout);

   TGGroupFrame *gf = new TGGroupFrame(f, "Fit Options");
   fTrashListFrame->AddFirst(gf);
   TGHorizontalFrame *h = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h);
   TGVerticalFrame *v1 = new TGVerticalFrame(h);
   fTrashListFrame->AddFirst(v1);
   fOptFitValues = AddCheckButton(v1, "Values", kStatOptFitValues);
   fOptFitProbability = AddCheckButton(v1, "Probability",
                                       kStatOptFitProbability);
   h->AddFrame(v1, fLayoutExpandXY);
   TGVerticalFrame *v2 = new TGVerticalFrame(h);
   fTrashListFrame->AddFirst(v2);
   fOptFitErrors = AddCheckButton(v2, "Errors", kStatOptFitErrors);
   fOptFitChi = AddCheckButton(v2, "Chi", kStatOptFitChi);
   h->AddFrame(v2, fLayoutExpandXY);
   gf->AddFrame(h, fLayoutExpandX);
   TGHorizontalFrame *h2 = new TGHorizontalFrame(gf);
   fTrashListFrame->AddFirst(h2);
   TGHorizontalFrame *h3 = new TGHorizontalFrame(h2);
   fTrashListFrame->AddFirst(h3);
   fFitFormatLabel = new TGLabel(h3, "Paint format:");
   h3->AddFrame(fFitFormatLabel, layout);
   h2->AddFrame(h3, fLayoutExpandX);
   fFitFormat = AddTextEntry(h2, "", kStatFitFormat);
   gf->AddFrame(h2, fLayoutExpandX);
   f->AddFrame(gf, fLayoutExpandXMargin);

   fOptFitValues->SetToolTipText("Show / Hide the parameter name and value");
   fOptFitProbability->SetToolTipText("Show / Hide probability)");
   fOptFitErrors->SetToolTipText("Show / Hide the errors");
   fOptFitChi->SetToolTipText("Show / Hide Chisquare");
   fFitFormat->SetToolTipText("Paint format of fit options");
}

//______________________________________________________________________________
void TStyleManager::CreateTabPsPdf(TGCompositeFrame *tab)
{
   // Add the tab 'PS / PDF' to the editor.

   AddPsPdfHeader(tab);
   AddPsPdfTitle(tab);
   TGHorizontalFrame *h = new TGHorizontalFrame(tab);
   fTrashListFrame->AddFirst(h);
   AddPsPdfPaperSize(h);
   TGVerticalFrame *v = new TGVerticalFrame(h);
   fTrashListFrame->AddFirst(v);
   AddPsPdfLineScale(v);
   AddPsPdfColorModel(v);
   h->AddFrame(v, fLayoutExpandXY);
   tab->AddFrame(h, fLayoutExpandX);
}

//______________________________________________________________________________
void TStyleManager::AddPsPdfHeader(TGCompositeFrame *f)
{
   // Add the 'Header' group frame to the 'PS / PDF' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Header");
   fTrashListFrame->AddFirst(gf);
   fHeaderPS = AddTextEntry(gf, "", kPSPDFHeaderPS);
   f->AddFrame(gf, fLayoutExpandXMargin);

   fHeaderPS->SetToolTipText("PostScript header");
}

//______________________________________________________________________________
void TStyleManager::AddPsPdfTitle(TGCompositeFrame *f)
{
   // Add the 'Title' group frame to the 'PS / PDF' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Title");
   fTrashListFrame->AddFirst(gf);
   fTitlePS = AddTextEntry(gf, "", kPSPDFTitlePS);
   f->AddFrame(gf, fLayoutExpandXMargin);

   fTitlePS->SetToolTipText("PostScript title");
}

//______________________________________________________________________________
void TStyleManager::AddPsPdfColorModel(TGCompositeFrame *f)
{
   // Add the 'Color Model' group frame to the 'PS / PDF' tab.

   fColorModelPS = new TGButtonGroup(f, "Color Model",
                                    kChildFrame | kHorizontalFrame | kFitWidth);

   fColorModelPS->SetLayoutManager(new TGMatrixLayout(fColorModelPS, 1, 2, 15));
   fColorModelPSRGB = new TGRadioButton(fColorModelPS, "RGB",
                                          kPSPDFColorModelPSRGB);
   fColorModelPSRGB->Associate(this);
   fColorModelPSCMYK = new TGRadioButton(fColorModelPS, "CMYK",
                                          kPSPDFColorModelPSCMYK);
   fColorModelPSCMYK->Associate(this);
   fColorModelPS->Show();
   TGLayoutHints *layout2 =
                new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 12);
   fTrashListLayout->Add(layout2);
   f->AddFrame(fColorModelPS, layout2);
}

//______________________________________________________________________________
void TStyleManager::AddPsPdfPaperSize(TGCompositeFrame *f)
{
   // Add the 'Paper Size' group frame to the 'PS / PDF' tab.

   TGGroupFrame *gf = new TGGroupFrame(f, "Paper Size");
   fTrashListFrame->AddFirst(gf);
   fPaperSizePredef = AddPaperSizeEntry(gf, kPSPDFPaperSizePredef);
   fPaperSizeX = AddNumberEntry(gf, 0, 0, 0, kPSPDFPaperSizeX, "Width:",
                        0, 6, TGNumberFormat::kNESRealOne,
                        TGNumberFormat::kNEAPositive,
                        TGNumberFormat::kNELLimitMinMax, 1, 40);
   fPaperSizeY = AddNumberEntry(gf, 0, 0, 0, kPSPDFPaperSizeY, "Height:",
                        0, 6, TGNumberFormat::kNESRealOne,
                        TGNumberFormat::kNEAPositive,
                        TGNumberFormat::kNELLimitMinMax, 1, 40);
   f->AddFrame(gf, fLayoutExpandXMargin);

   fPaperSizeX->GetNumberEntry()->SetToolTipText("Width of the printing area");
   fPaperSizeY->GetNumberEntry()->SetToolTipText("Height of the printing area");
}

//______________________________________________________________________________
void TStyleManager::AddPsPdfLineScale(TGCompositeFrame *f)
{
   // Add the 'Line scale' number entry to the 'PS / PDF' tab.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 10, 20, 5, 5);
   fTrashListLayout->Add(layout);

   TGHorizontalFrame *gf = new TGHorizontalFrame(f);
   fTrashListFrame->AddFirst(gf);
   fLineScalePS = AddNumberEntry(gf, 0, 0, 0, kPSPDFLineScalePS, "Line scale:",
                        0, 6, TGNumberFormat::kNESRealOne,
                        TGNumberFormat::kNEAPositive,
                        TGNumberFormat::kNELLimitMinMax, 0.1, 10);
   f->AddFrame(gf, layout);

   fLineScalePS->GetNumberEntry()->SetToolTipText("Line scale factor when drawing lines on PostScript");
}

//______________________________________________________________________________
void TStyleManager::AddTitle(TGCompositeFrame *f, const char *s)
{
   // Add a title to the frame f.

   TGLayoutHints *layout1 = new TGLayoutHints(kLHintsExpandX, 5, 0, 7);
   fTrashListLayout->Add(layout1);
   TGLayoutHints *layout2 = new TGLayoutHints(kLHintsExpandX, 0, 0, 6, 6);
   fTrashListLayout->Add(layout2);

   TGHorizontalFrame *h = new TGHorizontalFrame(f);
   fTrashListFrame->AddFirst(h);

   TGLabel *lab = new TGLabel(h, s);
   fTrashListFrame->AddFirst(lab);
   h->AddFrame(lab);

   TGHorizontal3DLine *line = new TGHorizontal3DLine(h, 4, 2, kFALSE);
   fTrashListFrame->AddFirst(line);
   h->AddFrame(line, layout1);

   f->AddFrame(h, layout2);
}

//______________________________________________________________________________
TGColorSelect *TStyleManager::AddColorEntry(TGCompositeFrame *f, Int_t id)
{
   // Add a color entry to the frame f.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsBottom, 0, 5, 3, 3);
   fTrashListLayout->Add(layout);

   TGColorSelect *cs = new TGColorSelect(f, 0, id);
   cs->Associate(this);
   f->AddFrame(cs, layout);
   return cs;
}

//______________________________________________________________________________
TGedPatternSelect *TStyleManager::AddFillStyleEntry(TGCompositeFrame *f,
                                                    Int_t id)
{
   // Add a fill style entry to the frame f.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsBottom, 0, 0, 3, 3);
   fTrashListLayout->Add(layout);

   TGedPatternSelect *gps = new TGedPatternSelect(f, 0, id);
   gps->Associate(this);
   f->AddFrame(gps, layout);
   return gps;
}

//______________________________________________________________________________
TGedMarkerSelect *TStyleManager::AddMarkerStyleEntry(TGCompositeFrame *f,
                                                     Int_t id)
{
   // Add a marker style entry to the frame f.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsCenterY, 0, 5, 3, 3);
   fTrashListLayout->Add(layout);

   TGedMarkerSelect *gms = new TGedMarkerSelect(f, 0, id);
   gms->Associate(this);
   f->AddFrame(gms, layout);
   return gms;
}

//______________________________________________________________________________
TGComboBox *TStyleManager::AddMarkerSizeEntry(TGCompositeFrame *f, Int_t id)
{
   // Add a marker size entry to the frame f.

   char *a = new char[10 * sizeof(char)];
   TGComboBox *cb = new TGComboBox(f, id);
   cb->Associate(this);
   for (Int_t i = 1; i <= 15; i++) {
      sprintf(a, "%.1f", 0.2 * i);
      cb->AddEntry(a, i);
   }
   delete [] a;
   cb->Resize(1, 22);
   f->AddFrame(cb, fLayoutExpandXCenterYMargin);
   return cb;
}

//______________________________________________________________________________
TGNumberEntry *TStyleManager::AddNumberEntry(TGCompositeFrame *f, Int_t e1,
         Int_t e2, Int_t e3, Int_t id, const char *s, Double_t init, Int_t digits,
         TGNumberFormat::EStyle nfS, TGNumberFormat::EAttribute nfA,
         TGNumberFormat::ELimit nfL, Double_t min, Double_t max)
{
   // Add a number entry to the frame f. A caption can be added.

   TGHorizontalFrame *h = new TGHorizontalFrame(f);
   fTrashListFrame->AddFirst(h);
   if (strlen(s)) {
      TGLabel *lab = new TGLabel(h, s);
      fTrashListFrame->AddFirst(lab);
      TGLayoutHints *layout = new TGLayoutHints(kLHintsNormal, e1, 0, 3);
      fTrashListLayout->Add(layout);
      h->AddFrame(lab, layout);
   }
   TGNumberEntry *ne = new TGNumberEntry(h, init, digits, id,
                                         nfS, nfA, nfL, min, max);
   ne->Associate(this);
   if ((e1 == 0) && (e2 == 0) && (e3 == 0)) {
      TGLayoutHints *layout1 = new TGLayoutHints(kLHintsRight);
      fTrashListLayout->Add(layout1);
      h->AddFrame(ne, layout1);
   } else {
      TGLayoutHints *layout2 = new TGLayoutHints(kLHintsNormal, e2, e3);
      fTrashListLayout->Add(layout2);
      h->AddFrame(ne, layout2);
   }
   if (strlen(s)) {
      TGLayoutHints *layout3 =
                 new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 0, 2, 3, 3);
      fTrashListLayout->Add(layout3);
      f->AddFrame(h, layout3);
   } else {
      TGLayoutHints *layout4 =
                 new TGLayoutHints(kLHintsNormal | kLHintsCenterY, 0, 2, 3, 3);
      fTrashListLayout->Add(layout4);
      f->AddFrame(h, layout4);
   }
   return ne;
}

//______________________________________________________________________________
TGLineWidthComboBox *TStyleManager::AddLineWidthEntry(TGCompositeFrame *f,
                                                      Int_t id)
{
   // Add a line width entry to the frame f.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 0, 0, 3, 3);
   fTrashListLayout->Add(layout);

   TGHorizontalFrame *h = new TGHorizontalFrame(f);
   fTrashListFrame->AddFirst(h);
   TGLineWidthComboBox *lwcb = new TGLineWidthComboBox(h, id);
   lwcb->Associate(this);
   lwcb->Resize(1, 22);
   h->AddFrame(lwcb, fLayoutExpandX);
   f->AddFrame(h, layout);
   return lwcb;
}

//______________________________________________________________________________
TGLineStyleComboBox *TStyleManager::AddLineStyleEntry(TGCompositeFrame *f,
                                                      Int_t id)
{
   // Add a line style entry to the frame f.

   TGLineStyleComboBox *lscb = new TGLineStyleComboBox(f, id);
   lscb->Associate(this);
   lscb->Resize(1, 22);
   f->AddFrame(lscb, fLayoutExpandXCenterYMargin);
   return lscb;
}

//______________________________________________________________________________
TGTextButton *TStyleManager::AddTextButton(TGCompositeFrame *f,
                                           const char *s, Int_t id)
{
   // Add a text button to the frame f.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsExpandX, 0, 0, 3, 3);
   fTrashListLayout->Add(layout);

   TGTextButton *tb = new TGTextButton(f, s, id);
   tb->Associate(this);
   f->AddFrame(tb, layout);
   return tb;
}

//______________________________________________________________________________
TGFontTypeComboBox *TStyleManager::AddFontTypeEntry(TGCompositeFrame *f,
                                                    Int_t id)
{
   // Add a font type combo box to the frame f.

   TGFontTypeComboBox *ftcb = new TGFontTypeComboBox(f, id);
   ftcb->Associate(this);
   ftcb->Resize(1, 22);
   f->AddFrame(ftcb, fLayoutExpandXCenterYMargin);
   return ftcb;
}

//______________________________________________________________________________
TGComboBox *TStyleManager::AddTextAlignEntry(TGCompositeFrame *f, Int_t id)
{
   // Add a text align combo box to the frame f.

   TGComboBox *cb = new TGComboBox(f, id);
   cb->Associate(this);
   cb->AddEntry("11 Bottom, Left",   11);
   cb->AddEntry("21 Bottom, Middle", 21);
   cb->AddEntry("31 Bottom, Right",  31);
   cb->AddEntry("12 Middle, Left",   12);
   cb->AddEntry("22 Middle, Middle", 22);
   cb->AddEntry("32 Middle, Right",  32);
   cb->AddEntry("13 Top, Left",      13);
   cb->AddEntry("23 Top, Middle",    23);
   cb->AddEntry("33 Top, Right",     33);
   cb->Resize(1, 22);
   f->AddFrame(cb, fLayoutExpandXCenterYMargin);
   return cb;
}

//______________________________________________________________________________
TGButtonGroup *TStyleManager::AddBorderModeEntry(TGCompositeFrame *f,
                                                Int_t id1, Int_t id2, Int_t id3)
{
   // Add a border mode button group to the frame f.

   TGButtonGroup *bg = new TGButtonGroup(f, "Border");
   TGRadioButton *sunk = new TGRadioButton(bg, "Sunken", id1);
   sunk->Associate(this);
   fTrashListFrame->AddFirst(sunk);
   TGRadioButton *none = new TGRadioButton(bg, "None"  , id2);
   none->Associate(this);
   fTrashListFrame->AddFirst(none);
   TGRadioButton *rais = new TGRadioButton(bg, "Raised", id3);
   rais->Associate(this);
   fTrashListFrame->AddFirst(rais);
   bg->Show();
   f->AddFrame(bg, fLayoutExpandXYMargin);
   return bg;
}

//______________________________________________________________________________
TGComboBox *TStyleManager::AddDateFormatEntry(TGCompositeFrame *f, Int_t id)
{
   // Add a date format combo box to the frame f.

   TGComboBox *cb = new TGComboBox(f, id);
   cb->Associate(this);
   cb->AddEntry("Wed Sep 25 17:10:35 2002", 1);
   cb->AddEntry("2002-09-25",               2);
   cb->AddEntry("2002-09-25 17:10:35",      3);
   cb->Resize(1, 22);
   cb->GetListBox()->Resize(cb->GetListBox()->GetDefaultSize().fWidth, 55);
   f->AddFrame(cb, fLayoutExpandXCenterYMargin);
   return cb;
}

//______________________________________________________________________________
TGCheckButton *TStyleManager::AddCheckButton(TGCompositeFrame *f, const char *s,
                                             Int_t id, Int_t e1, Int_t e2)
{
   // Add a check button to the frame f.

   TGLayoutHints *layout = new TGLayoutHints(kLHintsNormal, 0, e1, 4, e2);
   fTrashListLayout->Add(layout);

   TGHorizontalFrame *h = new TGHorizontalFrame(f);
   fTrashListFrame->AddFirst(h);
   TGCheckButton *cb = new TGCheckButton(h, s, id);
   cb->Associate(this);
   h->AddFrame(cb, layout);
   f->AddFrame(h);
   return cb;
}

//______________________________________________________________________________
TGTextEntry *TStyleManager::AddTextEntry(TGCompositeFrame *f,
                                         const char *s, Int_t id)
{
   // Add a text entry to the frame f. A caption can be added.

   TGHorizontalFrame *h = new TGHorizontalFrame(f);
   fTrashListFrame->AddFirst(h);
   if (strlen(s)) {
      TGLabel *lab = new TGLabel(h, s);
      fTrashListFrame->AddFirst(lab);
      TGLayoutHints *layout1 = new TGLayoutHints(kLHintsNormal, 0, 0, 3);
      fTrashListLayout->Add(layout1);
      h->AddFrame(lab, layout1);
   }
   TGTextEntry *te = new TGTextEntry(h, "", id);
   te->Associate(this);
   te->Resize(57, 22);
   if (strlen(s)) {
      TGLayoutHints *layout2 = new TGLayoutHints(kLHintsRight, 20);
      fTrashListLayout->Add(layout2);
      h->AddFrame(te, layout2);
   } else
      h->AddFrame(te, fLayoutExpandX);
   TGLayoutHints *layout3 =
                 new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 0, 2, 3, 3);
   fTrashListLayout->Add(layout3);
   f->AddFrame(h, layout3);
   return te;
}

//______________________________________________________________________________
TGComboBox *TStyleManager::AddPaperSizeEntry(TGCompositeFrame *f, Int_t id)
{
   // Add a prefered paper size combo box to the frame f.

   TGComboBox *cb = new TGComboBox(f, id);
   cb->Associate(this);
   cb->AddEntry("Custom size (cm)",   1);
   cb->AddEntry("Custom size (inch)", 2);
   cb->AddEntry("A4 (cm)",            3);
   cb->AddEntry("US Letter (inch)",   4);
   cb->AddEntry("US Letter (cm)",   4);
   cb->Resize(1, 22);
   cb->GetListBox()->Resize(cb->GetListBox()->GetDefaultSize().fWidth, 70);
   f->AddFrame(cb, fLayoutExpandXCenterYMargin);
   return cb;
}

//______________________________________________________________________________
void TStyleManager::DoMenu(Int_t menuID)
{
   // Slot called when an item of the menu is selected.

   switch (menuID) {
      case kMenuNew:          DoNew();              break;
      case kMenuDelete:       DoDelete();           break;
      case kMenuRename:       DoRename();           break;
      case kMenuImportCanvas: DoImportCanvas();     break;
      case kMenuImportMacro:  DoImportMacro(kTRUE); break;
      case kMenuExport:       DoExport();           break;
      case kMenuExit:         DoExit();             break;
      case kMenuHelp:         DoHelp(42);           break;
      case kMenuHelpEditor:   DoHelp(fCurTabNum);   break;
      case kMenuHelpGeneral:  DoHelp(0);            break;
      case kMenuHelpCanvas:   DoHelp(1);            break;
      case kMenuHelpPad:      DoHelp(2);            break;
      case kMenuHelpHistos:   DoHelp(3);            break;
      case kMenuHelpAxis:     DoHelp(4);            break;
      case kMenuHelpTitle:    DoHelp(5);            break;
      case kMenuHelpStats:    DoHelp(6);            break;
      case kMenuHelpPSPDF:    DoHelp(7);            break;
   }
}

//______________________________________________________________________________
void TStyleManager::DoImportMacro(Bool_t create)
{
   // Slot called to import a style from a C++ macro file. If create=kTRUE,
   // a new style is created. Otherwise, the current style is reseted.

   // Import a style from a macro.
   // If create = kTRUE, a new style is created.
   // Otherwise, the selected style is:
   //             - reseted with the macro's values.
   //             - recreated (if it is one of the 5 basic styles).

   if ((!create) && (!strcmp(fCurSelStyle->GetName(), "Default"))) {
      if ((!strcmp(fCurSelStyle->GetName(),gStyle->GetName()))) 
         gStyle->Reset("Default");
      else {
         delete gROOT->GetStyle("Default");
         new TStyle("Default", "Default Style");
      }
   } else if ((!create) && (!strcmp(fCurSelStyle->GetName(), "Plain"))) {
      if ((!strcmp(fCurSelStyle->GetName(),gStyle->GetName()))) 
         gStyle->Reset("Plain");
      else {
         delete gROOT->GetStyle("Plain");
         new TStyle("Plain",   "Plain Style (no colors/fill areas)");
      }
   } else if ((!create) && (!strcmp(fCurSelStyle->GetName(), "Bold"))) {
      if ((!strcmp(fCurSelStyle->GetName(),gStyle->GetName()))) 
         gStyle->Reset("Bold");
      else {
         delete gROOT->GetStyle("Bold");
         new TStyle("Bold",    "Bold Style");
      }
   } else if ((!create) && (!strcmp(fCurSelStyle->GetName(), "Video"))) {
      if ((!strcmp(fCurSelStyle->GetName(),gStyle->GetName()))) 
         gStyle->Reset("Video");
      else {
         delete gROOT->GetStyle("Video");
         new TStyle("Video",   "Style for video presentation histograms");
      }
   } else if ((!create) && (!strcmp(fCurSelStyle->GetName(), "Pub"))) {
      if ((!strcmp(fCurSelStyle->GetName(),gStyle->GetName()))) 
         gStyle->Reset("Pub");
      else {
         delete gROOT->GetStyle("Pub");
         new TStyle("Pub",     "Style for Publications");
      }
   } else {
      CreateMacro();
      if (!create) {
         char* newName = new char[(10 + strlen(fCurSelStyle->GetName())) * sizeof(char)];
         sprintf(newName, "Style_%s.C", fCurSelStyle->GetName());
         fCurMacro->fFilename = StrDup(newName);
         delete [] newName;
      }
      new TGFileDialog(gClient->GetRoot(), this, kFDOpen, fCurMacro);
      if (fCurMacro->fFilename != 0) {
         gROOT->ProcessLine(Form(".x %s", fCurMacro->fFilename));
         fCurMacro->fFilename = StrDup(gSystem->BaseName(fCurMacro->fFilename));
      }
   }

   BuildList();
}

//______________________________________________________________________________
void TStyleManager::DoListSelect()
{
   //  Slot called when the user select an item in the available styles' list.
   // Update the preview, the editor, the status bar. The current selected
   // style is changed.

   // Select the new style and update the state of the style manager.
   fCurSelStyle = gROOT->GetStyle(((TGTextLBEntry*) fListComboBox->
                                    GetSelectedEntry())->GetText()->GetString());

   fStyleChanged = kFALSE;

   // Update the status bar.
   UpdateStatusBar();

   // Update the editor (if opened).
   if (fMoreAndNotLess) {
      DisconnectEditor(fCurTabNum);
      UpdateEditor(fCurTabNum);
      ConnectEditor(fCurTabNum);
   }

   // Update the preview, if it exists and is visible.
   if (fPreviewWindow && fPreviewWindow->IsMapped())
      DoEditionUpdatePreview();

   // Refresh the tooltip of the fMakeDefault's button.
   Int_t length = strlen(fCurSelStyle->GetName());
   char *newTip = new char[(25 + length) * sizeof(char)];
   sprintf(newTip, "'%s'", fCurSelStyle->GetName());
   sprintf(newTip + length + 2, " become current style");
   fMakeDefault->SetToolTipText(newTip);
   delete [] newTip;

   // Refresh.
   fListComboBox->MapSubwindows();
   fListComboBox->Layout();
}

//______________________________________________________________________________
void TStyleManager::DoRealTime(Bool_t b)
{
   //  Slot called when the user click on the run time update check button.
   // If b=kTRUE, the user asks for a real time preview.

   if (b) {
      fEditionUpdatePreview->SetEnabled(kFALSE);
      fRealTimePreview = kTRUE;
      DoEditionUpdatePreview();
   } else {
      fEditionUpdatePreview->SetEnabled(kTRUE);
      fRealTimePreview = kFALSE;
   }
}

//______________________________________________________________________________
void TStyleManager::DoPreview(Bool_t b)
{
   //  Slot called when the user click on the preview check button. If b=kTRUE,
   // the user asks for a preview, otherwise he wants to close it.

   if (b) {
      fPreviewButton->SetState(kButtonDown, kFALSE);
      if (fPreviewWindow) {
         DoEditionUpdatePreview();
         fPreviewWindow->MapTheWindow();
      } else {
         if (fCurPad && fCurObj) {
            TQObject::Disconnect("TCanvas", "Closed()");
            fPreviewWindow = new TStylePreview(GetMainFrame(), fCurSelStyle, fCurPad);
            TQObject::Connect("TCanvas", "Closed()", "TStyleManager", this, "DoSelectNoCanvas()");
         }
      }
      fPreviewWindow->Connect("CloseWindow()", "TStyleManager", this, "DoPreviewClosed()");
      fPreviewRealTime->SetEnabled(kTRUE);
      if (fRealTimePreview) {
         fPreviewRealTime->SetState(kButtonDown, kFALSE);
         fEditionUpdatePreview->SetEnabled(kFALSE);
      } else {
         fPreviewRealTime->SetState(kButtonUp, kFALSE);
         fEditionUpdatePreview->SetEnabled(kTRUE);
      }
   } else DoPreviewClosed();
}

//______________________________________________________________________________
void TStyleManager::DoPreviewClosed()
{
   //  Slot called to close the preview, via the preview check button, or
   // when the preview window is closed via the window manager.

   fPreviewWindow->Disconnect("CloseWindow()");
   fPreviewButton->SetState(kButtonUp, kFALSE);
   fPreviewRealTime->SetEnabled(kFALSE);
   fEditionUpdatePreview->SetEnabled(kFALSE);
   fPreviewWindow->UnmapWindow();
}

//______________________________________________________________________________
void TStyleManager::DoMakeDefault()
{
   //  Slot called to make the current selected style (in the ComboBox)
   // become gStyle.

   gROOT->SetStyle(fCurSelStyle->GetName());
   fCurStyle->SetText(gStyle->GetName());
}

//______________________________________________________________________________
void TStyleManager::DoApplyOnSelect(Int_t i)
{
   //  Slot called to choose on which object(s) the 'Apply' button will
   // have an effect.

   fAllAndNotCurrent = (i == kTopApplyOnAll);
}

//______________________________________________________________________________
void TStyleManager::DoApplyOn()
{
   //  Slot called when the user clicks on the 'Apply' button. Apply the
   // current selected style to the specified object(s)

   TStyle *tmp = gStyle;
   gStyle = fCurSelStyle;

   if (fAllAndNotCurrent) {
      // Apply on all canvases, excluding the preview.
      TCanvas *tmpCanvas = (TCanvas *) (gROOT->GetListOfCanvases()->First());
      while (tmpCanvas) {
         if ((!fPreviewWindow) || (tmpCanvas != fPreviewWindow->GetMainCanvas())) {
            tmpCanvas->UseCurrentStyle();
            tmpCanvas->Modified();
            tmpCanvas->Update();
         }
         tmpCanvas = (TCanvas *) (gROOT->GetListOfCanvases()->After(tmpCanvas));
      }
   } else
      if (fCurPad && fCurObj) {
         // Apply on selected object and refresh all canvases containing the object.
         fCurObj->UseCurrentStyle();
         TCanvas *tmpCanvas = (TCanvas *) (gROOT->GetListOfCanvases()->First());
         while (tmpCanvas) {
            if (((!fPreviewWindow) || (tmpCanvas != fPreviewWindow->GetMainCanvas()))
             && ((tmpCanvas == fCurObj) || tmpCanvas->FindObject(fCurObj))) {
               tmpCanvas->Modified();
               tmpCanvas->Update();
            }
            tmpCanvas = (TCanvas *) (gROOT->GetListOfCanvases()->After(tmpCanvas));
         }
      }

   gStyle = tmp;
}

//______________________________________________________________________________
void TStyleManager::DoMoreLess()
{
   //  Slot called when the user try to show or hide the editor part of the
   // style manager.

   fMoreAndNotLess = !fMoreAndNotLess;
   if (fMoreAndNotLess) {
      // Redraw the tabs.
      SetWMSizeHints(fSMWidth, fSMHeight, fSMWidth, fSMHeight, 0, 0);
      fEditionFrame->ShowFrame(fEditionTab);
      fEditionFrame->ShowFrame(fEditionButtonFrame);
      fMoreLess->SetText("&Close <<");
      Resize(fSMWidth, fSMHeight);

      // Update Editor's values.
      DisconnectEditor(fCurTabNum);
      UpdateEditor(fCurTabNum);
      ConnectEditor(fCurTabNum);
      fMoreLess->SetToolTipText("Close the editor");
   } else {
      // Hide the tabs.
      SetWMSizeHints(fSMWidth, 319, fSMWidth, 319, 0, 0);
      fEditionFrame->HideFrame(fEditionTab);
      fEditionFrame->HideFrame(fEditionButtonFrame);
      fMoreLess->SetText("&Edit >>");
      Resize(fSMWidth, 317);
      fMoreLess->SetToolTipText("Open the editor");
   }
}

//______________________________________________________________________________
void TStyleManager::DoEditionUpdatePreview()
{
   // Slot called when the user clicks on the 'Update preview' button.

   if ((!fCurPad) || (!fCurObj)) return;

   if (fPreviewWindow) {
      TQObject::Disconnect("TCanvas", "Closed()");
      fPreviewWindow->Update(fCurSelStyle, fCurPad);
      TQObject::Connect("TCanvas", "Closed()", "TStyleManager", this, "DoSelectNoCanvas()");
   }
}

//______________________________________________________________________________
void TStyleManager::DoChangeTab(Int_t i)
{
   // Slot called when the user changes the current tab.

   //  Disconnect the signal/slots communication mechanism from the previous
   // tab and connect them onto the new one.
   DisconnectEditor(fCurTabNum);
   fCurTabNum = i;
   UpdateEditor(fCurTabNum);
   ConnectEditor(fCurTabNum);
}

//______________________________________________________________________________
void TStyleManager::DoChangeAxisTab(Int_t i)
{
   // Slot called when the user changes the current axis tab.

   fCurTabAxisNum = i;
}

//______________________________________________________________________________
void TStyleManager::DoSelectNoCanvas()
{
   //  Slot called when the user close a TCanvas. Update the labels and the
   // pointers to the current pad and the current object.

   fCurPad = 0;
   fCurObj = 0;

   if (fPreviewWindow && fPreviewWindow->IsMapped())
      DoPreview(kFALSE);

   fCurPadTextEntry->SetText("No pad selected");
   fCurObjTextEntry->SetText("No object selected");
   fImportCascade->DisableEntry(kMenuImportCanvas);
   fApplyOnButton->SetEnabled(kFALSE);
   fToolBarImportCanvas->SetEnabled(kFALSE);
   fPreviewButton->SetEnabled(kFALSE);
   fPreviewRealTime->SetEnabled(kFALSE);
   fEditionUpdatePreview->SetEnabled(kFALSE);
}

//______________________________________________________________________________
void TStyleManager::DoSelectCanvas(TVirtualPad *pad, TObject *obj, Int_t mouseButton)
{
   //  Slot called when the user clicks on a TCanvas or on any object inside
   // a TCanvas. Update the pointers to the current pad and the current object.

   if (mouseButton != kButton2Down) return;

   if (!pad || !obj) {
      DoSelectNoCanvas();
      return;
   }

   // Disable the selection of the preview.
   if (fPreviewWindow && (pad->GetCanvas() == fPreviewWindow->GetMainCanvas()))
      return;

   // Did the user select the same canvas as before ?
   Bool_t samePad = (fCurPad && (pad->GetCanvas() == fCurPad->GetCanvas()));

   fCurPad = pad;
   fCurObj = obj;
   Bool_t preview = (fPreviewWindow && fPreviewWindow->IsMapped());

   if ((!samePad) && preview) DoPreview(kFALSE);

   // Update the informations' label about the selected objects.
   TString sPad;
   if (strlen(fCurPad->GetName())) sPad.Append(fCurPad->GetName());
                              else sPad.Append("[no name]");
   sPad.Append(" - '");
   if (strlen(fCurPad->GetTitle())) 
      sPad.Append(fCurPad->GetTitle());
   else 
      sPad.Append("[no title]");
   sPad.Append("'::");
   sPad.Append(fCurPad->ClassName());
   fCurPadTextEntry->SetText(sPad);
   TString sObj;
   if (strlen(fCurObj->GetName())) 
      sObj.Append(fCurObj->GetName());
   else 
      sObj.Append("[no name]");
   sObj.Append("::");
   sObj.Append(fCurObj->ClassName());
   fCurObjTextEntry->SetText(sObj);

   if (!samePad) {
      fImportCascade->EnableEntry(kMenuImportCanvas);
      fApplyOnButton->SetEnabled(kTRUE);
      fToolBarImportCanvas->SetEnabled(kTRUE);
      if (preview) {
         DoPreview(kTRUE);
      } else {
         fPreviewButton->SetEnabled(kTRUE);
         fPreviewRealTime->SetEnabled(kFALSE);
         fEditionUpdatePreview->SetEnabled(kFALSE);
      }
   }
}

//______________________________________________________________________________
void TStyleManager::CloseWindow()
{
   // Slot called to close the style manager via the window manager.

   Hide();
}

//______________________________________________________________________________
void TStyleManager::ModFillColor()
{
   // Slot called whenever the fill color is modified by the user.

   fCurSelStyle->SetFillColor(TColor::GetColor(fFillColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFillStyle()
{
   // Slot called whenever the fill style is modified by the user.

   fCurSelStyle->SetFillStyle(fFillStyle->GetPattern());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModHatchesLineWidth()
{
   // Slot called whenever the hatches line width is modified by the user.

   fCurSelStyle->SetHatchesLineWidth(fHatchesLineWidth->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModHatchesSpacing()
{
   // Slot called whenever the hatches spacing is modified by the user.

   fCurSelStyle->SetHatchesSpacing(fHatchesSpacing->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModMarkerColor()
{
   // Slot called whenever the marker color is modified by the user.

   fCurSelStyle->SetMarkerColor(TColor::GetColor(fMarkerColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModMarkerStyle()
{
   // Slot called whenever the marker style is modified by the user.

   fCurSelStyle->SetMarkerStyle(fMarkerStyle->GetMarkerStyle());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModMarkerSize()
{
   // Slot called whenever the marker size is modified by the user.

   fCurSelStyle->SetMarkerSize(fMarkerSize->GetSelected() * 0.2);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModScreenFactor()
{
   // Slot called whenever the screen factor is modified by the user.

   fCurSelStyle->SetScreenFactor(fScreenFactor->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModLineColor()
{
   // Slot called whenever the line color is modified by the user.

   fCurSelStyle->SetLineColor(TColor::GetColor(fLineColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModLineWidth()
{
   // Slot called whenever the line width is modified by the user.

   fCurSelStyle->SetLineWidth(fLineWidth->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModLineStyle()
{
   // Slot called whenever the line style is modified by the user.

   fCurSelStyle->SetLineStyle(fLineStyle->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModLineStyleEdit()
{
   // Slot called whenever the line style editor is opened by the user.

   // TODO Open a LineStyle editor
}

//______________________________________________________________________________
void TStyleManager::ModTextColor()
{
   // Slot called whenever the text color is modified by the user.

   fCurSelStyle->SetTextColor(TColor::GetColor(fTextColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTextSize()
{
   // Slot called whenever the text size is modified by the user.

   fCurSelStyle->SetTextSize(fTextSize->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTextSizeInPixels(Bool_t b)
{
   // Slot called whenever the text size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetTextFont() / 10;
   Int_t mod = fCurSelStyle->GetTextFont() % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);
   if (b) {
      fCurSelStyle->SetTextFont(tmp * 10 + 3);
      fTextSize->SetFormat(TGNumberFormat::kNESInteger,
                           TGNumberFormat::kNEAPositive);
      fTextSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->SetTextSize(fCurSelStyle->GetTextSize() * h);
   } else {
      fCurSelStyle->SetTextFont(tmp * 10 + 2);
      fTextSize->SetFormat(TGNumberFormat::kNESRealThree,
                           TGNumberFormat::kNEAPositive);
      fTextSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 1);
      if (mod == 3)
         fCurSelStyle->SetTextSize(fCurSelStyle->GetTextSize() / h);
   }
   fTextSize->SetNumber(fCurSelStyle->GetTextSize());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTextFont()
{
   // Slot called whenever the text font is modified by the user.

   Int_t tmp = fCurSelStyle->GetTextFont() % 10;
   fCurSelStyle->SetTextFont(fTextFont->GetSelected() * 10 + tmp);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTextAlign()
{
   // Slot called whenever the text align is modified by the user.

   fCurSelStyle->SetTextAlign(fTextAlign->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTextAngle()
{
   // Slot called whenever the text angle is modified by the user.

   fCurSelStyle->SetTextAngle(fTextAngle->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModCanvasColor()
{
   // Slot called whenever the canvas color is modified by the user.

   fCurSelStyle->SetCanvasColor(TColor::GetColor(fCanvasColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModCanvasDefX()
{
   // Slot called whenever the canvas default abscissa is modified by the user.

   fCurSelStyle->SetCanvasDefX(fCanvasDefX->GetIntNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModCanvasDefY()
{
   // Slot called whenever the canvas default ordinate is modified by the user.

   fCurSelStyle->SetCanvasDefY(fCanvasDefY->GetIntNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModCanvasDefW()
{
   // Slot called whenever the canvas default width is modified by the user.

   fCurSelStyle->SetCanvasDefW(fCanvasDefW->GetIntNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModCanvasDefH()
{
   // Slot called whenever the canvas default height is modified by the user.

   fCurSelStyle->SetCanvasDefH(fCanvasDefH->GetIntNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModCanvasBorderMode()
{
   // Slot called whenever the canvas border mode is modified by the user.

   Int_t i = kCanvasBorderModeSunken;
   while (!fCanvasBorderMode->Find(i)->IsDown()) 
      i++;
   fCurSelStyle->SetCanvasBorderMode(i - 1 - kCanvasBorderModeSunken);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModCanvasBorderSize()
{
   // Slot called whenever the canvas border size is modified by the user.

   fCurSelStyle->SetCanvasBorderSize(fCanvasBorderSize->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModOptDateBool()
{
   // Slot called whenever the OptDate boolean is modified by the user.

   if (fOptDateBool->IsDown()) 
      fCurSelStyle->SetOptDate(4);
   else 
      fCurSelStyle->SetOptDate(0);
   DisconnectEditor(fCurTabNum);
   UpdateEditor(fCurTabNum);
   ConnectEditor(fCurTabNum);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModAttDateTextColor()
{
   // Slot called whenever the date text color is modified by the user.

   // To modify this entry, the user must have check 'Show'
   fCurSelStyle->GetAttDate()->SetTextColor(TColor::GetColor(fAttDateTextColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModAttDateTextSize()
{
   // Slot called whenever the date text size is modified by the user.

   fCurSelStyle->GetAttDate()->SetTextSize(fAttDateTextSize->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModAttDateTextSizeInPixels(Bool_t b)
{
   // Slot called whenever the date text size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetAttDate()->GetTextFont() / 10;
   Int_t mod = fCurSelStyle->GetAttDate()->GetTextFont() % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);

   if (b) {
      fCurSelStyle->GetAttDate()->SetTextFont(tmp * 10 + 3);
      fAttDateTextSize->SetFormat(TGNumberFormat::kNESInteger,
                           TGNumberFormat::kNEAPositive);
      fAttDateTextSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->GetAttDate()->SetTextSize(fCurSelStyle->GetAttDate()->GetTextSize() * h);
   } else {
      fCurSelStyle->GetAttDate()->SetTextFont(tmp * 10 + 2);
      fAttDateTextSize->SetFormat(TGNumberFormat::kNESRealThree,
                           TGNumberFormat::kNEAPositive);
      fAttDateTextSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 1);
      if (mod == 3)
         fCurSelStyle->GetAttDate()->SetTextSize(fCurSelStyle->GetAttDate()->GetTextSize() / h);
   }
   fAttDateTextSize->SetNumber(fCurSelStyle->GetAttDate()->GetTextSize());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModOptDateFormat()
{
   // Slot called whenever the date text format is modified by the user.

   Int_t formatPrec = fCurSelStyle->GetOptDate() % 10;
   fCurSelStyle->SetOptDate((fOptDateFormat->GetSelected() - 1) * 10
                              + formatPrec);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModAttDateTextFont()
{
   // Slot called whenever the date text font is modified by the user.

   Int_t fontPrec = fCurSelStyle->GetAttDate()->GetTextFont() % 10;
   fCurSelStyle->GetAttDate()->SetTextFont(fAttDateTextFont->GetSelected() * 10
                                          + fontPrec);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModAttDateTextAlign()
{
   // Slot called whenever the date text align is modified by the user.

   fCurSelStyle->GetAttDate()->SetTextAlign(fAttDateTextAlign->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModAttDateTextAngle()
{
   // Slot called whenever the date text angle is modified by the user.

   fCurSelStyle->GetAttDate()->SetTextAngle(fAttDateTextAngle->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModDateX()
{
   // Slot called whenever the date abscissa is modified by the user.

   fCurSelStyle->SetDateX(fDateX->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModDateY()
{
   // Slot called whenever the date ordinate is modified by the user.

   fCurSelStyle->SetDateY(fDateY->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadLeftMargin()
{
   // Slot called whenever the pad left margin is modified by the user.

   fCurSelStyle->SetPadLeftMargin(fPadLeftMargin->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadRightMargin()
{
   // Slot called whenever the pad right margin is modified by the user.

   fCurSelStyle->SetPadRightMargin(fPadRightMargin->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadTopMargin()
{
   // Slot called whenever the pad top margin is modified by the user.

   fCurSelStyle->SetPadTopMargin(fPadTopMargin->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadBottomMargin()
{
   // Slot called whenever the pad bottom margin is modified by the user.

   fCurSelStyle->SetPadBottomMargin(fPadBottomMargin->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadBorderMode()
{
   // Slot called whenever the pad border mode is modified by the user.

   Int_t i = kPadBorderModeSunken;
   while (!fPadBorderMode->Find(i)->IsDown()) 
      i++;
   fCurSelStyle->SetPadBorderMode(i - 1 - kPadBorderModeSunken);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadBorderSize()
{
   // Slot called whenever the pad border size is modified by the user.

   fCurSelStyle->SetPadBorderSize(fPadBorderSize->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadColor()
{
   // Slot called whenever the pad color is modified by the user.

   fCurSelStyle->SetPadColor(TColor::GetColor(fPadColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadTickX()
{
   // Slot called whenever the pad tick X boolean is modified by the user.

   fCurSelStyle->SetPadTickX(fPadTickX->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadTickY()
{
   // Slot called whenever the pad tick Y boolean is modified by the user.

   fCurSelStyle->SetPadTickY(fPadTickY->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadGridX()
{
   // Slot called whenever the pad grid X boolean is modified by the user.

   fCurSelStyle->SetPadGridX(fPadGridX->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPadGridY()
{
   // Slot called whenever the pad grid Y boolean is modified by the user.

   fCurSelStyle->SetPadGridY(fPadGridY->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModGridColor()
{
   // Slot called whenever the grid line color is modified by the user.

   fCurSelStyle->SetGridColor(TColor::GetColor(fGridColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModGridWidth()
{
   // Slot called whenever the grid line width is modified by the user.

   fCurSelStyle->SetGridWidth(fGridWidth->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModGridStyle()
{
   // Slot called whenever the grid line style is modified by the user.

   fCurSelStyle->SetGridStyle(fGridStyle->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModHistFillColor()
{
   // Slot called whenever the histos fill color is modified by the user.

   fCurSelStyle->SetHistFillColor(TColor::GetColor(fHistFillColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModHistFillStyle()
{
   // Slot called whenever the histos fill style is modified by the user.

   fCurSelStyle->SetHistFillStyle(fHistFillStyle->GetPattern());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModHistLineColor()
{
   // Slot called whenever the histos line color is modified by the user.

   fCurSelStyle->SetHistLineColor(TColor::GetColor(fHistLineColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModHistLineWidth()
{
   // Slot called whenever the histos line width is modified by the user.

   fCurSelStyle->SetHistLineWidth(fHistLineWidth->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModHistLineStyle()
{
   // Slot called whenever the histos line style is modified by the user.

   fCurSelStyle->SetHistLineStyle(fHistLineStyle->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModBarWidth()
{
   // Slot called whenever the histos bar width is modified by the user.

   fCurSelStyle->SetBarWidth(fBarWidth->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModBarOffset()
{
   // Slot called whenever the histos bar offset is modified by the user.

   fCurSelStyle->SetBarOffset(fBarOffset->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModHistMinimumZero()
{
   //  Slot called whenever the histos minimum zero boolean is modified
   // by the user.

   fCurSelStyle->SetHistMinimumZero(fHistMinimumZero->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPaintTextFormat()
{
   // Slot called whenever the paint text format is modified by the user.

   fCurSelStyle->SetPaintTextFormat(fPaintTextFormat->GetText());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModNumberContours()
{
   // Slot called whenever the number of contours is modified by the user.

   fCurSelStyle->SetNumberContours(fNumberContours->GetIntNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModLegoInnerR()
{
   // Slot called whenever the lego inner radius is modified by the user.

   fCurSelStyle->SetLegoInnerR(fLegoInnerR->GetIntNumber() *0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFrameFillColor()
{
   // Slot called whenever the frame fill color is modified by the user.

   fCurSelStyle->SetFrameFillColor(TColor::GetColor(fFrameFillColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFrameFillStyle()
{
   // Slot called whenever the frame fill style is modified by the user.

   fCurSelStyle->SetFrameFillStyle(fFrameFillStyle->GetPattern());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFrameLineColor()
{
   // Slot called whenever the frame line color is modified by the user.

   fCurSelStyle->SetFrameLineColor(TColor::GetColor(fFrameLineColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFrameLineWidth()
{
   // Slot called whenever the frame line width is modified by the user.

   fCurSelStyle->SetFrameLineWidth(fFrameLineWidth->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFrameLineStyle()
{
   // Slot called whenever the frame line style is modified by the user.

   fCurSelStyle->SetFrameLineStyle(fFrameLineStyle->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPaletteEdit()
{
   // Slot called whenever the palette editor is opened by the user.

   // TODO Open a palette editor
}

//______________________________________________________________________________
void TStyleManager::ModFrameBorderMode()
{
   // Slot called whenever the frame border mode is modified by the user.

   Int_t i = kFrameBorderModeSunken;
   while (!fFrameBorderMode->Find(i)->IsDown()) 
      i++;
   fCurSelStyle->SetFrameBorderMode(i - 1 - kFrameBorderModeSunken);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFrameBorderSize()
{
   // Slot called whenever the frame border size is modified by the user.

   fCurSelStyle->SetFrameBorderSize(fFrameBorderSize->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFuncColor()
{
   // Slot called whenever the function line color is modified by the user.

   fCurSelStyle->SetFuncColor(TColor::GetColor(fFuncColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFuncWidth()
{
   // Slot called whenever the function line width is modified by the user.

   fCurSelStyle->SetFuncWidth(fFuncWidth->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFuncStyle()
{
   // Slot called whenever the function line style is modified by the user.

   fCurSelStyle->SetFuncStyle(fFuncStyle->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModDrawBorder()
{
   // Slot called whenever the draw border boolean is modified by the user.

   fCurSelStyle->SetDrawBorder(fDrawBorder->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModEndErrorSize()
{
   // Slot called whenever the end error size is modified by the user.

   fCurSelStyle->SetEndErrorSize(fEndErrorSize->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModErrorX()
{
   // Slot called whenever the error along X is modified by the user.

   fCurSelStyle->SetErrorX(fErrorX->GetIntNumber() * 0.001);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTimeOffset()
{
   // Slot called whenever the time offset is modified by the user.

   Double_t offset = 0;
   Int_t year  =  ((Int_t) fTimeOffsetDate->GetNumber())/10000;
   Int_t month = (((Int_t) fTimeOffsetDate->GetNumber())/100) % 100;
   Int_t day   =  ((Int_t) fTimeOffsetDate->GetNumber()) % 100;

   while (day > 1) {
      day--;
      offset += 3600 * 24;
   }

   Int_t oneMonthInSecs;
   while (month > 1) {
      month--;
      switch (month) {
         case 2:
            if (year % 4) oneMonthInSecs = 3600 * 24 * 28;
                     else oneMonthInSecs = 3600 * 24 * 29;
            break;
         case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            oneMonthInSecs = 3600 * 24 * 31;
            break;
         default:
            oneMonthInSecs = 3600 * 24 * 30;
      }
      offset += oneMonthInSecs;
   }

   Int_t oneYearInSecs;
   while (year < 1995) {
      if (year % 4) oneYearInSecs = 3600 * 24 * 365;
               else oneYearInSecs = 3600 * 24 * 366;
      offset -= oneYearInSecs;
      year++;
   }
   while (year > 1995) {
      year--;
      if (year % 4) oneYearInSecs = 3600 * 24 * 365;
               else oneYearInSecs = 3600 * 24 * 366;
      offset += oneYearInSecs;
   }

   offset += 788918400 + fTimeOffsetTime->GetNumber();

   fCurSelStyle->SetTimeOffset(offset);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStripDecimals()
{
   // Slot called whenever the strip decimal boolean is modified by the user.

   fCurSelStyle->SetStripDecimals(!fStripDecimals->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModApplyOnXYZ()
{
   //  Slot called whenever the apply on XYZ button is clicked. The settings of
   // the current selected axis pad are applyed on all axis.
   // NB: The logarithmic scale option isn't modified by this method.

   switch (fCurTabAxisNum) {
      case 0: // X axis
         fCurSelStyle->SetAxisColor(fCurSelStyle->GetAxisColor("x"), "yz");
         fCurSelStyle->SetTickLength(fCurSelStyle->GetTickLength("x"), "yz");
         fCurSelStyle->SetTitleColor(fCurSelStyle->GetTitleColor("x"), "yz");
         fCurSelStyle->SetTitleFont(fCurSelStyle->GetTitleFont("x"), "yz");
         fCurSelStyle->SetTitleSize(fCurSelStyle->GetTitleSize("x"), "yz");
         fCurSelStyle->SetTitleOffset(fCurSelStyle->GetTitleOffset("x"), "yz");
         fCurSelStyle->SetNdivisions(fCurSelStyle->GetNdivisions("x"), "yz");
         fCurSelStyle->SetLabelColor(fCurSelStyle->GetLabelColor("x"), "yz");
         fCurSelStyle->SetLabelFont(fCurSelStyle->GetLabelFont("x"), "yz");
         fCurSelStyle->SetLabelSize(fCurSelStyle->GetLabelSize("x"), "yz");
         fCurSelStyle->SetLabelOffset(fCurSelStyle->GetLabelOffset("x"), "yz");
         break;
      case 1: // Y axis
         fCurSelStyle->SetAxisColor(fCurSelStyle->GetAxisColor("y"), "xz");
         fCurSelStyle->SetTickLength(fCurSelStyle->GetTickLength("y"), "xz");
         fCurSelStyle->SetTitleColor(fCurSelStyle->GetTitleColor("y"), "xz");
         fCurSelStyle->SetTitleFont(fCurSelStyle->GetTitleFont("y"), "xz");
         fCurSelStyle->SetTitleSize(fCurSelStyle->GetTitleSize("y"), "xz");
         fCurSelStyle->SetTitleOffset(fCurSelStyle->GetTitleOffset("y"), "xz");
         fCurSelStyle->SetNdivisions(fCurSelStyle->GetNdivisions("y"), "xz");
         fCurSelStyle->SetLabelColor(fCurSelStyle->GetLabelColor("y"), "xz");
         fCurSelStyle->SetLabelFont(fCurSelStyle->GetLabelFont("y"), "xz");
         fCurSelStyle->SetLabelSize(fCurSelStyle->GetLabelSize("y"), "xz");
         fCurSelStyle->SetLabelOffset(fCurSelStyle->GetLabelOffset("y"), "xz");
         break;

      case 2: // Z axis
         fCurSelStyle->SetAxisColor(fCurSelStyle->GetAxisColor("z"), "xy");
         fCurSelStyle->SetTickLength(fCurSelStyle->GetTickLength("z"), "xy");
         fCurSelStyle->SetTitleColor(fCurSelStyle->GetTitleColor("z"), "xy");
         fCurSelStyle->SetTitleFont(fCurSelStyle->GetTitleFont("z"), "xy");
         fCurSelStyle->SetTitleSize(fCurSelStyle->GetTitleSize("z"), "xy");
         fCurSelStyle->SetTitleOffset(fCurSelStyle->GetTitleOffset("z"), "xy");
         fCurSelStyle->SetNdivisions(fCurSelStyle->GetNdivisions("z"), "xy");
         fCurSelStyle->SetLabelColor(fCurSelStyle->GetLabelColor("z"), "xy");
         fCurSelStyle->SetLabelFont(fCurSelStyle->GetLabelFont("z"), "xy");
         fCurSelStyle->SetLabelSize(fCurSelStyle->GetLabelSize("z"), "xy");
         fCurSelStyle->SetLabelOffset(fCurSelStyle->GetLabelOffset("z"), "xy");
         break;
   }

   DisconnectEditor(fCurTabNum);
   UpdateEditor(fCurTabNum);
   ConnectEditor(fCurTabNum);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXTitleSize()
{
   // Slot called whenever the X axis title size is modified by the user.

   fCurSelStyle->SetTitleSize(fXTitleSize->GetNumber(), "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXTitleSizeInPixels(Bool_t b)
{
   // Slot called whenever the X axis title size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetTitleFont("X") / 10;
   Int_t mod = fCurSelStyle->GetTitleFont("X") % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);
   if (b) {
      fCurSelStyle->SetTitleFont(tmp * 10 + 3, "X");
      fXTitleSize->SetFormat(TGNumberFormat::kNESInteger,
                           TGNumberFormat::kNEAPositive);
      fXTitleSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->SetTitleSize(fCurSelStyle->GetTitleSize("X") * h, "X");
   } else {
      fCurSelStyle->SetTitleFont(tmp * 10 + 2, "X");
      fXTitleSize->SetFormat(TGNumberFormat::kNESRealThree,
                           TGNumberFormat::kNEAPositive);
      fXTitleSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 1);
      if (mod == 3)
         fCurSelStyle->SetTitleSize(fCurSelStyle->GetTitleSize("X") / h, "X");
   }
   fXTitleSize->SetNumber(fCurSelStyle->GetTitleSize("X"));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXTitleColor()
{
   // Slot called whenever the X axis title color is modified by the user.

   fCurSelStyle->SetTitleColor(TColor::GetColor(fXTitleColor->GetColor()), "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXTitleOffset()
{
   // Slot called whenever the X axis title offset is modified by the user.

   fCurSelStyle->SetTitleOffset(fXTitleOffset->GetNumber(), "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXTitleFont()
{
   // Slot called whenever the X axis title font is modified by the user.

   Int_t fontPrec = fCurSelStyle->GetTitleFont("X") % 10;
   fCurSelStyle->SetTitleFont(fXTitleFont->GetSelected() * 10 + fontPrec, "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXLabelSize()
{
   // Slot called whenever the X axis label size is modified by the user.

   fCurSelStyle->SetLabelSize(fXLabelSize->GetNumber(), "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXLabelSizeInPixels(Bool_t b)
{
   // Slot called whenever the X axis label size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetLabelFont("X") / 10;
   Int_t mod = fCurSelStyle->GetLabelFont("X") % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);
   if (b) {
      fCurSelStyle->SetLabelFont(tmp * 10 + 3, "X");
      fXLabelSize->SetFormat(TGNumberFormat::kNESInteger,
                           TGNumberFormat::kNEAPositive);
      fXLabelSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->SetLabelSize(fCurSelStyle->GetLabelSize("X") * h, "X");
   } else {
      fCurSelStyle->SetLabelFont(tmp * 10 + 2, "X");
      fXLabelSize->SetFormat(TGNumberFormat::kNESRealThree,
                           TGNumberFormat::kNEAPositive);
      fXLabelSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 1);
      if (mod == 3)
         fCurSelStyle->SetLabelSize(fCurSelStyle->GetLabelSize("X") / h, "X");
   }
   fXLabelSize->SetNumber(fCurSelStyle->GetLabelSize("X"));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXLabelColor()
{
   // Slot called whenever the X axis label color is modified by the user.

   fCurSelStyle->SetLabelColor(TColor::GetColor(fXLabelColor->GetColor()), "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXLabelOffset()
{
   // Slot called whenever the X axis label offset is modified by the user.

   fCurSelStyle->SetLabelOffset(fXLabelOffset->GetNumber(), "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXLabelFont()
{
   // Slot called whenever the X axis label font is modified by the user.

   Int_t fontPrec = fCurSelStyle->GetLabelFont("X") % 10;
   fCurSelStyle->SetLabelFont(fXLabelFont->GetSelected() * 10 + fontPrec, "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXAxisColor()
{
   // Slot called whenever the X axis color is modified by the user.

   fCurSelStyle->SetAxisColor(TColor::GetColor(fXAxisColor->GetColor()), "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXTickLength()
{
   // Slot called whenever the X axis tick length is modified by the user.

   fCurSelStyle->SetTickLength(fXTickLength->GetNumber(), "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModOptLogx()
{
   //  Slot called whenever the X axis log scale boolean is modified
   // by the user.

   fCurSelStyle->SetOptLogx(fOptLogx->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModXNdivisions()
{
   //  Slot called whenever the X axis Number of divisions is modified
   // by the user.

   Int_t sgn = -1;
   if (fXNdivisionsOptimize->IsDown()) sgn = 1;
   fCurSelStyle->SetNdivisions(sgn * (fXNdivMain->GetIntNumber()
                               + 100 * fXNdivSub->GetIntNumber()
                               + 10000 * fXNdivSubSub->GetIntNumber()), "X");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYTitleSize()
{
   // Slot called whenever the Y axis title size is modified by the user.

   fCurSelStyle->SetTitleSize(fYTitleSize->GetNumber(), "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYTitleSizeInPixels(Bool_t b)
{
   // Slot called whenever the Y axis title size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetTitleFont("Y") / 10;
   Int_t mod = fCurSelStyle->GetTitleFont("Y") % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);
   if (b) {
      fCurSelStyle->SetTitleFont(tmp * 10 + 3, "Y");
      fYTitleSize->SetFormat(TGNumberFormat::kNESInteger,
                             TGNumberFormat::kNEAPositive);
      fYTitleSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->SetTitleSize(fCurSelStyle->GetTitleSize("Y") * h, "Y");
   } else {
      fCurSelStyle->SetTitleFont(tmp * 10 + 2, "Y");
      fYTitleSize->SetFormat(TGNumberFormat::kNESRealThree,
                             TGNumberFormat::kNEAPositive);
      fYTitleSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 1);
      if (mod == 3)
         fCurSelStyle->SetTitleSize(fCurSelStyle->GetTitleSize("Y") / h, "Y");
   }
   fYTitleSize->SetNumber(fCurSelStyle->GetTitleSize("Y"));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYTitleColor()
{
   // Slot called whenever the Y axis title color is modified by the user.

   fCurSelStyle->SetTitleColor(TColor::GetColor(fYTitleColor->GetColor()), "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYTitleOffset()
{
   // Slot called whenever the Y axis title offset is modified by the user.

   fCurSelStyle->SetTitleOffset(fYTitleOffset->GetNumber(), "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYTitleFont()
{
   // Slot called whenever the Y axis title font is modified by the user.

   Int_t fontPrec = fCurSelStyle->GetTitleFont("Y") % 10;
   fCurSelStyle->SetTitleFont(fYTitleFont->GetSelected() * 10 + fontPrec, "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYLabelSize()
{
   // Slot called whenever the Y axis label size is modified by the user.

   fCurSelStyle->SetLabelSize(fYLabelSize->GetNumber(), "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYLabelSizeInPixels(Bool_t b)
{
   // Slot called whenever the Y axis label size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetLabelFont("Y") / 10;
   Int_t mod = fCurSelStyle->GetLabelFont("Y") % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);
   if (b) {
      fCurSelStyle->SetLabelFont(tmp * 10 + 3, "Y");
      fYLabelSize->SetFormat(TGNumberFormat::kNESInteger,
                             TGNumberFormat::kNEAPositive);
      fYLabelSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->SetLabelSize(fCurSelStyle->GetLabelSize("Y") * h, "Y");
   } else {
      fCurSelStyle->SetLabelFont(tmp * 10 + 2, "Y");
      fYLabelSize->SetFormat(TGNumberFormat::kNESRealThree,
                             TGNumberFormat::kNEAPositive);
      fYLabelSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 1);
      if (mod == 3)
         fCurSelStyle->SetLabelSize(fCurSelStyle->GetLabelSize("Y") / h, "Y");
   }
   fYLabelSize->SetNumber(fCurSelStyle->GetLabelSize("Y"));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYLabelColor()
{
   // Slot called whenever the Y axis label color is modified by the user.

   fCurSelStyle->SetLabelColor(TColor::GetColor(fYLabelColor->GetColor()), "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYLabelOffset()
{
   // Slot called whenever the Y axis label offset is modified by the user.

   fCurSelStyle->SetLabelOffset(fYLabelOffset->GetNumber(), "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYLabelFont()
{
   // Slot called whenever the Y axis label font is modified by the user.

   Int_t fontPrec = fCurSelStyle->GetLabelFont("Y") % 10;
   fCurSelStyle->SetLabelFont(fYLabelFont->GetSelected() * 10 + fontPrec, "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYAxisColor()
{
   // Slot called whenever the Y axis color is modified by the user.

   fCurSelStyle->SetAxisColor(TColor::GetColor(fYAxisColor->GetColor()), "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYTickLength()
{
   // Slot called whenever the Y axis tick length is modified by the user.

   fCurSelStyle->SetTickLength(fYTickLength->GetNumber(), "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModOptLogy()
{
   // Slot called whenever the Y axis log scale boolean is modified by the user.

   fCurSelStyle->SetOptLogy(fOptLogy->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModYNdivisions()
{
   //  Slot called whenever the Y axis Number of divisions is modified
   // by the user.

   Int_t sgn = -1;
   if (fYNdivisionsOptimize->IsDown()) sgn = 1;
   fCurSelStyle->SetNdivisions(sgn * (fYNdivMain->GetIntNumber()
                               + 100 * fYNdivSub->GetIntNumber()
                               + 10000 * fYNdivSubSub->GetIntNumber()), "Y");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZTitleSize()
{
   // Slot called whenever the Z axis title size is modified by the user.

   fCurSelStyle->SetTitleSize(fZTitleSize->GetNumber(), "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZTitleSizeInPixels(Bool_t b)
{
   // Slot called whenever the Z axis title size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetTitleFont("Z") / 10;
   Int_t mod = fCurSelStyle->GetTitleFont("Z") % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);
   if (b) {
      fCurSelStyle->SetTitleFont(tmp * 10 + 3, "Z");
      fZTitleSize->SetFormat(TGNumberFormat::kNESInteger,
                             TGNumberFormat::kNEAPositive);
      fZTitleSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->SetTitleSize(fCurSelStyle->GetTitleSize("Z") * h, "Z");
   } else {
      fCurSelStyle->SetTitleFont(tmp * 10 + 2, "Z");
      fZTitleSize->SetFormat(TGNumberFormat::kNESRealThree,
                             TGNumberFormat::kNEAPositive);
      fZTitleSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 1);
      if (mod == 3)
         fCurSelStyle->SetTitleSize(fCurSelStyle->GetTitleSize("Z") / h, "Z");
   }
   fZTitleSize->SetNumber(fCurSelStyle->GetTitleSize("Z"));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZTitleColor()
{
   // Slot called whenever the Z axis title color is modified by the user.

   fCurSelStyle->SetTitleColor(TColor::GetColor(fZTitleColor->GetColor()), "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZTitleOffset()
{
   // Slot called whenever the Z axis title offset is modified by the user.

   fCurSelStyle->SetTitleOffset(fZTitleOffset->GetNumber(), "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZTitleFont()
{
   // Slot called whenever the Z axis title font is modified by the user.

   Int_t fontPrec = fCurSelStyle->GetTitleFont("Z") % 10;
   fCurSelStyle->SetTitleFont(fZTitleFont->GetSelected() * 10 + fontPrec, "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZLabelSize()
{
   // Slot called whenever the Z axis label size is modified by the user.

   fCurSelStyle->SetLabelSize(fZLabelSize->GetNumber(), "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZLabelSizeInPixels(Bool_t b)
{
   // Slot called whenever the Z axis Label size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetLabelFont("Z") / 10;
   Int_t mod = fCurSelStyle->GetLabelFont("Z") % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);
   if (b) {
      fCurSelStyle->SetLabelFont(tmp * 10 + 3, "Z");
      fZLabelSize->SetFormat(TGNumberFormat::kNESInteger,
                             TGNumberFormat::kNEAPositive);
      fZLabelSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->SetLabelSize(fCurSelStyle->GetLabelSize("Z") * h, "Z");
   } else {
      fCurSelStyle->SetLabelFont(tmp * 10 + 2, "Z");
      fZLabelSize->SetFormat(TGNumberFormat::kNESRealThree,
                             TGNumberFormat::kNEAPositive);
      fZLabelSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 1);
      if (mod == 3)
         fCurSelStyle->SetLabelSize(fCurSelStyle->GetLabelSize("Z") / h, "Z");
   }
   fZLabelSize->SetNumber(fCurSelStyle->GetLabelSize("Z"));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZLabelColor()
{
   // Slot called whenever the Z axis label color is modified by the user.

   fCurSelStyle->SetLabelColor(TColor::GetColor(fZLabelColor->GetColor()), "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZLabelOffset()
{
   // Slot called whenever the Z axis label offset is modified by the user.

   fCurSelStyle->SetLabelOffset(fZLabelOffset->GetNumber(), "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZLabelFont()
{
   // Slot called whenever the Z axis label font is modified by the user.

   Int_t fontPrec = fCurSelStyle->GetLabelFont("Z") % 10;
   fCurSelStyle->SetLabelFont(fZLabelFont->GetSelected() * 10 + fontPrec, "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZAxisColor()
{
   // Slot called whenever the Z axis color is modified by the user.

   fCurSelStyle->SetAxisColor(TColor::GetColor(fZAxisColor->GetColor()), "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZTickLength()
{
   // Slot called whenever the Z axis tick length is modified by the user.

   fCurSelStyle->SetTickLength(fZTickLength->GetNumber(), "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModOptLogz()
{
   // Slot called whenever the Z axis log scale boolean is modified by the user.

   fCurSelStyle->SetOptLogz(fOptLogz->IsDown());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModZNdivisions()
{
   //  Slot called whenever the Z axis Number of divisions is modified
   // by the user.

   Int_t sgn = -1;
   if (fZNdivisionsOptimize->IsDown()) sgn = 1;
   fCurSelStyle->SetNdivisions(sgn * (fZNdivMain->GetIntNumber()
                               + 100 * fZNdivSub->GetIntNumber()
                               + 10000 * fZNdivSubSub->GetIntNumber()), "Z");
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModOptTitle()
{
   // Slot called whenever the OptTitle boolean is modified by the user.

   fCurSelStyle->SetOptTitle(fOptTitle->IsDown());
   DisconnectEditor(fCurTabNum);
   UpdateEditor(fCurTabNum);
   ConnectEditor(fCurTabNum);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleFillColor()
{
   // Slot called whenever the title fill color is modified by the user.

   fCurSelStyle->SetTitleFillColor(TColor::GetColor(fTitleColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleStyle()
{
   // Slot called whenever the title fill style is modified by the user.

   fCurSelStyle->SetTitleStyle(fTitleStyle->GetPattern());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleTextColor()
{
   // Slot called whenever the title text color is modified by the user.

   fCurSelStyle->SetTitleTextColor(TColor::GetColor(fTitleTextColor->GetColor()));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleFontSize()
{
   // Slot called whenever the text size is modified by the user.

   fCurSelStyle->SetTitleFontSize(fTitleFontSize->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleFontSizeInPixels(Bool_t b)
{
   // Slot called whenever the text size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetTitleFont() / 10;
   Int_t mod = fCurSelStyle->GetTitleFont() % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);
   if (b) {
      fCurSelStyle->SetTitleFont(tmp * 10 + 3);
      fTitleFontSize->SetFormat(TGNumberFormat::kNESInteger,
                                TGNumberFormat::kNEAPositive);
      fTitleFontSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->SetTitleFontSize(fCurSelStyle->GetTitleFontSize() * h);
   } else {
      fCurSelStyle->SetTitleFont(tmp * 10 + 2);
      fTitleFontSize->SetFormat(TGNumberFormat::kNESRealThree,
                                TGNumberFormat::kNEAPositive);
      fTitleFontSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 1);
      if (mod == 3)
         fCurSelStyle->SetTitleFontSize(fCurSelStyle->GetTitleFontSize() / h);
   }
   fTitleFontSize->SetNumber(fCurSelStyle->GetTitleFontSize());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleFont()
{
   // Slot called whenever the title text font is modified by the user.

   Int_t tmp = fCurSelStyle->GetTitleFont() % 10;
   fCurSelStyle->SetTitleFont(fTitleFont->GetSelected() * 10 + tmp);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleAlign()
{
   // Slot called whenever the title text align is modified by the user.

   fCurSelStyle->SetTitleAlign(fTitleAlign->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleBorderSize()
{
   // Slot called whenever the title border size is modified by the user.

   fCurSelStyle->SetTitleBorderSize(fTitleBorderSize->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModLegendBorderSize()
{
   // Slot called whenever the legend border size is modified by the user.

   fCurSelStyle->SetLegendBorderSize(fLegendBorderSize->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleX()
{
   // Slot called whenever the title abscissa is modified by the user.

   fCurSelStyle->SetTitleX(fTitleX->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleY()
{
   // Slot called whenever the title ordinate is modified by the user.

   fCurSelStyle->SetTitleY(fTitleY->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleW()
{
   // Slot called whenever the title width is modified by the user.

   fCurSelStyle->SetTitleW(fTitleW->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitleH()
{
   // Slot called whenever the title height is modified by the user.

   fCurSelStyle->SetTitleH(fTitleH->GetIntNumber() * 0.01);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatColor(Pixel_t color)
{
   // Slot called whenever the stats fill color is modified by the user.

   fCurSelStyle->SetStatColor(TColor::GetColor(color));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatStyle(Style_t pattern)
{
   // Slot called whenever the stats fill style is modified by the user.

   fCurSelStyle->SetStatStyle(pattern);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatTextColor(Pixel_t color)
{
   // Slot called whenever the stats text color is modified by the user.

   fCurSelStyle->SetStatTextColor(TColor::GetColor(color));
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatFontSize()
{
   // Slot called whenever the text size is modified by the user.
   fCurSelStyle->SetStatFontSize(fStatFontSize->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatFontSizeInPixels(Bool_t b)
{
   // Slot called whenever the text size mode is modified by the user.

   Int_t tmp = fCurSelStyle->GetStatFont() / 10;
   Int_t mod = fCurSelStyle->GetStatFont() % 10;
   Double_t h = TMath::Max(fCurSelStyle->GetCanvasDefH(), 100);
   if (b) {
      fCurSelStyle->SetStatFont(tmp * 10 + 3);
      fStatFontSize->SetFormat(TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEANonNegative);
      fStatFontSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, h);
      if (mod == 2)
         fCurSelStyle->SetStatFontSize(fCurSelStyle->GetStatFontSize() * h);
      fStatFontSize->SetNumber(fCurSelStyle->GetStatFontSize());
   } else {
      fCurSelStyle->SetStatFont(tmp * 10 + 2);
      fStatFontSize->SetFormat(TGNumberFormat::kNESRealThree,
                               TGNumberFormat::kNEANonNegative);
      fStatFontSize->SetLimits(TGNumberFormat::kNELLimitMinMax, 0., 1.);
      if (mod == 3)
         fCurSelStyle->SetStatFontSize(fCurSelStyle->GetStatFontSize() / h);
      fStatFontSize->SetNumber(fCurSelStyle->GetStatFontSize());
   }
   fStatFontSize->SetNumber(fCurSelStyle->GetStatFontSize());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatFont()
{
   // Slot called whenever the stats text font is modified by the user.

   Int_t tmp = fCurSelStyle->GetStatFont() % 10;
   fCurSelStyle->SetStatFont(fStatFont->GetSelected() * 10 + tmp);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatX()
{
   // Slot called whenever the stats abscissa is modified by the user.

   fCurSelStyle->SetStatX((Float_t)fStatX->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatY()
{
   // Slot called whenever the stats ordinate is modified by the user.

   fCurSelStyle->SetStatY((Float_t)fStatY->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatW()
{
   // Slot called whenever the stats width is modified by the user.

   fCurSelStyle->SetStatW((Float_t)fStatW->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatH()
{
   // Slot called whenever the stats height is modified by the user.

   fCurSelStyle->SetStatH((Float_t)fStatH->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatBorderSize()
{
   // Slot called whenever the stats border size is modified by the user.

   fCurSelStyle->SetStatBorderSize(fStatBorderSize->GetSelected());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModOptStat()
{
   // Slot called whenever one of the stats options is modified by the user.

   Int_t stat = 0;
   if (fOptStatName->IsDown())        stat +=1;
   if (fOptStatEntries->IsDown())     stat +=10;
   if (fOptStatMean->IsDown())        stat +=100;
   if (fOptStatRMS->IsDown())         stat +=1000;
   if (fOptStatUnderflow->IsDown())   stat +=10000;
   if (fOptStatOverflow->IsDown())    stat +=100000;
   if (fOptStatIntegral->IsDown())    stat +=1000000;
   if (fOptStatSkewness->IsDown())    stat +=10000000;
   if (fOptStatKurtosis->IsDown())    stat +=100000000;
   if (fOptStatErrors->IsDown()) {
      if (fOptStatMean->IsDown())     stat +=100;
      if (fOptStatRMS->IsDown())      stat +=1000;
      if (fOptStatSkewness->IsDown()) stat +=10000000;
      if (fOptStatKurtosis->IsDown()) stat +=100000000;
   }
   if (stat == 1) stat = 1000000001;
   fCurSelStyle->SetOptStat(stat);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModStatFormat(const char *sformat)
{
   // Slot called whenever the stats paint format is modified by the user.

   fCurSelStyle->SetStatFormat(sformat);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModOptFit()
{
   // Slot called whenever one of the fit options is modified by the user.

   Int_t fit = 0;
   if (fOptFitValues->IsDown())      fit +=1;
   if (fOptFitErrors->IsDown())      fit +=10;
   if (fOptFitChi->IsDown())         fit +=100;
   if (fOptFitProbability->IsDown()) fit +=1000;
   if (fit == 1) fit = 10001;
   fCurSelStyle->SetOptFit(fit);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModFitFormat(const char *fitformat)
{
   // Slot called whenever the fit paint format is modified by the user.

   fCurSelStyle->SetFitFormat(fitformat);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModHeaderPS()
{
   // Slot called whenever the PS header is modified by the user.

   fCurSelStyle->SetHeaderPS(fHeaderPS->GetText());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModTitlePS()
{
   // Slot called whenever the PS title is modified by the user.

   fCurSelStyle->SetTitlePS(fTitlePS->GetText());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModColorModelPS()
{
   // Slot called whenever the PS color model is modified by the user.

   Int_t i = kPSPDFColorModelPSRGB;
   while (!fColorModelPS->Find(i)->IsDown()) i++;
   fCurSelStyle->SetColorModelPS(i - kPSPDFColorModelPSRGB);
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModLineScalePS()
{
   // Slot called whenever the PS line scale is modified by the user.

   fCurSelStyle->SetLineScalePS(fLineScalePS->GetNumber());
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPaperSizePredef()
{
   // Slot called whenever the PS paper size is modified by the user.

   Float_t papSizeX;
   Float_t papSizeY;
   fCurSelStyle->GetPaperSize(papSizeX, papSizeY);

   if (fPaperSizePredef->GetSelected() == 1) {
      if (!fPaperSizeEnCm) {
         fPaperSizeEnCm = kTRUE;
         fPaperSizeX->SetNumber(papSizeX);
         fPaperSizeY->SetNumber(papSizeY);
      }
   } else if (fPaperSizePredef->GetSelected() == 2) {
      if (fPaperSizeEnCm) {
         fPaperSizeEnCm = kFALSE;
         fPaperSizeX->SetNumber(papSizeX * 0.394);
         fPaperSizeY->SetNumber(papSizeY * 0.394);
      }
   } else if (fPaperSizePredef->GetSelected() == 3) {
      fPaperSizeEnCm = kTRUE;
      fPaperSizeX->SetNumber(20);
      fPaperSizeY->SetNumber(26);
      fCurSelStyle->SetPaperSize(20, 26);
   } else if (fPaperSizePredef->GetSelected() == 4) {
      fPaperSizeEnCm = kFALSE;
      fPaperSizeX->SetNumber(20 * 0.394);
      fPaperSizeY->SetNumber(24 * 0.394);
      fCurSelStyle->SetPaperSize(20, 24);
   }
   DoEditor();
}

//______________________________________________________________________________
void TStyleManager::ModPaperSizeXY()
{
   // Slot called whenever the PS paper size is modified by the user.

   if (fPaperSizeEnCm) {
      fCurSelStyle->SetPaperSize(fPaperSizeX->GetNumber(),
                                 fPaperSizeY->GetNumber());
      fPaperSizePredef->Select(1);
   } else {
      fCurSelStyle->SetPaperSize(fPaperSizeX->GetNumber() * 2.54,
                                 fPaperSizeY->GetNumber() * 2.54);
      fPaperSizePredef->Select(2);
   }
   DoEditor();
}
