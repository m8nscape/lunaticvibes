#include "skin.h"
#include "data.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

vSkin::vSkin()
{
    sf::Image err;
    err.loadFromFile(defs::file::errorTextureImage);
    vecImage.push_back(std::move(err));

    imgBlackDot.create(1, 1, sf::Color::Black);
    imgWhiteDot.create(1, 1, sf::Color::White);
    texStageFile.create(640, 480);
    texBackBmp.create(256, 256);
    texBanner.create(300, 80);
    texSkinThumbnail.create(640, 480);
    texBlackDot.loadFromImage(imgBlackDot);
    texWhiteDot.loadFromImage(imgWhiteDot);
}

vSkin::vSkin(path path, bool extended) : vSkin()
{
    init(path, extended);
}

void vSkin::init(path path, bool extended)
{
    if (!extended)
        loadLR2Skin(path);
    // TODO Extended skin format ?

    //data().setTimer(timer::INPUT_START, timeIntro);
    freeImages();
}


size_t vSkin::getImageCount()
{
    return imagePath.size();
}

void vSkin::loadImages()
{
    for (auto& path : imagePath)
        if (!path.empty())
        {
            if (path != "CONTINUE")
            {
                sf::Image img;
                if (img.loadFromFile(std::string(path.string())))
                {
                    vecImage.push_back(std::move(img));
                }
                else
                {
                    LOG_WARNING << "[Skin] Image loading failed: " << path;
                    vecImage.push_back(vecImage[ERROR_IMAGE]);
                }
            }
            else
            {
                // TODO CONTINUE
            }
        }
        else
        {
            vecImage.emplace_back();
        }
}

void vSkin::freeImages()
{
    vecImage.clear();
}

void vSkin::convertImageToTexture()
{
    for (auto& img : vecImage)
    {
        vecTexture.emplace_back();
        vecTexture.back().loadFromImage(img);
    }
}

void vSkin::createSprite(element& e)
{
    if (e.empty()) return;

    if (e.getTextureIdx() > vecTexture.size())
    {
        switch (e.getTextureIdx())
        {
        case STAGEFILE:
            e.setTexture(texStageFile);
            break;
        case BACKBMP:
            e.setTexture(texBackBmp);
            break;
        case BANNER:
            e.setTexture(texBanner);
            break;
        case SKIN_THUMBNAIL:
            e.setTexture(texSkinThumbnail);
            break;
        case BLACK_DOT:
            e.setTexture(texBlackDot);
            break;
        case WHITE_DOT:
            e.setTexture(texWhiteDot);
            break;
        default:
            LOG_WARNING << "[Skin] Image index undefined: " << e.getTextureIdx();
            e.setTexture(vecTexture[ERROR_TEXTURE]);
        }
    }
    else
    {
        e.setTexture(vecTexture[e.getTextureIdx()]);
    }

    e.createSprite();
}

void vSkin::updateSprite(element& e, long long time) const
{
    e.update(e.getRTime(time));
}

void vSkin::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    long long rTime = data().getTimeFromStart();
    for (auto& e : elements)
    {
        if (e)
        {
            updateSprite(*e, rTime);
            e->draw(target, states);
        }
    }
}
