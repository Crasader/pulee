//
//
//  GamePlay.cpp
//  IkasGame
//
//  Created by Axier Fernandez on 10/2/15.
//
//

#include "GamePlay.h"
#include "../Singletons/GameSettingsManager.h"
#include "../Singletons/GamePlayPointsManager.h"
#include "../Singletons/AppManager.h"
#include "../Singletons/SceneManager.h"
#include "../Singletons/SoundManager.h"
#include "../Helpers/LanguageManager.h"
#include "../Helpers/ScreenSizeManager.h"
#include "../Helpers/ImageManager.h"
#include "../Helpers/DataFileManager.h"

const Value PlayMaxTime = Value(40.0f);
const Value TimeToReduce = Value(2.0f);
const Value TimeMinPercentage = Value(25);
const Value SuccessTime = Value(1.5);

const int MaxVerticalItems = 3;
float allowedItemErrorOffset = 250; // Error permitido al soltar el Sprite en su destino
int minimumItems = 3;
int maximunItems = 5;

Layer *_touchLayer;
EventListenerTouchOneByOne *_touchListener;

Scene* GamePlay::createScene()
{
    auto *scene = Scene::create();
    auto *layer = GamePlay::create();
    layer->setTag(2339);
    
    scene->addChild(layer);
    
    return scene;
}

bool GamePlay::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    DataFileManager *dataManager = DataFileManager::getInstance();
    _categories = dataManager->getMainCategories();
    if (_categories.empty()) {
        dataManager->parseDataFile();
        _categories = dataManager->getMainCategories();
    }
    _subCategories = _categories.at(GamePlayPointsManager::getInstance()->getCurrentCategory())->getFilteredSubCategoriesByLevel(GamePlayPointsManager::getInstance()->getCurrentDifficulty());
    AppManager::getInstance()->setGamePlayDelegate(this);
    
    Rect visibleRect = ScreenSizeManager::getVisibleRect();
    
    auto yellowLayer = LayerColor::create(IkasYellow);
    yellowLayer->setContentSize(visibleRect.size);
    yellowLayer->setIgnoreAnchorPointForPosition(false);
    yellowLayer->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    yellowLayer->setAnchorPoint(Point::ANCHOR_MIDDLE);
    this->addChild(yellowLayer);
    
//    auto background = Sprite::create(ImageManager::getImage("background"), visibleRect);
//    background->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
//    background->setAnchorPoint(Point::ANCHOR_MIDDLE);
//    this->addChild(background);

    _labelLevel = Label::createWithTTF("0", MainBoldFont, 50);
    _labelCategory = Label::createWithTTF("0", MainBoldFont, 50);
    _labelPoints = Label::createWithTTF("0", MainBoldFont, 50);
    
    _borderLayer = LayerColor::create(BorderColor);
    _gameLayer = LayerColor::create(IkasWhite);
    
//    _gameBackground = Sprite::create(ImageManager::getImage("background"), visibleRect);
//    _gameBackground->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
//    
//    _gameLayer->addChild(_gameBackground);
    
    this->addChild(_borderLayer);
    this->addChild(_gameLayer);
    
    _hudLayer = Layer::create();
    this->addChild(_hudLayer);
    
    _layerCategory = Layer::create();
    _labelTitleCategory = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "category-title"), MainBoldFont, 45);
    _labelTitleCategory->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    _labelTitleCategory->setVerticalAlignment(TextVAlignment::BOTTOM);
    _labelTitleCategory->setTextColor(IkasWhite);
    _layerCategory->addChild(_labelTitleCategory);
    
    _labelCategory->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelCategory->setVerticalAlignment(TextVAlignment::TOP);
    _labelCategory->setTextColor(IkasBlue);
    _layerCategory->addChild(_labelCategory);
    
    _labelCategory->setPosition(0, 0);
    _hudLayer->addChild(_layerCategory);
    
    
    _layerLevel = Layer::create();
    _labelTitleLevel = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "level-title"), MainBoldFont, 45);
    _labelTitleLevel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    _labelTitleLevel->setVerticalAlignment(TextVAlignment::BOTTOM);
    _labelTitleLevel->setTextColor(IkasWhite);
    _layerLevel->addChild(_labelTitleLevel);
    
    _labelLevel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelLevel->setVerticalAlignment(TextVAlignment::TOP);
    _labelLevel->setTextColor(IkasBlue);
    _layerLevel->addChild(_labelLevel);
    
    _labelLevel->setPosition(0, 0);
    _hudLayer->addChild(_layerLevel);
    
    
    _layerPoints = Layer::create();
    _labelTitlePoints = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "points-title"), MainBoldFont, 45);
    _labelTitlePoints->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    _labelTitlePoints->setVerticalAlignment(TextVAlignment::BOTTOM);
    _labelTitlePoints->setTextColor(IkasWhite);
    _layerPoints->addChild(_labelTitlePoints);
    
    _labelPoints->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelPoints->setVerticalAlignment(TextVAlignment::TOP);
    _labelPoints->setTextColor(IkasBlue);
    _layerPoints->addChild(_labelPoints);
    
    _labelPoints->setPosition(0, 0);
    _hudLayer->addChild(_layerPoints);
    
    
    _pauseButton = SpriteButton::create(ImageManager::getImage("pause"), 0.4f, CC_CALLBACK_1(GamePlay::pauseGame, this));
    _pauseButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _hudLayer->addChild(_pauseButton);
    
    this->setupLevelSettings();
    this->loadNextGamePlay();
    
    return true;
}

