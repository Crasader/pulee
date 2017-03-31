//
//  MiniGame.m
//  IkasGame
//
//  Created by Sergio Garcia on 7/2/17.
//
//

#include "./MiniGame.h"
#include "../Constants/Constants.h"
#include "../Helpers/ScreenSizeManager.h"
#include "../Helpers/ImageManager.h"
#include "../Helpers/LanguageManager.h"
#include "../Singletons/GamePlayPointsManager.h"
#include "../Singletons/SoundManager.h"
#include "../Singletons/GamePlayPointsManager.h"

const int DefaultDelaySeconds = 2;
bool playing = false;

MiniGame* MiniGame::create(vector<string> nodes, vector<float> scales)
{
    MiniGame *miniGame = new MiniGame();
    if (miniGame && miniGame->init(nodes, scales)) {
        miniGame->autorelease();
        return miniGame;
    }
    CC_SAFE_DELETE(miniGame);
    return nullptr;
}

MiniGame::~MiniGame()
{
    log("adios");
}

bool MiniGame::init(vector<string> nodes, vector<float> scales)
{
    if (!LayerColor::init())
    {
        return false;
    }
    
    for (int i = 0; i < nodes.size(); i++) {
        _nodeNames.push_back(nodes.at(i));
    }
    
//    Rect visibleRect = ScreenSizeManager::getVisibleRect();
    this->setColor(Color3B(255, 255, 255));
    
    this->startCountDown();
    this->updateScreen();
    return true;
}

void MiniGame::updateScreen()
{
    float levelPoints = GamePlayPointsManager::getInstance()->getCurrentMatchPoints();
    std::ostringstream levelPointsString;
    levelPointsString << levelPoints;
    if (_labelLevelPoints != nullptr) {
        _labelLevelPoints->setString(levelPointsString.str());
    }
    
    float explosionPoints = GamePlayPointsManager::getInstance()->getCurrentExplosionsPoints();
    std::ostringstream explosionPointsString;
    explosionPointsString << explosionPoints;
    if (_labelExplosionPoints != nullptr) {
        _labelExplosionPoints->setString("+" + explosionPointsString.str());
    }
}

void MiniGame::startCountDown()
{
    auto labelThree = makeLabel("3");
    this->addChild(labelThree);
    auto labelTwo = makeLabel("2");
    this->addChild(labelTwo);
    auto labelOne = makeLabel("1");
    this->addChild(labelOne);
    
    
    auto startThree = CallFunc::create(CC_CALLBACK_0(MiniGame::setActionForCountDownLabel, this, labelThree));
    auto startTwo = CallFunc::create(CC_CALLBACK_0(MiniGame::setActionForCountDownLabel, this, labelTwo));
    auto startOne = CallFunc::create(CC_CALLBACK_0(MiniGame::setActionForCountDownLabel, this, labelOne));
    
    auto startGame = CallFunc::create(CC_CALLBACK_0(MiniGame::startGame, this));
    
    auto sequence = Sequence::create(DelayTime::create(0.5), startThree, DelayTime::create(1), startTwo, DelayTime::create(1), startOne, DelayTime::create(1), startGame, NULL);
    this->runAction(sequence);
}

void MiniGame::setActionForCountDownLabel(Label *label)
{
    CallFunc *show = CallFunc::create(CC_CALLBACK_0(MiniGame::showLabel, this, label, 0.1));
    CallFunc *hide = CallFunc::create(CC_CALLBACK_0(MiniGame::hideLabel, this, label, 0.5));
    CallFunc *remove = CallFunc::create(CC_CALLBACK_0(MiniGame::removeLabel, this, label));
    
    auto sequence = Sequence::create(show, hide, DelayTime::create(0.6), remove, NULL);
    label->runAction(sequence);
}

void MiniGame::showLabel(Label *label, float time)
{
    auto show = Spawn::createWithTwoActions(FadeTo::create(time, 255), ScaleTo::create(time, label->getScale() + 0.15));
    label->runAction(show);
}

void MiniGame::hideLabel(Label *label, float time)
{
    auto show = Spawn::createWithTwoActions(FadeTo::create(time, 220), ScaleTo::create(time, 0.75));
    label->runAction(show);
}

void MiniGame::removeLabel(Label *label)
{
    this->removeChild(label);
}

