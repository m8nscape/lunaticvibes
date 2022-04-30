#include "gmock/gmock.h"
#include "common/chartformat/chartformat_bms.h"
#include "../../src/common/utils.h"
#include <fstream>

bool ExpectNotePosition(BMS& bms, LaneCode area, int ch, int bar, int res, std::vector<int>& segments)
{
	auto& lane = bms.getLane(area, ch, bar);
	unsigned maxres = lane.resolution * res;

	auto it1 = lane.notes.begin();
	auto it2 = segments.begin();
	for (size_t i = 0; i < segments.size(); ++i, ++it1, ++it2)
	{
		if ((maxres * it1->segment / lane.resolution) != (*it2 * (maxres / res)))
			return false;
	}
	return true;
};
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
	EXPECT_STRCASEEQ(bms->fileHash.hexdigest().c_str(), "C396F76B2DE46425ED22B196230173F0");
	EXPECT_EQ(bms->haveNote, false);
	EXPECT_EQ(bms->notes, 0);
	EXPECT_EQ(bms->notes_ln, 0);
}

TEST(tBMS, bar_length_change)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/bar.bms"));
	ASSERT_EQ(bms->isLoaded(), true);
	ASSERT_EQ(bms->lastBarIdx, 3);
	EXPECT_EQ(bms->barLength[0], Beat(4, 4));
	EXPECT_EQ(bms->barLength[1], Beat(8, 4));
	EXPECT_EQ(bms->barLength[2], Beat(3, 4));
	EXPECT_EQ(bms->barLength[3], Beat(4, 4));
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

TEST(tBMS, note_5k)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/5k.bms"));
	ASSERT_EQ(bms->isLoaded(), true);

	// note count
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 0, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 1, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 2, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 3, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 4, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 5, 1).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 0, 2).notes.size(), 4);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 1, 2).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 2, 2).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 3, 2).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 4, 2).notes.size(), 0);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 5, 2).notes.size(), 1);

	// note position
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 1, 1, 4, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 2, 1, 4, std::vector<int>{ 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 3, 1, 4, std::vector<int>{ 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 5, 2, 4, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 0, 2, 4, std::vector<int>{ 0, 1, 2, 3 }));
}
TEST(tBMS, note_7k)
{
	std::shared_ptr<BMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<BMS>("bms/7k.bme"));
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

	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 0, 1, 8, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 1, 1, 8, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 2, 1, 8, std::vector<int>{ 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 3, 1, 8, std::vector<int>{ 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 4, 1, 8, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 5, 1, 8, std::vector<int>{ 4 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 6, 1, 8, std::vector<int>{ 5 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 7, 1, 8, std::vector<int>{ 6 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 0, 2, 8, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 1, 2, 8, std::vector<int>{ 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 2, 2, 8, std::vector<int>{ 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 3, 2, 8, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 4, 2, 8, std::vector<int>{ 4 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 5, 2, 8, std::vector<int>{ 5 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 6, 2, 8, std::vector<int>{ 6 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 7, 2, 8, std::vector<int>{ 7 }));
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