void GamePlay::setupLevelSettings() {
    
    _totalSuccessScreens = 0;
    Difficulty difficulty = GamePlayPointsManager::getInstance()->getCurrentDifficulty();
    switch (difficulty) {
        default:
        case Difficulty::EASY:
            allowedItemErrorOffset = 250;
            minimumItems = 3;
            maximunItems = 3;
            break;
        case Difficulty::MEDIUM:
            allowedItemErrorOffset = 150;
            minimumItems = 3;
            maximunItems = 6;
            break;
        case Difficulty::HARD:
            allowedItemErrorOffset = 50;
            minimumItems = 5;
            maximunItems = 10;
            break;
    }
}

void GamePlay::setupLevelUI(GamePlayOrientation orientation) {
    
    float margins = ScreenSizeManager::getHeightFromPercentage(3);
    float hudHeight = ScreenSizeManager::getHeightFromPercentage(10);
    
    Size gameLayerSize = Size(0, 0);
    if (orientation == GamePlayOrientation::PORTRAIT) {
        gameLayerSize.height = ScreenSizeManager::getHeightFromPercentage(100);
        gameLayerSize.height -= margins * 2;
        gameLayerSize.width = ScreenSizeManager::getWidthFromPercentage(80);
    } else {
//        gameLayerSize.height = ScreenSizeManager::getHeightFromPercentage(80);
//        gameLayerSize.width = ScreenSizeManager::getWidthFromPercentage(100);
//        gameLayerSize.width -= margins * 2;
        gameLayerSize.height = ScreenSizeManager::getHeightFromPercentage(100) - hudHeight - margins;
        gameLayerSize.height -= margins * 2;
        gameLayerSize.width = ScreenSizeManager::getWidthFromPercentage(100) - 2 * margins;
    }
    _gameLayer->setContentSize(gameLayerSize);
    
    
    if (orientation == GamePlayOrientation::PORTRAIT) {
        _gameLayer->setPosition(ScreenSizeManager::getWidthFromPercentage(100) - margins - _gameLayer->getBoundingBox().size.width,
                                this->getBoundingBox().size.height / 2 - _gameLayer->getBoundingBox().size.height / 2);
    } else {
        _gameLayer->setPosition(margins, this->getBoundingBox().size.height / 2 - _gameLayer->getBoundingBox().size.height / 2 - hudHeight / 2 - margins / 2);
    }
    
//    if (_gameBackground != nullptr && _gameBackground->getParent() != nullptr) {
//        _gameBackground->removeFromParent();
//        _gameBackground = nullptr;
//    }
    
    auto back = _gameLayer->getChildByTag(323232);
    if (back == nullptr) {
        _gameBackground = Sprite::create(ImageManager::getImage("background-blue"), _gameLayer->getBoundingBox());
        _gameBackground->setTag(323232);
        _gameBackground->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        _gameLayer->addChild(_gameBackground);
    }
    
    
    
//    _gameBackground->setTextureRect(_gameLayer->getBoundingBox());
//    _gameBackground->setrect
//    _gameBackground->setContentSize(_gameLayer->getBoundingBox().size);
    
//    _gameBackground->setPosition(0, 0);
    
    Size borderLayerSize = _gameLayer->getBoundingBox().size;
    borderLayerSize.width += 2 * BorderWidth;
    borderLayerSize.height += 2 * BorderWidth;
    _borderLayer->setContentSize(borderLayerSize);
    Vec2 borderLayerPosition = _gameLayer->getPosition();
    borderLayerPosition.x -= BorderWidth;
    borderLayerPosition.y -= BorderWidth;
    _borderLayer->setPosition(borderLayerPosition);
    
    /*
     * Si se pasa al init la capa no se visualiza ni aparece. Motivo=¿?¿?¿?
     */
    // INICIO crear layer touch
    if (_touchLayer != nullptr) {
        _gameLayer->removeChild(_touchLayer);
        _touchLayer = nullptr;
    }
    
    if (_touchListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);
        _touchListener = nullptr;
    }
    
    _touchLayer = LayerColor::create(Color4B(0, 0, 0, 0));
    _gameLayer->addChild(_touchLayer);
    
    _touchListener = addEvents();
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, _touchLayer);
    // FIN crear layer touch
    
    _touchLayer->setContentSize(_gameLayer->getBoundingBox().size);
    _touchLayer->setPosition(Vec2::ZERO);
    _touchState = TouchState::FINISH;
    
    Size hudLayerSize = Size(0, 0);
    if (orientation == GamePlayOrientation::PORTRAIT) {
        hudLayerSize.width = _gameLayer->getBoundingBox().origin.x - 2 * margins;
        hudLayerSize.height = _gameLayer->getBoundingBox().size.height;
    } else {
        hudLayerSize.width = _gameLayer->getBoundingBox().size.width;
        hudLayerSize.height = ScreenSizeManager::getHeightFromPercentage(100) - _gameLayer->getBoundingBox().size.height - 3 * margins;
    }
    _hudLayer->setContentSize(hudLayerSize);
    
    if (orientation == GamePlayOrientation::PORTRAIT) {
        _hudLayer->setPosition(margins, this->getBoundingBox().size.height / 2 - _hudLayer->getBoundingBox().size.height / 2);
    } else {
        _hudLayer->setPosition(margins, _gameLayer->getPosition().y + _gameLayer->getBoundingBox().size.height + margins);
    }
    
    
    Size miniLayerSizes = Size(0, 0);
    if (orientation == GamePlayOrientation::PORTRAIT) {
        miniLayerSizes.width = _hudLayer->getBoundingBox().size.width;
        miniLayerSizes.height = _hudLayer->getBoundingBox().size.height / 10;
    } else {
        miniLayerSizes.width = _hudLayer->getBoundingBox().size.width / 4;
        miniLayerSizes.height = _hudLayer->getBoundingBox().size.height;
    }
    
    
    if (orientation == GamePlayOrientation::PORTRAIT) {
        _layerCategory->setPosition(margins / 2, _hudLayer->getBoundingBox().size.height * 0.75);
    } else {
        _layerCategory->setPosition(margins, margins / 2);
    }
    
    _labelTitleCategory->setWidth(miniLayerSizes.width);
    _labelCategory->setWidth(miniLayerSizes.width);
    
    
    miniLayerSizes.height = _labelTitleCategory->getContentSize().height + _labelCategory->getContentSize().height;
    _layerCategory->setContentSize(miniLayerSizes);
    
    _labelTitleCategory->setPosition(0, _layerCategory->getBoundingBox().size.height);
    
    if (orientation == GamePlayOrientation::PORTRAIT) {
        _layerLevel->setPosition(margins / 2, _hudLayer->getBoundingBox().size.height * 0.6);
    } else {
        _layerLevel->setPosition(margins + _hudLayer->getBoundingBox().size.width * 0.25, margins / 2);
    }
    
    
    _labelTitleLevel->setWidth(miniLayerSizes.width);
    _labelLevel->setWidth(miniLayerSizes.width);
    
    
    miniLayerSizes.height = _labelTitleLevel->getContentSize().height + _labelLevel->getContentSize().height;
    _layerLevel->setContentSize(miniLayerSizes);
    
    _labelTitleLevel->setPosition(0, _layerLevel->getBoundingBox().size.height);
    
    if (orientation == GamePlayOrientation::PORTRAIT) {
        _layerPoints->setPosition(margins / 2, _hudLayer->getBoundingBox().size.height * 0.45);
    } else {
        _layerPoints->setPosition(margins + _hudLayer->getBoundingBox().size.width * 0.5, margins / 2);
    }
    
    _labelTitlePoints->setWidth(miniLayerSizes.width);
    _labelPoints->setWidth(miniLayerSizes.width);
    
    miniLayerSizes.height = _labelTitlePoints->getContentSize().height + _labelPoints->getContentSize().height;
    _layerPoints->setContentSize(miniLayerSizes);
    
    _labelTitlePoints->setPosition(0, _layerPoints->getBoundingBox().size.height);
    
    if (orientation == GamePlayOrientation::PORTRAIT) {
        _pauseButton->setPosition(_hudLayer->getBoundingBox().size.width / 2, _hudLayer->getBoundingBox().size.height * 0.10);
    } else {
        _pauseButton->setScale(0.3f);
        _pauseButton->setPosition(_pauseButton->getBoundingBox().size.width / 2 + _hudLayer->getBoundingBox().size.width * 0.85, _hudLayer->getBoundingBox().size.height / 2);
    }
}

