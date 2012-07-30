#include "IHatMode.h"
#include "LPD8806.h"

class HMSolid : public IHatMode {
public:
  HMSolid(LPD8806 *strip, Settings *settings);

  void init();
  void loop();


private:


};