Label* MiniGame::makeLabel(std::string text)
{
    Vec2 center = Vec2(this->getBoundingBox().size.width / 2, this->getBoundingBox().size.height / 2);
    
    auto label = Label::createWithTTF(text, MainBoldFont, 650);
    label->setTextColor(IkasBlue);
    label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    label->setScale(1.5);
    label->setPosition(center);
    label->setOpacity(0);
    
    return label;
}

void MiniGame::startGame()
{
    this->makeUI();
    
    // A JUGAR!!
    this->startGameTimer();
    this->makeItem();
}


void MiniGame::makeItem()
{
    auto delay = DefaultDelaySeconds;
    switch (GamePlayPointsManager::getInstance()->getCurrentDifficulty()) {
        case Difficulty::HARD:
            delay = delay * 0.5;
            break;
        case Difficulty::MEDIUM:
            delay = delay * 0.75;
            break;
        case Difficulty::EASY:
        default:
            delay = delay;
            break;
    }
    auto makeNewItem = CallFunc::create(CC_CALLBACK_0(MiniGame::makeItem, this));
    auto sequence = Sequence::create(DelayTime::create(delay), makeNewItem, nullptr);
    
    if (!playing || _gameLayer->getChildrenCount() >= _nodeNames.size() + 1) {// +1 for back layer
        log("juego pausado o maximos elementos mostrados");
        if (playing) {
            this->runAction(sequence);
        }
        return;
    }
    
    auto itemSize = getItemSize();
    auto index = random(0, (int)_nodeNames.size() - 1);
//    auto item = SpriteButton::create(_nodeNames.at(index), 1.0f, CC_CALLBACK_1(MiniGame::buttonPressed, this));
    auto item = SpriteButton::create(_nodeNames.at(index), 1.0f, CC_CALLBACK_1(MiniGame::buttonPressed, this));
    item->setContentSize(itemSize);
    item->setTag(index);
    
    Vec2 position = this->getItemPosition(itemSize);
    item->setPosition(position);
    
    item->setOpacity(0);
    item->setScale(0.7f);
    
    auto visible = FadeIn::create(0.1f);
    auto scale = ScaleTo::create(0.1f, 1);
    auto spawn = Spawn::create(visible, scale, NULL);
    
    _gameLayer->addChild(item);
    item->runAction(spawn);
    
    this->runAction(sequence);
}

Vec2 MiniGame::getItemPosition(Size itemSize)
{
    auto margin = 10;
    auto maxSize = max(itemSize.width, itemSize.height);
    auto radious = maxSize / 2;
    auto minDistance = maxSize + margin;

    bool valid = true;
    Vec2 position = Vec2(random((radious + margin), _gameLayer->getBoundingBox().size.width - (radious + margin)), random((radious + margin), _gameLayer->getBoundingBox().size.height - (radious + margin)));
    for (int i = 0; i < _gameLayer->getChildren().size(); i++) {
        auto item = _gameLayer->getChildren().at(i);
        auto distance = item->getPosition().distance(position);
        if (distance < minDistance) {
            valid = false;
            break;
        }
    }
    if (!valid) {
        return this->getItemPosition(itemSize);
    } else {
        return position;
    }
}

Size MiniGame::getItemSize()
{
    int maxItemsVertically = 3;
    switch (GamePlayPointsManager::getInstance()->getCurrentDifficulty()) {
        case Difficulty::EMPTY:
        case Difficulty::EASY:
            maxItemsVertically = 3;
            break;
        case Difficulty::MEDIUM:
            maxItemsVertically = 4;
            break;
        case Difficulty::HARD:
            maxItemsVertically = 6;
            break;
    }
    auto margin = ScreenSizeManager::getHeightFromPercentage(1);
    auto maxSize = (_gameLayer->getBoundingBox().size.height - 2 * margin) / maxItemsVertically;
    Size size = Size(maxSize, maxSize);
    return size;
}

