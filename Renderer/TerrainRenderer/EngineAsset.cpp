#include "EngineAsset.hpp"
#include "GLShader.hpp"

EngineAsset::EngineAsset()
{
}

EngineAsset::~EngineAsset()
{
	assetPaths.clear();
}

template <> std::shared_ptr<GLShader> make_shared_from_list(std::initializer_list<std::string>&& list);
template <> std::shared_ptr<GLShader> make_shared_from_list(std::initializer_list<std::string>&& list);

template <> std::unique_ptr<EngineAsset> make_unique_from_list(std::initializer_list<std::string>&& list);
template <> std::unique_ptr<EngineAsset> make_unique_from_list(std::initializer_list<std::string>&& list);