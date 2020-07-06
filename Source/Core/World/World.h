#pragma once

#include <iostream>
#include <string>
#include <array>
#include <map>
#include <vector>
#include <thread>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Skybox.h"
#include "../Chunk.h"
#include "../Block.h"
#include "../Utils/Defs.h"
#include "../Utils/Logger.h"
#include "../Utils/Timer.h"
#include "../Player/Player.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Renderer2D.h"
#include "../Application/Events.h"
#include "WorldGenerator.h"

namespace Minecraft
{
	class World
	{
	public : 

		World();
		~World();

		void OnUpdate(GLFWwindow* window);
		void RenderWorld();
		void OnEvent(EventSystem::Event e);

		// Gets a world block from the respective chunk. Returns nullptr if invalid
		std::pair<Block*, Chunk*> GetWorldBlockFromPosition(const glm::vec3& block_loc);

		Player* p_Player;

	private:
		void RenderChunkFromMap(int cx, int cz);

		Chunk* GetChunkFromMap(int cx, int cz);
		bool ChunkInViewFrustum(Chunk* chunk);

		int m_ChunkCount;

		// Renderers
		Renderer m_Renderer;
		Renderer2D m_Renderer2D;
		
		std::map<int, std::map<int, Chunk>> m_WorldChunks;
		Skybox m_Skybox;
		glm::vec3 m_StartRay;
		glm::vec3 m_EndRay;

		OrthographicCamera m_Camera2D;
		GLClasses::Texture m_CrosshairTexture;
	};
}