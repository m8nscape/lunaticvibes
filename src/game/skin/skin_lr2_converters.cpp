#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/data/data_types.h"
#include "game/arena/arena_data.h"

namespace lunaticvibes
{

namespace lr2skin
{

const char* timer(int n)
{
    static const std::map<int, const char*> timers =
    {
        { 0, "system.scene" }, // ���C���^�C�}�[
        { 1, "system.start_input" }, // ���͊J�n�^�C�}�[
        { 2, "system.fadeout" }, // �t�F�[�h�A�E�g�^�C�}�[
        { 3, "play.fail" }, // �X�^�C�}�[
        { 4, "select.input_end" }, // �������͏I���^�C�}�[
        { 10, "select.list_move" }, // �ȃ��X�g�ړ��^�C�}�[
        { 11, "select.list_entry_change" }, // �ȕύX�^�C�}�[
        // 12 �ȃ��X�g��ړ��^�C�}�[ (��)
        // 13 �ȃ��X�g���ړ��^�C�}�[ (��)
        // 14 �ȃ��X�g��~�^�C�}�[ (��)
        { 15, "select.readme_open" }, // ���[�h�~�[�J�n�^�C�}�[
        { 16, "select.readme_close" }, // ���[�h�~�[�I���^�C�}�[
        // 17 �R�[�X�Z���N�g�J�n�^�C�}�[
        // 18 �R�[�X�Z���N�g�I���^�C�}�[
        { 21, "select.panel1_start" }, // �p�l��1�N��
        { 22, "select.panel2_start" }, // �p�l��2�N��
        { 23, "select.panel3_start" }, // �p�l��3�N��
        { 24, "select.panel4_start" }, // �p�l��4�N��
        { 25, "select.panel5_start" }, // �p�l��5�N��
        { 26, "select.panel6_start" }, // �p�l��6�N��
        { 27, "select.panel7_start" }, // �p�l��7�N��
        { 28, "select.panel8_start" }, // �p�l��8�N��
        { 29, "select.panel9_start" }, // �p�l��9�N��
        { 31, "select.panel1_end" }, // �p�l��1�I��
        { 32, "select.panel2_end" }, // �p�l��2�I��
        { 33, "select.panel3_end" }, // �p�l��3�I��
        { 34, "select.panel4_end" }, // �p�l��4�I��
        { 35, "select.panel5_end" }, // �p�l��5�I��
        { 36, "select.panel6_end" }, // �p�l��6�I��
        { 37, "select.panel7_end" }, // �p�l��7�I��
        { 38, "select.panel8_end" }, // �p�l��8�I��
        { 39, "select.panel9_end" }, // �p�l��9�I��
        { 40, "play.ready" }, // READY
        { 41, "play.play_start" }, // �v���C�J�n
        { 42, "play.gauge_up_1p" }, // �Q�[�W�㏸�^�C�}�[ 1P
        { 43, "play.gauge_up_2p" }, // �Q�[�W�㏸�^�C�}�[ 2P
        { 44, "play.gauge_max_1p" }, // �Q�[�W�}�b�N�X�^�C�}�[ 1P
        { 45, "play.gauge_max_2p" }, // �Q�[�W�}�b�N�X�^�C�}�[ 2P
        { 46, "play.judge_1p" }, // �W���b�W�^�C�}�[ 1P
        { 47, "play.judge_2p" }, // �W���b�W�^�C�}�[ 2P
        { 48, "play.fullcombo_1p" }, // �t���R���^�C�}�[ 1P
        { 49, "play.fullcombo_2p" }, // �t���R���^�C�}�[ 2P
        { 50, "play.bomb_s_1p" }, // 1P�{���^�C�}�[ �M
        { 51, "play.bomb_1_1p" }, // 1P�{���^�C�}�[ 1��
        { 52, "play.bomb_2_1p" }, // 1P�{���^�C�}�[ 2��
        { 53, "play.bomb_3_1p" }, // 1P�{���^�C�}�[ 3��
        { 54, "play.bomb_4_1p" }, // 1P�{���^�C�}�[ 4��
        { 55, "play.bomb_5_1p" }, // 1P�{���^�C�}�[ 5��
        { 56, "play.bomb_6_1p" }, // 1P�{���^�C�}�[ 6��
        { 57, "play.bomb_7_1p" }, // 1P�{���^�C�}�[ 7��
        { 58, "play.bomb_8_1p" }, // 1P�{���^�C�}�[ 8��
        { 59, "play.bomb_9_1p" }, // 1P�{���^�C�}�[ 9��
        { 60, "play.bomb_s_2p" }, // 2p�{���^�C�}�[ �M
        { 61, "play.bomb_1_2p" }, // 2p�{���^�C�}�[ 1��
        { 62, "play.bomb_2_2p" }, // 2p�{���^�C�}�[ 2��
        { 63, "play.bomb_3_2p" }, // 2p�{���^�C�}�[ 3��
        { 64, "play.bomb_4_2p" }, // 2p�{���^�C�}�[ 4��
        { 65, "play.bomb_5_2p" }, // 2p�{���^�C�}�[ 5��
        { 66, "play.bomb_6_2p" }, // 2p�{���^�C�}�[ 6��
        { 67, "play.bomb_7_2p" }, // 2p�{���^�C�}�[ 7��
        { 68, "play.bomb_8_2p" }, // 2p�{���^�C�}�[ 8��
        { 69, "play.bomb_9_2p" }, // 2p�{���^�C�}�[ 9��
        { 70, "play.bomb_ln_s_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ �M
        { 71, "play.bomb_ln_1_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ 1��
        { 72, "play.bomb_ln_2_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ 2��
        { 73, "play.bomb_ln_3_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ 3��
        { 74, "play.bomb_ln_4_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ 4��
        { 75, "play.bomb_ln_5_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ 5��
        { 76, "play.bomb_ln_6_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ 6��
        { 77, "play.bomb_ln_7_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ 7��
        { 78, "play.bomb_ln_8_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ 8��
        { 79, "play.bomb_ln_9_1p" }, // 1PLN�G�t�F�N�g�^�C�}�[ 9��
        { 80, "play.bomb_ln_s_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ �M
        { 81, "play.bomb_ln_1_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ 1��
        { 82, "play.bomb_ln_2_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ 2��
        { 83, "play.bomb_ln_3_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ 3��
        { 84, "play.bomb_ln_4_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ 4��
        { 85, "play.bomb_ln_5_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ 5��
        { 86, "play.bomb_ln_6_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ 6��
        { 87, "play.bomb_ln_7_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ 7��
        { 88, "play.bomb_ln_8_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ 8��
        { 89, "play.bomb_ln_9_2p" }, // 2pLN�G�t�F�N�g�^�C�}�[ 9��
        { 100, "system.key_on_s_1p" }, // 1P�L�[�I���^�C�}�[ �M
        { 101, "system.key_on_1_1p" }, // 1P�L�[�I���^�C�}�[ 1��
        { 102, "system.key_on_2_1p" }, // 1P�L�[�I���^�C�}�[ 2��
        { 103, "system.key_on_3_1p" }, // 1P�L�[�I���^�C�}�[ 3��
        { 104, "system.key_on_4_1p" }, // 1P�L�[�I���^�C�}�[ 4��
        { 105, "system.key_on_5_1p" }, // 1P�L�[�I���^�C�}�[ 5��
        { 106, "system.key_on_6_1p" }, // 1P�L�[�I���^�C�}�[ 6��
        { 107, "system.key_on_7_1p" }, // 1P�L�[�I���^�C�}�[ 7��
        { 108, "system.key_on_8_1p" }, // 1P�L�[�I���^�C�}�[ 8��
        { 109, "system.key_on_9_1p" }, // 1P�L�[�I���^�C�}�[ 9��
        { 110, "system.key_on_s_2p" }, // 2p�L�[�I���^�C�}�[ �M
        { 111, "system.key_on_1_2p" }, // 2p�L�[�I���^�C�}�[ 1��
        { 112, "system.key_on_2_2p" }, // 2p�L�[�I���^�C�}�[ 2��
        { 113, "system.key_on_3_2p" }, // 2p�L�[�I���^�C�}�[ 3��
        { 114, "system.key_on_4_2p" }, // 2p�L�[�I���^�C�}�[ 4��
        { 115, "system.key_on_5_2p" }, // 2p�L�[�I���^�C�}�[ 5��
        { 116, "system.key_on_6_2p" }, // 2p�L�[�I���^�C�}�[ 6��
        { 117, "system.key_on_7_2p" }, // 2p�L�[�I���^�C�}�[ 7��
        { 118, "system.key_on_8_2p" }, // 2p�L�[�I���^�C�}�[ 8��
        { 119, "system.key_on_9_2p" }, // 2p�L�[�I���^�C�}�[ 9��
        { 120, "system.key_off_s_1p" }, // 1P�L�[�I�t�^�C�}�[ �M
        { 121, "system.key_off_1_1p" }, // 1P�L�[�I�t�^�C�}�[ 1��
        { 122, "system.key_off_2_1p" }, // 1P�L�[�I�t�^�C�}�[ 2��
        { 123, "system.key_off_3_1p" }, // 1P�L�[�I�t�^�C�}�[ 3��
        { 124, "system.key_off_4_1p" }, // 1P�L�[�I�t�^�C�}�[ 4��
        { 125, "system.key_off_5_1p" }, // 1P�L�[�I�t�^�C�}�[ 5��
        { 126, "system.key_off_6_1p" }, // 1P�L�[�I�t�^�C�}�[ 6��
        { 127, "system.key_off_7_1p" }, // 1P�L�[�I�t�^�C�}�[ 7��
        { 128, "system.key_off_8_1p" }, // 1P�L�[�I�t�^�C�}�[ 8��
        { 129, "system.key_off_9_1p" }, // 1P�L�[�I�t�^�C�}�[ 9��
        { 130, "system.key_off_s_2p" }, // 2p�L�[�I�t�^�C�}�[ �M
        { 131, "system.key_off_1_2p" }, // 2p�L�[�I�t�^�C�}�[ 1��
        { 132, "system.key_off_2_2p" }, // 2p�L�[�I�t�^�C�}�[ 2��
        { 133, "system.key_off_3_2p" }, // 2p�L�[�I�t�^�C�}�[ 3��
        { 134, "system.key_off_4_2p" }, // 2p�L�[�I�t�^�C�}�[ 4��
        { 135, "system.key_off_5_2p" }, // 2p�L�[�I�t�^�C�}�[ 5��
        { 136, "system.key_off_6_2p" }, // 2p�L�[�I�t�^�C�}�[ 6��
        { 137, "system.key_off_7_2p" }, // 2p�L�[�I�t�^�C�}�[ 7��
        { 138, "system.key_off_8_2p" }, // 2p�L�[�I�t�^�C�}�[ 8��
        { 139, "system.key_off_9_2p" }, // 2p�L�[�I�t�^�C�}�[ 9��
        // 140 ���Y���^�C�}�[
        { 143, "play.last_note_1p" }, // �ŏI�m�[�g�^�C�}�[ 1P
        { 144, "play.last_note_2p" }, // �ŏI�m�[�g�^�C�}�[ 2P
        { 150, "result.graph_start" }, // ���U���g�O���t�`��J�n�^�C�}�[
        { 151, "result.graph_end" }, // ���U���g�O���t�`��I���^�C�}�[(�v���C�����N�\���^�C�}�[
        { 152, "result.sub_page" }, // ���U���g�n�C�X�R�A�X�V�^�C�}�[
        // 170 �f�[�^�x�[�X���[�h���^�C�}�[
        // 171 �f�[�^�x�[�X���[�h�����^�C�}�[
        // 172 IR �ڑ��J�n�^�C�}�[
        // 173 IR �ڑ������^�C�}�[
        // 174 IR �ڑ����s�^�C�}�[
        { 180, "select.course_edit_start" }, // �R�[�X�G�f�B�^�[�J�n�^�C�}�[
        { 181, "select.course_edit_end" }, // �R�[�X�G�f�B�^�[�I���^�C�}�[

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

std::function<bool()> dst1(int n)
{


}

}
}