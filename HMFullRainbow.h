

#include "HM_Base.h"
#include "LPD8806.h"

class HMFullRainbow : public HM_Base {
public:
  HMFullRainbow();

  void init(LPD8806 *strip, Settings *settings);
  void loop();


private:
    float j;

};