void GamePlay::pauseGame(Ref* sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    SceneManager::getInstance()->runSceneWithType(SceneType::PAUSE);
}

// GamePlayDelegate
void GamePlay::loadNextGamePlay()
{
    this->restartGame();
    
    // Aleatory unique order
    if (_loadedIndex.size() >= _subCategories.size() * 0.75) {
        _loadedIndex.clear();
    }
    int subCategoryPosition;
    do {
        int size = (int)_subCategories.size();
        subCategoryPosition = RandomHelper::random_int(0, size - 1);// Son índices de un array
        if (_subCategories.at(subCategoryPosition)->getOptions().size() < minimumItems) {
            subCategoryPosition = -1;
            _loadedIndex.push_back(subCategoryPosition);
        }
    } while (find(_loadedIndex.begin(), _loadedIndex.end(), subCategoryPosition) != _loadedIndex.end() || subCategoryPosition == -1);
    _loadedIndex.push_back(subCategoryPosition);
    
    // Load Game
    int maxItems = maximunItems;
    if (_subCategories.at(subCategoryPosition)->getOptions().size() < maxItems) {
        maxItems = (int)_subCategories.at(subCategoryPosition)->getOptions().size();
    }
    int currentMaxItems = RandomHelper::random_int(minimumItems, maxItems);
    
    vector<int> loadedOptions;
    for (int i = 0; i < currentMaxItems; i++) {
        int optionIndex = -1;
        do {
            optionIndex = RandomHelper::random_int(0, currentMaxItems - 1);// Son índices de un array
        } while (find(loadedOptions.begin(), loadedOptions.end(), optionIndex) != loadedOptions.end() || optionIndex == -1);
        loadedOptions.push_back(optionIndex);
    }
    
    auto orientation = GamePlayOrientation::PORTRAIT;
    if (loadedOptions.size() > MaxVerticalItems) {
        orientation = GamePlayOrientation::LANDSCAPE;
    }
    
    this->setupLevelUI(orientation);
    
    auto margin = _gameLayer->getBoundingBox().size.height * 0.02;
    auto spacing = margin * (loadedOptions.size() + 1);
    float layerMaxSize = _gameLayer->getBoundingBox().size.height;
    if (orientation == GamePlayOrientation::LANDSCAPE) { // Horizontal
        layerMaxSize = _gameLayer->getBoundingBox().size.width;
    }
    
    auto itemSize = (layerMaxSize - spacing) / loadedOptions.size();
    auto itemMaxSpace = layerMaxSize / loadedOptions.size();
    
    std::vector<int> destinations = this->makeDestinations((int)loadedOptions.size());
    
    for (int i = 0; i < loadedOptions.size(); i++) {
        auto option = _subCategories.at(subCategoryPosition)->getOptions().at(loadedOptions.at(i));
//        auto item = Sprite::create(ZipManager::getInstance()->getDataFolderFullPath("hd/options/" + option->getFileName()));
        auto item = FSprite::createFromHdOptions(option->getFileName());
        item->setAnchorPoint(Point::ANCHOR_MIDDLE);
        item->setIgnoreAnchorPointForPosition(false);
        item->setContentSize(Size(itemSize, itemSize));
        
        Vec2 startPosition = Vec2(margin + itemSize / 2, itemMaxSpace * i + itemMaxSpace / 2);
        if (orientation == GamePlayOrientation::LANDSCAPE) { // Horizontal
            startPosition = Vec2(itemMaxSpace * i + itemMaxSpace / 2, margin + itemSize / 2);
        }
        item->setPosition(startPosition);
        _gameLayer->addChild(item);
        
        
        auto result = Sprite::create(ZipManager::getInstance()->getDataFolderFullPath("hd/options/" + option->getFileName()));
        result->setAnchorPoint(item->getAnchorPoint());
        result->setIgnoreAnchorPointForPosition(item->isIgnoreAnchorPointForPosition());
        result->setContentSize(item->getContentSize());
        result->setOpacity(25);
        
        Vec2 endPosition = Vec2(_gameLayer->getBoundingBox().size.width - margin - itemSize / 2, itemMaxSpace * destinations.at(i) + itemMaxSpace / 2);
        if (orientation == GamePlayOrientation::LANDSCAPE) { // Horizontal
            endPosition = Vec2(itemMaxSpace * destinations.at(i) + itemMaxSpace / 2, _gameLayer->getBoundingBox().size.height - margin - itemSize / 2);
        }
        result->setPosition(endPosition);
        _gameLayer->addChild(result);
        
        auto scale = itemSize / item->getBoundingBox().size.width;
        item->setScale(scale);
        
        item->setTag(i);
        _successMovedNodes.push_back(false);
        _loadedNodes.push_back(item);
        _resultNodes.push_back(result);
        _loadedNodesStartPosition.push_back(startPosition);
        _loadedNodesEndPosition.push_back(endPosition);
        _loadedNodesDefaultScale.push_back(scale);
    }
}

