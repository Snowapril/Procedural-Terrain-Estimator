#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <vector>
#include <thread>
#include "EngineAsset.hpp"
#include <mutex>

class AssetManager
{
private:
	std::thread changeListener;
	std::mutex fileChangeMutex;
	bool stopListen;

	std::vector<std::shared_ptr<EngineAsset>> assets;
	std::vector<std::shared_ptr<EngineAsset>> dirtyAssets; //To be reloaded.
protected:
public:
	AssetManager();
	~AssetManager();

	template <class AssetType>
	AssetType* addAsset(std::initializer_list<std::string>&& assetPath)
	{
		std::shared_ptr<EngineAsset> asset = make_shared_from_list<AssetType, std::string>(std::move(assetPath));
		AssetType* retPtr = static_cast<AssetType*>(asset.get());

		assets.push_back(asset);
		
		return retPtr;
	}
	void listenToAssetChanges(void);
	void refreshDirtyAssets(void);
};

#endif