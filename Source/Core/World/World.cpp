#include "World.h"

namespace Minecraft
{
	static void PrintVec3(const glm::vec3& val)
	{
		std::cout << std::endl << "X : " << val.x << " Y : " << val.y << " Z : " << val.z;
		return;
	}

	World::World() : m_Camera2D(0.0f, 800.0f, 0.0f, 600.0f)
	{
		// Generate all the chunks 

		p_Player = new Player;

		// Set the players position
		p_Player->p_Camera.SetPosition(glm::vec3(0, (ChunkSizeY - MaxStructureY) - 2, 0));

		Logger::LogToConsole("World Generation Began");

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Timer timer("First Chunk generation");

				m_WorldChunks[std::pair<int, int>(i, j)].p_Position = glm::vec3(i, 1, j);
				GenerateChunk(&m_WorldChunks[std::pair<int, int>(i, j)]);
				m_ChunkCount++;
			}
		}

		Logger::LogToConsole("World Generation Ended");
		Logger::LogToConsole("Chunk Mesh construction began");

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Timer timer("First Chunks Construction!");

				m_WorldChunks[std::pair<int, int>(i, j)].Construct(glm::vec3(i, 1, j));
			}
		}

		Logger::LogToConsole("Chunk Mesh construction ended");

		m_CrosshairTexture.CreateTexture("Resources/crosshair.png");
	}

	World::~World()
	{

	}

	void World::OnUpdate(GLFWwindow* window)
	{
		p_Player->OnUpdate();

		const float camera_speed = 0.35f;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			p_Player->p_Camera.MoveCamera(MoveDirection::Front, camera_speed);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			p_Player->p_Camera.MoveCamera(MoveDirection::Back, camera_speed);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			p_Player->p_Camera.MoveCamera(MoveDirection::Left, camera_speed);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			p_Player->p_Camera.MoveCamera(MoveDirection::Right, camera_speed);

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			p_Player->p_Camera.MoveCamera(MoveDirection::Up, camera_speed);

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			p_Player->p_Camera.MoveCamera(MoveDirection::Down, camera_speed);

		// Get position relative to camera's direction
		float block_dist = 50.0f;

		std::vector<glm::vec3> traversed_voxels = FastVoxelTraversal(p_Player->p_Camera.GetPosition(), 
			p_Player->p_Camera.GetPosition() + p_Player->p_Camera.GetFront() * block_dist);

		for (int i = 0; i < traversed_voxels.size() ; i++)
		{
			// Check if the traversed voxel is valid
			if (traversed_voxels[i].y < ChunkSizeY && traversed_voxels[i].y >= 0)
			{
				std::pair<Block*, Chunk*> block = GetWorldBlockFromPosition(traversed_voxels[i]);

				if (block.first->p_BlockType != BlockType::Air && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
				{
					block.second->SetBlock(BlockType::Dirt, block.first->p_Position);
					block.second->Construct(block.second->p_Position);

					break;
				}

				else if (block.first->p_BlockType != BlockType::Air && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
				{
					block.second->SetBlock(BlockType::Air, block.first->p_Position);
					block.second->Construct(block.second->p_Position);

					break;
				}
			}
		}
	}

	void World::RenderWorld()
	{
		int player_chunk_x = 0;
		int player_chunk_y = 0;
		int player_chunk_z = 0;

		m_Skybox.RenderSkybox(&p_Player->p_Camera);

		player_chunk_x = floor(p_Player->p_Position.x / ChunkSizeX);
		player_chunk_z = floor(p_Player->p_Position.z / ChunkSizeZ);

		int render_distance_x = 3, render_distance_z = 3;
		int chunks_rendered = 0;
		for (int i = player_chunk_x - render_distance_x; i < player_chunk_x + render_distance_x; i++)
		{
			for (int j = player_chunk_z - render_distance_z; j < player_chunk_z + render_distance_z; j++)
			{
				chunks_rendered++;
				RenderChunkFromMap(i, j);
			}
		}


		m_Renderer2D.RenderQuad(glm::vec3(400.0f - (m_CrosshairTexture.GetWidth() / 2), 300.0f - (m_CrosshairTexture.GetHeight() / 2), 1.0f), &m_CrosshairTexture, &m_Camera2D);
	}

	void World::OnEvent(EventSystem::Event e)
	{
		p_Player->OnEvent(e);

		if (e.type == EventSystem::EventTypes::MousePress)
		{

		}
	}

	std::pair<Block*, Chunk*> World::GetWorldBlockFromPosition(const glm::vec3& block_loc)
	{
		int block_chunk_x = (int)(block_loc.x / ChunkSizeX);
		int block_chunk_z = (int)(block_loc.z / ChunkSizeZ);
		int bx = ((int)block_loc.x % ChunkSizeX + ChunkSizeX) % ChunkSizeX;
		int by = block_loc.y;
		int bz = ((int)block_loc.z % ChunkSizeZ + ChunkSizeZ) % ChunkSizeZ;

		Chunk* chunk = GetChunkFromMap(block_chunk_x, block_chunk_z);

		return { &chunk->m_ChunkContents->at(bx).at(by).at(bz), chunk };
	}

	void World::SetWorldBlockFromPosition(BlockType type, const glm::vec3& block_loc)
	{
		int block_chunk_x = (int)(block_loc.x / ChunkSizeX);
		int block_chunk_z = (int)(block_loc.z / ChunkSizeZ);
		int bx = ((int)block_loc.x % ChunkSizeX + ChunkSizeX) % ChunkSizeX;
		int by = block_loc.y;
		int bz = ((int)block_loc.z % ChunkSizeZ + ChunkSizeZ) % ChunkSizeZ;

		GetChunkFromMap(block_chunk_x, block_chunk_z)->SetBlock(type, glm::vec3(bx, by, bz));
	}

	BlockType World::GetWorldBlockTypeFromPosition(const glm::vec3& block_loc)
	{
		int block_chunk_x = (int)(block_loc.x / ChunkSizeX);
		int block_chunk_z = (int)(block_loc.z / ChunkSizeZ);
		int bx = ((int)block_loc.x % ChunkSizeX + ChunkSizeX) % ChunkSizeX;
		int by = block_loc.y;
		int bz = ((int)block_loc.z % ChunkSizeZ + ChunkSizeZ) % ChunkSizeZ;

		return GetChunkFromMap(block_chunk_x, block_chunk_z)->m_ChunkContents->at(bx).at(by).at(bz).p_BlockType;
	}

	void World::RenderChunkFromMap(int cx, int cz)
	{
		m_Renderer.RenderChunk(GetChunkFromMap(cx, cz), &p_Player->p_Camera);
		return;
	}

	void World::UnloadFarChunks()
	{

		return;
	}

	Chunk* World::GetChunkFromMap(int cx, int cz)
	{
		std::stringstream str;

		str << "Chunk Building ! X : " << cx << " | Z : " << cz;

		std::map<std::pair<int, int>, Chunk>::iterator chunk_exists = m_WorldChunks.find(std::pair<int,int>(cx, cz));

		if (chunk_exists == m_WorldChunks.end())
		{
			Timer timer(str.str());

			m_WorldChunks[std::pair<int, int>(cx, cz)].p_Position = glm::vec3(cx, 1, cz);
			GenerateChunk(&m_WorldChunks[std::pair<int, int>(cx, cz)]);
			m_WorldChunks[std::pair<int, int>(cx, cz)].Construct(glm::vec3(cx, 1, cz));
			m_ChunkCount++;
		}

		return &m_WorldChunks.at(std::pair<int, int>(cx, cz));
	}
}