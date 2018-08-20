#include "AssetManager.hpp"
#include "EngineAsset.hpp"
#include "EngineLogger.hpp"
#include <filesystem>

class FileInfo
{
public:
	std::string fileName;
	int64_t lastWriteTime;
public:
	FileInfo(const std::string& _fileName, int64_t _lastWriteTime)
	{
		fileName = _fileName;
		lastWriteTime = _lastWriteTime;
	}
};

class AssetInfo
{
public:
	std::shared_ptr<EngineAsset> asset;
	std::vector<FileInfo> info;
public:
	AssetInfo(std::shared_ptr<EngineAsset> _asset, const std::vector<FileInfo> _info)
	{
		asset = _asset;
		info  = _info;
	}
};


AssetManager::AssetManager()
	: assets(0)
{
}

AssetManager::~AssetManager()
{
	assets.clear();
}

void AssetManager::addAsset(std::shared_ptr<EngineAsset> asset, const std::initializer_list<std::string>& assetPath)
{
	namespace fs = std::experimental::filesystem;
	std::vector<FileInfo> infoVec;
	infoVec.reserve(assetPath.size());

	for (const auto& path : assetPath)
	{
		auto lastWriteTime = fs::last_write_time(path);
		infoVec.push_back(FileInfo(std::move(path), static_cast<int64_t>(lastWriteTime.time_since_epoch().count())));
	}

	assets.push_back(AssetInfo(asset, infoVec));
}

void AssetManager::listenToAssetChanges(void)
{
	using namespace std::experimental::filesystem;

}
