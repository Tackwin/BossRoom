#pragma once
#include <filesystem>

extern std::filesystem::path get_executable_path() noexcept;
extern std::filesystem::path get_executable_dir() noexcept;