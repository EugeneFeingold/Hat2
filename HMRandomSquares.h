

#include "HM_Base.h"
#include "LPD8806.h"

class HMRandomSquares : public HM_Base {
public:
  HMRandomSquares();

  void init(LPD8806 *strip, Settings *settings);
  void loop();


private:

};

