#include "ChunkMesh.h"
#include "Chunk.h"

namespace Minecraft
{
	ChunkMesh::ChunkMesh() : m_VBO(GL_ARRAY_BUFFER), m_TransparentVBO(GL_ARRAY_BUFFER)
	{
		static bool IndexBufferInitialized = false;

		// Static index buffer
		static GLClasses::IndexBuffer StaticIBO;

		if (IndexBufferInitialized == false)
		{
			IndexBufferInitialized = true;

			GLuint* IndexBuffer = nullptr;

			int index_size = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6;
			int index_offset = 0;

			IndexBuffer = new GLuint[index_size * 6];

			for (size_t i = 0; i < index_size; i += 6)
			{
				IndexBuffer[i] = 0 + index_offset;
				IndexBuffer[i + 1] = 1 + index_offset;
				IndexBuffer[i + 2] = 2 + index_offset;
				IndexBuffer[i + 3] = 2 + index_offset;
				IndexBuffer[i + 4] = 3 + index_offset;
				IndexBuffer[i + 5] = 0 + index_offset;

				index_offset = index_offset + 4;
			}

			StaticIBO.BufferData(index_size * 6 * sizeof(GLuint), IndexBuffer, GL_STATIC_DRAW);

			delete[] IndexBuffer;
		}

		int stride = (7 * sizeof(GLfloat));

		p_VAO.Bind();
		m_VBO.Bind();
		StaticIBO.Bind();
		m_VBO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		m_VBO.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		m_VBO.VertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
		m_VBO.VertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		p_VAO.Unbind();

		p_TransparentVAO.Bind();
		m_TransparentVBO.Bind();
		StaticIBO.Bind();
		m_TransparentVBO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		m_TransparentVBO.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		m_TransparentVBO.VertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
		m_TransparentVBO.VertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		p_TransparentVAO.Unbind();

		// Set the values of the 2D planes

		m_ForwardFace[0] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		m_ForwardFace[1] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		m_ForwardFace[2] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		m_ForwardFace[3] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

		m_BackFace[0] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		m_BackFace[1] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		m_BackFace[2] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		m_BackFace[3] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

		m_TopFace[0] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		m_TopFace[1] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		m_TopFace[2] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		m_TopFace[3] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

		m_BottomFace[0] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		m_BottomFace[1] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		m_BottomFace[2] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		m_BottomFace[3] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

		m_LeftFace[0] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		m_LeftFace[1] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		m_LeftFace[2] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		m_LeftFace[3] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

		m_RightFace[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		m_RightFace[1] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		m_RightFace[2] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		m_RightFace[3] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}

	ChunkMesh::~ChunkMesh()
	{
		m_Vertices.clear();
	}

	// Construct mesh using greedy meshing for maximum performance
	void ChunkMesh::ConstructMesh(Chunk* chunk, const glm::vec3& chunk_pos)
	{
		ChunkDataTypePtr ChunkData = &chunk->p_ChunkContents;
		
		glm::vec3 world_position;
		m_Vertices.clear();

		ChunkDataTypePtr ForwardChunkData = _GetChunkDataForMeshing(static_cast<int>(chunk_pos.x), static_cast<int>(chunk_pos.z + 1));
		ChunkDataTypePtr BackwardChunkData = _GetChunkDataForMeshing(static_cast<int>(chunk_pos.x), static_cast<int>(chunk_pos.z - 1));
		ChunkDataTypePtr RightChunkData = _GetChunkDataForMeshing(static_cast<int>(chunk_pos.x + 1), static_cast<int>(chunk_pos.z));
		ChunkDataTypePtr LeftChunkData = _GetChunkDataForMeshing(static_cast<int>(chunk_pos.x - 1), static_cast<int>(chunk_pos.z));

		for (int x = 0; x < CHUNK_SIZE_X; x++)
		{
			for (int y = 0; y < CHUNK_SIZE_Y; y++)
			{
				for (int z = 0; z < CHUNK_SIZE_Z; z++)
				{
					if (ChunkData->at(x).at(y).at(z).p_BlockType != BlockType::Air)
					{
						Block* block = &ChunkData->at(x).at(y).at(z);

						// To fix chunk edge mesh building issues, both faces are added if it is in the edge

						float light_level = chunk->GetTorchLightAt(x, y, z);
						//float light_level = 1.0f;
						world_position.x = chunk_pos.x * CHUNK_SIZE_X + x;
						world_position.y = 0 * CHUNK_SIZE_Y + y;
						world_position.z = chunk_pos.z * CHUNK_SIZE_Z + z;

						if (z <= 0)
						{
							if (block->IsTransparent())
							{
								if (BackwardChunkData->at(x).at(y).at(CHUNK_SIZE_Z - 1).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}

								else if (ChunkData->at(x).at(y).at(1).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}
							}

							else
							{ 
								if (BackwardChunkData->at(x).at(y).at(CHUNK_SIZE_Z - 1).IsOpaque() == false)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}

								else if (ChunkData->at(x).at(y).at(1).IsOpaque() == false)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}
							}
						}

						else if (z >= CHUNK_SIZE_Z - 1)
						{
							if (block->IsTransparent())
							{
								if (ForwardChunkData->at(x).at(y).at(0).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}

								else if (ChunkData->at(x).at(y).at(CHUNK_SIZE_Z - 2).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}
							}

							else
							{
								if (ForwardChunkData->at(x).at(y).at(0).IsOpaque() == false)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}

								else if (ChunkData->at(x).at(y).at(CHUNK_SIZE_Z - 2).IsOpaque() == false)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}
							}
						}

						else
						{
							if (block->IsTransparent())
							{
								if (ChunkData->at(x).at(y).at(z + 1).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}

								if (ChunkData->at(x).at(y).at(z - 1).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}
							}

							else
							{
								//If the forward block is an air block, add the forward face to the mesh
								if (ChunkData->at(x).at(y).at(z + 1).IsOpaque() == false)
								{
									AddFace(BlockFaceType::front, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}

								// If the back (-forward) block is an air block, add the back face to the mesh
								if (ChunkData->at(x).at(y).at(z - 1).IsOpaque() == false)
								{
									AddFace(BlockFaceType::backward, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}
							}
						}

						if (x <= 0)
						{
							if (block->IsTransparent())
							{
								if (LeftChunkData->at(CHUNK_SIZE_X - 1).at(y).at(z).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}

								else if (ChunkData->at(1).at(y).at(z).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}

							}

							else
							{
								if (LeftChunkData->at(CHUNK_SIZE_X - 1).at(y).at(z).IsOpaque() == false)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}

								else if (ChunkData->at(1).at(y).at(z).IsOpaque() == false)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}
							}
						}

						else if (x >= CHUNK_SIZE_X - 1)
						{
							if (block->IsTransparent())
							{
								if (RightChunkData->at(0).at(y).at(z).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}

								else if (ChunkData->at(CHUNK_SIZE_X - 2).at(y).at(z).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}
							}

							else
							{
								if (RightChunkData->at(0).at(y).at(z).IsOpaque() == false)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}

								else if (ChunkData->at(CHUNK_SIZE_X - 2).at(y).at(z).IsOpaque() == false)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}
							}

						}

