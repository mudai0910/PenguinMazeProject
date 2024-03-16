# include "Editor.hpp"

Editor::Editor(const InitData& init)
	: IScene{ init }
{
	//初期化
	initialize();
}
void Editor::initialize()
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

	sealGrid.set(-3, 5);
	birdGrid.set(7, -3);
	penguinGrid.set(6, 5);
	goalGrid.set(4, 4);
	activeGrid.set(-1, -1);

	heightArray[penguinGrid.x][penguinGrid.y] = 1; //ペンギンブロック
	heightArray[goalGrid.x][goalGrid.y] = 5; //ゴールブロック

	//最初はブロック追加モード
	isDeleteMode = false;

}

void Editor::update()
{
	//ボタンの挙動

	addModeTransition.update(addModeButton.mouseOver());
	deleteModeTransition.update(deleteModeButton.mouseOver());
	decideTransition.update(decideButton.mouseOver());
	restartTransition.update(restartButton.mouseOver());
	exitTransition.update(exitButton.mouseOver());

	//ボタンにマウスカーソルを置いたときにカーソルの見た目を変化させる
	if (addModeButton.mouseOver() || deleteModeButton.mouseOver() ||decideButton.mouseOver()|| restartButton.mouseOver() || exitButton.mouseOver())
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
					const Box box = MakeBox(x, height, z, gridNum);

					//マウスカーソルから出たレイがボックスと交差していたら
					if (Optional<float> distance = box.intersects(ray))
					{
						if (*distance < minDistance)
						{
							minDistance = *distance;

							//ペンギンがいないブロックのみ操作可能
							if ((x != penguinGrid.x || z != penguinGrid.y) && (x != goalGrid.x || z != goalGrid.y))
							{
								if ((isDeleteMode == false && GetHeight(Point{ x,z }) <= MAXHEIGHT) || (isDeleteMode == true && GetHeight(Point{ x,z }) > 0))
								{
									activeGrid.set(x, z);
								}
							}
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
					//削除モードならすべて消す，追加モードなら1つ増やす
					int num = isDeleteMode ? -(GetHeight(activeGrid)) : 1;
					AddBlock(activeGrid, num);
				}
			}
		}

		cameraController.update();
	}

	//ボタンを押したときの挙動
	if (addModeButton.leftClicked())
	{
		isDeleteMode = false;
	}
	if (deleteModeButton.leftClicked())
	{
		isDeleteMode = true;
	}
	if (decideButton.leftClicked())
	{
		//CSVデータの書き出し処理

		CSV csv;
		//1マスずつ書き出す
		for (int x = 0; x < gridNum; x++)
		{
			for (int y = 0; y < gridNum; y++)
			{
				csv.write(heightArray[x][y]);
			}
			csv.newLine();
		}
		// ダイアログを表示
		Optional<FilePath> path = Dialog::SaveFile({ FileFilter::CSV() }, FileSystem::FullPath(U"example/MazeDataFiles/"), U"迷路に名前を付けてください");
		if (path)
		{
			// 保存
			csv.save(*path);
			// OK のメッセージボックスを表示する
			System::MessageBoxOK(U"保存されました！");

		}
		else
		{
			System::MessageBoxOK(U"キャンセルされました");
		}
		//タイトルに戻る
		changeScene(State::Title);
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

void Editor::draw() const
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
	DrawEditor(blockColor);

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
		//追加OR削除予定のブロックの描画
		MakeBox(activeGrid.x, heightArray[activeGrid.x][activeGrid.y], activeGrid.y, gridNum, !(isDeleteMode)).draw(highlightColor);

	}

	// ペンギンの描画
	DrawAnimal(penguin, penguinGrid,0);
	DrawAnimal(babyPenguin, goalGrid, 90);

	Graphics3D::Flush();
	renderTexture.resolve();
	Shader::LinearToScreen(renderTexture);

	////////////////////////////////
	//
	//	2D 描画
	//
	////////////////////////////////

	 //クラフトモード用のボタン
	addModeButton.draw(ColorF{ 1.0,addModeTransition.value() }).drawFrame(2);
	deleteModeButton.draw(ColorF{ 1.0,deleteModeTransition.value() }).drawFrame(2);
	decideButton.draw(ColorF{ 1.0,decideTransition.value() }).drawFrame(2);
	FontAsset(U"Menu")(U"ブロック追加").drawAt(30, addModeButton.center(), ColorF{ 0.25 });
	FontAsset(U"Menu")(U"ブロック削除").drawAt(30, deleteModeButton.center(), ColorF{ 0.25 });
	FontAsset(U"Menu")(U"かんせい!").drawAt(30, decideButton.center(), ColorF{ 0.25 });

	restartButton.draw(ColorF{ 1.0,restartTransition.value() }).drawFrame(2);
	exitButton.draw(ColorF{ 1.0,exitTransition.value() }).drawFrame(2);
	FontAsset(U"Menu")(U"やりなおす").drawAt(30, restartButton.center(), ColorF{ 0.25 });
	FontAsset(U"Menu")(U"タイトルに戻る").drawAt(30, exitButton.center(), ColorF{ 0.25 });

}

// テーブルを描画する関数
void Editor::DrawTable(const ColorF color) const
{
	// 半透明を有効に
	const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };
	Plane{ Vec3{ 0, -0.4, 0 }, 50 }.draw(seaColor);
}

// 盤を描画する関数
void Editor::DrawBoard(const Mesh& mesh, int gridNum)const
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
void Editor::DrawEditor(ColorF color)const
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
void Editor::DrawAnimal(Model model, Point point, int direction)const
{
	//動物モデルの位置調整用
	int offset = gridNum / 2 - 1;
	int height = GetHeight(point) > 0 ? GetHeight(point) : 1;
	model.draw(Vec3{ -offset + point.x - 0.5,height,offset - point.y + 0.5 }, Quaternion::RotateY(direction * 1_deg));
}

// ブロックを作成する関数
//1ブロックを作成するかをオプションで選べる
Box Editor::MakeBox(int32 x, int32 y, int32 z, int gridNum, bool isunit)const
{
	int gridhalf = gridNum / 2;
	if (isunit)
	{
		return Box::FromPoints(Vec3{ (x - gridhalf),y + 1, (gridhalf - z) }, Vec3{ (x - gridhalf + 1), y, (gridhalf - 1 - z) });
	}
	return Box::FromPoints(Vec3{ (x - gridhalf),y, (gridhalf - z) }, Vec3{ (x - gridhalf + 1), -1, (gridhalf - 1 - z) });
}

// 進行可能なマスを示すハイライト用のGridを作成する関数
Box Editor::MakeGrid(int32 x, int32 y, int32 z, int gridNum)const
{
	int gridhalf = gridNum / 2;
	return Box::FromPoints(Vec3{ (x - gridhalf),y + 0.01, (gridhalf - z) }, Vec3{ (x - gridhalf + 1), y, (gridhalf - 1 - z) });
}

