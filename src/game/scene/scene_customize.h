#pragma once
#include "scene.h"

class SceneCustomize : public SceneBase
{
private:
    bool exiting = false;
    std::shared_ptr<Texture> graphics_get_screen_texture;

public:
    SceneCustomize();
    virtual ~SceneCustomize();

protected:
    struct Option
    {
        int id;
        StringContent displayName;
        std::vector<StringContent> entries;
        size_t selectedEntry;
    };
    size_t topOptionIndex = 0;
    std::map<StringContent, Option> optionsMap;
    std::vector<StringContent> optionsKeyList;

    std::map<SkinType, std::vector<Path>> skinList;
    std::vector<Path> soundsetList;

    SkinType selectedMode;

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    std::function<void()> _updateCallback;
    void updateStart();
    void updateMain();
    void updateFadeout();

public:
    static StringPath getConfigFileName(StringPathView skinPath);

protected:
    void setOption(size_t idxOption, size_t idxEntry);
    void load(SkinType mode);
    void save(SkinType mode) const;
    void updateTexts() const;

protected:
    void inputGamePress(InputMask&, const Time&);

protected:
    std::shared_ptr<SceneBase> pSubScene = nullptr;
public:
    virtual void draw() const override;
};