#include "showbms.h"
#include "../../input/gamepad.h"
#include "../../utils.h"
#include <cmath>
#include <filesystem>
using namespace std::chrono;

namespace game
{
	showbms::showbms(std::shared_ptr<Sound> pSound): vScene(pSound)
	{
		if (!font.loadFromFile("resources/sansation.ttf"))
			LOG_WARNING << "Load font file failed!";
		int idx = 0;
		for (auto& t : text)
		{
			t.setFont(font);
			t.setCharacterSize(24);
			t.setPosition(0, 30.0f * idx++);
			t.setFillColor(sf::Color::White);
		}

		//if (objBms.initWithFile("resources/test.bms"))
		//if (objBms.initWithFile("resources/_goodbounce_yukuri.bms"))
		if (objBms.initWithFile("resources/bms/asdf.bme"))
		{
			LOG_WARNING << "load bms failed";
			text[0].setString("load failed");
			return;
		}

		auto genre = objBms.getGenre();
		auto artist = objBms.getArtist();
		auto title = objBms.getTitle();
		auto bpm = objBms.getBPM();
		auto notes = objBms.getNoteCount();
		text[0].setString(sf::String::fromUtf32(genre.begin(), genre.end()));
		text[1].setString(sf::String::fromUtf32(artist.begin(), artist.end()));
		text[2].setString(sf::String::fromUtf32(title.begin(), title.end()));
		text[3].setString(std::to_string(bpm));
		text[4].setString(std::to_string(notes));
		text[12].setString("0");
		text[13].setString("0");
		text[14].setString("0");
		text[15].setString("0");
		text[16].setString("0");
		text[17].setString("0");
		text[18].setString("0");
		text[19].setString("0");

		// TODO in range check
		judgeTime = defs::judge::judgeTime[objBms.getJudgeRank()];

		loadSprites();
		createNoteList();
		loadKeySamples();

		LOG_DEBUG << "showbms scene created";
	}

	showbms::~showbms()
	{
		started = false;
		close();
		LOG_DEBUG << "showbms scene destroyed";
	}
	
	//////////////////////////////////////////////////////////
	// Graphic Implements
	//////////////////////////////////////////////////////////

	void showbms::loadSprites()
	{
		auto noteImageIdx = loadImage("resources/note.png");
		loadTexture(noteImageIdx, 0, 0, 32, 4);
		loadTexture(noteImageIdx, 0, 4, 32, 4);
		loadTexture(noteImageIdx, 0, 8, 32, 4);
		notesTextureIdx = { 1, 3, 2, 3, 2, 3, 2, 3, 0, 0 };
		auto noteIdx = createSprite(noteImageIdx, 0, 0, 32, 4);
		vecSprite[noteIdx].setPosition(400, 400);
		for (size_t i = 0; i < notesTextureIdx.size(); i++)
		{
			float w = static_cast<float>(vecTexture[notesTextureIdx[i]].getSize().x);
			float h = static_cast<float>(vecTexture[notesTextureIdx[i]].getSize().y);
			noteSizeArray[i] = { w, h };
		}
	}

	void showbms::preDraw()
	{
		if (!started) return;

		// Convert current time(ms) to scale-based position.

		auto relativeTime = duration_cast<milliseconds>(system_clock::now() - startTime).count();
		while (drawMeasure < objBms.getMaxMeasure() && relativeTime >= measureTimeList[drawMeasure + 1])
		{
			drawMeasure++;
			drawBaseseg = fraction(0, 1);
			drawBasetime = measureTimeList[drawMeasure];
			drawItBpm = itBpmList;
			drawItStop = itStopList;
		}

		// BPM
		while (drawItBpm != bpmList.end() && relativeTime >= get<TIME>(*drawItBpm)
			&& (drawItStop == stopList.end() || get<TIME>(*drawItBpm) <= get<TIME>(*drawItStop)))
		{
			drawBaseseg = get<BEAT>(*drawItBpm);
			drawBasetime = get<TIME>(*drawItBpm);
			drawBPM = get<VALUE>(*drawItBpm);
			drawItBpm++;
		}

		// Stop
		while (drawItStop != stopList.end() && relativeTime >= get<TIME>(*drawItStop)
			&& (drawItBpm == bpmList.end() || get<TIME>(*drawItBpm) > get<TIME>(*drawItStop)))
		{
			double stopTimeMs = 1250.0 / drawBPM * get<VALUE>(*drawItStop);
			drawBasetime += stopTimeMs;
			drawStopUntil = { get<BEAT>(*drawItStop), get<TIME>(*drawItStop) + stopTimeMs };
			drawItStop++;
		}

		if (relativeTime < drawStopUntil.second)
		{
			drawSeg = drawStopUntil.first;
		}
		else
			drawSeg = drawBaseseg + (relativeTime - drawBasetime) / (objBms.getMeasureLength(drawMeasure) * 2.4e5 / drawBPM);

		text[6].setString("#" + std::to_string(drawMeasure));
		text[7].setString(std::to_string(drawSeg * objBms.getMeasureLength(drawMeasure)));
		text[10].setString("Samples: " + std::to_string(soundSystem->getChannelsPlaying()));

	}

