//
//  FSprite.m
//  IkasGame
//
//  Created by Sergio Garcia on 8/2/17.
//
//

#include "FSprite.h"
#include "../Singletons/ZipManager.h"

FSprite* FSprite::createFromHdOptions(string fileName)
{
    auto fullFileName = ZipManager::getInstance()->getDataFolderFullPath("hd/options/" + fileName);
    return FSprite::create(fullFileName);
}

FSprite* FSprite::create(string fileName)
{
    FSprite *fSprite = new FSprite();
    if (fSprite && fSprite->initWithFile(fileName)) {
        fSprite->autorelease();
        return fSprite;
    }
    CC_SAFE_DELETE(fSprite);
    return nullptr;
}

std::string FSprite::getFileName()
{
    return _fileName;
}
