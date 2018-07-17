#pragma once

class Removable {
public:

	void remove() noexcept;
	bool toRemove() const noexcept;

protected:
	bool _toRemove{ false };
};