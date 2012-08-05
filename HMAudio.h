#include "HM_Base.h"
#include "LPD8806.h"

class HMAudio : public HM_Base {
public:
  HMAudio();

  void init(LPD8806 *strip, Settings *settings);
  void loop();


private:



};

