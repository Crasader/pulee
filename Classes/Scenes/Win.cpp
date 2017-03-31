//
//  Win.cpp
//  IkasGame
//
//  Created by Sergio Garcia on 11/2/15.
//
//

#include "Win.h"
#include "../Singletons/GameSettingsManager.h"
#include "../Singletons/GamePlayPointsManager.h"
#include "../Singletons/SceneManager.h"
#include "../Singletons/SoundManager.h"
#include "../Singletons/AppManager.h"
#include "../Helpers/LanguageManager.h"
#include "../Helpers/ScreenSizeManager.h"
#include "../Helpers/ImageManager.h"
#include "../CustomGUI/SpriteButton.h"

const float TableViewItemHeightValue = 100;

Scene* Win::createScene()
{
    SceneManager::getInstance()->saveCurrentScene();
    
    auto *scene = Scene::create();
    auto *layer = Win::create();
    layer->setTag(2339);
    
    scene->addChild(layer);
    
    return scene;
}

bool Win::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    Rect visibleRect = ScreenSizeManager::getVisibleRect();
    
    auto whiteLayer = LayerColor::create(IkasWhite);
    whiteLayer->setContentSize(visibleRect.size);
    whiteLayer->setIgnoreAnchorPointForPosition(false);
    whiteLayer->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    whiteLayer->setAnchorPoint(Point::ANCHOR_MIDDLE);
    this->addChild(whiteLayer);
    
    auto background = Sprite::create(ImageManager::getImage("background"), visibleRect);
    background->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    background->setAnchorPoint(Point::ANCHOR_MIDDLE);
    this->addChild(background);
    
    auto buttonNextGamePlay = SpriteButton::create(ImageManager::getImage("play"), 1.0f, CC_CALLBACK_1(Win::loadNextGamePlay, this));
    buttonNextGamePlay->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonNextGamePlay = ScreenSizeManager::getScreenPositionFromPercentage(90, 60);
    positionButtonNextGamePlay.x -= buttonNextGamePlay->getBoundingBox().size.width / 2;
    buttonNextGamePlay->setPosition(positionButtonNextGamePlay);
    this->addChild(buttonNextGamePlay);
    
    auto winTitle = Sprite::create(ImageManager::getImage("win-title"));
    winTitle->setScale(0.75f);
    Vec2 positionWinTitle = ScreenSizeManager::getScreenPositionFromPercentage(50, 75);
//    positionWinTitle.y += buttonNextGamePlay->getBoundingBox().size.height / 2;
//    positionWinTitle.y += ScreenSizeManager::getHeightFromPercentage(2);
    winTitle->setPosition(positionWinTitle);
    winTitle->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    this->addChild(winTitle);
    
    auto labelResume = Label::createWithTTF(LanguageManager::getLocalizedText("Win", "resume"), MainBoldFont, 70);
    labelResume->setAlignment(TextHAlignment::CENTER);
    labelResume->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
    labelResume->setTextColor(IkasBlue);
    Vec2 positionLabelResume = buttonNextGamePlay->getPosition();
    positionLabelResume.y -= buttonNextGamePlay->getBoundingBox().size.height / 2;
    positionLabelResume.y -= ScreenSizeManager::getHeightFromPercentage(1);
    labelResume->setPosition(positionLabelResume);
    this->addChild(labelResume);
    
    auto buttonSoundSettings = SpriteButton::create(ImageManager::getImage(GameSettingsManager::getInstance()->getIsSFXOn() ? SoundEnableImage : SoundDisableImage), 0.30f, CC_CALLBACK_1(Win::switchSoundSettings, this));
    buttonSoundSettings->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonSoundSettings = ScreenSizeManager::getScreenPositionFromPercentage(80, 15);
    buttonSoundSettings->setPosition(positionButtonSoundSettings);
    this->addChild(buttonSoundSettings);
    
    auto buttonHome = SpriteButton::create(ImageManager::getImage("home"), 0.30f, CC_CALLBACK_1(Win::returnHome, this));
    buttonHome->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonHome = ScreenSizeManager::getScreenPositionFromPercentage(22, 15);
    buttonHome->setPosition(positionButtonHome);
    this->addChild(buttonHome);
    
    auto maxHeight = winTitle->getPosition().y - winTitle->getBoundingBox().size.height / 2 - buttonHome->getPosition().y + buttonHome->getBoundingBox().size.height / 2 - ScreenSizeManager::getHeightFromPercentage(10);
    auto maxWidth = buttonNextGamePlay->getPosition().x - buttonNextGamePlay->getBoundingBox().size.width / 2 - ScreenSizeManager::getWidthFromPercentage(15);
    
    auto pointsPosition = Vec2(ScreenSizeManager::getWidthFromPercentage(10), buttonHome->getPosition().y + buttonHome->getBoundingBox().size.height / 2 + ScreenSizeManager::getHeightFromPercentage(5));
    
