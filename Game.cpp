# include "Game.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
{
	//初期化
	initialize();
}
void Game::initialize()
{
	auto& data = getData();
	//迷路データをロードする
	for (int x = 0; x < gridNum; x++)
	{
		for (int y = 0; y < gridNum; y++)
		{
			heightArray[x][y] = data.mazeData[x][y];
		}
	}

	//環境光を設定する
	Graphics3D::SetGlobalAmbientColor(ColorF{ 0.75, 0.75, 0.75 });

	//太陽光を設定する
	Graphics3D::SetSunColor(ColorF{ 0.5, 0.5, 0.5 });

	//太陽の方向を設定する
	Graphics3D::SetSunDirection(Vec3{ 0, 1, -0.3 }.normalized());

	//それぞれのマス位置と高さの初期化

	sealGrid.set(-3,5);
	birdGrid.set(7,-3);
	penguinGrid.set(6, 5);
	goalGrid.set(4,4);
	activeGrid.set(-1, -1);

	heightArray[penguinGrid.x][penguinGrid.y] = 1; //ペンギンブロック
	heightArray[goalGrid.x][goalGrid.y] = 5; //ゴールブロック
	//ペンギンの向きの初期化
	penguinDir=0;

}

void Game::update()
{
	//ボタンの挙動
	restartTransition.update(restartButton.mouseOver());
	exitTransition.update(exitButton.mouseOver());

	if (restartButton.mouseOver()||exitButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	// アクティブ状態の初期化
	activeGrid.set(-1, -1);
	////////////////////////////////
	//
	//	盤面の更新
	//
	////////////////////////////////
	{
		//盤面をつかんでいない状態だったら
		if (not cameraController.isGrabbing())
		{
			const Ray ray = cameraController.getMouseRay();
			float minDistance = 99999.9f;
			//各マスでループ
			for (int32 x = 0; x < gridNum; ++x)
			{
				for (int32 z = 0; z < gridNum; ++z)
				{
					//ブロックの高さを取得
					const int32 height = GetHeight(Point{ x,z });

					//アクティブのブロック位置にboxを作成
					const Box box = MakeGrid(x, height, z, gridNum);

					//マウスカーソルから出たレイがボックスと交差していたら
					if (Optional<float> distance = box.intersects(ray))
					{
						if (*distance < minDistance)
						{
							minDistance = *distance;
							//ペンギンの移動範囲を選択できるようにする
							if (Point{ x,z } == leftGrid || Point{ x, z } == rightGrid || Point{ x, z } == upperGrid || Point{ x, z } == lowerGrid)activeGrid.set(x, z);
						}
					}
				}
			}
			//マスがフォーカスされていたら
			if (activeGrid != Point{ -1, -1 })
			{
				//左クリックで選択状態にする
				//選択状態で配置可能なマスをクリックしたら移動する
				if (MouseL.down())
				{
					//クリックした場所にペンギンを移動する
					penguinGrid = activeGrid;
					//ペンギンの向きを変える
					if (penguinGrid == leftGrid)penguinDir = 180;
					else if (penguinGrid == rightGrid)penguinDir = 0;
					else if (penguinGrid == upperGrid)penguinDir = -90;
					else if (penguinGrid == lowerGrid)penguinDir = 90;
				}
			}
		}

		cameraController.update();
	}

	////////////////////////////////
	//
	//	ペンギンの位置の更新
	//
	////////////////////////////////
	//☆関数化する
	{
		//四方のマスをリセット
		leftGrid = { penguinGrid.x - 1, penguinGrid.y };
		rightGrid = { penguinGrid.x + 1, penguinGrid.y };
		upperGrid = { penguinGrid.x, penguinGrid.y - 1 };
		lowerGrid = { penguinGrid.x, penguinGrid.y + 1 };

		//★ゴールマスの隣かつ高さが同じならゲームクリア！
		if ((((abs(penguinGrid.x - goalGrid.x) <= 1 && abs(penguinGrid.y - goalGrid.y) == 0)) || ((abs(penguinGrid.x - goalGrid.x) == 0 && abs(penguinGrid.y - goalGrid.y) <= 1)))
			&& GetHeight(penguinGrid) == GetHeight(goalGrid))
		{
			//シーン遷移
			changeScene(State::Clear);
		}

		//移動可能なマスの表示(現在のブロックと移動先のブロックの高低差が1以内)

		if (GetHeight(leftGrid) <= 0 || abs(GetHeight(penguinGrid) - GetHeight(leftGrid)) > 1
			|| leftGrid == goalGrid)leftGrid = Point{ -1, -1 };
		if (GetHeight(rightGrid) <= 0 || abs(GetHeight(penguinGrid) - GetHeight(rightGrid)) > 1
			|| rightGrid == goalGrid)rightGrid = Point{ -1, -1 };
		if (GetHeight(upperGrid) <= 0 || abs(GetHeight(penguinGrid) - GetHeight(upperGrid)) > 1
			|| upperGrid == goalGrid)upperGrid = Point{ -1, -1 };
		if (GetHeight(lowerGrid) <= 0 || abs(GetHeight(penguinGrid) - GetHeight(lowerGrid)) > 1
			|| lowerGrid == goalGrid)lowerGrid = Point{ -1, -1 };
	}

	if (restartButton.leftClicked())
	{
		//ペンギンの位置と向きをもどす
		initialize();
	}
	if (exitButton.leftClicked())
	{
		//タイトルに戻る
		changeScene(State::Title);
	}

}

void Game::draw() const
{
	//////////////////////////////////
	//
	//	3D 描画
	//
	////////////////////////////////
	
	//renderTexture を背景色で塗りつぶし、3D 描画のレンダーターゲットに
	const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

	DrawTable(seaColor);
	DrawBoard(meshBoard, gridNum);
	DrawGame(blockColor);

	//背景ブロックと動物の描画
	MakeBox(sealGrid.x, 1, sealGrid.y, gridNum).draw(fixedBlockColor);
	DrawAnimal(seal, sealGrid, 0);
	MakeBox(birdGrid.x, 1, birdGrid.y, gridNum).draw(fixedBlockColor);
	DrawAnimal(bird, birdGrid, 90);

	//ハイライトの描画
	if (activeGrid != Point{ -1, -1 })
	{
		// 半透明を有効に
		const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };
		//ペンギンが移動するグリッドの描画
		MakeGrid(activeGrid.x, heightArray[activeGrid.x][activeGrid.y], activeGrid.y, gridNum).draw(highlightColor);

	}

	// ペンギンの描画
	DrawAnimal(penguin, penguinGrid, penguinDir);
	DrawAnimal(babyPenguin, goalGrid, 90);

	Graphics3D::Flush();
	renderTexture.resolve();
	Shader::LinearToScreen(renderTexture);	

	////////////////////////////////
	//
	//	2D 描画
	//
	////////////////////////////////

	restartButton.draw(ColorF{ 1.0,restartTransition.value() }).drawFrame(2);
	exitButton.draw(ColorF{ 1.0,exitTransition.value() }).drawFrame(2);
	FontAsset(U"Menu")(U"やりなおす").drawAt(30,restartButton.center(), ColorF{ 0.25 });
	FontAsset(U"Menu")(U"タイトルに戻る").drawAt(30,exitButton.center(), ColorF{ 0.25 });
	
}

