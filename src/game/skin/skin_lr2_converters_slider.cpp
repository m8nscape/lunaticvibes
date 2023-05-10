#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/data/data_types.h"
#include "game/arena/arena_data.h"

namespace lv
{

using namespace data;

class SliderConverter
{
public:
    static Ratio slider_0() { return 0.0; }
    static Ratio slider_1()
    {
        if (!SelectData.entries.empty())
            return SelectData.selectedEntryIndexRolling / SelectData.entries.size();
        return 0.0;
    }
    static Ratio slider_2() { return PlayData.player[PLAYER_SLOT_PLAYER].hispeed / 10.0; }
    static Ratio slider_3() { return PlayData.player[PLAYER_SLOT_TARGET].lanecoverTop / 1000.0; }
    static Ratio slider_4() { return PlayData.player[PLAYER_SLOT_PLAYER].hispeed / 10.0; }
    static Ratio slider_5() { return PlayData.player[PLAYER_SLOT_TARGET].lanecoverTop / 1000.0; }
    static Ratio slider_6()
    {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c && c->getTotalLength() != 0 && SystemData.gNextScene == SceneType::PLAY && PlayData.playStarted)
        {
            auto t = Time().norm() - PlayData.timers["play_start"];
            return double(t) / c->getTotalLength().norm();
        }
        return 0.0;
    }
    static Ratio slider_7()
    {
        if (!LR2CustomizeData.optionsMap.empty())
            return double(LR2CustomizeData.topOptionIndex) / (LR2CustomizeData.optionsMap.size() - 1);
        return 0.0;
    }
    static Ratio slider_10() { return (SystemData.equalizerVal62_5hz + 12) / 24.0; }
    static Ratio slider_11() { return (SystemData.equalizerVal160hz + 12) / 24.0; }
    static Ratio slider_12() { return (SystemData.equalizerVal400hz + 12) / 24.0; }
    static Ratio slider_13() { return (SystemData.equalizerVal1khz + 12) / 24.0; }
    static Ratio slider_14() { return (SystemData.equalizerVal2_5khz + 12) / 24.0; }
    static Ratio slider_15() { return (SystemData.equalizerVal6_25khz + 12) / 24.0; }
    static Ratio slider_16() { return (SystemData.equalizerVal16khz + 12) / 24.0; }
    static Ratio slider_17() { return SystemData.volumeMaster; }
    static Ratio slider_18() { return SystemData.volumeKey; }
    static Ratio slider_19() { return SystemData.volumeBgm; }
    static Ratio slider_20() { return SystemData.fxVal; }
    static Ratio slider_26() { return (SystemData.freqVal + 12) / 24.0; }

    static Ratio slider_31()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K11];
        }
        return 0.0;
    }
    static Ratio slider_32()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K12];
        }
        return 0.0;
    }
    static Ratio slider_33()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K13];
        }
        return 0.0;
    }
    static Ratio slider_34()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K14];
        }
        return 0.0;
    }
    static Ratio slider_35()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K15];
        }
        return 0.0;
    }
    static Ratio slider_36()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K16];
        }
        return 0.0;
    }
    static Ratio slider_37()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K17];
        }
        return 0.0;
    }
    static Ratio slider_38()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K18];
        }
        return 0.0;
    }
    static Ratio slider_39()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K19];
        }
        return 0.0;
    }
    static Ratio slider_40()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K1START];
        }
        return 0.0;
    }
    static Ratio slider_41()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K1SELECT];
        }
        return 0.0;
    }
    static Ratio slider_42()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::S1L];
        }
        return 0.0;
    }
    static Ratio slider_43()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::S1R];
        }
        return 0.0;
    }
    static Ratio slider_44()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.scratchAxisSpeed[0];
        }
        return 0.0;
    }
    static Ratio slider_51()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K21];
        }
        return 0.0;
    }
    static Ratio slider_52()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K22];
        }
        return 0.0;
    }
    static Ratio slider_53()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K23];
        }
        return 0.0;
    }
    static Ratio slider_54()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K24];
        }
        return 0.0;
    }
    static Ratio slider_55()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K25];
        }
        return 0.0;
    }
    static Ratio slider_56()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K26];
        }
        return 0.0;
    }
    static Ratio slider_57()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K27];
        }
        return 0.0;
    }
    static Ratio slider_58()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K28];
        }
        return 0.0;
    }
    static Ratio slider_59()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K29];
        }
        return 0.0;
    }
    static Ratio slider_60()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K2START];
        }
        return 0.0;
    }
    static Ratio slider_61()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::K2SELECT];
        }
        return 0.0;
    }
    static Ratio slider_62()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::S2L];
        }
        return 0.0;
    }
    static Ratio slider_63()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.deadzone[k::S2R];
        }
        return 0.0;
    }
    static Ratio slider_64()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.scratchAxisSpeed[1];
        }
        return 0.0;
    }
};