						else
						{
							if (block->IsTransparent())
							{
								// If the next block is an air block, add the right face to the mesh
								if (ChunkData->at(x + 1).at(y).at(z).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}

								// If the previous block is an air block, add the left face to the mesh
								if (ChunkData->at(x - 1).at(y).at(z).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}
							}

							else
							{
								// If the next block is an air block, add the right face to the mesh
								if (ChunkData->at(x + 1).at(y).at(z).IsOpaque() == false)
								{
									AddFace(BlockFaceType::right, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}

								// If the previous block is an air block, add the left face to the mesh
								if (ChunkData->at(x - 1).at(y).at(z).IsOpaque() == false)
								{
									AddFace(BlockFaceType::left, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}
							}
						}

						if (y <= 0)
						{
							if (ChunkData->at(x).at(y + 1).at(z).IsOpaque() == false)
							{
								AddFace(BlockFaceType::bottom, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
							}
						}

						else if (y >= CHUNK_SIZE_Y - 1)
						{
							AddFace(BlockFaceType::top, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
						}

						else
						{
							if (block->IsTransparent())
							{
								// If the top block is an air block, add the top face to the mesh
								if (ChunkData->at(x).at(y - 1).at(z).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::bottom, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}

								// If the bottom block is an air block, add the top face to the mesh
								if (ChunkData->at(x).at(y + 1).at(z).p_BlockType == BlockType::Air)
								{
									AddFace(BlockFaceType::top, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk, false);
								}
							}

							else
							{
								// If the top block is an air block, add the top face to the mesh
								if (ChunkData->at(x).at(y - 1).at(z).IsOpaque() == false)
								{
									AddFace(BlockFaceType::bottom, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}

								// If the bottom block is an air block, add the top face to the mesh
								if (ChunkData->at(x).at(y + 1).at(z).IsOpaque() == false)
								{
									AddFace(BlockFaceType::top, world_position, static_cast<BlockType>(ChunkData->at(x).at(y).at(z).p_BlockType), light_level, chunk);
								}
							}
						}
					}
				}
			}
		}

		// Upload the data to the GPU whenever the mesh is reconstructed

		m_VBO.BufferData(this->m_Vertices.size() * sizeof(Vertex), &this->m_Vertices.front(), GL_STATIC_DRAW);
		p_VerticesCount = m_Vertices.size();
		m_Vertices.clear();

		m_TransparentVBO.BufferData(this->m_TransparentVertices.size() * sizeof(Vertex), &this->m_TransparentVertices.front(), GL_STATIC_DRAW);
		p_TransparentVerticesCount = m_TransparentVertices.size();
		m_TransparentVertices.clear();
	}

