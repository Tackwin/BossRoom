#include "Scheduler.hpp"

using Task = Scheduler::Task;

Task& Scheduler::getTask(UUID id) noexcept {
	auto inIt = std::find_if(
		std::begin(_in),
		std::end(_in),
		[id](const Task& a) {
			return a.id == id;
		}
	);
	if (inIt != std::end(_in)) {
		return *inIt;
	}

	auto inTill = std::find_if(
		std::begin(_till),
		std::end(_till),
		[id](const Task& a) {
		return a.id == id;
	}
	);
	if (inTill != std::end(_till)) {
		return *inTill;
	}

	auto inEvery = std::find_if(
		std::begin(_every),
		std::end(_every),
		[id](const std::pair<double, Task>& a) {
		return a.second.id == id;
	}
	);
	if (inEvery != std::end(_every)) {
		return inEvery->second;
	}
	std::abort();
}
const Task& Scheduler::getTask(UUID id) const noexcept {
	auto inIt = std::find_if(
		std::begin(_in),
		std::end(_in),
		[id](const Task& a) {
		return a.id == id;
	}
	);
	if (inIt != std::end(_in)) {
		return *inIt;
	}

	auto inTill = std::find_if(
		std::begin(_till),
		std::end(_till),
		[id](const Task& a) {
		return a.id == id;
	}
	);
	if (inTill != std::end(_till)) {
		return *inTill;
	}

	auto inEvery = std::find_if(
		std::begin(_every),
		std::end(_every),
		[id](const std::pair<double, Task>& a) {
		return a.second.id == id;
	}
	);
	if (inEvery != std::end(_every)) {
		return inEvery->second;
	}
	std::abort();
}


UUID Scheduler::in(double seconds, Task::Fun&& lamb) noexcept {
	Task t;
	t.f = std::move(lamb);
	t.time = seconds;

	// we want to store the next to execute first
	auto pred = [](Task a, Task b) -> bool {
		return a.time < b.time;
	};

	// sorted insert;
	_in.insert(std::upper_bound(std::begin(_in), std::end(_in), t, pred), t);

	return t.id;
}

UUID Scheduler::till(double seconds, Task::Fun&& lamb) noexcept {
	Task t;
	t.f = std::move(lamb);
	t.time = seconds;

	// we want to store the next to delete last.
	auto pred = [](Task a, Task b) -> bool {
		return a.time > b.time;
	};

	_till.insert(std::upper_bound(std::begin(_till), std::end(_till), t, pred), t);

	return t.id;
}

UUID Scheduler::every(double seconds, Task::Fun&& lamb) noexcept {
	Task t;
	t.f = std::move(lamb);
	t.time = seconds;

	// we want to store the next to execute first.
	auto pred = [](std::pair<double, Task> a, std::pair<double, Task> b) -> bool {
		return a.second.time > b.second.time;
	};

	_every.insert(
		std::upper_bound(
			std::begin(_every), std::end(_every), std::pair{ seconds, t }, pred
		),
		std::pair{ seconds, t }
	);

	return t.id;
}

void Scheduler::update(double dt) noexcept {

	size_t iIn = 0;
	for (size_t i = 0; i < _in.size(); ++i, ++iIn) {
		auto& in = _in[i];
		in.time -= dt;

		if (in.time <= 0.0) {
			in.f(in.id);
		}
		else {

			for (size_t j = i; j < _in.size(); ++j) {
				_in[j].time -= dt;
			}

			break;
		}
	}
	std::rotate(std::begin(_in), std::begin(_in) + iIn, std::end(_in));
	_in.erase(std::end(_in) - iIn, std::end(_in));

	for (size_t i = 0; i < _till.size(); ++i) {
		auto& till = _till[i];
		till.time -= dt;
		if (till.time >= 0.0) {
			till.f(till.id);
		}

		_till.erase(std::begin(_till) + i, std::end(_till));
	}

	for (size_t i = 0; i < _every.size(); ++i) {
		auto& every = _every[i].second;
		every.time -= dt;

		if (every.time <= 0.0) {
			every.f(every.id);
		}
		else {

			for (size_t j = 0; j < i; ++j) {
				_every[j].second.time = _every[j].first;
			}

			auto pred = [](std::pair<double, Task> a, std::pair<double, Task> b) {
				return a.second.time < b.second.time;
			};
			std::sort(std::begin(_every), std::end(_every), pred);

			break;
		}
	}
}