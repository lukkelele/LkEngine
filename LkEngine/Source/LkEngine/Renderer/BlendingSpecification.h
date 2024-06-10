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

}
