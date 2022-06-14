#include "scene_Customize.h"
#include "scene_context.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"
#include "game/skin/skin_mgr.h"
#include "game/skin/skin_lr2.h"

SceneCustomize::SceneCustomize() : vScene(eMode::THEME_SELECT, 240)
{
    _updateCallback = std::bind(&SceneCustomize::updateStart, this);

    gCustomizeContext.skinDir = 0;
    gCustomizeContext.optionUpdate = 0;

    // topest entry is PLAY7
    selectedMode = eMode::PLAY7;
    gCustomizeContext.mode = selectedMode;
    load(selectedMode);

    loopStart();
    _input.loopStart();
    LOG_DEBUG << "[Customize] Start";
}


void SceneCustomize::_updateAsync()
{
    _updateCallback();
}

void SceneCustomize::updateStart()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);
    if (rt.norm() > _skin->info.timeIntro)
    {
        _updateCallback = std::bind(&SceneCustomize::updateMain, this);
        using namespace std::placeholders;
        _input.register_p("SCENE_PRESS", std::bind(&SceneCustomize::inputGamePress, this, _1, _2));
        LOG_DEBUG << "[Customize] State changed to Main";
    }
}

void SceneCustomize::updateMain()
{
    Time t;
    if (gCustomizeContext.mode != selectedMode)
    {
        eMode modeOld = selectedMode;
        selectedMode = gCustomizeContext.mode;
        save(modeOld);
        SkinMgr::unload(modeOld);
        load(selectedMode);
    }
    if (gCustomizeContext.skinDir != 0)
    {
        // TODO change skin
    }
    if (gCustomizeContext.optionUpdate)
    {
        gCustomizeContext.optionUpdate = false;

        size_t idxOption = topOptionIndex + gCustomizeContext.optionIdx;
        if (idxOption < optionsKeyList.size())
        {
            Option& op = optionsMap[optionsKeyList[idxOption]];
            size_t idxEntry = op.selectedEntry;
            if (gCustomizeContext.optionDir > 0)
            {
                if (idxEntry + 1 >= op.entries.size())
                    idxEntry = 0;
                else
                    idxEntry++;
            }
            else
            {
                if (idxEntry == 0 && gCustomizeContext.optionDir < 0)
                    idxEntry = !op.entries.empty() ? (op.entries.size() - 1) : 0;
                else
                    idxEntry--;
            }
            if (idxEntry != op.selectedEntry)
            {
                setOption(idxOption, idxEntry);
            }
        }
    }
    if (_exiting)
    {
        save(selectedMode);

        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        _updateCallback = std::bind(&SceneCustomize::updateFadeout, this);
        using namespace std::placeholders;
        _input.unregister_p("SCENE_PRESS");
        LOG_DEBUG << "[Customize] State changed to Fadeout";
    }
}

void SceneCustomize::updateFadeout()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (rt.norm() > _skin->info.timeOutro)
    {
        loopEnd();
        _input.loopEnd();
        SkinMgr::unload(selectedMode);
        gNextScene = eScene::SELECT;
    }
}

////////////////////////////////////////////////////////////////////////////////

StringPath SceneCustomize::getConfigFileName(StringPathView skinPath) 
{
    Path p(skinPath);
    std::string md5 = HashMD5(p.string()).hexdigest();
    md5 += ".yaml";
    return Path(md5).native();
}


void SceneCustomize::setOption(size_t idxOption, size_t idxEntry)
{
    switch (_skin->type())
    {
    case eSkinType::LR2:
    {
        assert(idxOption < optionsKeyList.size());
        Option& op = optionsMap[optionsKeyList[idxOption]];
        if (op.id != 0)
        {
            for (size_t i = 0; i < op.entries.size(); ++i)
            {
                setCustomDstOpt(op.id, i, false);
            }
            setCustomDstOpt(op.id, idxEntry, true);
        }
        else
        {
            // save to file when exit
        }
        op.selectedEntry = idxEntry;
        updateTexts();
        break;
    }

    default:
        break;
    }
}