std::vector<int> GamePlay::makeDestinations(int totalItems)
{
    std::vector<int> destionations;
    for (int i = 0; i < totalItems; i++) {
        destionations.push_back(i);
    }
    std::random_shuffle(destionations.begin(), destionations.end());
    
    if (destionations.size() > 1) {
        for (int i = 0; i < totalItems; i++) {
            if (destionations.at(i) != i) {
                return destionations;
            }
        }
    }
    return makeDestinations(totalItems);
}

void GamePlay::resumeGamePlay()
{
//    this->resetTimer();
}

void GamePlay::restartGame()
{
    GamePlayPointsManager::getInstance()->resetCurrentMatch();
    this->updateScreenGameStats();
    if (_gameLayer != nullptr) {
        _gameLayer->removeAllChildren();
    }
    _loadedNodes.clear();
    _resultNodes.clear();
    _loadedNodesDefaultScale.clear();
    _loadedNodesStartPosition.clear();
    _loadedNodesEndPosition.clear();
    _successMovedNodes.clear();
    _movingNode = nullptr;
}

void GamePlay::updateScreenGameStats()
{
    Difficulty difficulty = GamePlayPointsManager::getInstance()->getCurrentDifficulty();
    std::string levelString = "";
    switch (difficulty) {
        case Difficulty::EASY:
            levelString = LanguageManager::getLocalizedText("Level", "easy");
            break;
        case Difficulty::MEDIUM:
            levelString = LanguageManager::getLocalizedText("Level", "medium");
            break;
        case Difficulty::HARD:
            levelString = LanguageManager::getLocalizedText("Level", "hard");
            break;
        default:
            break;
    }
    
    _labelLevel->setString(levelString);
    
    float points = GamePlayPointsManager::getInstance()->getCurrentMatchPoints();
    std::ostringstream pointsString;
    pointsString << points;
    _labelPoints->setString(pointsString.str());
    
    int index = GamePlayPointsManager::getInstance()->getCurrentCategory();
    std::string categoryName = _categories.at(index)->getName();
    _labelCategory->setString(categoryName);
}

