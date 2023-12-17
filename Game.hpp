# pragma once
# include "Common.hpp"
# include "CameraController.hpp"

// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	void initialize();

	void update() override;

	void draw() const override;

private:

	//盤面のマスの数
	const int gridNum=12;
	//ブロック高さの最大
	static constexpr int32 MaxHeight = 5;

	//各マスに配置されているブロックの高さを表す二次元配列
	int32 h[10][10] ;

	//現在積みあがっているブロックの数を返す
	int32 getHeight(Point point) const
	{
		//配列の要素を超えている場合
		if (point.x<0 || point.x>gridNum || point.y<0 || point.y>gridNum)return -1;
		//ブロックが配置されていたら
		return h[point.x][point.y];
	}
	//指定のマスにブロックを追加するOR削除する
	void addBlock(Point point, int blockNum)
	{
		h[point.x][point.y] += blockNum;
	}

	// 背景色 (3D 用の色は .removeSRGBCurve() で sRGB カーブを除去）
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	//ボードの色
	const ColorF BoardColor = ColorF{ 0.7, 0.9, 0.9 }.removeSRGBCurve();
	//ボードのラインの色
	const ColorF LineColor = ColorF{ 0.1, 0.2, 0.2 };
	//海の色
	const ColorF seaColor = ColorF{ 0, 0.2, 1,0.8 };
	//ブロックの色
	const ColorF blockColor = ColorF{ 0.7, 0.9, 0.9,0.9 };
	//固定ブロックの色
	const ColorF fixedBlockColor = ColorF{ 0.7, 0.9, 0.9, 0.8 };
	//ハイライトの色
	const ColorF moveGridColor = ColorF{ 0.8, 0.8, 0.2, 0.8 };
	// 3D シーンを描く、マルチサンプリング対応レンダーテクスチャ
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	// 盤用の 3D メッシュ
	const Mesh meshBoard{ MeshData::RoundedBox(0.1, Vec3{ gridNum + 2, 5, gridNum + 2 }, 5).translate(0, -2.5, 0) };
	// カメラ制御
	CameraController cameraController{ renderTexture.size() };

	// 動物モデルをロード
	const Model penguin{ U"example/obj/Penguin.obj" }; //ペンギン（操作キャラ）
	const Model babyPenguin{ U"example/obj/BabyPenguin.obj" }; //子ペンギン（ゴール）
	const Model seal{ U"example/obj/seal.obj" }; //アザラシ（背景キャラ）
	const Model bird{ U"example/obj/bird.obj" }; //鳥（背景キャラ）

	//背景ブロックの位置
	Point sealGrid;
	Point birdGrid;

	//ペンギンとゴールの初期位置
	Point penguinGrid;
	Point goalGrid;

	//カーソル上にあるマス
	Point activeGrid;

	//ブロックの初期配置
	Array<Point>fixedGrids{};

	//ペンギンマスの四方の移動可能マス
	Point leftGrid{ -1,-1 };
	Point rightGrid{ -1,-1 };
	Point upperGrid{ -1,-1 };
	Point lowerGrid{ -1,-1 };

	//プレイモードか否か
	bool isPlayMode;
	//ブロック削除モードか否か
	bool isDeleteMode;
	//ペンギンの向き
	int penguinDir;

	//UI関連
	Rect addModeButton{ 1050,180, 220, 45 }; //追加モードに切り替え
	Rect deleteModeButton{ 1050,260, 220, 45 }; //削除モードに切り替え
	Rect decideButton{ 1050,420, 220, 45 }; //迷路を作成するボタン
	Rect restartButton{ 1050,500, 220, 45 }; //やりなおすボタン
	Rect exitButton{ 1050,580, 220, 45 }; //タイトルに戻るボタン

	Transition addModeTransition{ 0.4s, 0.2s };
	Transition deleteModeTransition{ 0.4s, 0.2s };
	Transition decideTransition{ 0.4s, 0.2s };
	Transition restartTransition{ 0.4s, 0.2s };
	Transition exitTransition{ 0.4s, 0.2s };

	//描画関連
	void DrawTable(const ColorF color)const;
	void DrawBoard(const Mesh& mesh, int gridNum)const;
	void DrawGame(ColorF color)const;
	void DrawAnimal(Model model, Point point, int direction)const;

	//ブロックの生成
	Box MakeBox(int32 x, int32 y, int32 z, int gridNum, bool isunit = false)const;
	Box MakeGrid(int32 x, int32 y, int32 z, int gridNum)const;
};