void SceneCustomize::load(eMode mode)
{
    SkinMgr::unload(mode);
    pSkin ps = SkinMgr::get(mode);
    Path pCustomize = ConfigMgr::Profile()->getPath() / "customize" / getConfigFileName(ps->getFilePath());
    optionsMap.clear();
    optionsKeyList.clear();

    gTexts.set(eText::SKIN_NAME, ps->getName());
    gTexts.set(eText::SKIN_MAKER_NAME, ps->getMaker());

    // load names
    size_t count = ps->getCustomizeOptionCount();
    for (size_t i = 0; i < count; ++i)
    {
        vSkin::CustomizeOption opSkin = ps->getCustomizeOptionInfo(i);
        Option op;
        op.displayName = opSkin.displayName;
        op.selectedEntry = opSkin.defaultEntry;
        op.id = opSkin.id;
        op.entries = opSkin.entries;
        optionsMap[opSkin.internalName] = op;
        optionsKeyList.push_back(opSkin.internalName);
    }

    // load config from file
    try
    {
        for (const auto& node : YAML::LoadFile(pCustomize.string()))
        {
            if (auto itOp = optionsMap.find(node.first.as<std::string>()); itOp != optionsMap.end())
            {
                Option& op = itOp->second;
                auto selectedEntryName = node.second.as<std::string>();
                if (const auto itEntry = std::find(op.entries.begin(), op.entries.end(), selectedEntryName); itEntry != op.entries.end())
                {
                    op.selectedEntry = std::distance(op.entries.begin(), itEntry);
                }
            }
        }
    }
    catch (YAML::BadFile&)
    {
        LOG_WARNING << "[Customize] Bad file: " << pCustomize.string();
    }
    updateTexts();
}

void SceneCustomize::save(eMode mode) const
{
    pSkin ps = SkinMgr::get(mode);
    Path pCustomize = ConfigMgr::Profile()->getPath() / "customize";
    fs::create_directories(pCustomize);
    pCustomize /= getConfigFileName(ps->getFilePath());

    YAML::Node yaml;
    for (const auto& itOp: optionsMap)
    {
        auto& [tag, op] = itOp;
        yaml[tag] = op.entries[op.selectedEntry];
    }

    std::ofstream fout(pCustomize, std::ios_base::trunc);
    fout << yaml;
    fout.close();
}

void SceneCustomize::updateTexts() const
{
    for (size_t i = 0; i < 10; ++i)
    {
        eText optionNameId = eText(size_t(eText::スキンカスタマイズカテゴリ名1個目) + i);
        eText entryNameId = eText(size_t(eText::スキンカスタマイズ項目名1個目) + i);
        size_t idx = topOptionIndex + i;
        if (idx < optionsKeyList.size())
        {
            const Option& op = optionsMap.at(optionsKeyList[idx]);
            gTexts.queue(optionNameId, op.displayName);
            gTexts.queue(entryNameId, op.entries[op.selectedEntry]);
        }
        else
        {
            gTexts.queue(optionNameId, "");
            gTexts.queue(entryNameId, "");
        }
    }
    gTexts.flush();
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneCustomize::inputGamePress(InputMask& m, const Time& t)
{
    if (m[Input::Pad::ESC]) _exiting = true;
    if (m[Input::Pad::M2]) _exiting = true;

    if (m[Input::Pad::MWHEELUP] && topOptionIndex > 0)
    {
        topOptionIndex--;
        if (!optionsMap.empty())
        {
            gSliders.set(eSlider::SKIN_CONFIG_OPTIONS, double(topOptionIndex) / (optionsMap.size() - 1));
        }
        updateTexts();
    }

    if (m[Input::Pad::MWHEELDOWN] && topOptionIndex + 1 < optionsMap.size())
    {
        topOptionIndex++;
        if (!optionsMap.empty())
        {
            gSliders.set(eSlider::SKIN_CONFIG_OPTIONS, double(topOptionIndex) / (optionsMap.size() - 1));
        }
        updateTexts();
    }
}
