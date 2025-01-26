#pragma once


namespace LkEngine {

    enum class ESourceBlendFunction
    {
        Zero = 0,
        One,
        Color,
        Alpha,
        One_Minus_DestinationAlpha
    };

    enum class EDestinationBlendFunction 
    {
        Zero = 0,
        One,
        Alpha,
        Color,
        One_Minus_SourceAlpha,
    };

	enum class EDepthFunction
	{
		Never = 0,
		Less,
		Equal,
		LessOrEqual,
        Greater,
		NotEqual,
		GreaterOrEqual,
		Always
	};

    struct FBlendFunction
    {
        ESourceBlendFunction Source{};
        EDestinationBlendFunction Destination;

        FBlendFunction() 
            : Source(ESourceBlendFunction::Alpha)
            , Destination(EDestinationBlendFunction::One_Minus_SourceAlpha) 
        {
        }
        FBlendFunction(const ESourceBlendFunction& InSourceBlendFunction, 
                       const EDestinationBlendFunction& InDestinationBlendFunction)
            : Source(InSourceBlendFunction)
            , Destination(InDestinationBlendFunction)
        {
        }
    };


	namespace Enum
	{
		static constexpr const char* ToString(const EDepthFunction DepthFunc)
		{
			switch (DepthFunc)
			{
				case EDepthFunction::Never:          return "Never";
				case EDepthFunction::Less:           return "Less";
				case EDepthFunction::Equal:          return "Equal";
				case EDepthFunction::LessOrEqual:    return "LessOrEqual";
				case EDepthFunction::Greater:        return "Greater";
				case EDepthFunction::NotEqual:       return "NotEqual";
				case EDepthFunction::GreaterOrEqual: return "GreaterOrEqual";
				case EDepthFunction::Always:         return "Always";
			}

			LK_CORE_VERIFY(false, "Enum::ToString(EDepthFunction) failed for value: {}", static_cast<int>(DepthFunc));
			return nullptr;
		}
	}


}
