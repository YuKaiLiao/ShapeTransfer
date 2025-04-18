// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _StepVisual_PresentationLayerAssignment_HeaderFile
#define _StepVisual_PresentationLayerAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_HArray1OfLayeredItem.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepVisual_LayeredItem;

class StepVisual_PresentationLayerAssignment;
DEFINE_STANDARD_HANDLE(StepVisual_PresentationLayerAssignment, Standard_Transient)

class StepVisual_PresentationLayerAssignment : public Standard_Transient
{

public:
  //! Returns a PresentationLayerAssignment
  Standard_EXPORT StepVisual_PresentationLayerAssignment();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&        aName,
                            const Handle(TCollection_HAsciiString)&        aDescription,
                            const Handle(StepVisual_HArray1OfLayeredItem)& aAssignedItems);

  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& aName);

  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& aDescription);

  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  Standard_EXPORT void SetAssignedItems(
    const Handle(StepVisual_HArray1OfLayeredItem)& aAssignedItems);

  Standard_EXPORT Handle(StepVisual_HArray1OfLayeredItem) AssignedItems() const;

  Standard_EXPORT StepVisual_LayeredItem AssignedItemsValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbAssignedItems() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_PresentationLayerAssignment, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)        name;
  Handle(TCollection_HAsciiString)        description;
  Handle(StepVisual_HArray1OfLayeredItem) assignedItems;
};

#endif // _StepVisual_PresentationLayerAssignment_HeaderFile