EventListenerTouchOneByOne *GamePlay::addEvents()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    
    listener->onTouchBegan = [&](cocos2d::Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            return false;
        }
        return GamePlay::touchBegan(touch, event);
    };
    
    listener->onTouchMoved = [=](Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            GamePlay::touchCancelled(touch, event);
            return;
        }
        GamePlay::touchMoved(touch, event);
    };
    
    listener->onTouchEnded = [=](Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            _touchState = TouchState::FINISH;
            return;
        }
        GamePlay::touchEnded(touch, event);
    };
    
    listener->onTouchCancelled = [=](Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            _touchState = TouchState::FINISH;
            return;
        }
        GamePlay::touchCancelled(touch, event);
    };
    
    return listener;
}

void GamePlay::openSuccessScreen()
{
    _totalSuccessScreens++;
    GamePlayPointsManager::getInstance()->saveCurrentMatchPointsToTotal();
    SceneManager::getInstance()->runSceneWithType(SceneType::WIN);
    _pauseButton->setVisible(true);
    _pauseButton->setEnabled(true);
}

bool GamePlay::touchBegan(Touch *touch, Event *pEvent)
{
    if (_touchState != TouchState::FINISH) {
        return false;
    }
    
    Vec2 touchLocation = _gameLayer->convertTouchToNodeSpace(touch);
    
    for (int i = 0; i < _loadedNodes.size(); i++) {
        auto item = _loadedNodes.at(i);
        if (item->getBoundingBox().containsPoint(touchLocation) && _successMovedNodes.at(item->getTag()) == false) {
            _touchState = TouchState::START;
            _movingNode = item;
            item->retain();
            _gameLayer->removeChild(item);
            _touchLayer->addChild(item);
            _gameLayer->reorderChild(_touchLayer, 99);
            item->release();

            _offsetFromCenter = Vec2(touchLocation, item->getPosition());
            item->setScale(_loadedNodesDefaultScale.at(i));
            
            auto scaleToSmaller = ScaleTo::create(0.05f, item->getScale() - 0.02f);
            auto scaleToBigger = ScaleTo::create(0.05f, item->getScale() + 0.1f);
            auto sequece = Sequence::create(scaleToSmaller, scaleToBigger, nullptr);
            item->runAction(sequece);

            return true;
        }
    }
    return false;
}

