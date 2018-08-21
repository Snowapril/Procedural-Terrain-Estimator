#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <vector>
#include <memory>
#include <filesystem>

class EngineAsset;

class AssetManager
{
private:
	std::vector<int> assets;
protected:
public:
	AssetManager();
	~AssetManager();

	template <class AssetType>
	std::shared_ptr<AssetType> addAsset(const std::initializer_list<std::string>& assetPath)
	{

	}
	void listenToAssetChanges(void);
};

#endif