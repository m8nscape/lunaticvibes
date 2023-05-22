#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/data/data_types.h"

namespace lunaticvibes
{

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
        //if (!LR2CustomizeData.optionsMap.empty())
        //    return double(LR2CustomizeData.topOptionIndex) / (LR2CustomizeData.optionsMap.size() - 1);
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

#pragma region declare_member1

declare_member(SliderConverter, Ratio, slider, 0);
declare_member(SliderConverter, Ratio, slider, 1);
declare_member(SliderConverter, Ratio, slider, 2);
declare_member(SliderConverter, Ratio, slider, 3);
declare_member(SliderConverter, Ratio, slider, 4);
declare_member(SliderConverter, Ratio, slider, 5);
declare_member(SliderConverter, Ratio, slider, 6);
declare_member(SliderConverter, Ratio, slider, 7);
declare_member(SliderConverter, Ratio, slider, 8);
declare_member(SliderConverter, Ratio, slider, 9);
declare_member(SliderConverter, Ratio, slider, 10);
declare_member(SliderConverter, Ratio, slider, 11);
declare_member(SliderConverter, Ratio, slider, 12);
declare_member(SliderConverter, Ratio, slider, 13);
declare_member(SliderConverter, Ratio, slider, 14);
declare_member(SliderConverter, Ratio, slider, 15);
declare_member(SliderConverter, Ratio, slider, 16);
declare_member(SliderConverter, Ratio, slider, 17);
declare_member(SliderConverter, Ratio, slider, 18);
declare_member(SliderConverter, Ratio, slider, 19);
declare_member(SliderConverter, Ratio, slider, 20);
declare_member(SliderConverter, Ratio, slider, 21);
declare_member(SliderConverter, Ratio, slider, 22);
declare_member(SliderConverter, Ratio, slider, 23);
declare_member(SliderConverter, Ratio, slider, 24);
declare_member(SliderConverter, Ratio, slider, 25);
declare_member(SliderConverter, Ratio, slider, 26);
declare_member(SliderConverter, Ratio, slider, 27);
declare_member(SliderConverter, Ratio, slider, 28);
declare_member(SliderConverter, Ratio, slider, 29);
declare_member(SliderConverter, Ratio, slider, 30);
declare_member(SliderConverter, Ratio, slider, 31);
declare_member(SliderConverter, Ratio, slider, 32);
declare_member(SliderConverter, Ratio, slider, 33);
declare_member(SliderConverter, Ratio, slider, 34);
declare_member(SliderConverter, Ratio, slider, 35);
declare_member(SliderConverter, Ratio, slider, 36);
declare_member(SliderConverter, Ratio, slider, 37);
declare_member(SliderConverter, Ratio, slider, 38);
declare_member(SliderConverter, Ratio, slider, 39);
declare_member(SliderConverter, Ratio, slider, 40);
declare_member(SliderConverter, Ratio, slider, 41);
declare_member(SliderConverter, Ratio, slider, 42);
declare_member(SliderConverter, Ratio, slider, 43);
declare_member(SliderConverter, Ratio, slider, 44);
declare_member(SliderConverter, Ratio, slider, 45);
declare_member(SliderConverter, Ratio, slider, 46);
declare_member(SliderConverter, Ratio, slider, 47);
declare_member(SliderConverter, Ratio, slider, 48);
declare_member(SliderConverter, Ratio, slider, 49);
declare_member(SliderConverter, Ratio, slider, 50);
declare_member(SliderConverter, Ratio, slider, 51);
declare_member(SliderConverter, Ratio, slider, 52);
declare_member(SliderConverter, Ratio, slider, 53);
declare_member(SliderConverter, Ratio, slider, 54);
declare_member(SliderConverter, Ratio, slider, 55);
declare_member(SliderConverter, Ratio, slider, 56);
declare_member(SliderConverter, Ratio, slider, 57);
declare_member(SliderConverter, Ratio, slider, 58);
declare_member(SliderConverter, Ratio, slider, 59);
declare_member(SliderConverter, Ratio, slider, 60);
declare_member(SliderConverter, Ratio, slider, 61);
declare_member(SliderConverter, Ratio, slider, 62);
declare_member(SliderConverter, Ratio, slider, 63);
declare_member(SliderConverter, Ratio, slider, 64);
declare_member(SliderConverter, Ratio, slider, 65);
declare_member(SliderConverter, Ratio, slider, 66);
declare_member(SliderConverter, Ratio, slider, 67);
declare_member(SliderConverter, Ratio, slider, 68);
declare_member(SliderConverter, Ratio, slider, 69);
declare_member(SliderConverter, Ratio, slider, 70);
declare_member(SliderConverter, Ratio, slider, 71);
declare_member(SliderConverter, Ratio, slider, 72);
declare_member(SliderConverter, Ratio, slider, 73);
declare_member(SliderConverter, Ratio, slider, 74);
declare_member(SliderConverter, Ratio, slider, 75);
declare_member(SliderConverter, Ratio, slider, 76);
declare_member(SliderConverter, Ratio, slider, 77);
declare_member(SliderConverter, Ratio, slider, 78);
declare_member(SliderConverter, Ratio, slider, 79);
declare_member(SliderConverter, Ratio, slider, 80);
declare_member(SliderConverter, Ratio, slider, 81);
declare_member(SliderConverter, Ratio, slider, 82);
declare_member(SliderConverter, Ratio, slider, 83);
declare_member(SliderConverter, Ratio, slider, 84);
declare_member(SliderConverter, Ratio, slider, 85);
declare_member(SliderConverter, Ratio, slider, 86);
declare_member(SliderConverter, Ratio, slider, 87);
declare_member(SliderConverter, Ratio, slider, 88);
declare_member(SliderConverter, Ratio, slider, 89);
declare_member(SliderConverter, Ratio, slider, 90);
declare_member(SliderConverter, Ratio, slider, 91);
declare_member(SliderConverter, Ratio, slider, 92);
declare_member(SliderConverter, Ratio, slider, 93);
declare_member(SliderConverter, Ratio, slider, 94);
declare_member(SliderConverter, Ratio, slider, 95);
declare_member(SliderConverter, Ratio, slider, 96);
declare_member(SliderConverter, Ratio, slider, 97);
declare_member(SliderConverter, Ratio, slider, 98);
declare_member(SliderConverter, Ratio, slider, 99);

#define slider(index) member(SliderConverter, Ratio, slider, index)

#pragma endregion

namespace lr2skin
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
}
