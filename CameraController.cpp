# include "CameraController.hpp"

CameraController::CameraController(const Size& sceneSize)
	: m_camera{ sceneSize, m_verticalFOV, m_eyePosition, m_focusPosition } {}

void CameraController::update()
{
	const Ray ray = getMouseRay();
	//1辺の数の半分(gameの参照を持たせた方が良いかも)
	int halfgrid = 5;

	// 盤のまわり部分
	const std::array<Box, 4> boxes =
	{
		Box::FromPoints(Vec3{ -halfgrid - 1, 0.0, halfgrid }, Vec3{ halfgrid, -0.4, halfgrid }),
		Box::FromPoints(Vec3{ halfgrid , 0.0, halfgrid + 1 }, Vec3{ halfgrid + 1, -0.4, -halfgrid + 1 }),
		Box::FromPoints(Vec3{ -halfgrid - 1, 0.0, -halfgrid }, Vec3{ halfgrid + 1, -0.4, -halfgrid - 1 }),
		Box::FromPoints(Vec3{ -halfgrid - 1, 0.0, halfgrid + 1 }, Vec3{ -halfgrid, -0.4, -halfgrid - 1 })
	};

	if (MouseL.up())
	{
		m_grabbed = false;
	}

	if (m_grabbed)
	{
		const double before = (m_cursorPos - Scene::Center()).getAngle();
		const double after = (Cursor::Pos() - Scene::Center()).getAngle();
		m_phi -= (after - before);
		m_cursorPos = Cursor::Pos();
	}

	for (const auto& box : boxes)
	{
		if (box.intersects(ray))
		{
			// マウスカーソルを手のアイコンにする
			Cursor::RequestStyle(CursorStyle::Hand);

			if ((not m_grabbed) && MouseL.down())
			{
				m_grabbed = true;
				m_cursorPos = Cursor::Pos();
			}
		}
	}

	// 視点を球面座標系で計算する(第一引数がカメラの距離)
	m_eyePosition = Spherical{ 30, m_theta, (270_deg - m_phi) };

	// カメラを更新する
	m_camera.setView(m_eyePosition, m_focusPosition);

	// シーンにカメラを設定する
	Graphics3D::SetCameraTransform(m_camera);
	
}

Ray CameraController::getMouseRay() const
{
	return m_camera.screenToRay(Cursor::PosF());
}

bool CameraController::isGrabbing() const
{
	return m_grabbed;
}
