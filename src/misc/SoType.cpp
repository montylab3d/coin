/**************************************************************************\
 *
 *  Copyright (C) 1998-1999 by Systems in Motion.  All rights reserved.
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
  \class SoType Inventor/SoType.h
  \brief The SoType class is the identifier class used in the run-time type
  check system used in Coin.

  All class types derived from SoBase must be registered before any instances
  are created, and base classes must be registered before any of their derived
  classes are.

  A notable feature of the SoType class is that it is only 16 bits long and
  therefore should be passed around by value for efficiency reasons.
*/

#include <Inventor/SoType.h>

#include <Inventor/SbString.h>
#include <Inventor/SbName.h>
#ifdef COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG
#include <Inventor/lists/SoTypeList.h>
#include <Inventor/lists/SbList.h>
#include <Inventor/system/kosher.h>

#include <assert.h>
#include <stdlib.h> // NULL

struct SoTypeData {
  SoTypeData(const SbName theName,
             const SbBool ispublic = FALSE,
             const uint16_t theData = 0,
             const SoType theParent = SoType::badType(),
             const SoType::instantiationMethod createMethod = NULL)
    : name(theName), isPublic(ispublic), data(theData),
      parent(theParent), method(createMethod) { };

  SbName name;
  SbBool isPublic;
  uint16_t data;
  SoType parent;
  SoType::instantiationMethod method;
};

#if defined(NEED_TEMPLATE_DEFINITION)
template class SbList<SoTypeData *>;
#else // ! defined(NEED_TEMPLATE_DEFINITION)
#ifndef DONT_NEED_TEMPLATE_DEFINITION
#error "kosher.h hasn't been included."
#endif // ! DONT_NEED_TEMPLATE_DEFINITION
#endif //NEED_TEMPLATE_DEFINITION

SoTypeList SoType::typeList;
SbList<SoTypeData *> SoType::typeDataList;
SbDict SoType::typeDict(512); //increase if necessary

/*!
  \typedef SoType::instantiationMethod

  FIXME: write doc.
*/

/*!
  This static method initializes the type system.

*/

void
SoType::init(void)
{
  // If this assert fails, it is probably because SoType::init() has
  // been called for a second time. --mortene
  assert(SoType::typeList.getLength() == 0);
  assert(SoType::typeDataList.getLength() == 0);

  SoType::typeList.append(SoType::badType()); // bad type at index 0
  SoType::typeDataList.append(new SoTypeData(SbName("BadType")));
  SoType::typeDict.enter((unsigned long)SbName("BadType").getString(), 0);
}

#if 0 // FIXME: re-code to be run automatically upon exit. 19991106 mortene.
/*!
  This function cleans up the type system.
*/

void
SoType::clean(void)
{
  // clear SoType::typeList
  SoType::typeList.truncate(0);
  SoType::typeList.fit();

  // clean SoType::typeDataList (first delete structures)
  const int num = SoType::typeDataList.getLength();
  for (int i = 0; i < num; i++) {
    delete SoType::typeDataList[ i ];
  }
  SoType::typeDataList.truncate(0);
  SoType::typeDataList.fit();

  SoType::typeDict.clear();
}
#endif // re-code


/*!
  This method creates and registers a new class type.

  Abstract types should use NULL for the \a method argument.

*/

const SoType
SoType::createType(const SoType parent, const SbName name,
                   const instantiationMethod method,
                   const uint16_t data)
{
#ifdef COIN_DEBUG
  if (SoType::fromName(name.getString()) != SoType::badType()) {
    SoDebugError::post("SoType::createType",
                       "a type with name ``%s'' already created",
                       name.getString());
    return SoType::fromName(name.getString());
  }
#endif // COIN_DEBUG

  SoTypeData * typeData = new SoTypeData(name, TRUE, data, parent, method);
  SoType newType;
  newType.index = SoType::typeList.getLength();
  SoType::typeList.append(newType);
  SoType::typeDataList.append(typeData);
  // add to dictionary for fast lookup
  SoType::typeDict.enter((unsigned long)name.getString(),
                         (void *)newType.index);
  return newType;
}

/*!
  This method makes a new class override an existing class.

  The new type should be a C++ subclass of the original class type, but
  this won't be checked though.

*/

const SoType
SoType::overrideType(const SoType originalType,
                     const instantiationMethod method)
{
  SoType::typeDataList[(int)originalType.getKey()]->method = method;
  return originalType;
}

/*!
  This static method returns the SoType object associated with name \a name.
  If no known type matches the given name, SoType::badType() is returned.
*/

SoType
SoType::fromName(const SbName name)
{
  // It should be possible to specify a type name with the "So" prefix
  // and get the correct type id, even though the types in some type
  // hierarchies are named internally without the prefix.
  SbString tmp(name.getString());
  if ((tmp.getLength() > 2) && (strcmp(tmp.getSubString(0, 1).getString(), "So") == 0))
    tmp = tmp.getSubString(2);
  SbName noprefixname(tmp);

  void * temp = NULL;
  if (SoType::typeDict.find((unsigned long)name.getString(), temp) ||
      SoType::typeDict.find((unsigned long)noprefixname.getString(), temp)) {
    const int index = (int)temp;
    assert(index >= 0 && index < SoType::typeList.getLength());
    assert((SoType::typeDataList[index]->name == name) ||
           (SoType::typeDataList[index]->name == noprefixname));
    return SoType::typeList[index];
  }
  return SoType::badType();
}

