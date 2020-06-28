#include "WorldGenerator.h"

namespace Minecraft
{
    void SetVerticalBlocks(Chunk* chunk, int x, int z, int y_level)
    {
        if (y_level >= ChunkSizeY)
        {
            y_level = ChunkSizeY - 1;
        }

        for (int i = 0; i < y_level; i++)
        {
            if (i >= y_level - 5)
            {
                chunk->AddBlock(BlockType::Dirt, glm::vec3(x, i, z));
            }

            else
            {
                chunk->AddBlock(BlockType::Stone, glm::vec3(x, i, z));
            }
        }
    }

    void GenerateChunk(Chunk* chunk)
    {
        static Random rand_engine;
        static FastNoise myNoise(rand_engine.Int(4000));
        myNoise.SetNoiseType(FastNoise::Simplex);

        float generated_x = 0;
        float generated_y = 0;
        float generated_z = 0;

        static float HeightMap[ChunkSizeX][ChunkSizeY]; // 2D heightmap to create terrain

        for (int x = 0; x < ChunkSizeX; x++)
        {
            for (int y = 0; y < ChunkSizeY; y++)
            {
                HeightMap[x][y] = myNoise.GetNoise(x + chunk->p_Position.x * ChunkSizeX, y + chunk->p_Position.z * ChunkSizeZ);
            }
        }

        for (int x = 0; x < ChunkSizeX; x++)
        {
            for (int z = 0; z < ChunkSizeZ; z++)
            {
                generated_x = x;
                generated_y = (HeightMap[x][z] / 2 + 1) * ChunkSizeY;
                generated_z = z;

                SetVerticalBlocks(chunk, generated_x, generated_z, generated_y);
            }
        }


    }
}