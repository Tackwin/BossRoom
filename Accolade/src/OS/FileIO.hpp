#pragma once
#include <filesystem>
#include <optional>
#include <vector>

extern std::optional<std::vector<char>>
read_whole_file(const std::filesystem::path& path) noexcept;