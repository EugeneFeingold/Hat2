#pragma once
#ifndef inclIHatMode

#define inclIHatMode


#include "LPD8806.h"
#include "Settings.h"


class HM_Base
{
public:

  virtual ~HM_Base() {
  }

  void init(LPD8806 *strip, Settings *settings);
  virtual void loop() = 0;
  void kill();

protected:
  LPD8806* strip; 
  Settings* settings;


  typedef HM_Base super;
  

};


#endif

