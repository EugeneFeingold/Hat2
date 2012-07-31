#include "HM_Base.h"
#include "LPD8806.h"

class HMBoxes : public HM_Base {
public:


  void init(LPD8806 *strip, Settings *settings);
  void loop();


private:

  int j;

};

