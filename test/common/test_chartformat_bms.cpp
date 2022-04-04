#include "gmock/gmock.h"
#include "common/chartformat/chartformat_bms.h"
#include <fstream>

TEST(tBMS, folder_not_exist)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bbb/asdlfkjasdlfkjsdalgjsdalgjasd.bms"));
	EXPECT_EQ(bms->isLoaded(), false);
}

TEST(tBMS, file_not_exist)
{
	std::shared_ptr<BMS> bms = nullptr; 
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/asdlfkjasdlfkjsdalgjsdalgjasd.bms"));
	EXPECT_EQ(bms->isLoaded(), false);
}

TEST(tBMS, meta_basic)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/bgm32.bms"));
	ASSERT_EQ(bms->isLoaded(), true);
	EXPECT_EQ(bms->fileHash, HashMD5("2803C539385E76F13919A99615DC4480"));
	EXPECT_EQ(bms->haveNote, false);
	EXPECT_EQ(bms->notes, 0);
	EXPECT_EQ(bms->notes_ln, 0);
}

TEST(tBMS, bar_length_change)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/bar.bms"));
	ASSERT_EQ(bms->isLoaded(), true);
}

TEST(tBMS, bpm_change)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/bpm.bms"));
	ASSERT_EQ(bms->isLoaded(), true);
}

TEST(tBMS, stop)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/stop.bms"));
	ASSERT_EQ(bms->isLoaded(), true);
}

TEST(tBMS, stop)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/stop.bms"));
	ASSERT_EQ(bms->isLoaded(), true);
}

TEST(tBMS, note_5k)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/5k.bms"));
	ASSERT_EQ(bms->isLoaded(), true);

	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 0, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 1, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 2, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 3, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 4, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 0, 2).notes.size(), 4);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 5, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 0, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 1, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 2, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 3, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 4, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 5, 1).notes.size(), 0);

	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 1, 1).resolution / bms->getLane(bms::LaneCode::NOTE1, 1, 1).notes.begin()->segment, 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 2, 1).resolution / bms->getLane(bms::LaneCode::NOTE1, 2, 1).notes.begin()->segment, 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 3, 1).resolution / bms->getLane(bms::LaneCode::NOTE1, 3, 1).notes.begin()->segment, 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 4, 1).resolution / bms->getLane(bms::LaneCode::NOTE1, 4, 1).notes.begin()->segment, 3);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 5, 2).resolution / bms->getLane(bms::LaneCode::NOTE1, 5, 2).notes.begin()->segment, 0);
	for (int i = 0; i < 4; ++i)
	{
		EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 0, 2).resolution / bms->getLane(bms::LaneCode::NOTE1, 0, 2).notes.begin()->segment, i);
	}
}
TEST(tBMS, note_7k)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/7k.bms"));
	ASSERT_EQ(bms->isLoaded(), true);

	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 0, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 1, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 2, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 3, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 4, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 5, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 6, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 7, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 0, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 1, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 2, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 3, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 4, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 5, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 6, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 7, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 0, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 1, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 2, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 3, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 4, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 5, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 6, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 7, 1).notes.size(), 0);

	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 0, 1).resolution * bms->getLane(bms::LaneCode::NOTE1, 0, 1).notes.begin()->segment, 0);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 1, 1).resolution * bms->getLane(bms::LaneCode::NOTE1, 1, 1).notes.begin()->segment, 0);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 2, 1).resolution * bms->getLane(bms::LaneCode::NOTE1, 2, 1).notes.begin()->segment, 1);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 3, 1).resolution * bms->getLane(bms::LaneCode::NOTE1, 3, 1).notes.begin()->segment, 2);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 4, 1).resolution * bms->getLane(bms::LaneCode::NOTE1, 4, 1).notes.begin()->segment, 3);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 5, 1).resolution * bms->getLane(bms::LaneCode::NOTE1, 5, 1).notes.begin()->segment, 4);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 6, 1).resolution * bms->getLane(bms::LaneCode::NOTE1, 6, 1).notes.begin()->segment, 5);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 7, 1).resolution * bms->getLane(bms::LaneCode::NOTE1, 7, 1).notes.begin()->segment, 6);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 0, 2).resolution * bms->getLane(bms::LaneCode::NOTE1, 0, 2).notes.begin()->segment, 0);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 1, 2).resolution * bms->getLane(bms::LaneCode::NOTE1, 1, 2).notes.begin()->segment, 1);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 2, 2).resolution * bms->getLane(bms::LaneCode::NOTE1, 2, 2).notes.begin()->segment, 2);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 3, 2).resolution * bms->getLane(bms::LaneCode::NOTE1, 3, 2).notes.begin()->segment, 3);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 4, 2).resolution * bms->getLane(bms::LaneCode::NOTE1, 4, 2).notes.begin()->segment, 4);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 5, 2).resolution * bms->getLane(bms::LaneCode::NOTE1, 5, 2).notes.begin()->segment, 5);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 6, 2).resolution * bms->getLane(bms::LaneCode::NOTE1, 6, 2).notes.begin()->segment, 6);
	EXPECT_EQ(8 / bms->getLane(bms::LaneCode::NOTE1, 7, 2).resolution * bms->getLane(bms::LaneCode::NOTE1, 7, 2).notes.begin()->segment, 7);
}
TEST(tBMS, note_10k)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/10k.bms"));
	ASSERT_EQ(bms->isLoaded(), true);
}
TEST(tBMS, note_14k)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/14k.bms"));
	ASSERT_EQ(bms->isLoaded(), true);
}