#define define_has_member(index)                                                            \
class has_slider_##index                                                                  \
{                                                                                           \
private:                                                                                    \
    typedef long yes_type;                                                                  \
    typedef char no_type;                                                                   \
    template <typename U> static yes_type test(decltype(&U::slider_##index));             \
    template <typename U> static no_type  test(...);                                        \
public:                                                                                     \
    static constexpr bool has_func = sizeof(test<SliderConverter>()) == sizeof(yes_type); \
private:                                                                                    \
    template <typename U, typename = std::enable_if_t<!has_func>>                           \
    static constexpr Ratio(*func())() { return &U::slider_0; }                            \
    template <typename U, typename = std::enable_if_t<has_func>>                            \
    static constexpr Ratio(*func())() { return &U::slider_##index; }                      \
public:                                                                                     \
    static constexpr Ratio(*value)() = func<SliderConverter>();                           \
}

#define has_slider(index)  has_slider_##index::has_func
#define slider(index) has_slider_##index::value

#pragma region define_has_member

define_has_member(0);
define_has_member(1);
define_has_member(2);
define_has_member(3);
define_has_member(4);
define_has_member(5);
define_has_member(6);
define_has_member(7);
define_has_member(8);
define_has_member(9);
define_has_member(10);
define_has_member(11);
define_has_member(12);
define_has_member(13);
define_has_member(14);
define_has_member(15);
define_has_member(16);
define_has_member(17);
define_has_member(18);
define_has_member(19);
define_has_member(20);
define_has_member(21);
define_has_member(22);
define_has_member(23);
define_has_member(24);
define_has_member(25);
define_has_member(26);
define_has_member(27);
define_has_member(28);
define_has_member(29);
define_has_member(30);
define_has_member(31);
define_has_member(32);
define_has_member(33);
define_has_member(34);
define_has_member(35);
define_has_member(36);
define_has_member(37);
define_has_member(38);
define_has_member(39);
define_has_member(40);
define_has_member(41);
define_has_member(42);
define_has_member(43);
define_has_member(44);
define_has_member(45);
define_has_member(46);
define_has_member(47);
define_has_member(48);
define_has_member(49);
define_has_member(50);
define_has_member(51);
define_has_member(52);
define_has_member(53);
define_has_member(54);
define_has_member(55);
define_has_member(56);
define_has_member(57);
define_has_member(58);
define_has_member(59);
define_has_member(60);
define_has_member(61);
define_has_member(62);
define_has_member(63);
define_has_member(64);
define_has_member(65);
define_has_member(66);
define_has_member(67);
define_has_member(68);
define_has_member(69);
define_has_member(70);
define_has_member(71);
define_has_member(72);
define_has_member(73);
define_has_member(74);
define_has_member(75);
define_has_member(76);
define_has_member(77);
define_has_member(78);
define_has_member(79);
define_has_member(80);
define_has_member(81);
define_has_member(82);
define_has_member(83);
define_has_member(84);
define_has_member(85);
define_has_member(86);
define_has_member(87);
define_has_member(88);
define_has_member(89);
define_has_member(90);
define_has_member(91);
define_has_member(92);
define_has_member(93);
define_has_member(94);
define_has_member(95);
define_has_member(96);
define_has_member(97);
define_has_member(98);
define_has_member(99);

#pragma endregion

}

namespace lv
{

std::function<Ratio()> convertSliderIndex(int n)
{
    if (n >= 0 && n <= 99)
    {
        static constexpr Ratio(*kv[100])() =
        {
            slider(0),
            slider(1),
            slider(2),
            slider(3),
            slider(4),
            slider(5),
            slider(6),
            slider(7),
            slider(8),
            slider(9),
            slider(10),
            slider(11),
            slider(12),
            slider(13),
            slider(14),
            slider(15),
            slider(16),
            slider(17),
            slider(18),
            slider(19),
            slider(20),
            slider(21),
            slider(22),
            slider(23),
            slider(24),
            slider(25),
            slider(26),
            slider(27),
            slider(28),
            slider(29),
            slider(30),
            slider(31),
            slider(32),
            slider(33),
            slider(34),
            slider(35),
            slider(36),
            slider(37),
            slider(38),
            slider(39),
            slider(40),
            slider(41),
            slider(42),
            slider(43),
            slider(44),
            slider(45),
            slider(46),
            slider(47),
            slider(48),
            slider(49),
            slider(50),
            slider(51),
            slider(52),
            slider(53),
            slider(54),
            slider(55),
            slider(56),
            slider(57),
            slider(58),
            slider(59),
            slider(60),
            slider(61),
            slider(62),
            slider(63),
            slider(64),
            slider(65),
            slider(66),
            slider(67),
            slider(68),
            slider(69),
            slider(70),
            slider(71),
            slider(72),
            slider(73),
            slider(74),
            slider(75),
            slider(76),
            slider(77),
            slider(78),
            slider(79),
            slider(80),
            slider(81),
            slider(82),
            slider(83),
            slider(84),
            slider(85),
            slider(86),
            slider(87),
            slider(88),
            slider(89),
            slider(90),
            slider(91),
            slider(92),
            slider(93),
            slider(94),
            slider(95),
            slider(96),
            slider(97),
            slider(98),
            slider(99),
        };

        return kv[n];
    }

    return [] { return 0.0; };
}

}
