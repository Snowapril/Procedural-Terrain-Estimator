#include "EngineAsset.hpp"
#include "GLShader.hpp"
#include "GLTexture.hpp"
#include <filesystem>

template <> std::shared_ptr<GLShader>  make_shared_from_list(iList<std::string>&& list);
template <> std::shared_ptr<GLShader>  make_shared_from_list(iList<std::string>&& list);
template <> std::shared_ptr<GLTexture> make_shared_from_list(iList<std::string>&& list);
template <> std::shared_ptr<GLTexture> make_shared_from_list(iList<std::string>&& list);

template <> std::unique_ptr<GLShader>  make_unique_from_list(iList<std::string>&& list);
template <> std::unique_ptr<GLShader>  make_unique_from_list(iList<std::string>&& list);
template <> std::unique_ptr<GLTexture> make_unique_from_list(iList<std::string>&& list);
template <> std::unique_ptr<GLTexture> make_unique_from_list(iList<std::string>&& list);

EngineAsset::EngineAsset()
{
}

EngineAsset::~EngineAsset()
{
	assetPaths.clear();
}

EngineAsset::EngineAsset(const EngineAsset & other)
	: assetPaths(other.assetPaths)
{
}

EngineAsset & EngineAsset::operator=(const EngineAsset & other)
{
	if (&other == this)
		return *this;

	assetPaths = other.assetPaths;

	return *this;
}

bool EngineAsset::listenToAssetChange(void)
{
	namespace fs = std::experimental::filesystem;

	for (auto& file : assetPaths)
	{
		if (!file.second.empty())
		{
			auto& lastTime = file.first;
			const auto newLastTime = fs::last_write_time(file.second).time_since_epoch().count();

			if (newLastTime != lastTime)
			{
				lastTime = newLastTime;
				return true;
			}
		}
	}

	return false;
}