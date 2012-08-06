

#include "HM_Base.h"
#include "LPD8806.h"

class HMRandomStripes : public HM_Base {
public:
  HMRandomStripes();

  void init(LPD8806 *strip, Settings *settings);
  void loop();


private:

};

