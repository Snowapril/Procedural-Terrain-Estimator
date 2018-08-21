#include "AssetManager.hpp"
#include "EngineAsset.hpp"
#include "EngineLogger.hpp"

AssetManager::AssetManager()
	: assets(0)
{
}

AssetManager::~AssetManager()
{
	assets.clear();
}

void AssetManager::listenToAssetChanges(void)
{
	using namespace std::experimental::filesystem;

}
