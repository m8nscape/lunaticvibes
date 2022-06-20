#pragma once
#include "scene.h"

class SceneCustomize : public vScene
{
private:
    bool _exiting = false;

public:
    SceneCustomize();
    virtual ~SceneCustomize() = default;

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

    std::map<eMode, std::vector<Path>> skinList;

    eMode selectedMode;

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
    void load(eMode mode);
    void save(eMode mode) const;
    void updateTexts() const;


protected:
    void inputGamePress(InputMask&, const Time&);
};