/*!
  FIXME: write doc.
*/

SoType
SoType::fromKey(uint16_t key)
{
  assert(key < SoType::typeList.getLength());
  return SoType::typeList[(int)key];
}

/*!
  This method returns the name of the SoBase-derived class type the SoType
  object is configured for.
*/

SbName
SoType::getName(void) const
{
  return SoType::typeDataList[ (int)this->getKey() ]->name;
}

/*!
  \fn uint16_t SoType::getData(void) const

  This method returns a type specific data variable.

*/

uint16_t
SoType::getData(void) const
{
  return SoType::typeDataList[(int)this->getKey()]->data;
}

/*!
  This method returns the SoType type for the parent class of the
  SoBase-derived class the SoType object is configured for.
*/

const SoType
SoType::getParent(void) const
{
  return SoType::typeDataList[(int)this->getKey()]->parent;
}

/*!
  This method returns an illegal type, useful for returning errors.

  \sa SbBool SoType::isBad() const
*/

SoType
SoType::badType(void)
{
  SoType bad;
  bad.index = 0;
  return bad;
}

/*!
  \fn SbBool SoType::isBad(void) const

  This method returns TRUE if the SoType object represents an illegal class
  type.
*/

/*!
  This method returns TRUE if the given type is derived from (or \e is) the
  \a parent type, and FALSE otherwise.
*/

SbBool
SoType::isDerivedFrom(const SoType parent) const
{
  assert(!this->isBad());

  if (parent.isBad()) {
#if COIN_DEBUG
    SoDebugError::postWarning("SoType::isDerivedFrom",
                              "can't compare against an invalid type");
#endif // COIN_DEBUG
    return FALSE;
  }

  SoType type = *this;
  do {
#if 0 // debug
    SoDebugError::postInfo("SoType::isDerivedFrom",
                           "this: '%s' parent: '%s'",
                           type.getName().getString(),
                           parent.getName().getString());
#endif // debug
    if (type == parent) return TRUE;
    type = SoType::typeDataList[(int)type.getKey()]->parent;
  } while (!type.isBad());

  return FALSE;
}

/*!
  This method appends all the class types derived from \a type to \a list,
  and returns the number of types added to the list.  Internal types are not
  included in the list, nor are they counted.

  \a type itself is also added to the list, as a type is seen as a derivation
  of its own type.

  NB: do not write code which depends in any way on the order of the
  elements returned in \a list.
*/
int
SoType::getAllDerivedFrom(const SoType type, SoTypeList & list)
{
  int counter = 0;
  int n = SoType::typeList.getLength();
  for (int i = 0; i < n; i++) {
    SoType chktype = SoType::typeList[i];
    if (!chktype.isInternal() && chktype.isDerivedFrom(type)) {
      list.append(chktype);
      counter++;
    }
  }
  return counter;
}

/*!
  This method returns FALSE for abstract base classes, and TRUE for class
  types that can be instanciated.
*/

SbBool
SoType::canCreateInstance(void) const
{
  return (SoType::typeDataList[(int)this->getKey()]->method != NULL);
}

/*!
  This method instantiates an object of the current type.

  For types that can not be instanciated, NULL is returned.
*/

void *
SoType::createInstance(void) const
{
  if (this->canCreateInstance()) {
    return (*(SoType::typeDataList[(int)this->getKey()]->method))();
  }
  else {
#if COIN_DEBUG
    SoDebugError::postWarning("SoType::createInstance",
                              "can't create instance of class type '%s', "
                              " use SoType::canCreateInstance()",
                              this->getName().getString());
#endif // COIN_DEBUG
    return NULL;
  }
}

/*!
  This function returns the number of types registered in the run-time type
  system.
*/

int
SoType::getNumTypes(void)
{
  return SoType::typeList.getLength();
}

/*!
  \fn int16_t SoType::getKey(void) const

  This method returns the type's index in the internal typeList.

*/

/*!
  This method turns the specific type into an internal type.
*/

void
SoType::makeInternal(void)
{
  SoType::typeDataList[(int)this->getKey()]->isPublic = FALSE;
}

/*!
  This function returns TRUE if the type is an internal type.
*/

SbBool
SoType::isInternal(void) const
{
    return SoType::typeDataList[(int)this->getKey()]->isPublic == FALSE;
}

/*!
  \fn SbBool SoType::operator != (const SoType type) const

  FIXME: write doc.
*/

/*!
  \fn SbBool SoType::operator == (const SoType type) const

  FIXME: write doc.
*/

/*!
  \fn SbBool SoType::operator <  (const SoType type) const

  FIXME: write doc.
*/

/*!
  \fn SbBool SoType::operator <= (const SoType type) const

  FIXME: write doc.
*/

/*!
  \fn SbBool SoType::operator >= (const SoType type) const

  FIXME: write doc.
*/

/*!
  \fn SbBool SoType::operator >  (const SoType type) const

  FIXME: write doc.
*/
