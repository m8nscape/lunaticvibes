#pragma once
#include <SFML/Graphics.hpp>
#include <fmod.hpp>
#include <chrono>
#include <mutex>
#include <array>
#include "../../sound/sound.h"
#include "../../bms/bms.h"
#include "../scene.h"
#include "../../defs.h"
#include "../../types.hpp"
using namespace game::defs::bms;
using namespace game::defs::judge;
using std::get;

namespace game
{
	size_t keyToU(const int k);

	class showbms : public vScene
	{
	public:
		showbms() = delete;
		showbms(std::shared_ptr<Sound> pSound);
		~showbms();

	private:
		// resources
		sf::Font font;
		sf::Text text[24];
		void loadSprites() override;
		void loadKeySamples();

		// variables
		bms objBms;

		enum noteParam { MEASURE, BEAT, TIME, VALUE, HIT };
		typedef std::tuple<unsigned, fraction, double, unsigned, bool> note;
		typedef std::tuple<unsigned, fraction, double, double> subNote;

		// noteLists [key] {beat, time(ms), sample/value, hit}}
		std::array<std::list<note>, 20> noteLists;
		std::array<std::list<note>, BGMCHANNELS> bgmLists;
		std::list<subNote> bpmList;
		std::list<subNote> stopList;

		// iterators
		std::array<std::list<note>::iterator, 20> itNoteLists;
		std::array<std::list<note>::iterator, BGMCHANNELS> itBgmLists;
		std::list<subNote>::iterator itBpmList;
		std::list<subNote>::iterator itStopList;

		std::array<double, MAXMEASUREIDX + 1> measureTimeList;
		void createNoteList();

		// graphics
		double baseX = 400.0f;
		double baseY = 700.0f;
		double hispeedFactor = baseY / 350.0;
		std::array<size_t, 20> notesTextureIdx{};
		std::array<sf::Vector2f, 20> noteSizeArray{};
		void createNoteVertices(std::array<sf::VertexArray, 20>&) const;

		// control variables
		int hispeed = 100;
		std::array<note*, 10> keyNoteBinding{};
		judge_t judgeTime{};
		judgeArea judgeAreaCheck(double noteTime, double time);
		std::pair<judgeArea, double> judgeNote(note& note, double time);
		bool judgeNoteMiss(note& note, double time);

		// state variables
		unsigned drawMeasure = 0;
		double drawSeg = 0;
		double drawBPM = 0;
		double drawBasetime = 0;
		fraction drawBaseseg = fraction(0, 1);
		std::list<subNote>::iterator drawItBpm;
		std::list<subNote>::iterator drawItStop;
		std::pair<fraction, double> drawStopUntil;

		bool started = false;
		std::chrono::time_point<std::chrono::system_clock> startTime;

	protected:
		void mainLoop() override;
		void start();
	public:
		virtual void preDraw() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	};
}
