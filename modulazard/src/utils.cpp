#include "utils.h"

#include <cassert>
#include <vector>

using namespace mzd;

std::vector<std::filesystem::path> GetFilesFromDir(const std::filesystem::path& path_dir, std::filesystem::path& ext)
{
	std::vector<std::filesystem::path> vPathNameList;
	for (auto& fs : std::filesystem::directory_iterator(path_dir))
	{
		const auto& file = fs.path();
		if (!std::filesystem::is_regular_file(file))
			continue;

		if (auto extension = file.extension(); !extension.empty() && extension == ext)
			vPathNameList.push_back(file);
	}
	return vPathNameList;
}

std::vector<std::filesystem::path> GetFilesFromDir(const std::filesystem::path& path_dir, const std::vector<std::filesystem::path>& exts)
{
	assert(!exts.empty());
	std::vector<std::filesystem::path> vPathNameList;
	for (auto& fs : std::filesystem::directory_iterator(path_dir))
	{
		const auto& file = fs.path();
		if (!std::filesystem::is_regular_file(file))
			continue;

		if (auto extension = file.extension(); !extension.empty() && std::find(std::begin(exts), std::end(exts), extension) != std::end(exts))
			vPathNameList.push_back(file);
	}
	return vPathNameList;
}

