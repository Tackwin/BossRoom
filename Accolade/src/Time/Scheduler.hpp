#pragma once
#include <unordered_map>
#include <functional>
#include <vector>

#include "Utils/UUID.hpp"

class Scheduler {
public:
	struct Task {

		using Fun = std::function<void(UUID)>;

		Fun f;
		UUID id;
		double time;
		void* userPtr{ nullptr };
	};

public:

	Task& getTask(UUID id) noexcept;
	const Task& getTask(UUID id) const noexcept;

	// run lamb in x seconds.
	UUID in(double seconds, Task::Fun&& lamb) noexcept;

	// run lamb every frame till x seconds.
	UUID till(double seconds, Task::Fun&& lamb) noexcept;

	// run lamb every x seconds.
	UUID every(double seconds, Task::Fun&& lamb) noexcept;

	void update(double dt) noexcept;

private:

	std::vector<Task> in_;
	std::vector<Task> till_;
	std::vector<std::pair<double, Task>> every_;
};