	void showbms::createNoteVertices(std::array<sf::VertexArray, 20>& noteVertices) const
	{
		double baseY = this->baseY;
		unsigned cm = drawMeasure;
		baseY += drawSeg * objBms.getMeasureLength(cm) * hispeed * hispeedFactor;
		decltype(itNoteLists) note = itNoteLists;

		for (unsigned m = cm; m <= objBms.getMaxMeasure(); m++)
		{
			auto mlen = objBms.getMeasureLength(m);
			for (unsigned k = 0; k < 10; k++)
			{
				while (note[k] != noteLists[k].end())
				{
					if (get<MEASURE>(*note[k]) < m) { ++note[k]; continue; }
					if (get<MEASURE>(*note[k]) > m) break;
					if (!get<HIT>(*note[k]))
					{
						double y = baseY - get<BEAT>(*note[k]) * mlen * hispeed * hispeedFactor;
						if (y < 0) break;

						sf::Vector2f align7(static_cast<float>(baseX + k * noteSizeArray[k].x + 5.0f), static_cast<float>(y));
						sf::Vector2f align1 = align7 + sf::Vector2f(noteSizeArray[k].x, 0.0f);
						sf::Vector2f align3 = align1 + sf::Vector2f(0.0f, noteSizeArray[k].y);
						sf::Vector2f align9 = align7 + sf::Vector2f(0.0f, noteSizeArray[k].y);
						noteVertices[k].append(sf::Vertex(align7, align7));
						noteVertices[k].append(sf::Vertex(align1, align1));
						noteVertices[k].append(sf::Vertex(align9, align9));
						noteVertices[k].append(sf::Vertex(align1, align1));
						noteVertices[k].append(sf::Vertex(align9, align9));
						noteVertices[k].append(sf::Vertex(align3, align3));
					}
					++note[k];
				}
			}
			baseY -= mlen * hispeed * hispeedFactor;
		}
	}

