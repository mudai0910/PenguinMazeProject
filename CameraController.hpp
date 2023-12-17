# pragma once
class CameraController
{
public:

	explicit CameraController(const Size& sceneSize);
	void update();

	Ray getMouseRay() const;
	bool isGrabbing() const;

private:

	// 縦方向の視野角（ラジアン）
	//double m_verticalFOV = 25_deg;
	double m_verticalFOV = 25_deg;
	// カメラの視点の位置
	Vec3 m_eyePosition{ 16, 16, -16 };
	// カメラの注視点の位置
	Vec3 m_focusPosition{ 0, 0, 0 };
	double m_phi = -20_deg;
	double m_theta = 50_deg;
	// カメラ
	BasicCamera3D m_camera;
	bool m_grabbed = false;
	Vec2 m_cursorPos = Scene::Center();
};

