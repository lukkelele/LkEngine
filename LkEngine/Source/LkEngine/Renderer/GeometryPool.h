#pragma once

namespace LkEngine::Geometry {

	namespace Vertices 
	{
		inline static constexpr float Plane[] = {
		/*       Positions         Texture Coordinates   */
			 5.0f, -0.5f,  5.0f,     2.0f, 0.0f,
			-5.0f, -0.5f,  5.0f,     0.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,     0.0f, 2.0f,
		
			 5.0f, -0.5f,  5.0f,     2.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,     0.0f, 2.0f,
			 5.0f, -0.5f, -5.0f,     2.0f, 2.0f
		};
		static_assert(sizeof(Plane) > 0);

		inline static constexpr float Skybox[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		static_assert(sizeof(Skybox) > 0);
	}

	/**
	 * ArraySize
	 * 
	 *  The count of elements in an array. 
	 *  Not to be confused with the actual memory size of the entire container.
	 */
	namespace ArraySize 
	{
		inline static constexpr int Plane  = sizeof(Vertices::Plane)  / sizeof(Vertices::Plane[0]);
		inline static constexpr int Skybox = sizeof(Vertices::Skybox) / sizeof(Vertices::Skybox[0]);
	}

    static float CubeVertices[] = {
		/*   Vertices        Texture Coordinates   */
        // Back face
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,  // Bottom-right    
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,  // Top-right              
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,  // Top-right
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  // Top-left
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  // Bottom-left                

        // Front face
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  // Bottom-left
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,  // Top-right
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  // Bottom-right        
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,  // Top-right
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  // Bottom-left
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,  // Top-left        

        // Left face
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,  // Top-right
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,  // Bottom-left
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,  // Top-left       
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,  // Bottom-left
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,  // Top-right
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  // Bottom-right

        // Right face
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,  // Top-left
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,  // Top-right      
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,  // Bottom-right          
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,  // Bottom-right
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  // Bottom-left
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,  // Top-left

        // Bottom face            
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,  // Top-right
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f, -0.5f,    1.0f, 1.0f,  // Top-left        
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  // Bottom-left
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,  // Top-right
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  // Bottom-right

        // Top face
        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f,  // Top-left
         0.5f,  0.5f, -0.5f,     1.0f, 1.0f,  // Top-right
         0.5f,  0.5f,  0.5f,     1.0f, 0.0f,  // Bottom-right                 
         0.5f,  0.5f,  0.5f,     1.0f, 0.0f,  // Bottom-right
        -0.5f,  0.5f,  0.5f,     0.0f, 0.0f,  // Bottom-left  
        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f   // Top-left              
    };


}
