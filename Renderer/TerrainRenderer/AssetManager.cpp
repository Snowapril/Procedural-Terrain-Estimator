#include "AssetManager.hpp"
#include "EngineAsset.hpp"
#include "EngineLogger.hpp"
#include "GLShader.hpp"

AssetManager::AssetManager()
{
	changeListener = std::thread(&AssetManager::listenToAssetChanges, this);
	stopListen = false;
}

AssetManager::~AssetManager()
{
	stopListen = true; /// if not set this to true, change listener's loop will be never ended.
	changeListener.join();

	assets.clear();
}

AssetManager::AssetManager(const AssetManager & other)
{
	stopListen = true;
	changeListener.join();

	changeListener = std::thread(&AssetManager::listenToAssetChanges, this);
	stopListen = other.stopListen;
	assets = other.assets;
	dirtyAssets = other.dirtyAssets;
}

AssetManager & AssetManager::operator=(const AssetManager & other)
{
	if (&other == this)
		return *this;

	stopListen = true;
	changeListener.join();

	changeListener = std::thread(&AssetManager::listenToAssetChanges, this);
	stopListen = other.stopListen;
	assets = other.assets;
	dirtyAssets = other.dirtyAssets;

	return *this;
}

/**
* @ brief		reload changed assets
* @ details		changed assets are pushed into dirtyAssets by listenToAssetChanges method.
				before reload asset, lock guard is needed because for preventing use of same resource by other thread.
*/
bool AssetManager::refreshDirtyAssets(void) noexcept
{
	/// before reload assets, lock it from the other thread.
	const bool isEmpty = dirtyAssets.empty();

	std::lock_guard<std::mutex> lockGuard(fileChangeMutex);
	for (auto& asset : dirtyAssets)
		asset->reloadAsset();

	dirtyAssets.clear();

	return isEmpty;
}

/**
* @ brief		listen to assets change which are in manage list (asset vector)
* @ details		before instance of this class is destructed, while loop is continued.
				check whether if asset is modified or not with 1 second term.
*/
void AssetManager::listenToAssetChanges(void) noexcept
{
	using namespace std::chrono_literals;
	
	while (!stopListen)
	{
		for (auto& asset : assets)
		{
			if (asset->listenToAssetChange())
			{
				dirtyAssets.push_back(asset.get());
			}
		}

		std::this_thread::sleep_for(1s);
	}
}

template <> GLShader* AssetManager::addAsset(iList<std::string>&& assetPath);
template <> GLShader* AssetManager::addAsset(iList<std::pair<uint32_t, std::string>>&& assetPath);