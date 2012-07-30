
#include "IHatMode.h"
#include "LPD8806.h"

class HMRainbow : public IHatMode {
public:
  HMRainbow(LPD8806 *strip, Settings *settings);

  void init();
  void loop();


private:
  int j;

};