// テーブルを描画する関数
void Game::DrawTable(const ColorF color) const
{
	// 半透明を有効に
	const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };
	Plane{ Vec3{ 0, -0.4, 0 }, 50 }.draw(seaColor);
}

// 盤を描画する関数
void Game::DrawBoard(const Mesh& mesh, int gridNum)const
{
	// 半透明を有効に
	const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };
	const ColorF BoardColor = ColorF{ 0.7, 0.9, 0.9 }.removeSRGBCurve();
	const ColorF LineColor = ColorF{ 0.1, 0.2, 0.2 };

	mesh.draw(BoardColor);

	// 盤上の線
	for (int32 i = -gridNum / 2; i <= gridNum / 2; ++i)
	{
		Line3D{ Vec3{ -gridNum / 2, 0.01, i }, Vec3{ gridNum / 2, 0.01, i } }.draw(LineColor);
		Line3D{ Vec3{ i, 0.01, -gridNum / 2 }, Vec3{ i, 0.01, gridNum / 2 } }.draw(LineColor);
	}
}

// ゲームの状態に基づいてブロックを描く関数
void Game::DrawGame(ColorF color)const
{
	// 半透明を有効に
	const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };

	for (int32 x = 0; x < gridNum; ++x)
	{
		for (int32 z = 0; z < gridNum; ++z)
		{
			//ブロックが配置されていたら
			if (heightArray[x][z] > 0)
			{
				MakeBox(x, heightArray[x][z], z, gridNum).draw(color);
			}
		}
	}
}
//動物モデルの描画
void Game::DrawAnimal(Model model, Point point, int direction)const
{
	//動物モデルの位置調整用
	int offset = gridNum / 2 - 1;
	int height = GetHeight(point) > 0 ? GetHeight(point) : 1;
	model.draw(Vec3{ -offset + point.x - 0.5,height,offset - point.y + 0.5 }, Quaternion::RotateY(direction * 1_deg));
}

// ブロックを作成する関数
//1ブロックを作成するかをオプションで選べる
Box Game::MakeBox(int32 x, int32 y, int32 z, int gridNum, bool isunit)const
{
	int gridhalf = gridNum / 2;
	if (isunit)
	{
		return Box::FromPoints(Vec3{ (x - gridhalf),y + 1, (gridhalf - z) }, Vec3{ (x - gridhalf + 1), y, (gridhalf - 1 - z) });
	}
	return Box::FromPoints(Vec3{ (x - gridhalf),y, (gridhalf - z) }, Vec3{ (x - gridhalf + 1), -1, (gridhalf - 1 - z) });
}

// 進行可能なマスを示すハイライト用のGridを作成する関数
Box Game::MakeGrid(int32 x, int32 y, int32 z, int gridNum)const
{
	int gridhalf = gridNum / 2;
	return Box::FromPoints(Vec3{ (x - gridhalf),y + 0.01, (gridhalf - z) }, Vec3{ (x - gridhalf + 1), y, (gridhalf - 1 - z) });
}

