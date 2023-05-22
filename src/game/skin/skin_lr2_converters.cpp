#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/data/data_types.h"

namespace lunaticvibes
{

namespace lr2skin
{

const char* timer(int n)
{
    static const std::map<int, const char*> timers =
    {
        { 0, "system.scene" }, // メインタイマー
        { 1, "system.start_input" }, // 入力開始タイマー
        { 2, "system.fadeout" }, // フェードアウトタイマー
        { 3, "play.fail" }, // 閉店タイマー
        { 4, "select.input_end" }, // 文字入力終了タイマー
        { 10, "select.list_move" }, // 曲リスト移動タイマー
        { 11, "select.list_entry_change" }, // 曲変更タイマー
        // 12 曲リスト上移動タイマー (未)
        // 13 曲リスト下移動タイマー (未)
        // 14 曲リスト停止タイマー (未)
        { 15, "select.readme_open" }, // リードミー開始タイマー
        { 16, "select.readme_close" }, // リードミー終了タイマー
        // 17 コースセレクト開始タイマー
        // 18 コースセレクト終了タイマー
        { 21, "select.panel1_start" }, // パネル1起動
        { 22, "select.panel2_start" }, // パネル2起動
        { 23, "select.panel3_start" }, // パネル3起動
        { 24, "select.panel4_start" }, // パネル4起動
        { 25, "select.panel5_start" }, // パネル5起動
        { 26, "select.panel6_start" }, // パネル6起動
        { 27, "select.panel7_start" }, // パネル7起動
        { 28, "select.panel8_start" }, // パネル8起動
        { 29, "select.panel9_start" }, // パネル9起動
        { 31, "select.panel1_end" }, // パネル1終了
        { 32, "select.panel2_end" }, // パネル2終了
        { 33, "select.panel3_end" }, // パネル3終了
        { 34, "select.panel4_end" }, // パネル4終了
        { 35, "select.panel5_end" }, // パネル5終了
        { 36, "select.panel6_end" }, // パネル6終了
        { 37, "select.panel7_end" }, // パネル7終了
        { 38, "select.panel8_end" }, // パネル8終了
        { 39, "select.panel9_end" }, // パネル9終了
        { 40, "play.ready" }, // READY
        { 41, "play.play_start" }, // プレイ開始
        { 42, "play.gauge_up_1p" }, // ゲージ上昇タイマー 1P
        { 43, "play.gauge_up_2p" }, // ゲージ上昇タイマー 2P
        { 44, "play.gauge_max_1p" }, // ゲージマックスタイマー 1P
        { 45, "play.gauge_max_2p" }, // ゲージマックスタイマー 2P
        { 46, "play.judge_1p" }, // ジャッジタイマー 1P
        { 47, "play.judge_2p" }, // ジャッジタイマー 2P
        { 48, "play.fullcombo_1p" }, // フルコンタイマー 1P
        { 49, "play.fullcombo_2p" }, // フルコンタイマー 2P
        { 50, "play.bomb_1p_s" }, // 1Pボムタイマー 皿
        { 51, "play.bomb_1p_k1" }, // 1Pボムタイマー 1鍵
        { 52, "play.bomb_1p_k2" }, // 1Pボムタイマー 2鍵
        { 53, "play.bomb_1p_k3" }, // 1Pボムタイマー 3鍵
        { 54, "play.bomb_1p_k4" }, // 1Pボムタイマー 4鍵
        { 55, "play.bomb_1p_k5" }, // 1Pボムタイマー 5鍵
        { 56, "play.bomb_1p_k6" }, // 1Pボムタイマー 6鍵
        { 57, "play.bomb_1p_k7" }, // 1Pボムタイマー 7鍵
        { 58, "play.bomb_1p_k8" }, // 1Pボムタイマー 8鍵
        { 59, "play.bomb_1p_k9" }, // 1Pボムタイマー 9鍵
        { 60, "play.bomb_2p_s" }, // 2pボムタイマー 皿
        { 61, "play.bomb_2p_k1" }, // 2pボムタイマー 1鍵
        { 62, "play.bomb_2p_k2" }, // 2pボムタイマー 2鍵
        { 63, "play.bomb_2p_k3" }, // 2pボムタイマー 3鍵
        { 64, "play.bomb_2p_k4" }, // 2pボムタイマー 4鍵
        { 65, "play.bomb_2p_k5" }, // 2pボムタイマー 5鍵
        { 66, "play.bomb_2p_k6" }, // 2pボムタイマー 6鍵
        { 67, "play.bomb_2p_k7" }, // 2pボムタイマー 7鍵
        { 68, "play.bomb_2p_k8" }, // 2pボムタイマー 8鍵
        { 69, "play.bomb_2p_k9" }, // 2pボムタイマー 9鍵
        { 70, "play.bomb_ln_1p_s" }, // 1PLNエフェクトタイマー 皿
        { 71, "play.bomb_ln_1p_k1" }, // 1PLNエフェクトタイマー 1鍵
        { 72, "play.bomb_ln_1p_k2" }, // 1PLNエフェクトタイマー 2鍵
        { 73, "play.bomb_ln_1p_k3" }, // 1PLNエフェクトタイマー 3鍵
        { 74, "play.bomb_ln_1p_k4" }, // 1PLNエフェクトタイマー 4鍵
        { 75, "play.bomb_ln_1p_k5" }, // 1PLNエフェクトタイマー 5鍵
        { 76, "play.bomb_ln_1p_k6" }, // 1PLNエフェクトタイマー 6鍵
        { 77, "play.bomb_ln_1p_k7" }, // 1PLNエフェクトタイマー 7鍵
        { 78, "play.bomb_ln_1p_k8" }, // 1PLNエフェクトタイマー 8鍵
        { 79, "play.bomb_ln_1p_k9" }, // 1PLNエフェクトタイマー 9鍵
        { 80, "play.bomb_ln_2p_s" }, // 2pLNエフェクトタイマー 皿
        { 81, "play.bomb_ln_2p_k1" }, // 2pLNエフェクトタイマー 1鍵
        { 82, "play.bomb_ln_2p_k2" }, // 2pLNエフェクトタイマー 2鍵
        { 83, "play.bomb_ln_2p_k3" }, // 2pLNエフェクトタイマー 3鍵
        { 84, "play.bomb_ln_2p_k4" }, // 2pLNエフェクトタイマー 4鍵
        { 85, "play.bomb_ln_2p_k5" }, // 2pLNエフェクトタイマー 5鍵
        { 86, "play.bomb_ln_2p_k6" }, // 2pLNエフェクトタイマー 6鍵
        { 87, "play.bomb_ln_2p_k7" }, // 2pLNエフェクトタイマー 7鍵
        { 88, "play.bomb_ln_2p_k8" }, // 2pLNエフェクトタイマー 8鍵
        { 89, "play.bomb_ln_2p_k9" }, // 2pLNエフェクトタイマー 9鍵
        { 100, "system.key_on_s_1p" }, // 1Pキーオンタイマー 皿
        { 101, "system.key_on_1_1p" }, // 1Pキーオンタイマー 1鍵
        { 102, "system.key_on_2_1p" }, // 1Pキーオンタイマー 2鍵
        { 103, "system.key_on_3_1p" }, // 1Pキーオンタイマー 3鍵
        { 104, "system.key_on_4_1p" }, // 1Pキーオンタイマー 4鍵
        { 105, "system.key_on_5_1p" }, // 1Pキーオンタイマー 5鍵
        { 106, "system.key_on_6_1p" }, // 1Pキーオンタイマー 6鍵
        { 107, "system.key_on_7_1p" }, // 1Pキーオンタイマー 7鍵
        { 108, "system.key_on_8_1p" }, // 1Pキーオンタイマー 8鍵
        { 109, "system.key_on_9_1p" }, // 1Pキーオンタイマー 9鍵
        { 110, "system.key_on_s_2p" }, // 2pキーオンタイマー 皿
        { 111, "system.key_on_1_2p" }, // 2pキーオンタイマー 1鍵
        { 112, "system.key_on_2_2p" }, // 2pキーオンタイマー 2鍵
        { 113, "system.key_on_3_2p" }, // 2pキーオンタイマー 3鍵
        { 114, "system.key_on_4_2p" }, // 2pキーオンタイマー 4鍵
        { 115, "system.key_on_5_2p" }, // 2pキーオンタイマー 5鍵
        { 116, "system.key_on_6_2p" }, // 2pキーオンタイマー 6鍵
        { 117, "system.key_on_7_2p" }, // 2pキーオンタイマー 7鍵
        { 118, "system.key_on_8_2p" }, // 2pキーオンタイマー 8鍵
        { 119, "system.key_on_9_2p" }, // 2pキーオンタイマー 9鍵
        { 120, "system.key_off_s_1p" }, // 1Pキーオフタイマー 皿
        { 121, "system.key_off_1_1p" }, // 1Pキーオフタイマー 1鍵
        { 122, "system.key_off_2_1p" }, // 1Pキーオフタイマー 2鍵
        { 123, "system.key_off_3_1p" }, // 1Pキーオフタイマー 3鍵
        { 124, "system.key_off_4_1p" }, // 1Pキーオフタイマー 4鍵
        { 125, "system.key_off_5_1p" }, // 1Pキーオフタイマー 5鍵
        { 126, "system.key_off_6_1p" }, // 1Pキーオフタイマー 6鍵
        { 127, "system.key_off_7_1p" }, // 1Pキーオフタイマー 7鍵
        { 128, "system.key_off_8_1p" }, // 1Pキーオフタイマー 8鍵
        { 129, "system.key_off_9_1p" }, // 1Pキーオフタイマー 9鍵
        { 130, "system.key_off_s_2p" }, // 2pキーオフタイマー 皿
        { 131, "system.key_off_1_2p" }, // 2pキーオフタイマー 1鍵
        { 132, "system.key_off_2_2p" }, // 2pキーオフタイマー 2鍵
        { 133, "system.key_off_3_2p" }, // 2pキーオフタイマー 3鍵
        { 134, "system.key_off_4_2p" }, // 2pキーオフタイマー 4鍵
        { 135, "system.key_off_5_2p" }, // 2pキーオフタイマー 5鍵
        { 136, "system.key_off_6_2p" }, // 2pキーオフタイマー 6鍵
        { 137, "system.key_off_7_2p" }, // 2pキーオフタイマー 7鍵
        { 138, "system.key_off_8_2p" }, // 2pキーオフタイマー 8鍵
        { 139, "system.key_off_9_2p" }, // 2pキーオフタイマー 9鍵
        // 140 リズムタイマー "play.beat" handled directly inside getTimerValue("")
        { 143, "play.last_note_1p" }, // 最終ノートタイマー 1P
        { 144, "play.last_note_2p" }, // 最終ノートタイマー 2P
        { 150, "result.graph_start" }, // リザルトグラフ描画開始タイマー
        { 151, "result.graph_end" }, // リザルトグラフ描画終了タイマー(プレイランク表示タイマー
        { 152, "result.sub_page" }, // リザルトハイスコア更新タイマー
        // 170 データベースロード中タイマー
        // 171 データベースロード完了タイマー
        // 172 IR 接続開始タイマー
        // 173 IR 接続成功タイマー
        // 174 IR 接続失敗タイマー
        { 180, "select.course_edit_start" }, // コースエディター開始タイマー
        { 181, "select.course_edit_end" }, // コースエディター終了タイマー

        { 200, "system.key_on_sl_1p" },
        { 201, "system.key_off_sl_1p" },
        { 202, "system.key_on_sr_1p" },
        { 203, "system.key_off_sr_1p" },
        { 204, "system.key_on_start_1p" },
        { 205, "system.key_on_select_1p" },
        { 206, "system.key_on_spdup_1p" },
        { 207, "system.key_on_spddn_1p" },
        { 208, "system.key_on_start_1p" },
        { 209, "system.key_on_select_1p" },
        { 210, "system.key_on_spdup_1p" },
        { 211, "system.key_on_spddn_1p" },
        { 212, "system.scratch_axis_move_1p" },

        { 220, "system.key_on_sl_2p" },
        { 221, "system.key_off_sl_2p" },
        { 222, "system.key_on_sr_2p" },
        { 223, "system.key_off_sr_2p" },
        { 224, "system.key_on_start_2p" },
        { 225, "system.key_on_select_2p" },
        { 226, "system.key_on_spdup_2p" },
        { 227, "system.key_on_spddn_2p" },
        { 228, "system.key_on_start_2p" },
        { 229, "system.key_on_select_2p" },
        { 230, "system.key_on_spdup_2p" },
        { 231, "system.key_on_spddn_2p" },
        { 232, "system.scratch_axis_move_2p" },

        { 240, "arena.show_lobby" },
        { 241, "arena.play_finish_wait" },
        { 242, "arena.result_wait" },
    };

    if (timers.find(n) != timers.end())
    {
        return timers.at(n);
    }
    else
    {
        return "";
    }
}

}
}