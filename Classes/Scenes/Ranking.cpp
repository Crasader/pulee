//
//  Ranking.cpp
//  IkasIkusi
//
//  Created by Asier Cortes on 23/4/15.
//
//

#include "Ranking.h"
#include "../Singletons/GameSettingsManager.h"
#include "../Singletons/GamePlayPointsManager.h"
#include "../Singletons/SceneManager.h"
#include "../Singletons/SoundManager.h"
#include "../Singletons/AppManager.h"
#include "../Helpers/LanguageManager.h"
#include "../Helpers/ScreenSizeManager.h"
#include "../Helpers/ImageManager.h"
#include "../Helpers/DataFileManager.h"
#include "../CustomGUI/SpriteButton.h"
#include <string>

const float TableViewItemHeightValue = 150;

const float TableViewWidthPercentage = 80;
const float TableViewHeightPercentage = 70;
const float TableViewXPositionPercentage = 50;
const float TableViewYPositionPercentage = 50;

enum class TablewViewPageDirection
{
    PREVIOUS,
    NEXT
};



Scene* Ranking::createScene()
{
    auto *scene = Scene::create();
    
    auto *layer = Ranking::create();
    layer->setTag(2339);
    
    scene->addChild(layer);
    
    return scene;
}

bool Ranking::init()
{
    if (!Layer::init()) {
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
    
    buttonsLayer = Layer::create();
    buttonsLayer->setContentSize(visibleRect.size);
    buttonsLayer->setPosition(0, 0);
    
    auto buttonBack = SpriteButton::create(ImageManager::getImage("back"), 0.30f, CC_CALLBACK_1(Ranking::changeScene, this));
    buttonBack->setTag(static_cast<int>(SceneType::MAIN));
    buttonBack->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonBack = ScreenSizeManager::getScreenPositionFromPercentage(5, 5);
    positionButtonBack.x += buttonBack->getBoundingBox().size.width / 2;
    positionButtonBack.y += buttonBack->getBoundingBox().size.height / 2;
    buttonBack->setPosition(positionButtonBack);
    buttonsLayer->addChild(buttonBack);
    
    auto buttonAbout = SpriteButton::create(ImageManager::getImage("info"), 0.30f, CC_CALLBACK_1(Ranking::openInfo, this));
    buttonAbout->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonAbout = ScreenSizeManager::getScreenPositionFromPercentage(95, 5);
    positionButtonAbout.x -= buttonAbout->getBoundingBox().size.width / 2;
    positionButtonAbout.y += buttonAbout->getBoundingBox().size.height / 2;
    buttonAbout->setPosition(positionButtonAbout);
    buttonsLayer->addChild(buttonAbout);
    
    auto buttonSFXSettings = SpriteButton::create(ImageManager::getImage(GameSettingsManager::getInstance()->getIsSFXOn() ? SoundEnableImage : SoundDisableImage), 0.30f, CC_CALLBACK_1(Ranking::switchSoundSettings, this));
    buttonSFXSettings->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonSFXSettings = buttonAbout->getPosition();
    positionButtonSFXSettings.x -= ScreenSizeManager::getWidthFromPercentage(5);
    positionButtonSFXSettings.x -= buttonSFXSettings->getBoundingBox().size.width;
    buttonSFXSettings->setPosition(positionButtonSFXSettings);
    buttonsLayer->addChild(buttonSFXSettings);
    
    this->addChild(buttonsLayer);
    
    /* TableView */
    _tableViewRanking = TableView::create(this, Size(ScreenSizeManager::getWidthFromPercentage(TableViewWidthPercentage), ScreenSizeManager::getHeightFromPercentage(TableViewHeightPercentage)));
    _tableViewRanking->setBounceable(false);
    _tableViewRanking->setColor(Color3B(Color3B(200, 200, 200)));
    Vec2 positionTablewView = ScreenSizeManager::getScreenPositionFromPercentage(TableViewXPositionPercentage, TableViewYPositionPercentage);
    positionTablewView.x -= _tableViewRanking->getBoundingBox().size.width/2;
    positionTablewView.y -= _tableViewRanking->getBoundingBox().size.height/2;
    _tableViewRanking->setPosition(positionTablewView);
    this->addChild(_tableViewRanking);
    
    auto labelPointsTitle = Label::createWithTTF(LanguageManager::getLocalizedText("Ranking", "points"), MainBoldFont, 80);
    labelPointsTitle->setAnchorPoint(Point::ANCHOR_MIDDLE);
    labelPointsTitle->setPosition(Vec2(ScreenSizeManager::getWidthFromPercentage(50), _tableViewRanking->getPositionY() + (_tableViewRanking->getBoundingBox().size.height) * 1.1f));
    labelPointsTitle->setTextColor(IkasBlue);
    this->addChild(labelPointsTitle);
    
    return true;
}

/*TableViewDataSoruce */
ssize_t Ranking::numberOfCellsInTableView(TableView *tableView) {
    return 5;
}

Size Ranking::tableCellSizeForIndex(TableView *tableView, ssize_t idx) {
    return Size(tableView->getBoundingBox().size.width, TableViewItemHeightValue);
}

TableViewCell* Ranking::tableCellAtIndex(TableView *tableView, ssize_t idx) {
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
            oss << GamePlayPointsManager::getInstance()->getTotalPoints();
            value = oss.str();
            break;
        case 1:
            item = LanguageManager::getLocalizedText("Points", "total-matches");
            oss << GamePlayPointsManager::getInstance()->getTotalMatches();
            value = oss.str();
            break;
        case 2:
            item = LanguageManager::getLocalizedText("Points", "points-in-matches");
            oss << GamePlayPointsManager::getInstance()->getTotalMatchesPoints();
            value = oss.str();
            break;
        case 3:
            item = LanguageManager::getLocalizedText("Points", "total-explosions");
            oss << GamePlayPointsManager::getInstance()->getTotalExplosions();
            value = oss.str();
            break;
        case 4:
            item = LanguageManager::getLocalizedText("Points", "points-in-explosions");
            oss << GamePlayPointsManager::getInstance()->getTotalExplosionsPoints();
            value = oss.str();
            break;
        default:
            item = "";
            value = "";
            break;
    }
    
    auto name = Label::createWithTTF(item, MainBoldFont, 70);
    name->setContentSize(Size((tableView->getBoundingBox().size.width / 2) - 30, TableViewItemHeightValue));
    name->setWidth(tableView->getBoundingBox().size.width / 2);
    name->setHeight(TableViewItemHeightValue);
    name->setVerticalAlignment(TextVAlignment::CENTER);
    name->setHorizontalAlignment(TextHAlignment::LEFT);
    name->setAnchorPoint(Point::ZERO);
    name->setTextColor(IkasBlue);
    name->setPosition(Vec2(30, 0));
    cell->addChild(name);

    auto pointsLabel = Label::createWithTTF(value, MainBoldFont, 70);
    pointsLabel->setContentSize(Size(tableView->getBoundingBox().size.width / 2, TableViewItemHeightValue));
    pointsLabel->setHeight(TableViewItemHeightValue);
    pointsLabel->setWidth(tableView->getBoundingBox().size.width / 2);
    pointsLabel->setVerticalAlignment(TextVAlignment::CENTER);
    pointsLabel->setHorizontalAlignment(TextHAlignment::CENTER);
    pointsLabel->setAnchorPoint(Point::ZERO);
    pointsLabel->setTextColor(IkasBlue);
    pointsLabel->setPosition(Vec2(tableView->getBoundingBox().size.width / 2, 0));
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

void Ranking::switchSoundSettings(Ref* sender) {
    SoundManager::getInstance()->sfxPlay("button");
    GameSettingsManager::getInstance()->switchSFX();
    GameSettingsManager::getInstance()->switchMusic();
    SpriteButton *item = static_cast<SpriteButton*>(sender);
    item->setTexture(ImageManager::getImage(GameSettingsManager::getInstance()->getIsSFXOn() ? SoundEnableImage : SoundDisableImage));
}

void Ranking::changeScene(Ref* sender) {
    SoundManager::getInstance()->sfxPlay("button");
    SpriteButton *item = static_cast<SpriteButton*>(sender);
    SceneManager::getInstance()->runSceneWithType(static_cast<SceneType>(item->getTag()));
}

void Ranking::openInfo(Ref* sender) {
    SoundManager::getInstance()->sfxPlay("button");
    Application::getInstance()->openURL(InfoURL);
}