	void ChunkMesh::AddFace(BlockFaceType face_type, const glm::vec3& position, BlockType type, float light_level, Chunk* chunk,
							bool buffer)
	{
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);

		Vertex v1, v2, v3, v4, v5, v6;

		// To fix a face culling issue. I inverted the vertices for the left, front and bottom face so the winding order will be correct
		bool reverse_texture_coordinates = false;

		// Order
		// Top, bottom, front, back, left, right
		static const GLfloat lighting_levels[6] = { 1.0f, 0.2f, 0.6f, 0.7f, 0.6f, 0.7f };

		switch (face_type)
		{
		case BlockFaceType::top:
		{
			v1.position = translation * m_TopFace[0];
			v2.position = translation * m_TopFace[1];
			v3.position = translation * m_TopFace[2];
			v4.position = translation * m_TopFace[3];

			// Set the lighting level for the vertex
			v1.lighting_level = light_level;
			v2.lighting_level = light_level;
			v3.lighting_level = light_level;
			v4.lighting_level = light_level;

			v1.block_face_lighting = lighting_levels[0];
			v2.block_face_lighting = lighting_levels[0];
			v3.block_face_lighting = lighting_levels[0];
			v4.block_face_lighting = lighting_levels[0];

			break;
		}

		case BlockFaceType::bottom:
		{
			v1.position = translation * m_BottomFace[3];
			v2.position = translation * m_BottomFace[2];
			v3.position = translation * m_BottomFace[1];
			v4.position = translation * m_BottomFace[0];

			// Set the lighting level for the vertex
			v1.lighting_level = light_level;
			v2.lighting_level = light_level;
			v3.lighting_level = light_level;
			v4.lighting_level = light_level;

			v1.block_face_lighting = lighting_levels[1];
			v2.block_face_lighting = lighting_levels[1];
			v3.block_face_lighting = lighting_levels[1];
			v4.block_face_lighting = lighting_levels[1];

			reverse_texture_coordinates = true;

			break;
		}

		case BlockFaceType::front:
		{
			v1.position = translation * m_ForwardFace[3];
			v2.position = translation * m_ForwardFace[2];
			v3.position = translation * m_ForwardFace[1];
			v4.position = translation * m_ForwardFace[0];

			// Set the lighting level for the vertex
			v1.lighting_level = light_level;
			v2.lighting_level = light_level;
			v3.lighting_level = light_level;
			v4.lighting_level = light_level;

			v1.block_face_lighting = lighting_levels[2];
			v2.block_face_lighting = lighting_levels[2];
			v3.block_face_lighting = lighting_levels[2];
			v4.block_face_lighting = lighting_levels[2];

			reverse_texture_coordinates = true;

			break;
		}

		case BlockFaceType::backward:
		{
			v1.position = translation * m_BackFace[0];
			v2.position = translation * m_BackFace[1];
			v3.position = translation * m_BackFace[2];
			v4.position = translation * m_BackFace[3];

			v1.lighting_level = light_level;
			v2.lighting_level = light_level;
			v3.lighting_level = light_level;
			v4.lighting_level = light_level;

			v1.block_face_lighting = lighting_levels[3];
			v2.block_face_lighting = lighting_levels[3];
			v3.block_face_lighting = lighting_levels[3];
			v4.block_face_lighting = lighting_levels[3];

			break;
		}

		case BlockFaceType::left:
		{
			v1.position = translation * m_LeftFace[3];
			v2.position = translation * m_LeftFace[2];
			v3.position = translation * m_LeftFace[1];
			v4.position = translation * m_LeftFace[0];

			v1.lighting_level = light_level;
			v2.lighting_level = light_level;
			v3.lighting_level = light_level;
			v4.lighting_level = light_level;

			v1.block_face_lighting = lighting_levels[4];
			v2.block_face_lighting = lighting_levels[4];
			v3.block_face_lighting = lighting_levels[4];
			v4.block_face_lighting = lighting_levels[4];

			reverse_texture_coordinates = true;

			break;
		}

		case BlockFaceType::right:
		{
			v1.position = translation * m_RightFace[0];
			v2.position = translation * m_RightFace[1];
			v3.position = translation * m_RightFace[2];
			v4.position = translation * m_RightFace[3];

			v1.lighting_level = light_level;
			v2.lighting_level = light_level;
			v3.lighting_level = light_level;
			v4.lighting_level = light_level;

			v1.block_face_lighting = lighting_levels[5];
			v2.block_face_lighting = lighting_levels[5];
			v3.block_face_lighting = lighting_levels[5];
			v4.block_face_lighting = lighting_levels[5];

			break;
		}

		default:
		{
			// Todo : Throw an error here
			break;
		}
		}

