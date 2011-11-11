// @(#)root/core/utils:$Id: ClassSelectionRule.h 28529 2009-05-11 16:43:35Z pcanal $
// Author: Velislava Spasova September 2010

/*************************************************************************
 * Copyright (C) 1995-2011, Rene Brun, Fons Rademakers and al.           *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef R__CLASSSELECTIONRULE_H
#define R__CLASSSELECTIONRULE_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ClassSelection                                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "BaseSelectionRule.h"
#include "VariableSelectionRule.h"

#include <list>

class ClassSelectionRule: public BaseSelectionRule
{
private:
   std::list<VariableSelectionRule> fFieldSelectionRules;
   std::list<FunctionSelectionRule> fMethodSelectionRules;
   bool fIsInheritable;

   bool fPlus;        // for linkdef.h: true if we had '+' at the end of a class name
   bool fMinus;       // for linkdef.h: true if we had '-' or "-!" at the end of a class name
   bool fExclamation; // for linkdef.h: true if we had '!' at the end of a class name
   
public:
   ClassSelectionRule():
      BaseSelectionRule(), fIsInheritable(false), fPlus(false), fMinus(false), fExclamation(false) {}
   ClassSelectionRule(bool inherit, ESelect sel, std::string attributeName, std::string attributeValue):
      BaseSelectionRule(sel, attributeName, attributeValue), fIsInheritable(inherit), fPlus(false), fMinus(false), fExclamation(false) {}
   

   void AddFieldSelectionRule(VariableSelectionRule field); //adds entry to the filed selections list
   bool HasFieldSelectionRules();
   //const std::list<VariableSelectionRule>& getFieldSelectionRules(); //gets the field selections list
   std::list<VariableSelectionRule>& GetFieldSelectionRules(); //gets the field selections list
   
   void AddMethodSelectionRule(FunctionSelectionRule method); //adds entry to the method selections list
   bool HasMethodSelectionRules();
   //const std::list<FunctionSelectionRule>& getMethodSelectionRules(); //gets the method selections list
   std::list<FunctionSelectionRule>& GetMethodSelectionRules(); //gets the method selections list
   
   bool IsInheritable(); //checks if the class selection rule is inheritable
   void SetInheritable(bool inherit); //sets the inheritance rule for the class

   bool HasPlus();
   void SetPlus(bool pl);

   bool HasMinus();
   void SetMinus(bool mn);

   bool HasExclamation();
   void SetExclamation(bool excl);   
};

#endif
