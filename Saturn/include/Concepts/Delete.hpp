#pragma once

class NoCopy {
public:
	NoCopy() = default;

	NoCopy(const NoCopy&) = delete;
	NoCopy& operator=(const NoCopy&) = delete;
};

class NoMove {
public:
	NoMove() = default;

	NoMove(const NoMove&&) = delete;
	NoMove& operator=(const NoMove&&) = delete;
};