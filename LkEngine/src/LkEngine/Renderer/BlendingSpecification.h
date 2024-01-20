#pragma once


namespace LkEngine {

    enum class SourceBlendFunction
    {
        Zero = 0,
        One,
        Color,
        Alpha,
        One_Minus_DestinationAlpha
    };

    enum class DestinationBlendFunction 
    {
        Zero = 0,
        One,
        Alpha,
        Color,
        One_Minus_SourceAlpha,
    };

    struct BlendFunction
    {
        SourceBlendFunction Source;
        DestinationBlendFunction Destination;
        BlendFunction() 
            : Source(SourceBlendFunction::Alpha), Destination(DestinationBlendFunction::One_Minus_SourceAlpha) {}
        BlendFunction(const SourceBlendFunction& source, const DestinationBlendFunction& destination)
            : Source(source), Destination(destination) {}
    };

}
