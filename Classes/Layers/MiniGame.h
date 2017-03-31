//
//  MiniGame.h
//  IkasGame
//
//  Created by Sergio Garcia on 7/2/17.
//
//

#ifndef __IkasCocosGame__MiniGame__
#define __IkasCocosGame__MiniGame__

#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

#include "../CustomGUI/SpriteButton.h"

class MiniGameDelegate
{
public:
    virtual void didFinish() = 0;
};

class MiniGame : public LayerColor
{
 
public:
    static MiniGame* create(vector<string> nodes, vector<float> scales);
    ~MiniGame();
    bool init(vector<string> nodes, vector<float> scales);
//    CREATE_FUNC(MiniGame);
    
    void setDelegate(MiniGameDelegate *delegate) { _delegate = delegate;};

    void showLabel(Label *label, float time);
    void hideLabel(Label *label, float time);
    void removeLabel(Label *label);
    void startGame();
protected:
    void buttonPressed(Ref *sender);
    
private:
    MiniGameDelegate *_delegate;
    
    Label *_labelLevelPoints, *_labelExplosionPoints;
    void updateScreen();
    
    void startCountDown();
    void setActionForCountDownLabel(Label *label);
    
    Label* makeLabel(string text);
    void makeUI();
    void makeGameLayer();
    void makeHUD();
    Vec2 getItemPosition(Size itemSize);
    Size getItemSize();
    
    Layer *_borderLayer, *_gameLayer;
    
    vector<string> _nodeNames; // From GamePlay
    
    vector<Vec2> _loadedNodesPosition; // Dynamic
    void makeItem();
    
    // Timer
    void makeTimer(Layer* onLayer);
    void startGameTimer();
    void gameTimeElapsed();

    ProgressTimer* _progress;
    ProgressFromTo* _action;
    Sequence* _sequence;
};
#endif /* defined(__IkasCocosGame__MiniGame__) */
