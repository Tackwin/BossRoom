#pragma once

class Hitable {
public:
	virtual void hit(float dt) noexcept = 0;

};