#include "gmock/gmock.h"
#include "common/chartformat/chartformat_bms.h"
#include "../../src/common/utils.h"
#include <fstream>
using namespace lunaticvibes;

bool ExpectNotePosition(const ChartFormatBMS& bms, LaneCode area, int ch, int bar, int res, const std::vector<int>& segments)
{
	const auto& lane = bms.getLane(area, ch, bar);
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
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bbb/asdlfkjasdlfkjsdalgjsdalgjasd.bms"));
	EXPECT_EQ(bms->isLoaded(), false);
}

TEST(tBMS, file_not_exist)
{
	std::shared_ptr<ChartFormatBMS> bms = nullptr; 
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/asdlfkjasdlfkjsdalgjsdalgjasd.bms"));
	EXPECT_EQ(bms->isLoaded(), false);
}

TEST(tBMS, meta_basic)
{
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/bgm32.bms"));
	ASSERT_EQ(bms->isLoaded(), true);

	EXPECT_STRCASEEQ(bms->fileHash.hexdigest().c_str(), "C396F76B2DE46425ED22B196230173F0");
	EXPECT_EQ(bms->haveNote, false);
	EXPECT_EQ(bms->notes_total, 0);
	EXPECT_EQ(bms->notes_key_ln, 0);
}

TEST(tBMS, metre_change)
{
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/bar.bms"));
	ASSERT_EQ(bms->isLoaded(), true);

	ASSERT_EQ(bms->lastBarIdx, 3);
	EXPECT_EQ(bms->metres[0], Metre(4, 4));
	EXPECT_EQ(bms->metres[1], Metre(8, 4));
	EXPECT_EQ(bms->metres[2], Metre(3, 4));
	EXPECT_EQ(bms->metres[3], Metre(4, 4));
}

TEST(tBMS, bpm_change)
{
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/bpm.bms"));
	ASSERT_EQ(bms->isLoaded(), true);

	ASSERT_EQ(bms->lastBarIdx, 3);
	EXPECT_FLOAT_EQ(bms->bpm, 150.000000);
	EXPECT_TRUE(bms->haveBPMChange);
	EXPECT_FLOAT_EQ(bms->maxBPM, 300.000000);
	EXPECT_FLOAT_EQ(bms->minBPM, 0.100000);
	EXPECT_EQ(bms->getLane(bms::LaneCode::BPM, 0, 0).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::BPM, 0, 0).notes.begin()->value, 175);
	EXPECT_EQ(bms->getLane(bms::LaneCode::BPM, 0, 1).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::BPM, 0, 1).notes.begin()->value, 200);
	EXPECT_EQ(bms->getLane(bms::LaneCode::BPM, 0, 1).notes.rbegin()->value, 250);
	ASSERT_EQ(bms->getLane(bms::LaneCode::EXBPM, 0, 2).notes.size(), 2);
	EXPECT_FLOAT_EQ(bms->exBPM[bms->getLane(bms::LaneCode::EXBPM, 0, 2).notes.begin()->value], 300.0);
	EXPECT_FLOAT_EQ(bms->exBPM[bms->getLane(bms::LaneCode::EXBPM, 0, 2).notes.rbegin()->value], 0.1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::BPM, 0, 3).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::BPM, 0, 3).notes.begin()->value, 150);
}

TEST(tBMS, stop)
{
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/stop.bms"));
	ASSERT_EQ(bms->isLoaded(), true);

	EXPECT_EQ(bms->getLane(bms::LaneCode::STOP, 0, 1).notes.size(), 3);
	auto it = bms->getLane(bms::LaneCode::STOP, 0, 1).notes.begin();
	EXPECT_EQ(bms->stop[it->value], 48);
	it++;
	EXPECT_EQ(bms->stop[it->value], 96);
	it++;
	EXPECT_EQ(bms->stop[it->value], 192);
}