//    auto layer = LayerColor::create(IkasBlue, maxWidth, maxHeight);
//    layer->setPosition(pointsPosition);
//    this->addChild(layer);
    
    /* TableView */
    _tableViewPoints = TableView::create(this, Size(maxWidth, maxHeight));
    _tableViewPoints->setBounceable(false);
    _tableViewPoints->setColor(Color3B(Color3B(200, 200, 200)));
    _tableViewPoints->setPosition(pointsPosition);
    this->addChild(_tableViewPoints);

    
    return true;
}

void Win::loadNextGamePlay(Ref* sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    SceneManager::getInstance()->loadSavedScene();
    AppManager::getInstance()->loadNextGamePlay();
}

void Win::switchSoundSettings(Ref* sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    GameSettingsManager::getInstance()->switchSFX();
    GameSettingsManager::getInstance()->switchMusic();
    SpriteButton *item = static_cast<SpriteButton*>(sender);
    item->setTexture(ImageManager::getImage(GameSettingsManager::getInstance()->getIsSFXOn() ? SoundEnableImage : SoundDisableImage));
}

void Win::returnHome(Ref* sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    AppManager::getInstance()->setGamePlayDelegate(NULL);
    SceneManager::getInstance()->runSceneWithType(SceneType::MAIN);
    SceneManager::getInstance()->removeSavedScene();
}

/*TableViewDataSoruce */
ssize_t Win::numberOfCellsInTableView(TableView *tableView) {
    return 4;
}

Size Win::tableCellSizeForIndex(TableView *tableView, ssize_t idx) {
    return Size(tableView->getBoundingBox().size.width, TableViewItemHeightValue);
}

TableViewCell* Win::tableCellAtIndex(TableView *tableView, ssize_t idx) {
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    TableViewCell *cell = tableView->dequeueCell();
    
    
    LayerColor *colorLayer;
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
    }
    cell->removeAllChildren();
    
    colorLayer = LayerColor::create();
    colorLayer->setAnchorPoint(Point::ZERO);
    colorLayer->setContentSize(Size(tableView->getBoundingBox().size.width, TableViewItemHeightValue));
    colorLayer->setPosition(Vec2(0 * tableView->getBoundingBox().size.width, 0));
    cell->addChild(colorLayer);
    
    
    string item, value;
    ostringstream oss;
    
    switch ((int)idx) {
        case 0:
            item = LanguageManager::getLocalizedText("Points", "total-points");
            oss << GamePlayPointsManager::getInstance()->getCurrentPoints();
            value = oss.str();
            break;
        case 1:
            item = LanguageManager::getLocalizedText("Points", "match-points");
            oss << GamePlayPointsManager::getInstance()->getCurrentMatchPoints();
            value = oss.str();
            break;
        case 2:
            item = LanguageManager::getLocalizedText("Points", "total-explosions");
            oss << GamePlayPointsManager::getInstance()->getCurrentExplosions();
            value = oss.str();
            break;
        case 3:
            item = LanguageManager::getLocalizedText("Points", "points-in-explosions");
            oss << GamePlayPointsManager::getInstance()->getCurrentExplosionsPoints();
            value = oss.str();
            break;
        default:
            item = "";
            value = "";
            break;
    }
    
    auto name = Label::createWithTTF(item, MainBoldFont, 60);
    name->setContentSize(Size(tableView->getBoundingBox().size.width * 0.75 - 30, TableViewItemHeightValue));
    name->setWidth(tableView->getBoundingBox().size.width * 0.75 - 30);
    name->setHeight(TableViewItemHeightValue);
    name->setVerticalAlignment(TextVAlignment::CENTER);
    name->setHorizontalAlignment(TextHAlignment::LEFT);
    name->setAnchorPoint(Point::ZERO);
    name->setTextColor(IkasBlue);
    name->setPosition(Vec2(30, 0));
    cell->addChild(name);
    
    auto pointsLabel = Label::createWithTTF(value, MainBoldFont, 60);
    pointsLabel->setContentSize(Size(tableView->getBoundingBox().size.width * 0.25, TableViewItemHeightValue));
    pointsLabel->setHeight(TableViewItemHeightValue);
    pointsLabel->setWidth(tableView->getBoundingBox().size.width * 0.25);
    pointsLabel->setVerticalAlignment(TextVAlignment::CENTER);
    pointsLabel->setHorizontalAlignment(TextHAlignment::CENTER);
    pointsLabel->setAnchorPoint(Point::ZERO);
    pointsLabel->setTextColor(IkasBlue);
    pointsLabel->setPosition(Vec2(tableView->getBoundingBox().size.width * 0.75, 0));
    cell->addChild(pointsLabel);
    
    if (idx % 2 == 0) {
        colorLayer->setColor(Color3B(IkasYellow));
        colorLayer->setOpacity(255);
    } else {
        colorLayer->setColor(Color3B(IkasWhite));
        colorLayer->setOpacity(0);
    }
    return cell;
}

