# pragma once
# include "Common.hpp"

// クリアシーン
class Clear : public App::Scene
{
public:

	Clear(const InitData& init);

	void update() override;

	void draw() const override;

private:

	static constexpr int32 ClearCount = 5;

	int32 m_rank = -1;
};
