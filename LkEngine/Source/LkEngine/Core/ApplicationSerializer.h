/**
 * @file
 * @brief Application serializer.
 */
#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/ApplicationConfig.h"

#include "LkEngine/Serialization/FileStream.h"
#include "LkEngine/Serialization/Serializer.h"

namespace LkEngine {

    class LApplication;

    /**
     * @class LApplicationSerializer
     */
    class LApplicationSerializer
    {
    public:
        LApplicationSerializer(LApplication* InApplication);
        LApplicationSerializer(LApplication* InApplication, const std::filesystem::path& InConfigFile);
        LApplicationSerializer() = delete;
        ~LApplicationSerializer() = default;

        void Serialize(const std::filesystem::path& InConfigFile);
        bool Deserialize(const std::filesystem::path& InConfigFile, FApplicationSpecification& Spec);

    private:
		void SerializeToYaml(YAML::Emitter& Out);
        bool DeserializeFromYaml(const std::string& YamlString, FApplicationSpecification& Spec);

    private:
        std::filesystem::path ConfigFile{};

        LApplication* Application{};
    };

}
