#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Serialization/FileStream.h"
#include "LkEngine/Serialization/Serializer.h"


namespace LkEngine {

    class LApplication;

    class LApplicationSerializer : public ISerializer
    {
    public:
        LApplicationSerializer(LApplication* InApplication);
        LApplicationSerializer(LApplication* InApplication, const std::filesystem::path& InConfigFile);
        LApplicationSerializer() = delete;
        ~LApplicationSerializer() = default;

        virtual void Serialize(const std::filesystem::path& InConfigFile);
        virtual bool Deserialize(const std::filesystem::path& InConfigFile) override;

    private:
        template<ESerializeFormat Format, typename TStream>
        void SerializeTo(TStream& Stream)
        {
            static_assert(!std::is_same_v<TStream, ESerializeFormat::None>, "Serializer format is not valid");
        }

        template<>
        void SerializeTo<ESerializeFormat::Yaml>(YAML::Emitter& Out)
        {
            SerializeToYaml(Out);
        }

        // Yaml
        void SerializeToYaml(YAML::Emitter& Out);
        bool DeserializeFromYaml(const std::string& YamlString);
        // ~Yaml

    private:
        std::filesystem::path ConfigFile{};

        LApplication* Application{};
    };

}
