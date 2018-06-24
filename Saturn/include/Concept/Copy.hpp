#pragma once

class NonCopyable {
public:
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};
class Copyable {
public:
	Copyable() = default;
	Copyable(const Copyable&) = default;
	Copyable& operator=(const Copyable&) = default;
};