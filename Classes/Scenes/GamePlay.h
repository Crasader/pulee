//
//  GamePlay.h
//  IkasGame
//
//  Created by Axier Fernandez on 10/2/15.
//
//

#ifndef __IkasGame__GamePlay__
#define __IkasGame__GamePlay__

#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

#include "../Constants/CommonProtocols.h"
#include "../CustomGUI/SpriteButton.h"
#include "../CustomGUI/FSprite.h"

#include "../GameModels/MainCategory.h"
#include "../GameModels/SubCategory.h"
#include "../GameModels/Option.h"
#include "../Singletons/ZipManager.h"

#include "../Layers/MiniGame.h"

enum class TouchState
{
    START,
    MOVING,
    FINISH
};

enum class GamePlayOrientation
{
    PORTRAIT,
    LANDSCAPE
};

class GamePlay : public Layer, public GamePlayDelegate, public MiniGameDelegate
{
public:
    virtual bool init();
    static Scene* createScene();
    CREATE_FUNC(GamePlay);
    
    EventListenerTouchOneByOne *addEvents();
    
    virtual bool touchBegan(Touch *touch, Event *pEvent);
    virtual void touchEnded(Touch *touch, Event *pEvent);
    virtual void touchMoved(Touch *touch, Event *pEvent);
    virtual void touchCancelled(Touch *pTouch, Event *pEvent);
    
    // GamePlayDelegate
    virtual void loadNextGamePlay();
    virtual void resumeGamePlay();
    virtual void restartGame();
    virtual void updateScreenGameStats();
    
    // MiniGameDelegate
    virtual void didFinish();   
    
private:
    vector<MainCategory*> _categories;
    vector<SubCategory*> _subCategories;
    vector<int> _loadedIndex;
    
    void setupLevelUI(GamePlayOrientation orientation);
    void setupLevelSettings();
    void pauseGame(Ref* sender);

    std::vector<int>makeDestinations(int totalItems);
    void showFinishedMiniGame();
    
    Layer *_layerCategory, *_layerLevel, *_layerPoints;
    Label *_labelTitleCategory, *_labelTitleLevel, *_labelTitlePoints;
    Label *_labelCategory, *_labelPoints, *_labelLevel;
    
    Layer *_borderLayer, *_gameLayer, *_hudLayer;//, *_touchLayer;
    Sprite *_gameBackground;
    TouchState _touchState;
//    EventListenerTouchOneByOne *_touchListener;
    
    int _totalSuccessScreens;
    
    SpriteButton* _pauseButton;
    
    // Current Level
    Node *_movingNode;
    vector<FSprite *> _loadedNodes;
    vector<Node *> _resultNodes;
    vector<float> _loadedNodesDefaultScale;
    vector<Vec2> _loadedNodesStartPosition;
    vector<Vec2> _loadedNodesEndPosition;
    vector<bool> _successMovedNodes;
    
    // Touch Control
    Vec2 _offsetFromCenter;
    
    void checkGameStatus();
    void openSuccessScreen();
};
#endif /* defined(__IkasGame__GamePlay__) */
