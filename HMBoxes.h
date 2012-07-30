#include "IHatMode.h"
#include "LPD8806.h"

class HMBoxes : public IHatMode {
  public:
    HMBoxes(LPD8806 *strip, Settings *settings);
    
    void init();
    void loop();
    
    
  private:
    int j;
    
};
