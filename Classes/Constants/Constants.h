//
//  Constants.h
//  IkasGame
//
//  Created by Sergio Garcia on 30/01/15.
//
//

#ifndef __IkasCocosGame__Constants
#define __IkasCocosGame__Constants

#include "cocos2d.h"
using namespace cocos2d;

/**
 * Embedded zip name
 */
static std::string DefaultZipName = "pulee-58ac5d57ddfff.zip";

/**
 * URL´s
 */
static std::string ApiURL = "https://ikastek.net/ikastek-klear/apps-data/pulee";
static std::string RankingURL = "";// http://ikastek.net/ikastek-klear/rest/rankings?app=3
static std::string InfoURL = "https://ikastek.net/aplikazioak/umeentzako/pulee/";

/**
 * Custom callbacks
 */
typedef std::function<void(Ref*)> IkasHandler;
typedef std::function<void(Ref*, bool)> IkasResultHandler;

#include "../GameModels/RankingData.h"
typedef std::function<void(bool, std::vector<RankingData*>)> IkasRankingDataHandler;

typedef std::function<void()> IkasEmptyHandler;
typedef std::function<void(double)> IkasValueHandler;

/**
 * Main fonts names
 */
static const std::string MainRegularFont = "fonts/Quicksand-Regular.ttf";
static const std::string MainBoldFont = "fonts/Quicksand-Bold.ttf";
static const std::string MainLightFont = "fonts/Quicksand-Light.ttf";

/**
 * Sound settings filename
 */
static const std::string SoundEnableImage = "sound-on";
static const std::string SoundDisableImage = "sound-off";

/**
 * App colors
 */
static const cocos2d::Color4B IkasYellow = cocos2d::Color4B(249, 220, 92, 255);
static const cocos2d::Color4B IkasBlue = cocos2d::Color4B(126, 166, 255, 255);
static const cocos2d::Color4B IkasWhite = cocos2d::Color4B(255, 255, 255, 255);


/**
 * UI customization
 */
static const float BorderWidth = 10;
static const Color4B BorderColor = IkasBlue;

/**
 * Different scenes used
 */
enum class SceneType
{
	NONE,
    SPLASH,
	MAIN,
    CATEGORIES,
    LEVEL,
    GAMEPLAY,
    PAUSE,
    WIN,
    LOSE,
    RANKING
};

/**
 * Game difficuty options
 */
enum class Difficulty
{
    EMPTY = -1,
    EASY = 0,
    MEDIUM = 1,
    HARD = 2
};

/**
 * Different checkBox types used in Game Settings Scene
 */
enum class CheckBoxType
{
    NONE,
    MUSIC,
    SFX
};

/**
 * Different checkBox types used in Game Settings Scene
 */
enum class CustomLanguage
{
    EUSKARA = 0,
    SPANISH
};

#endif
