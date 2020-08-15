#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <array>

#include "OpenGL Classes/VertexBuffer.h"
#include "OpenGL Classes/IndexBuffer.h"
#include "OpenGL Classes/VertexArray.h"
#include "OpenGL Classes/Shader.h"
#include "OpenGL Classes/Texture.h"

#include "Models/Model.h"

#include "Utils/Vertex.h"
#include "BlockTextureManager.h"

namespace Minecraft
{
	class Chunk;
	typedef std::array<std::array<std::array<Block, CHUNK_SIZE_X>, CHUNK_SIZE_Y>, CHUNK_SIZE_Z>* ChunkDataTypePtr;

	// Forward declarations
	ChunkDataTypePtr _GetChunkDataForMeshing(int cx, int cz);

	class ChunkMesh
	{
	public : 

		ChunkMesh();
		~ChunkMesh();

		void ConstructMesh(Chunk* chunk, const glm::vec3& chunk_pos);
		
		std::uint32_t p_VerticesCount;
		std::uint32_t p_TransparentVerticesCount;
		std::uint32_t p_ModelVerticesCount;
		GLClasses::VertexArray p_VAO;
		GLClasses::VertexArray p_TransparentVAO;
		GLClasses::VertexArray p_ModelVAO;

	private : 

		void AddFace(BlockFaceType face_type, const glm::vec3& position, BlockType type, float light_level,
			bool buffer = true);

		void AddModel(const glm::vec3& position, BlockType type, float light_level);

		std::vector<Vertex> m_Vertices;
		std::vector<Vertex> m_TransparentVertices;
		std::vector<ModelVertex> m_ModelVertices;
		glm::vec4 m_TopFace[4];
		glm::vec4 m_BottomFace[4];
		glm::vec4 m_ForwardFace[4];
		glm::vec4 m_BackFace[4];
		glm::vec4 m_LeftFace[4];
		glm::vec4 m_RightFace[4];

		GLClasses::VertexBuffer m_VBO;
		GLClasses::VertexBuffer m_TransparentVBO; // Vertex buffer for trasparent blocks
		GLClasses::VertexBuffer m_ModelVBO; // Vertex buffer for trasparent blocks
	};
}
