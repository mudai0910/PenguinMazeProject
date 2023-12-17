# include "Common.hpp"
# include "Title.hpp"
# include "Game.hpp"
# include "Clear.hpp"

void Main()
{
	// ウインドウとシーンを 1280x720 にリサイズする
	Window::Resize(1280, 720);

	FontAsset::Register(U"TitleFont", FontMethod::MSDF, 50, U"example/font/RocknRoll/RocknRollOne-Regular.ttf");
	FontAsset(U"TitleFont").setBufferThickness(4);
	FontAsset::Register(U"Menu", FontMethod::MSDF, 40, Typeface::Medium);
	FontAsset::Register(U"Clear", 40, Typeface::Heavy);
	FontAsset::Register(U"GameScore", 30, Typeface::Light);
	AudioAsset::Register(U"Brick", GMInstrument::Woodblock, PianoKey::C5, 0.2s, 0.1s);

	App manager;
	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);
	manager.add<Clear>(State::Clear);

	// ゲームシーンから開始したい場合はこのコメントを外す
	//manager.init(State::Game);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
