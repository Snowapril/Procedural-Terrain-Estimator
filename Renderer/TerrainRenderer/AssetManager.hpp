#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <vector>
#include <memory>

class EngineAsset;

class FileInfo;
class AssetInfo;

class AssetManager
{
private:
	std::vector<AssetInfo> assets;
protected:
public:
	AssetManager();
	~AssetManager();

	void addAsset(std::shared_ptr<EngineAsset> asset, const std::initializer_list<std::string>& assetPath);
	void listenToAssetChanges(void);
};

#endif