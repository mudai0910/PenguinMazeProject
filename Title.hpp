# pragma once
# include "Common.hpp"
# include "MazeGenerator.hpp"

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init);

	void initialize();

	void update() override;

	void draw() const override;

private:


	Rect readMazeDataButton{ Arg::center = Scene::Center().movedBy(0, 0), 300, 60 };
	Transition m_exitTransition{ 0.4s, 0.2s };

	Rect createMazeButton{ Arg::center = Scene::Center().movedBy(0, 100), 300, 60 };
	Transition m_rankingTransition{ 0.4s, 0.2s };

	Rect sampleMazeButton{ Arg::center = Scene::Center().movedBy(0, 200), 300, 60 };
	Transition m_startTransition{ 0.4s, 0.2s };

	//迷路データを読み込む
	void ReadMazeData(FilePath filepath);
	//ランダムに迷路を生成する
	void GenerateMaze(int* mazeData);
	
};
