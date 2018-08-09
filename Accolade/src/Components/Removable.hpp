#pragma once

class Removable {
public:

	virtual void remove() noexcept = 0;
	virtual bool toRemove() const noexcept = 0;
};