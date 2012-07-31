
#include "HM_Base.h"
#include "LPD8806.h"

class HMRainbow : public HM_Base {
public:
  HMRainbow();

  void init(LPD8806 *strip, Settings *settings);
  void loop();


private:
  int j;

};

