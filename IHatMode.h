
#ifndef inclIHatMode

#define inclIHatMode 0


#include "LPD8806.h"
#include "Settings.h"


class IHatMode
{
    public:
        
        virtual ~IHatMode() {}
                
        virtual void init() = 0;
        virtual void loop() = 0;
    
   protected:
      LPD8806* strip; 
      Settings* settings;  
        
};


#endif
