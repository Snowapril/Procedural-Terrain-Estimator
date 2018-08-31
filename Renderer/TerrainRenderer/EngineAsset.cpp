#include "EngineAsset.hpp"

template <> std::shared_ptr<GLShader> make_shared_from_list(std::initializer_list<std::string>&& list);
template <> std::shared_ptr<GLShader> make_shared_from_list(std::initializer_list<std::string>&& list);

template <> std::unique_ptr<EngineAsset<GLShader>> make_unique_from_list(std::initializer_list<std::string>&& list);
template <> std::unique_ptr<EngineAsset<GLShader>> make_unique_from_list(std::initializer_list<std::string>&& list);