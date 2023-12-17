# include "MazeGenerator.hpp"

MazeGenerator::MazeGenerator()
{
	initialize();
}
void MazeGenerator::initialize()
{
	// 5未満のサイズや偶数では生成できない
	//if (width < 5 || height < 5) throw new ArgumentOutOfRangeException();
	//if (size % 2 == 0) size++;

	// 迷路情報を初期化
	size=0;
	//this.Height = height;
	//Maze = new int[width, height];
	//StartCells = new List<Cell>();
	//CurrentWallCells = new Stack<Cell>();
}

void MazeGenerator::GenerateMaze(int mazeSize)
{
	//迷路のサイズを取得(ステージマスの数*2+1)
	size = mazeSize * 2 + 1;
	// 迷路情報をセット
	cellArray.resize(size, size, 0);

	// 各マスの初期設定を行う
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			// 外周のみ壁にしておき、開始候補として保持
			if (x == 0 || y == 0 || x == size - 1 || y == size - 1)
			{
				//★壁データはmazeDataには含まない
				cellArray[x][y] = Wall;
			}
			else
			{
				//★Pathの値は隣のマスを調べて決定する
				cellArray[x][y] = Path;
				// 外周ではない偶数座標を壁伸ばし開始点にしておく
				if (x % 2 == 0 && y % 2 == 0)
				{
					// 開始候補座標を追加
					startCells.push_back(Point(x,y));
				}
			}
		}
	}

	// 壁が拡張できなくなるまでループ
	while (startCells.size() > 0)
	{
		// ランダムに開始セルを取得し、開始候補から削除
		Point cell = startCells.choice();
		startCells.remove(cell);

		// すでに壁の場合は何もしない
		if (cellArray[cell.x][cell.y] == Path)
		{
			// 拡張中の壁情報を初期化
			currentWallCells.clear();
			ExtendWall(cell.x, cell.y);
		}
	}

	//★mazeDataに値をセット
	
}

void MazeGenerator::ExtendWall(int x, int y)
{
	// 伸ばすことができる方向(1マス先が通路で2マス先まで範囲内)
	// 2マス先が壁で自分自身の場合伸ばせない
	Array<Direction> directions;
	if (cellArray[x][ y - 1] == Path && !IsCurrentWall(x, y - 2))
		directions.push_back(Up);
	if (cellArray[x + 1][y] == Path && !IsCurrentWall(x + 2, y))
		directions.push_back(Right);
	if (cellArray[x][ y + 1] == Path && !IsCurrentWall(x, y + 2))
		directions.push_back(Down);
	if (cellArray[x - 1][y] == Path && !IsCurrentWall(x - 2, y))
		directions.push_back(Left);

	// ランダムに伸ばす(2マス)
	if (directions.size() > 0)
	{
		// 壁を作成(この地点から壁を伸ばす)
		SetWall(x, y);

		// 伸ばす先が通路の場合は拡張を続ける
		bool isPath = false;
		//方向をランダムに設定
		Direction dirIndex = directions.choice();
		switch (dirIndex)
		{
		case Up:
			isPath = (cellArray[x][y - 2] == Path);
			SetWall(x, --y);
			SetWall(x, --y);
			break;
		case Right:
			isPath = (cellArray[x + 2][y] == Path);
			SetWall(++x, y);
			SetWall(++x, y);
			break;
		case Down:
			isPath = (cellArray[x][y + 2] == Path);
			SetWall(x, ++y);
			SetWall(x, ++y);
			break;
		case Left:
			isPath = (cellArray[x - 2][y] == Path);
			SetWall(--x, y);
			SetWall(--x, y);
			break;
		}
		if (isPath)
		{
			//既存の壁に接続できていない場合は拡張続行
			ExtendWall(x, y);
		}
	}
	else
	{
		// すべて現在拡張中の壁にぶつかる場合、バックして再開
		Point beforeCell = currentWallCells.front();
		currentWallCells.pop_back();
		ExtendWall(beforeCell.x, beforeCell.y);
	}
}
// 壁を拡張する
void MazeGenerator::SetWall(int x, int y)
{
	cellArray[x][y] = Wall;
	if (x % 2 == 0 && y % 2 == 0)
	{
		currentWallCells.push_back(Point(x, y));
	}
}
// 拡張中の座標かどうか判定
bool MazeGenerator::IsCurrentWall(int x, int y)
{
	return currentWallCells.contains(Point(x, y));
}
//高さを含む迷路データを作成
void MazeGenerator::SetMazeData(int mazeSize)
{
	mazeArray.resize(mazeSize, mazeSize, 0);
	//★スタートとゴールの位置と高さをセット


	
	//★壁をまたぐ通路は2マス以上離す

	//★隣り合う通路は1マス以内にする


}

// デバッグ用処理
 void MazeGenerator::DebugPrint()
{
	for (int y = 0; y < cellArray.width(); y++)
	{
		String test = U"";
		for (int x = 0; x < cellArray.height(); x++)
		{
			test+=(cellArray[x][y] == Wall ? U"■" : U"　");
		}
		Print<<test;
	}
}




	
