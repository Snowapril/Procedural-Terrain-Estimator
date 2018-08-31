/**
* @file		AssetManager.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	this class manage all assets for renderer. 
* @details	only AssetManager can have life cycle of all assets. with additional thread, listen to file modifying.
			if file modifying detected, then reload that asset without giving harm.
* @see		EngineAsset
*/

#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <vector>
#include <thread>
#include "EngineAsset.hpp"
#include <mutex>

class GLShader;

class AssetManager
{
private:
	volatile bool stopListen;
	std::thread changeListener;
	std::mutex fileChangeMutex;

	std::vector<EngineAsset<GLShader>*> dirtyAssets; //To be reloaded.
	std::vector<std::unique_ptr<EngineAsset<GLShader>>> assets;
protected:
public:
	AssetManager();
	~AssetManager();
	AssetManager(const AssetManager& other);
	AssetManager& operator=(const AssetManager& other);

	template <class AssetType>
	AssetType* addAsset(std::initializer_list<std::string>&& assetPath) noexcept
	{
		auto asset = make_unique_from_list<AssetType, std::string>(std::move(assetPath));
		AssetType* retPtr = static_cast<AssetType*>(asset.get());
		
		assets.emplace_back(std::move(asset));
		
		return retPtr;
	}

	void listenToAssetChanges(void);
	void refreshDirtyAssets(void);
};

#endif