void MiniGame::buttonPressed(Ref *sender)
{
    SpriteButton *button = (SpriteButton *)sender;
    button->setEnabled(false);
    
    // Puntos
    GamePlayPointsManager::getInstance()->addMiniGameExplosionPoints();
    this->updateScreen();
    
    // Explosion
    // http://cocos2d-x.org/reference/native-cpp/V3.2/d5/d1a/classcocos2d_1_1_particle_explosion.html
    auto particle = ParticleExplosion::createWithTotalParticles(1000);
    particle->setPosition(button->getPosition());
    particle->setDuration(0.1f);
    particle->setSpeed(250.0f);
    particle->setLife(0.1f);
    particle->setGravity(Vec2(0, 50));
    particle->setStartColor(Color4F(IkasYellow));
    particle->setStartColorVar(Color4F(0, 0, 0, 0));
    particle->setEndColor(Color4F(IkasYellow.r, IkasYellow.g, IkasYellow.b, 230));
    
    particle->setEndColorVar(Color4F(0, 0, 0, 30));
    particle->setAutoRemoveOnFinish(true);
    
    SoundManager::getInstance()->fireworks();
    auto hide = ScaleTo::create(0.1f, 0);
    button->runAction(hide);
    _gameLayer->addChild(particle);
    _gameLayer->removeChild(button);
}

void MiniGame::makeUI()
{
    this->makeGameLayer();
    this->makeHUD();
}

void MiniGame::makeGameLayer()
{
    _borderLayer = LayerColor::create(BorderColor);
    _gameLayer = LayerColor::create(Color4B(255, 255, 255, 255));
    
    this->addChild(_borderLayer);
    this->addChild(_gameLayer);
    
    float margins = ScreenSizeManager::getHeightFromPercentage(3);
    float hudHeight = ScreenSizeManager::getHeightFromPercentage(10);
    
    Size gameLayerSize = Size(0, 0);
    gameLayerSize.height = ScreenSizeManager::getHeightFromPercentage(100) - hudHeight - margins;
    gameLayerSize.height -= margins * 2;
    gameLayerSize.width = ScreenSizeManager::getWidthFromPercentage(100) - 2 * margins;
    _gameLayer->setContentSize(gameLayerSize);
    
    _gameLayer->setPosition(margins, this->getBoundingBox().size.height / 2 - _gameLayer->getBoundingBox().size.height / 2 - hudHeight / 2 - margins / 2);
    
    auto background = Sprite::create(ImageManager::getImage("background-blue"), _gameLayer->getBoundingBox());
    background->setPosition(0,0);
    background->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _gameLayer->addChild(background);
    
    Size borderLayerSize = _gameLayer->getBoundingBox().size;
    borderLayerSize.width += 2 * BorderWidth;
    borderLayerSize.height += 2 * BorderWidth;
    _borderLayer->setContentSize(borderLayerSize);
    Vec2 borderLayerPosition = _gameLayer->getPosition();
    borderLayerPosition.x -= BorderWidth;
    borderLayerPosition.y -= BorderWidth;
    _borderLayer->setPosition(borderLayerPosition);
    
    this->makeHUD();
}

