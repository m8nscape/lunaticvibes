#pragma once

namespace lv::data
{

inline struct Struct_LR2Customize
{
    std::string currentSkinName;
    std::string currentSkinMakerName;

    struct Option
    {
        int id;
        StringContent displayName;
        std::vector<StringContent> entries;
        size_t selectedEntry;
    };
    size_t topOptionIndex = 0;
    std::map<StringContent, std::shared_ptr<Option>> optionsMap;
    std::vector<StringContent> optionsKeyList;

} LR2CustomizeData;

}