//
//  GamePlayPointsManager.h
//  IkasGame
//
//  Created by Sergio Garcia on 16/2/15.
//
//

#ifndef __IkasGame__GamePlayPointsManager__
#define __IkasGame__GamePlayPointsManager__

#include "cocos2d.h"
#include "../Constants/Constants.h"

using namespace cocos2d;

class GamePlayPointsManager
{
public:
    static GamePlayPointsManager* getInstance();
    ~GamePlayPointsManager();
    void loadSettings();
    void saveSettings();
    
    void setCurrentCategory(int category);
    void setCurrentDifficulty(Difficulty difficulty);
    
    // Por partida
    void addSuccessPoints(); // Para cada elemento arrastrado correctamente
    void addMiniGameExplosionPoints(); // Para cada elemento explotada
    void setCurrentScreenFinished(); // Para cada pantalla terminada
    
    void saveCurrentMatchPointsToTotal(); // Suma los puntos de la partida actual al total
    
    // Globales
    CC_SYNTHESIZE_READONLY(int, _totalPoints, TotalPoints);
    CC_SYNTHESIZE_READONLY(int, _totalMatches, TotalMatches);
    CC_SYNTHESIZE_READONLY(int, _totalMatchesPoints, TotalMatchesPoints);
    CC_SYNTHESIZE_READONLY(int, _totalExplosions, TotalExplosions);
    CC_SYNTHESIZE_READONLY(int, _totalExplosionsPoints, TotalExplosionsPoints);
    
    void resetCurrentMatch();
    void resetCurrentGamePlaySettings();
    
    CC_SYNTHESIZE_READONLY(int, _currentCategory, CurrentCategory);
    CC_SYNTHESIZE_READONLY(Difficulty, _currentDifficulty, CurrentDifficulty);
    
    CC_SYNTHESIZE_READONLY(int, _currentPoints, CurrentPoints);// Match & Explosions
    CC_SYNTHESIZE_READONLY(int, _currentMatchPoints, CurrentMatchPoints);
    CC_SYNTHESIZE_READONLY(int, _currentExplosions, CurrentExplosions);
    CC_SYNTHESIZE_READONLY(int, _currentExplosionsPoints, CurrentExplosionsPoints);
    CC_SYNTHESIZE_READONLY(bool, _currentMatchFinished, CurrentMatchFinished);
protected:
    GamePlayPointsManager();
    static GamePlayPointsManager * _instance;
};
#endif /* defined(__IkasGame__GamePlayPointsManager__) */
