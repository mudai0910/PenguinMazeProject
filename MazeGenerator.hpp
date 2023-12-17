# pragma once
# include <Siv3D.hpp>

// タイトルシーン
class MazeGenerator 
{
public:

	MazeGenerator();
	void initialize();

	//ランダムに迷路を生成する
	void GenerateMaze(int gridSize);
	//デバッグ用
	void DebugPrint();
	// 2次元配列の迷路情報
	Grid<int32> mazeArray;

private:

	int size; //迷路のサイズ
	// 通路・壁情報
	const int Path = 1;
	const int Wall = 0;

	//壁を伸ばす方向
	enum Direction
	{
		Up,
		Right,
		Down,
		Left
	};

	//2次元配列のセル情報
	Grid<int32> cellArray;

	// 現在拡張中の壁情報を保持
	Array<Point> currentWallCells;
	// 壁の拡張を行う開始セルの情報
	Array<Point> startCells;

	void ExtendWall(int x, int y);
	void SetWall(int x, int y);
	bool IsCurrentWall(int x, int y);
	void SetMazeData(int mazeSize);
};