void GamePlay::touchMoved(Touch *touch, Event *pEvent)
{
    if (_touchState == TouchState::FINISH) {
        return;
    }
    
    Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
    if (_movingNode != nullptr) {
        auto testBoundingBox = _movingNode->getBoundingBox();
        _touchState = TouchState::MOVING;
        
        auto fixedCenter = touchLocation;
        fixedCenter.x += _offsetFromCenter.x;
        fixedCenter.y += _offsetFromCenter.y;
        _movingNode->setPosition(fixedCenter);
        
        float left = fixedCenter.x - testBoundingBox.size.width / 2;
        float right = fixedCenter.x + testBoundingBox.size.width / 2;
        float bottom = fixedCenter.y - testBoundingBox.size.height / 2;
        float top = fixedCenter.y + testBoundingBox.size.height / 2;
        
        if (left < _touchLayer->getBoundingBox().getMinX()) {
            // Left
            fixedCenter.x = testBoundingBox.size.width / 2;
            _movingNode->setPosition(fixedCenter);
        }
        if (bottom < _touchLayer->getBoundingBox().getMinY()) {
            // Bottom
            fixedCenter.y = testBoundingBox.size.height / 2;
            _movingNode->setPosition(fixedCenter);
        }
        if (right > _touchLayer->getBoundingBox().getMaxX()) {
            // Right
            fixedCenter.x = _touchLayer->getBoundingBox().getMaxX() - testBoundingBox.size.height / 2;
            _movingNode->setPosition(fixedCenter);
        }
        if (top > _touchLayer->getBoundingBox().getMaxY()) {
            // Top
            fixedCenter.y = _touchLayer->getBoundingBox().getMaxY() - testBoundingBox.size.height / 2;
            _movingNode->setPosition(fixedCenter);
        }
    }
}

