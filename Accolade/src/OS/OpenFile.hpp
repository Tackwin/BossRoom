#pragma once

#include <thread>
#include <functional>
#include <unordered_map>

struct OpenFileOpts {
	void* owner{ nullptr };

	std::unordered_map<std::string, std::vector<std::string>> ext_filters;
	std::string filepath{ NULL };
	std::string filename{ NULL };
	std::string dialog_title{ NULL };
	std::string default_ext{ NULL };

	bool allow_multiple{ false };
	bool prompt_for_create{ false };
	bool allow_redirect_link{ false };
};
struct OpenFileResult {
	bool succeded{ false };

	unsigned long error_code;

	std::string filepath;
	std::string filename;
};

extern void open_file_async(
	std::function<void(OpenFileResult)>&& callback, OpenFileOpts opts = OpenFileOpts{}
) noexcept;
