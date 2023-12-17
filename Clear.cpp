# include "Clear.hpp"

Clear::Clear(const InitData& init)
	: IScene{ init }
{

}

void Clear::update()
{
	if (MouseL.down())
	{
		// タイトルシーンへ
		changeScene(State::Title);
	}
}

void Clear::draw() const
{
	Scene::SetBackground(ColorF{ 0.5, 0.7, 1 }.removeSRGBCurve());

	FontAsset(U"TitleFont")(U"クリア！！")
		.drawAt(TextStyle::OutlineShadow(0.2, ColorF{ 0.4, 0.6, 0.8 }, Vec2{ 3, 3 }, ColorF{ 0.0, 0.5 }), 100, Vec2{ 400, 100 });
}