void MiniGame::makeHUD()
{
    float margins = ScreenSizeManager::getHeightFromPercentage(3);
    
    auto hudLayer = LayerColor::create(IkasWhite);
    hudLayer->setContentSize(Size(ScreenSizeManager::getWidthFromPercentage(100) - 2 * margins, ScreenSizeManager::getHeightFromPercentage(10)));
    hudLayer->setPosition(BorderWidth, BorderWidth);
    
    auto borderLayer = LayerColor::create(IkasYellow);
    Size borderLayerSize = hudLayer->getBoundingBox().size;
    borderLayerSize.width += 2 * BorderWidth;
    borderLayerSize.height += 2 * BorderWidth;
    borderLayer->setContentSize(borderLayerSize);
    
    Vec2 borderLayerPosition = _borderLayer->getPosition();
    borderLayerPosition.y += _borderLayer->getBoundingBox().size.height + margins;
    borderLayer->setPosition(borderLayerPosition);
    borderLayer->addChild(hudLayer);
    
    this->addChild(borderLayer);
    
    auto layerLevelPoints = Layer::create();
    layerLevelPoints->setContentSize(Size((hudLayer->getBoundingBox().size.width / 3) - 20, hudLayer->getBoundingBox().size.height - 20));
    layerLevelPoints->setPosition(hudLayer->getBoundingBox().size.width / 3 * 0, 10);
    
    auto labelTitlePoints = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "points-title"), MainBoldFont, 45);
    labelTitlePoints->setWidth(layerLevelPoints->getBoundingBox().size.width);
    labelTitlePoints->setHeight(layerLevelPoints->getBoundingBox().size.height * 0.4);
    labelTitlePoints->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    labelTitlePoints->setPosition(0, layerLevelPoints->getBoundingBox().size.height * 0.6);
    labelTitlePoints->setVerticalAlignment(TextVAlignment::CENTER);
    labelTitlePoints->setHorizontalAlignment(TextHAlignment::CENTER);
    labelTitlePoints->setTextColor(IkasBlue);
    layerLevelPoints->addChild(labelTitlePoints);
    
    _labelLevelPoints = Label::createWithTTF("0", MainBoldFont, 50);
    _labelLevelPoints->setWidth(layerLevelPoints->getBoundingBox().size.width);
    _labelLevelPoints->setHeight(layerLevelPoints->getBoundingBox().size.height * 0.6);
    _labelLevelPoints->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelLevelPoints->setVerticalAlignment(TextVAlignment::TOP);
    _labelLevelPoints->setHorizontalAlignment(TextHAlignment::CENTER);
    _labelLevelPoints->setTextColor(IkasBlue);
    layerLevelPoints->addChild(_labelLevelPoints);
    
    hudLayer->addChild(layerLevelPoints);
    
    _labelExplosionPoints = Label::createWithTTF("+0", MainBoldFont, 50);
    _labelExplosionPoints->setWidth(hudLayer->getBoundingBox().size.width / 3 - 20);
    _labelExplosionPoints->setHeight(hudLayer->getBoundingBox().size.height - 20);
    _labelExplosionPoints->setPosition(hudLayer->getBoundingBox().size.width - _labelExplosionPoints->getBoundingBox().size.width / 2 - 10, hudLayer->getBoundingBox().size.height / 2);
    _labelExplosionPoints->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _labelExplosionPoints->setVerticalAlignment(TextVAlignment::CENTER);
    _labelExplosionPoints->setHorizontalAlignment(TextHAlignment::CENTER);
    _labelExplosionPoints->setTextColor(IkasBlue);
    hudLayer->addChild(_labelExplosionPoints);
    
    this->makeTimer(hudLayer);
    this->updateScreen();
}

// Timer
void MiniGame::makeTimer(Layer* onLayer)
{
    _action = NULL;
    _sequence = NULL;
    if (_progress != NULL) {
        _progress->stopAllActions();
    }
    
    Sprite* timerBackground = Sprite::create(ImageManager::getImage("timer-background"));
    timerBackground->setScale(1);
    
    auto scale = (onLayer->getContentSize().height - 20) / timerBackground->getContentSize().height;
    timerBackground->setScale(scale);
    
    timerBackground->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 centerPos = Vec2(onLayer->getBoundingBox().size.width / 2, onLayer->getBoundingBox().size.height / 2);
    timerBackground->setPosition(centerPos);
    
    Sprite* timer = Sprite::create(ImageManager::getImage("timer-in"));
    
    _progress = ProgressTimer::create(timer);
    _progress->setScale(timerBackground->getScale());
    _progress->setAnchorPoint(timerBackground->getAnchorPoint());
    _progress->setPosition(timerBackground->getPosition());
    
    onLayer->addChild(timerBackground);
    onLayer->addChild(_progress);
    
    _progress->setType(ProgressTimer::Type::RADIAL);
    _progress->setMidpoint(Point::ANCHOR_MIDDLE);
    
    auto maxTimeInSeconds = 25;
//    Difficulty difficulty = GamePlayPointsManager::getInstance()->getCurrentDifficulty();
//    switch (difficulty) {
//        case Difficulty::EMPTY:
//        case Difficulty::EASY:
//            maxTimeInSeconds = 25;
//            break;
//        case Difficulty::MEDIUM:
//            maxTimeInSeconds = 20;
//            break;
//        case Difficulty::HARD:
//            maxTimeInSeconds = 15;
//            break;
//    }
    _action = ProgressFromTo::create(maxTimeInSeconds, 0, 100);
}

void MiniGame::startGameTimer()
{
    playing = true;
    CallFunc* callback = CallFunc::create(CC_CALLBACK_0(MiniGame::gameTimeElapsed, this));
    _sequence = Sequence::create(_action, callback, NULL);
    _progress->runAction(_sequence);
}

void MiniGame::gameTimeElapsed()
{
    playing = false;
    this->getEventDispatcher()->setEnabled(true);
    _delegate->didFinish();
}
