#include "ChunkFileHandler.h"

namespace Minecraft
{
	namespace ChunkFileHandler
	{

		void ExtendString(std::string& str, int ex_amt, std::string& ex_c)
		{
			ex_amt = abs(static_cast<int>(ex_amt - str.size()));

			for (int i = 0; i < ex_amt; i++)
			{
				str.insert(0, ex_c);
			}
		}

		std::string GenerateFileName(const glm::vec2& position, const std::string& dir)
		{
			std::stringstream s;
			
			s << dir << position.x << "," << position.y;
			return s.str();
		}

		bool WriteChunk(Chunk* chunk, const std::string& dir)
		{
			FILE* outfile;
			std::stringstream s;
			std::string file_name = GenerateFileName(glm::vec2(chunk->p_Position.x, chunk->p_Position.z), dir);

			outfile = fopen(file_name.c_str(), "wb+");

			if (outfile == NULL)
			{
				s << "Couldn't write chunk (" << file_name << ")";
				Logger::LogToConsole(s.str());

				return false;
			}

			fwrite(chunk->p_ChunkContents.data(), sizeof(Block), CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, outfile);
			fwrite(chunk->p_ChunkLightInformation.data(), sizeof(std::uint8_t), CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, outfile);

			fclose(outfile);
			return true;
		}

		bool ReadChunk(Chunk* chunk, const std::string& dir)
		{
			FILE* infile;
			std::stringstream s;
			std::string file_name = dir;

			infile = fopen(file_name.c_str(), "rb");

			if (infile == NULL)
			{
				s << "Couldn't read chunk (" << file_name << ")";
				Logger::LogToConsole(s.str());

				return false;
			}

			fread(&chunk->p_ChunkContents, sizeof(Block), CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, infile);
			fread(&chunk->p_ChunkLightInformation, sizeof(std::uint8_t), CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, infile);

			fclose(infile);
			return true;
		}
	}
}