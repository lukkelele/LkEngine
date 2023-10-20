#pragma once




namespace LkEngine {

    class CollisionDetector2D
    {
        CollisionDetector2D();
        ~CollisionDetector2D();

        virtual void BeginContact();
        virtual void EndContact();
    };


}