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

#ifndef __SOENABLEDELEMENTSLIST_H__
#define __SOENABLEDELEMENTSLIST_H__

#include <Inventor/lists/SoTypeList.h>

class SoEnabledElementsList {
public:
  SoEnabledElementsList(SoEnabledElementsList * const parentList);
  const SoTypeList & getElements(void) const;
  void enable(const SoType elementType, const int stackIndex);
  void merge(const SoEnabledElementsList & list);

  static int getCounter(void);

private:
  static int counter;
  int setUpCounter;
  SoTypeList elements;
  SoEnabledElementsList * parent;
};

#endif // !__SOENABLEDELEMENTSLIST_H__
