#include "World.h"

namespace Minecraft
{
	World::World()
	{
		// Generate all the chunks 

		p_Player = new Player;

		// Set the players position
		p_Player->p_Camera.SetPosition(glm::vec3(2 * 16, 10 * 16, 2 * 16));
		p_Player->p_Position = glm::vec3(2 * 16, 2 * 16, 2 * 16);

		Logger::LogToConsole("World Generation Began");

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m_WorldChunks[i][j].p_Position = glm::vec3(i, 1, j);
				GenerateChunk(&m_WorldChunks[i][j]);
			}
		}

		Logger::LogToConsole("World Generation Ended");
		Logger::LogToConsole("Chunk Mesh construction began");

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Timer timer("Mesh Construction!");

				m_WorldChunks[i][j].Construct(glm::vec3(i,1,j));
			}
		}

		Logger::LogToConsole("Chunk Mesh construction ended");
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
	}

	void World::RenderWorld()
	{
		int player_chunk_x = 0;
		int player_chunk_y = 0;
		int player_chunk_z = 0;

		m_Skybox.RenderSkybox(&p_Player->p_Camera);

		player_chunk_x = floor(p_Player->p_Position.x / ChunkSizeX);
		player_chunk_y = floor(p_Player->p_Position.y / ChunkSizeY);
		player_chunk_z = floor(p_Player->p_Position.z / ChunkSizeZ);

		int render_distance_x = 2, render_distance_z = 2;

		for (int i = player_chunk_x - render_distance_x; i < player_chunk_x + render_distance_x; i++)
		{
			for (int j = player_chunk_z - render_distance_z; j < player_chunk_z + render_distance_z; j++)
			{
				RenderChunkFromMap(i, j);
			}
		}
	}

	void World::OnEvent(EventSystem::Event e)
	{
		if (e.type == EventSystem::EventTypes::MouseMove)
		{
			p_Player->p_Camera.UpdateOnMouseMovement(e.mx, e.my);
		}
	}

	void World::RenderChunkFromMap(int cx, int cz)
	{
		std::map<int, std::map<int, Chunk>>::iterator chunk_exists_x = m_WorldChunks.find(cx);

		if (chunk_exists_x != m_WorldChunks.end())
		{
			std::map<int, Chunk>::iterator chunk_exists_z = m_WorldChunks.at(cx).find(cz);

			if (chunk_exists_z != m_WorldChunks.at(cx).end())
			{
				// The chunk exists. Render the already generated chunk
				m_Renderer.RenderChunk(&m_WorldChunks[cx][cz], &p_Player->p_Camera);

				return;
			}

			else
			{
				m_WorldChunks[cx][cz].p_Position = glm::vec3(cx, 1, cz);
				GenerateChunk(&m_WorldChunks[cx][cz]);
				m_WorldChunks[cx][cz].Construct(glm::vec3(cx,1,cz));
			}
		}

		else
		{
			m_WorldChunks[cx][cz].p_Position = glm::vec3(cx, 1, cz);
			GenerateChunk(&m_WorldChunks[cx][cz]);
			m_WorldChunks[cx][cz].Construct(glm::vec3(cx, 1, cz));
		}

		m_Renderer.RenderChunk(&m_WorldChunks[cx][cz], &p_Player->p_Camera);
		return;
	}
}