	void showbms::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		std::array<sf::VertexArray, 20> noteVertices{};
		for (unsigned k = 0; k < 10; k++)
			noteVertices[k].setPrimitiveType(sf::PrimitiveType::Triangles);
		createNoteVertices(noteVertices);
		for (auto& t: text)
			target.draw(t, states);
		for (unsigned k = 0; k < 10; k++)
		{
			auto s = states;
			s.texture = &vecTexture[notesTextureIdx[k]];
			target.draw(noteVertices[k], s);
		}
	}


	//////////////////////////////////////////////////////////
	// Functional Implements
	//////////////////////////////////////////////////////////

	void showbms::createNoteList()
	{
		double basetime = 0;
		double bpm = objBms.getBPM();
		bool bpmfucked = false;
		for (unsigned m = 0; m <= objBms.getMaxMeasure(); m++)
		{

			// notes [] {seg, {lane, sample/val}}
			std::vector<std::pair<fraction, std::pair<unsigned, unsigned>>> notes;

			// Visible Notes: 0~19
			for (unsigned i = 0; i < 10; i++)
			{
				auto ch = objBms.getChannel(bmsGetChannelCode::NOTE1, i, m);
				for (const auto& n : ch.notes)
					notes.push_back({ fraction(n.first, ch.segments), {i, n.second} });
			}

			// LN: 20~39
			// invisible: 40~59
			// mine: 60~79

			// BGM: 100 ~ 131
			for (unsigned i = 0; i < objBms.getBGMChannelCount(m); i++)
			{
				auto ch = objBms.getChannel(bmsGetChannelCode::BGM, i, m);
				for (const auto& n : ch.notes)
					notes.push_back({ fraction(n.first, ch.segments), {100 + i, n.second} });
			}

			// The following patterns must be arranged in specified order
			// to keep the process order by [Notes > BPM > Stop]

			// BPM Change: FE
			{
				auto ch = objBms.getChannel(bmsGetChannelCode::BPM, 0, m);
				for (const auto& n : ch.notes)
					notes.push_back({ fraction(n.first, ch.segments), {0xFE, n.second} });
			}

			// EX BPM: FD
			{
				auto ch = objBms.getChannel(bmsGetChannelCode::EXBPM, 0, m);
				for (const auto& n : ch.notes)
					notes.push_back({ fraction(n.first, ch.segments), {0xFD, n.second} });
			}

			// Stop: FF
			{
				auto ch = objBms.getChannel(bmsGetChannelCode::STOP, 0, m);
				for (const auto& n : ch.notes)
					notes.push_back({ fraction(n.first, ch.segments), {0xFF, n.second} });
			}

			// Sort by time / lane value
			std::sort(notes.begin(), notes.end());

			// Calculate note times and push to note list
			measureTimeList[m] = basetime;
			fraction baseseg(0, 1);
			double len = objBms.getMeasureLength(m) * 2.4e5;
			for (const auto& note : notes)
			{
				auto& seg = note.first;
				auto& lane = note.second.first;
				auto& val = note.second.second;
				double timems = bpmfucked ? INFINITY : basetime + (seg - baseseg) * len / bpm;
				if (lane >= 0 && lane < 20)		// Visible Notes
					noteLists[lane].push_back({ m, seg, timems, val, false });
				else if (lane >= 20 && lane < 40)
				{

				}
				else if (lane >= 40 && lane < 60)
				{

				}
				else if (lane >= 60 && lane < 80)
				{

				}
				else if (lane >= 100 && lane < 132)
					bgmLists[lane - 100].push_back({ m, seg, timems, val, false });

				else if (!bpmfucked) switch (lane)
				{
				case 0xFD:	// ExBPM Change
					basetime += (seg - baseseg) * len / bpm;
					baseseg = seg;
					bpm = objBms.getExBPM(val);
					bpmList.push_back({ m, seg, timems, bpm });
					if (bpm <= 0) bpmfucked = true;
					break;
				case 0xFE:	// BPM Change
					basetime += (seg - baseseg) * len / bpm;
					baseseg = seg;
					bpm = static_cast<double>(val);
					bpmList.push_back({ m, seg, timems, bpm });
					if (bpm <= 0) bpmfucked = true;
					break;
				case 0xFF:	// Stop
					double stoplen = objBms.getStop(val);
					if (stoplen <= 0) break;
					stopList.push_back({ m, seg, timems, stoplen });
					basetime += 1250.0 / bpm * stoplen;		// stoplen / 192
					break;
				}
			}
			basetime += (1.0 - baseseg) * len / bpm;
		}
		for (size_t i = 0; i < itNoteLists.size(); i++) itNoteLists[i] = noteLists[i].begin();
		for (size_t i = 0; i < itBgmLists.size(); i++) itBgmLists[i] = bgmLists[i].begin();
		itBpmList = bpmList.begin();
		itStopList = stopList.begin();

		/*
#ifdef _DEBUG
		for (size_t i = 0; i < 8; i++)
		{
			LOG_DEBUG << "Ch " << i << ":";
			for (auto& note : noteLists[i])
			{
				LOG_DEBUG << " " << get<MEASURE>(note) << "\t" << get<BEAT>(note) << "\t" << get<TIME>(note) << "\t"
					<< get<VALUE>(note);
			}
		}
#endif
*/
	}

	size_t keyToU(const int k)
	{
		size_t s = k;
		if (k >= S2L) s -= S2L;
		if (s == S1L) s++;
		return s - 1;
	}

	void showbms::loadKeySamples()
	{
		for (size_t i = 0; i <= MAXSAMPLEIDX; i++)
		{
			if (!objBms.getWavPath(i).empty())
			{
				soundSystem->loadKeySample(
					objBms.getDirectory()
					+ static_cast<char>(std::experimental::filesystem::path::preferred_separator)
					+ objBms.getWavPath(i)
					, i);
				LOG_DEBUG << "Load Sample " << i << ": " << objBms.getWavPath(i);
			}
		}
	}

	judgeArea showbms::judgeAreaCheck(double noteTime, double time)
	{
		int dTime = static_cast<int>(floor(noteTime - time));

		if (dTime > judgeTime.BPOOR)
			return judgeArea::BEFORE;
		else if (dTime > judgeTime.BAD)
			return judgeArea::BEFORE_BPOOR;
		else if (dTime > judgeTime.GOOD)
			return judgeArea::BEFORE_BAD;
		else if (dTime > judgeTime.GREAT)
			return judgeArea::BEFORE_GOOD;
		else if (dTime > judgeTime.PERFECT)
			return judgeArea::BEFORE_GREAT;
		else if (dTime >= 0)
			return judgeArea::BEFORE_PERFECT;
		else if (dTime >= -judgeTime.PERFECT)
			return judgeArea::AFTER_PERFECT;
		else if (dTime >= -judgeTime.GREAT)
			return judgeArea::AFTER_GREAT;
		else if (dTime >= -judgeTime.GOOD)
			return judgeArea::AFTER_GOOD;
		else if (dTime >= -judgeTime.BAD)
			return judgeArea::AFTER_BAD;
		else
			return judgeArea::AFTER;
	}

	std::pair<judgeArea, double> showbms::judgeNote(note& note, double time)
	{
		using area = judgeArea;
		auto noteTime = get<TIME>(note);
		auto a = judgeAreaCheck(noteTime, time);
		std::pair<judgeArea, double> ret;
		switch (a)
		{
		case area::BEFORE_BPOOR:	ret = { area::BEFORE_BPOOR, noteTime - time }; break;
		case area::BEFORE_BAD:		ret = { area::BEFORE_BAD, noteTime - time }; break;
		case area::BEFORE_GOOD:		ret = { area::BEFORE_GOOD, noteTime - time }; break;
		case area::BEFORE_GREAT:	ret = { area::BEFORE_GREAT, noteTime - time }; break;
		case area::BEFORE_PERFECT:	ret = { area::BEFORE_PERFECT, noteTime - time }; break;
		case area::AFTER_PERFECT:	ret = { area::AFTER_PERFECT, noteTime - time }; break;
		case area::AFTER_GREAT:		ret = { area::AFTER_GREAT, noteTime - time }; break;
		case area::AFTER_GOOD:		ret = { area::AFTER_GOOD, noteTime - time }; break;
		case area::AFTER_BAD:		ret = { area::AFTER_BAD, noteTime - time }; break;
		default:					ret = { area::NOTHING, 0 }; break;
		}
		// set hit
		if (ret.first != area::BEFORE_BPOOR
			&& ret.first != area::NOTHING)
			get<HIT>(note) = true;

		switch (ret.first)
		{
		case area::BEFORE_BPOOR:	LOG_DEBUG << "BEFORE_BPOOR " << ret.second; break;
		case area::BEFORE_BAD:		LOG_DEBUG << "BEFORE_BAD " << ret.second; break;
		case area::BEFORE_GOOD:		LOG_DEBUG << "BEFORE_GOOD " << ret.second; break;
		case area::BEFORE_GREAT:	LOG_DEBUG << "BEFORE_GREAT " << ret.second; break;
		case area::BEFORE_PERFECT:	LOG_DEBUG << "BEFORE_PERFECT " << ret.second; break;
		case area::AFTER_PERFECT:	LOG_DEBUG << "AFTER_PERFECT " << ret.second; break;
		case area::AFTER_GREAT:		LOG_DEBUG << "AFTER_GREAT " << ret.second; break;
		case area::AFTER_GOOD:		LOG_DEBUG << "AFTER_GOOD " << ret.second; break;
		case area::AFTER_BAD:		LOG_DEBUG << "AFTER_BAD " << ret.second; break;
		}

		return ret;
	}

	bool showbms::judgeNoteMiss(note& note, double time)
	{
		if (get<HIT>(note))
			return true;		// tricks

		auto j = judgeAreaCheck(get<TIME>(note), time);
		if (j == judgeArea::AFTER)
		{
			LOG_DEBUG << "MISS\t" << get<MEASURE>(note) << "\t" << get<BEAT>(note) << "\t";
			get<HIT>(note) = true;
			return true;
		}
		return false;
	}

	void showbms::mainLoop()
	{
		using namespace defs::general;

		if (!started)
		{
			// Hi Speed
			if (drawMeasure > 0)
				if (isFuncKeyPressed(functionalKeys::UP))
					drawMeasure -= 1;
			if (isFuncKeyPressed(functionalKeys::DOWN))
				drawMeasure += 1;
			if (isFuncKeyPressed(functionalKeys::F5))
				start();

		}
		else
		{
			auto relativeTime = duration_cast<milliseconds>(system_clock::now() - startTime).count();

			// bind sounds
			for (size_t u = 0; u < 8; u++)
			{
				if (get<HIT>(*keyNoteBinding[u]) || judgeNoteMiss(*keyNoteBinding[u], relativeTime))
				{
					// move iterator to clear miss notes
					if (itNoteLists[u] == noteLists[u].end()) continue;
					while (judgeNoteMiss(*itNoteLists[u], relativeTime) && ++itNoteLists[u] != noteLists[u].end())
					{
						keyNoteBinding[u] = &*itNoteLists[u];
						text[u + 12].setString(std::to_string(get<VALUE>(*keyNoteBinding[u])));
						//LOG_DEBUG << "Bind " << u << " To " << get<VALUE>(*keyNoteBinding[u]);
					}
				}
			}
			
			// keys 1P
			std::vector<size_t> keySamplePlayBuf_1P;
			using keys = gamepadKeys;
			for (unsigned k = keys::S1L; k <= keys::K19; k++)
			{
				size_t u = keyToU(k);
				if (isGamepadKeyPressed(static_cast<keys>(k)))
				{
					if (keyNoteBinding[u] == nullptr) continue;
					keySamplePlayBuf_1P.push_back(get<VALUE>(*keyNoteBinding[u]));
					judgeNote(*keyNoteBinding[u], relativeTime);
				}
			}
			if (!keySamplePlayBuf_1P.empty())
				soundSystem->playKeySample(keySamplePlayBuf_1P.size(), keySamplePlayBuf_1P.data());

			// bgms
			std::vector<size_t> samplePlayBuf;
			for (size_t k = 0; k < BGMCHANNELS; k++)
			{
				for (auto& note = itBgmLists[k]; note != bgmLists[k].end(); note++)
				{
					if (get<HIT>(*note)) continue;
					if (get<TIME>(*note) <= relativeTime)
					{
						get<HIT>(*note) = true;
						samplePlayBuf.push_back(get<VALUE>(*note));
					}
					else
					{
						itBgmLists[k] = note;
						break;
					}
				}
			}

			if (!samplePlayBuf.empty())
				soundSystem->playKeySample(samplePlayBuf.size(), samplePlayBuf.data());
		}

		if (isFuncKeyPressed(functionalKeys::RIGHT))
			hispeed += 25;
		if (hispeed > 25)
			if (isFuncKeyPressed(functionalKeys::LEFT))
				hispeed -= 25;
	}


	void showbms::start()
	{
		drawMeasure = 0;
		drawSeg = 0;
		drawBPM = objBms.getBPM();
		drawBasetime = 0;
		drawBaseseg = fraction(0, 1);
		drawItBpm = itBpmList;
		drawItStop = itStopList;
		drawStopUntil = { fraction(0, 1), 0 };
		for (size_t i = 0; i < 10; i++)
			if (itNoteLists[i] != noteLists[i].end())
			{
				keyNoteBinding[i] = &*itNoteLists[i];
					LOG_DEBUG << "Bind " << i << " To " << get<VALUE>(*keyNoteBinding[i]);
				text[i + 12].setString(std::to_string(get<VALUE>(*keyNoteBinding[i])));
			}
		startTime = system_clock::now();
		started = true;
	}
}