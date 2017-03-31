//
//  Win.h
//  IkasGame
//
//  Created by Sergio Garcia on 11/2/15.
//
//

#ifndef __IkasGame__Win__
#define __IkasGame__Win__

#include "../Constants/Constants.h"

#include "cocos2d.h"
using namespace cocos2d;

#include "../../cocos2d/extensions/GUI/CCScrollView/CCTableView.h"
using namespace cocos2d::extension;

class Win : public Layer, TableViewDataSource
{
public:
    virtual bool init();
    static Scene* createScene();
    CREATE_FUNC(Win);
    
protected:
    void loadNextGamePlay(Ref* sender);
    void switchSoundSettings(Ref* sender);
    void returnHome(Ref* sender);
    
private:
    TableView *_tableViewPoints;
    
    virtual Size tableCellSizeForIndex(TableView *tableView, ssize_t idx);
    virtual TableViewCell* tableCellAtIndex(TableView *tableView, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView *tableView);
};
#endif /* defined(__IkasGame__Win__) */
