//
//  GamePlayPointsManager.cpp
//  IkasGame
//
//  Created by Sergio Garcia on 16/2/15.
//
//

#include "GamePlayPointsManager.h"
#include "AppManager.h"

const int SuccessAnswerPoints = 100;
const int MiniGameExplosionPoints = 50;
const int ScreenFinishedPoints = 50;

// Singleton
GamePlayPointsManager * GamePlayPointsManager::_instance = NULL;

GamePlayPointsManager* GamePlayPointsManager::getInstance()
{
    if(!_instance) _instance = new GamePlayPointsManager();
    return _instance;
}

GamePlayPointsManager::GamePlayPointsManager()
{
    this->loadSettings();
}

GamePlayPointsManager::~GamePlayPointsManager()
{
    this->saveSettings();
}

/**
 * Load ALL settings variables from UserDefault
 */
void GamePlayPointsManager::loadSettings()
{
    UserDefault *userDefault = UserDefault::getInstance();
    
    _currentCategory = userDefault->getIntegerForKey("currentCategory", -1);
    _currentDifficulty = static_cast<Difficulty>(userDefault->getIntegerForKey("currentDifficulty", static_cast<int>(Difficulty::EMPTY)));
    
    _totalPoints = userDefault->getIntegerForKey("totalPoints", 0);
    _totalMatches = userDefault->getIntegerForKey("totalMatches", 0);
    _totalMatchesPoints = userDefault->getIntegerForKey("totalMatchesPoints", 0);
    _totalExplosions = userDefault->getIntegerForKey("totalExplosions", 0);
    _totalExplosionsPoints = userDefault->getIntegerForKey("totalExplosionsPoints", 0);
}

/**
 * Save ALL gameplay settings
 */
void GamePlayPointsManager::saveSettings()
{
    UserDefault *userDefault = UserDefault::getInstance();
    userDefault->setIntegerForKey("currentCategory", _currentCategory);
    userDefault->setIntegerForKey("currentDifficulty", static_cast<int>(_currentDifficulty));
    
    // Global points
    userDefault->setIntegerForKey("totalPoints", _totalPoints);
    userDefault->setIntegerForKey("totalMatches", _totalMatches);
    userDefault->setIntegerForKey("totalMatchesPoints", _totalMatchesPoints);
    userDefault->setIntegerForKey("totalExplosions", _totalExplosions);
    userDefault->setIntegerForKey("totalExplosionsPoints", _totalExplosionsPoints);
    
    userDefault->flush();
    AppManager::getInstance()->updateScreenGameStats();
}

/**
 * Set current gameplay category
 */
void GamePlayPointsManager::setCurrentCategory(int category)
{
    _currentCategory = category;
    AppManager::getInstance()->updateScreenGameStats();
}

/**
 * Set current gameplay difficulty
 */
void GamePlayPointsManager::setCurrentDifficulty(Difficulty difficulty)
{
    _currentDifficulty = difficulty;
    AppManager::getInstance()->updateScreenGameStats();
}

/**
 * Suma los puntos de la partida actual al total
 */
void GamePlayPointsManager::saveCurrentMatchPointsToTotal()
{
    _totalPoints += _currentPoints;
    if (_currentMatchFinished) {
        _totalMatches += 1;
    }
    _totalMatchesPoints += _currentMatchPoints;
    _totalExplosions += _currentExplosions;
    _totalExplosionsPoints += _currentExplosionsPoints;
    saveSettings();
}

/**
 * Add success anwer points to current gameplay
 */
void GamePlayPointsManager::addSuccessPoints()
{
    float levelPoints = 0;
    switch (_currentDifficulty) {
            break;
        case Difficulty::EASY:
            levelPoints = SuccessAnswerPoints * 0.75f;
            break;
        case Difficulty::MEDIUM:
            levelPoints = SuccessAnswerPoints * 1.0f;
            break;
        case Difficulty::HARD:
            levelPoints = SuccessAnswerPoints * 1.25f;
            break;
        default:
            levelPoints = 0;
            break;
    }
    _currentPoints += levelPoints;// All points
    _currentMatchPoints += levelPoints;// Only gameplay points
    AppManager::getInstance()->updateScreenGameStats();
}

/**
 * Puntos por cada elementos explotado
 */
void GamePlayPointsManager::addMiniGameExplosionPoints()
{
    float explosionPoints = 0;
    switch (_currentDifficulty) {
            break;
        case Difficulty::EASY:
            explosionPoints = MiniGameExplosionPoints * 0.75f;
            break;
        case Difficulty::MEDIUM:
            explosionPoints = MiniGameExplosionPoints * 1.0f;
            break;
        case Difficulty::HARD:
            explosionPoints = MiniGameExplosionPoints * 1.25f;
            break;
        default:
            explosionPoints = 0;
            break;
    }
    _currentPoints += explosionPoints;// All points
    _currentExplosions += 1;
    _currentExplosionsPoints += explosionPoints;// Explosions points
    AppManager::getInstance()->updateScreenGameStats();
}

/**
 * Puntos por terminar una pantalla
 */
void GamePlayPointsManager::setCurrentScreenFinished()
{
    _currentMatchFinished = true;
}

/**
 * Reset current match
 */
void GamePlayPointsManager::resetCurrentMatch()
{
    _currentPoints = 0;
    _currentMatchPoints = 0;
    _currentExplosions = 0;
    _currentExplosionsPoints = 0;
    _currentMatchFinished = false;
}

/**
 * Reset ALL gameplay settings
 */
void GamePlayPointsManager::resetCurrentGamePlaySettings()
{
    _currentCategory = -1;
    _currentDifficulty = Difficulty::EMPTY;
}
