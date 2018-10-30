#pragma once

class Clonable {
public:
	virtual Clonable* clone() const noexcept = 0;
};