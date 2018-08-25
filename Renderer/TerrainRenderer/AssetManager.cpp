#include "AssetManager.hpp"
#include "EngineAsset.hpp"
#include "EngineLogger.hpp"
#include "GLShader.hpp"

AssetManager::AssetManager()
	: assets(0), fileChangeMutex()
{
	changeListener = std::thread(&AssetManager::listenToAssetChanges, this);
	stopListen = false;
}

AssetManager::~AssetManager()
{
	stopListen = true;
	changeListener.join();

	assets.clear();
}

/**
* @ brief		reload changed assets
* @ details		changed assets are pushed into dirtyAssets by listenToAssetChanges method.
				before reload asset, lock guard is needed because for preventing use of same resource by other thread.
*/
void AssetManager::refreshDirtyAssets(void)
{
	std::lock_guard<std::mutex> lockGuard(fileChangeMutex);
	for (auto& asset : dirtyAssets)
		asset->reloadAsset();

	dirtyAssets.clear();
}

/**
* @ brief		listen to assets change which are in manage list (asset vector)
* @ details		before instance of this class is destructed, while loop is continued.
				check whether if asset is modified or not with 1 second term.
*/
void AssetManager::listenToAssetChanges(void)
{
	using namespace std::chrono_literals;
	while (!stopListen)
	{
		for (auto& asset : assets)
		{
			if (asset->listenToAssetChange())
			{
				dirtyAssets.push_back(asset);
			}
		}

		std::this_thread::sleep_for(1s);
	}
}

template <> GLShader* AssetManager::addAsset(std::initializer_list<std::string>&& assetPath);