TEST(tBMS, note_5k)
{
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/5k.bms"));
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
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/7k.bme"));
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
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/10k.bms"));
	ASSERT_EQ(bms->isLoaded(), true);

	// note count
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 0, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 1, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 2, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 3, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 4, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 5, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 1, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 2, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 3, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 4, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 5, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 0, 2).notes.size(), 1);

	// note position
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 0, 1, 8, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 1, 2, 8, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 2, 2, 8, std::vector<int>{ 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 3, 2, 8, std::vector<int>{ 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 4, 2, 8, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 5, 2, 8, std::vector<int>{ 4 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 1, 1, 8, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 2, 1, 8, std::vector<int>{ 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 3, 1, 8, std::vector<int>{ 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 4, 1, 8, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 5, 1, 8, std::vector<int>{ 4 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 0, 2, 8, std::vector<int>{ 0 }));
}
TEST(tBMS, note_14k)
{
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/14k.bme"));
	ASSERT_EQ(bms->isLoaded(), true);

	// note count
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 0, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 1, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 2, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 3, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 4, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 5, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 6, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE1, 7, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 1, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 2, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 3, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 4, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 5, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 6, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 7, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTE2, 0, 2).notes.size(), 1);

	// note position
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 0, 1, 8, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 1, 2, 8, std::vector<int>{ 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 2, 2, 8, std::vector<int>{ 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 3, 2, 8, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 4, 2, 8, std::vector<int>{ 4 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 5, 2, 8, std::vector<int>{ 5 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 6, 2, 8, std::vector<int>{ 6 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE1, 7, 2, 8, std::vector<int>{ 7 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 1, 1, 8, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 2, 1, 8, std::vector<int>{ 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 3, 1, 8, std::vector<int>{ 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 4, 1, 8, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 5, 1, 8, std::vector<int>{ 4 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 6, 1, 8, std::vector<int>{ 5 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 7, 1, 8, std::vector<int>{ 6 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTE2, 0, 2, 8, std::vector<int>{ 0 }));
}

TEST(tBMS, ln)
{
	std::shared_ptr<ChartFormatBMS> bms = nullptr;
	ASSERT_NO_THROW(bms = std::make_shared<ChartFormatBMS>("bms/ln.bme"));
	ASSERT_EQ(bms->isLoaded(), true);

	// note count
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN1, 1, 1).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN1, 2, 1).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN1, 3, 1).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN1, 4, 1).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN1, 4, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN1, 5, 2).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN1, 6, 2).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN1, 7, 2).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 1, 2).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 1, 3).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 2, 3).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 3, 3).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 4, 3).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 5, 3).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 5, 4).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 6, 4).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 7, 4).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN1, 0, 4).notes.size(), 2);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 0, 4).notes.size(), 1);
	EXPECT_EQ(bms->getLane(bms::LaneCode::NOTELN2, 0, 5).notes.size(), 1);

	// note position
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN1, 1, 1, 4, std::vector<int>{ 0, 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN1, 2, 1, 4, std::vector<int>{ 1, 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN1, 3, 1, 4, std::vector<int>{ 2, 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN1, 4, 1, 4, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN1, 4, 2, 4, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN1, 5, 2, 4, std::vector<int>{ 0, 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN1, 6, 2, 4, std::vector<int>{ 1, 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN1, 7, 2, 4, std::vector<int>{ 2, 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 1, 2, 4, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 1, 3, 4, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 2, 3, 4, std::vector<int>{ 0, 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 3, 3, 4, std::vector<int>{ 1, 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 4, 3, 4, std::vector<int>{ 2, 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 5, 3, 4, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 5, 4, 4, std::vector<int>{ 0 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 6, 4, 4, std::vector<int>{ 0, 1 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 7, 4, 4, std::vector<int>{ 1, 2 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN1, 0, 4, 4, std::vector<int>{ 2, 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 0, 4, 4, std::vector<int>{ 3 }));
	EXPECT_TRUE(ExpectNotePosition(*bms, bms::LaneCode::NOTELN2, 0, 5, 4, std::vector<int>{ 0 }));
}