#include "Scheduler.hpp"

using Task = Scheduler::Task;

Task& Scheduler::getTask(UUID id) noexcept {
	auto inIt = std::find_if(
		std::begin(in_),
		std::end(in_),
		[id](const Task& a) {
			return a.id == id;
		}
	);
	if (inIt != std::end(in_)) {
		return *inIt;
	}

	auto inTill = std::find_if(
		std::begin(till_),
		std::end(till_),
		[id](const Task& a) {
		return a.id == id;
	}
	);
	if (inTill != std::end(till_)) {
		return *inTill;
	}

	auto inEvery = std::find_if(
		std::begin(every_),
		std::end(every_),
		[id](const std::pair<double, Task>& a) {
		return a.second.id == id;
	}
	);
	if (inEvery != std::end(every_)) {
		return inEvery->second;
	}
	std::abort();
}
const Task& Scheduler::getTask(UUID id) const noexcept {
	auto inIt = std::find_if(
		std::begin(in_),
		std::end(in_),
		[id](const Task& a) {
		return a.id == id;
	}
	);
	if (inIt != std::end(in_)) {
		return *inIt;
	}

	auto inTill = std::find_if(
		std::begin(till_),
		std::end(till_),
		[id](const Task& a) {
		return a.id == id;
	}
	);
	if (inTill != std::end(till_)) {
		return *inTill;
	}

	auto inEvery = std::find_if(
		std::begin(every_),
		std::end(every_),
		[id](const std::pair<double, Task>& a) {
		return a.second.id == id;
	}
	);
	if (inEvery != std::end(every_)) {
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
	in_.insert(std::upper_bound(std::begin(in_), std::end(in_), t, pred), t);

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

	till_.insert(std::upper_bound(std::begin(till_), std::end(till_), t, pred), t);

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

	every_.insert(
		std::upper_bound(
			std::begin(every_), std::end(every_), std::pair{ seconds, t }, pred
		),
		std::pair{ seconds, t }
	);

	return t.id;
}

void Scheduler::update(double dt) noexcept {

	size_t iIn = 0;
	for (size_t i = 0; i < in_.size(); ++i, ++iIn) {
		auto& in = in_[i];
		in.time -= dt;

		if (in.time <= 0.0) {
			in.f(in.id);
		}
		else {

			for (size_t j = i; j < in_.size(); ++j) {
				in_[j].time -= dt;
			}

			break;
		}
	}
	std::rotate(std::begin(in_), std::begin(in_) + iIn, std::end(in_));
	in_.erase(std::end(in_) - iIn, std::end(in_));

	for (size_t i = 0; i < till_.size(); ++i) {
		auto& till = till_[i];
		till.time -= dt;
		if (till.time >= 0.0) {
			till.f(till.id);
		}

		till_.erase(std::begin(till_) + i, std::end(till_));
	}

	for (size_t i = 0; i < every_.size(); ++i) {
		auto& every = every_[i].second;
		every.time -= dt;

		if (every.time <= 0.0) {
			every.f(every.id);
		}
		else {

			for (size_t j = 0; j < i; ++j) {
				every_[j].second.time = every_[j].first;
			}

			auto pred = [](std::pair<double, Task> a, std::pair<double, Task> b) {
				return a.second.time < b.second.time;
			};
			std::sort(std::begin(every_), std::end(every_), pred);

			break;
		}
	}
}