void GamePlay::touchCancelled(Touch *pTouch, Event *pEvent)
{
    touchEnded(pTouch, pEvent);
}

void GamePlay::touchEnded(Touch *touch, Event *pEvent)
{
    if (_touchState == TouchState::FINISH) {
        return;
    }
    
    Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
    if (_movingNode != nullptr) {
        _movingNode->retain();
        _touchLayer->removeChild(_movingNode);
        _gameLayer->addChild(_movingNode);
        _movingNode->release();
        
        auto itemScale = _loadedNodesDefaultScale.at(_movingNode->getTag());
        auto endPosition = _loadedNodesEndPosition.at(_movingNode->getTag());
        if (endPosition.distance(_movingNode->getPosition()) <= allowedItemErrorOffset) {
            // SUCCESS
            GamePlayPointsManager::getInstance()->addSuccessPoints();
            _successMovedNodes.at(_movingNode->getTag()) = true;
            
            auto moveToCenter = MoveTo::create(0.1f, endPosition);
            auto scaleToSmaller = ScaleTo::create(0.1f, itemScale - 0.1f);
            auto scaleToBigger = ScaleTo::create(0.04f, itemScale + 0.02f);
            auto scaleToDefault = ScaleTo::create(0.01f, itemScale);
//            auto sequece = Sequence::create(moveToCenter, scaleToSmaller, scaleToBigger, scaleToDefault, nullptr);
            
            auto center = Spawn::createWithTwoActions(moveToCenter, scaleToSmaller);
            auto scale = Spawn::createWithTwoActions(scaleToBigger, scaleToDefault);
            auto sequece = Sequence::create(center, scale, nullptr);
            
            _movingNode->runAction(sequece);
            
            if(!(std::find(_successMovedNodes.begin(), _successMovedNodes.end(), false) != _successMovedNodes.end())) {
                // Todos los elementos colocados correctamente
                GamePlayPointsManager::getInstance()->setCurrentScreenFinished();
                this->showFinishedMiniGame();
            }
        } else {
            // ERROR
            auto moveToOrigin = MoveTo::create(0.1f, _loadedNodesStartPosition.at(_movingNode->getTag()));
            auto scaleToDefault = ScaleTo::create(0.01f, itemScale);
//            auto sequece = Sequence::create(moveToOrigin, scaleToDefault, nullptr);
//            _movingNode->runAction(sequece);
            
            auto restore = Spawn::createWithTwoActions(moveToOrigin, scaleToDefault);
            _movingNode->runAction(restore);
        }
        _movingNode = nullptr;
        _touchState = TouchState::FINISH;
    }
}

void GamePlay::showFinishedMiniGame()
{
    if (_touchListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);
        _touchListener = nullptr;
    }
    
    _pauseButton->setVisible(false);
    _pauseButton->setEnabled(false);
    
    Rect visibleRect = ScreenSizeManager::getVisibleRect();
//    vector<Node *> nodes = [];
    vector<string> nodeNames = {};
    for (int i = 0; i < _loadedNodes.size(); i++) {
        nodeNames.push_back(_loadedNodes.at(i)->getFileName());
    }
    auto miniGameLayer = MiniGame::create(nodeNames, _loadedNodesDefaultScale);
    miniGameLayer->setTag(333);
    miniGameLayer->setDelegate(this);
//    miniGameLayer->setOpacity(0);
    this->addChild(miniGameLayer);
    
    
//    for (int i = 0; i < _loadedNodes.size(); i++) {
//        
//    }
    
    // 0.25s
    auto fadeToStart = FadeTo::create(0.05f, 25);
    auto fadeToMiddle = FadeTo::create(0.15f, 180);
    auto fadeToEnd = FadeTo::create(0.05f, 235);
    auto sequece = Sequence::create(fadeToStart, fadeToMiddle, fadeToEnd, nullptr);
    miniGameLayer->runAction(sequece);
}

// MiniGameDelegate

void GamePlay::didFinish()
{
    auto miniGameLayer = this->getChildByTag(333);
    this->removeChild(miniGameLayer);
    openSuccessScreen();
}



