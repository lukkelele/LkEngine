#pragma once

namespace LkEngine::Geometry {

    static float CubeVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // Bottom-right    
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // Top-right              
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // Top-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // Top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // Bottom-left                
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // Bottom-left
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // Top-right
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // Bottom-right        
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // Top-right
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // Bottom-left
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // Top-left        
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // Top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // Bottom-left
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // Top-left       
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // Bottom-left
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // Top-right
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // Bottom-right
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // Top-left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // Top-right      
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // Bottom-right          
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // Bottom-right
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // Bottom-left
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // Top-left
        // Bottom face          
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // Top-right
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  // Top-left        
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // Bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // Top-right
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // Bottom-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // Top-left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // Top-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // Bottom-right                 
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // Bottom-right
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  // Bottom-left  
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f   // Top-left              
    };


}