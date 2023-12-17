# pragma once
# include <Siv3D.hpp>

// シーンの名前
enum class State
{
	Title,
	Game,
	Clear,
};

// 共有するデータ
struct GameData
{
	// 直前のゲームのスコア
	Optional<int32> lastGameScore;

	// ハイスコア
	//Array<int32> highScores = { 50, 40, 30, 20, 10 };

	//マスの数
	int gridNum = 11;

	//迷路データ
	int32 mazeData[11][11];

	//プレイモードかクラフトモードか
	bool isPlayMode;
};

using App = SceneManager<State, GameData>;
