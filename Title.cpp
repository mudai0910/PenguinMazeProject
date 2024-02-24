# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene{ init }
{
	initialize();
}
void Title::initialize()
{
}
void Title::update()
{
	//ボタンの挙動
	m_readMazeTransition.update(readMazeButton.mouseOver());
	m_criateMazeTransition.update(createMazeButton.mouseOver());
	m_startTransition.update(sampleMazeButton.mouseOver());

	if (readMazeButton.mouseOver() || createMazeButton.mouseOver()|| sampleMazeButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (sampleMazeButton.leftClicked())
	{
		auto& data = getData();
		//自動生成デバック
		//MazeGenerator mazeGenerator;
		//mazeGenerator.GenerateMaze(data.gridNum);
		//mazeGenerator.DebugPrint();

		//CSVデータから迷路データを書き出す
		ReadMazeData(U"example/csv/SampleMaze.csv");
	}
	else if (readMazeButton.leftClicked())
	{
		// ダイアログを表示
		Optional<FilePath> path = Dialog::OpenFile({ FileFilter::CSV() }, FileSystem::FullPath(U"example/MazeDataFiles/"), U"迷路のCSVファイルを選択してください");
		if (path)
		{
			//CSVデータから迷路データを書き出す
			ReadMazeData(*path);
		}
		else
		{
			System::MessageBoxOK(U"キャンセルされました");
		}

	}
	else if (createMazeButton.leftClicked())
	{
		auto& data = getData();
		//迷路データを初期化する
		for (int x = 0; x < data.gridNum; x++)
		{
			for (int y = 0; y < data.gridNum; y++)
			{
				data.mazeData[x][y] = 0;
			}
		}
		data.isPlayMode = false;
		// ゲームシーンへ
		changeScene(State::Game);
	}
}
void Title::draw() const
{
	Scene::SetBackground(ColorF{ 0.5, 0.7, 1 }.removeSRGBCurve());

	FontAsset(U"TitleFont")(U"ペンギンめいず")
		.drawAt(TextStyle::OutlineShadow(0.2, ColorF{ 0.4, 0.6, 0.8 }, Vec2{ 3, 3 }, ColorF{ 0.0, 0.5 }), 100, Vec2{ 400, 100 });

	readMazeButton.draw(ColorF{ 1.0, m_readMazeTransition.value() }).drawFrame(2);
	createMazeButton.draw(ColorF{ 1.0, m_criateMazeTransition.value() }).drawFrame(2);
	sampleMazeButton.draw(ColorF{ 1.0, m_startTransition.value() }).drawFrame(2);

	FontAsset(U"Menu")(U"めいろをえらぶ").drawAt(readMazeButton.center(), ColorF{ 0.25 });
	FontAsset(U"Menu")(U"めいろをつくる").drawAt(createMazeButton.center(), ColorF{ 0.25 });
	FontAsset(U"Menu")(U"おためしめいろ").drawAt(sampleMazeButton.center(), ColorF{ 0.25 });
}

void Title::ReadMazeData(FilePath filepath)
{
	//迷路データ読み込み
	auto& data = getData();

	//読み込みエラーフラグ;
	bool isReadError = false;

	// CSV ファイルからデータを読み込む
	const CSV csv{ filepath};

	if (not csv) // 読み込みに失敗したら
	{
		isReadError = true;
	}
	else // 全ての行を列挙
	{
		//行サイズが正しいかチェック
		if (csv.rows() != data.gridNum)
		{
			isReadError = true;
		}
		//要素を1つずつみていく
		for (size_t row = 0; row < csv.rows(); ++row)
		{
			//列サイズが正しいかチェック
			if (csv.columns(row) != data.gridNum)
			{
				isReadError = true;
			}

			for (size_t column = 0; column < csv.columns(row); ++column)
			{
				// int変換に失敗したときは-1を返す
				int32 blockheight = ParseOr<int32>(csv[row][column], -1);
				if (blockheight < 0)
				{
					isReadError = true;
				}
				else
				{
					data.mazeData[row][column] = blockheight;
					//データを追加
				}
			}
		}
	}
	//読み込みエラーがなかったら
	if (isReadError == false)
	{
		data.isPlayMode = true;
		// ゲームシーンへ
		changeScene(State::Game);
	}
	else
	{
		//読み込み失敗したことを通知
		System::MessageBoxOK(U"読み込みに失敗しました。");
	}
}
//void Title::GenerateMaze(int* mazeData)
//{
//
//}