		// Get required texture coordinates

		const std::array<GLfloat, 8>& TextureCoordinates = GetBlockTexture(type, face_type);

		if (reverse_texture_coordinates)
		{
			v1.texture_coords = glm::vec2(TextureCoordinates[6], TextureCoordinates[7]);
			v2.texture_coords = glm::vec2(TextureCoordinates[4], TextureCoordinates[5]);
			v3.texture_coords = glm::vec2(TextureCoordinates[2], TextureCoordinates[3]);
			v4.texture_coords = glm::vec2(TextureCoordinates[0], TextureCoordinates[1]);
		}

		else
		{
			v1.texture_coords = glm::vec2(TextureCoordinates[0], TextureCoordinates[1]);
			v2.texture_coords = glm::vec2(TextureCoordinates[2], TextureCoordinates[3]);
			v3.texture_coords = glm::vec2(TextureCoordinates[4], TextureCoordinates[5]);
			v4.texture_coords = glm::vec2(TextureCoordinates[6], TextureCoordinates[7]);
		}

		if (buffer)
		{
			m_Vertices.push_back(v1);
			m_Vertices.push_back(v2);
			m_Vertices.push_back(v3);
			m_Vertices.push_back(v4);
		}

		else if (!buffer)
		{
			m_TransparentVertices.push_back(v1);
			m_TransparentVertices.push_back(v2);
			m_TransparentVertices.push_back(v3);
			m_TransparentVertices.push_back(v4);
		}
	}
}