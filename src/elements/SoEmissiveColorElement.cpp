/**************************************************************************\
 *
 *  Copyright (C) 1998-2000 by Systems in Motion.  All rights reserved.
 *
 *  This file is part of the Coin library.
 *
 *  This file may be distributed under the terms of the Q Public License
 *  as defined by Troll Tech AS of Norway and appearing in the file
 *  LICENSE.QPL included in the packaging of this file.
 *
 *  If you want to use Coin in applications not covered by licenses
 *  compatible with the QPL, you can contact SIM to aquire a
 *  Professional Edition license for Coin.
 *
 *  Systems in Motion AS, Prof. Brochs gate 6, N-7030 Trondheim, NORWAY
 *  http://www.sim.no/ sales@sim.no Voice: +47 22114160 Fax: +47 67172912
 *
\**************************************************************************/

/*!
  \class SoEmissiveColorElement Inventor/elements/SoEmissiveColorElement.h
  \brief The SoEmissiveColorElement class is yet to be documented.

  FIXME: write doc.
*/

#include <Inventor/elements/SoEmissiveColorElement.h>

#include <Inventor/SbColor.h>
#include <assert.h>

// Dynamically allocated to avoid problems on systems which doesn't
// handle static constructors.
static SbColor * defaultemissivecolor = NULL;


/*!
  \fn SoEmissiveColorElement::numColors

  FIXME: write doc.
*/

/*!
  \fn SoEmissiveColorElement::colors

  FIXME: write doc.
*/

SO_ELEMENT_SOURCE(SoEmissiveColorElement);

/*!
  This static method initializes static data for the SoEmissiveColorElement
  class.
*/

void
SoEmissiveColorElement::initClass()
{
  SO_ELEMENT_INIT_CLASS(SoEmissiveColorElement, inherited);
  defaultemissivecolor = new SbColor; // FIXME: deallocate on exit. 20000406 mortene.
  defaultemissivecolor->setValue(0.0f, 0.0f, 0.0f);
}

/*!
  The destructor.
*/

SoEmissiveColorElement::~SoEmissiveColorElement()
{
}

//! FIXME: write doc.

void
SoEmissiveColorElement::init(SoState * /* state */)
{
  this->colors = defaultemissivecolor;
  this->numColors = 1;
}

//! FIXME: write doc.

void
SoEmissiveColorElement::set(SoState * const state, SoNode * const node,
                           const int32_t numColors,
                           const SbColor * const colors)
{
  SoEmissiveColorElement *elem = (SoEmissiveColorElement*)
    SoReplacedElement::getElement(state, classStackIndex, node);
  if (numColors > 0)
    elem->setElt(numColors, colors);
  else
    elem->setElt(1, defaultemissivecolor);
}

//! FIXME: write doc.

void
SoEmissiveColorElement::setElt(const int32_t numColors,
                              const SbColor * const colors)
{
  this->colors = colors;
  this->numColors = numColors;
}

//! FIXME: write doc.

//$ EXPORT INLINE
int32_t
SoEmissiveColorElement::getNum() const
{
  return this->numColors;
}

//! FIXME: write doc.

const SbColor &
SoEmissiveColorElement::get(const int index) const
{
  assert(index >= 0 && index <= this->numColors);
  return this->colors[index];
}

/*!
  Returns a pointer to the colora array. This method is not part of the OIV API.
*/
const SbColor *
SoEmissiveColorElement::getArrayPtr() const
{
  return this->colors;
}

//! FIXME: write doc.

//$ EXPORT INLINE
const SoEmissiveColorElement *
SoEmissiveColorElement::getInstance(SoState *state)
{
  return (const SoEmissiveColorElement *)
    SoElement::getConstElement(state, classStackIndex);
}
