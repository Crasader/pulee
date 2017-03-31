//
//  FSprite.h
//  IkasGame
//
//  Created by Sergio Garcia on 8/2/17.
//
//

#ifndef __IkasCocosGame__FSprite__
#define __IkasCocosGame__FSprite__

#include "cocos2d.h"

using namespace cocos2d;
using namespace std;

class FSprite: public Sprite
{
public:
    static FSprite* createFromHdOptions(string fileName);
    static FSprite* create(string fileName);
    
    string getFileName();
};

#endif /* FSprite_h */
