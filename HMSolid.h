#include "HM_Base.h"
#include "LPD8806.h"

class HMSolid : public HM_Base {
public:
  HMSolid();

  void init(LPD8806 *strip, Settings *settings);
  void loop();


private:


};

