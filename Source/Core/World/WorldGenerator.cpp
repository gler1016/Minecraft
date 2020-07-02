#include "WorldGenerator.h"

namespace Minecraft
{
    void SetVerticalBlocks(Chunk* chunk, int x, int z, int y_level, Biome biome)
    {
        if (y_level >= ChunkSizeY)
        {
            y_level = ChunkSizeY - 1;
        }

        for (int i = 0; i < y_level; i++)
        {
            if (biome == Biome::Grassland)
            {
                if (i >= y_level - 1)
                {
                    chunk->AddBlock(BlockType::Grass, glm::vec3(x, i, z));
                }

                else if (i >= y_level - 5)
                {
                    chunk->AddBlock(BlockType::Dirt, glm::vec3(x, i, z));
                }

                else
                {
                    chunk->AddBlock(BlockType::Stone, glm::vec3(x, i, z));
                }
            }

            else if (biome == Biome::Desert)
            {
                if (i >= y_level - 6)
                {
                    chunk->AddBlock(BlockType::Sand, glm::vec3(x, i, z));
                }

                else
                {
                    chunk->AddBlock(BlockType::Stone, glm::vec3(x, i, z));
                }
            }
        }
    }

    void FillInWorldStructure(Chunk* chunk, WorldStructure* structure, int x, int y, int z)
    {
        Block block;

        for (int i = x, sx = 0; i < x + MaxStructureX; i++, sx++)
        {
            for (int j = y, sy = 0; j < y + MaxStructureY; j++, sy++)
            {
                for (int k = z, sz = 0; k < z + MaxStructureZ; k++, sz++)
                {
                    if (i < ChunkSizeX && j < ChunkSizeY && k < ChunkSizeZ && sx < MaxStructureX && sy < MaxStructureY && sz < MaxStructureZ)
                    {
                        if (structure->p_Structure->at(sx).at(sy).at(sz).p_BlockType != BlockType::Air)
                        {
                            block.p_BlockType = structure->p_Structure->at(sx).at(sy).at(sz).p_BlockType;
                            block.p_Position = glm::vec3(i, j, k);
                            chunk->AddBlock(&block);
                        }
                    }
                }
            }
        }
    }

    Biome GetBiome(float chunk_noise)
    {
        // Quantize the noise into various levels and frequency

        const float grass_land = 0.1f;
        const float desert = 0.6f;
        const float ocean = 0.6f;

        if (chunk_noise < grass_land)
        {
            return Biome::Grassland;
        }

        else if (chunk_noise > 0 && chunk_noise < desert)
        {
            return Biome::Desert;
        }

        else
        {
            return Biome::Grassland;
        }
    }

    void GenerateChunk(Chunk* chunk)
    {
        static Random SeedEngine;
        static unsigned int WorldSeed = 1569;
        static Random WorldTreeGenerator(WorldSeed);
        static FastNoise WorldGenerator(WorldSeed);
        static FastNoise BiomeGenerator(WorldSeed);
        
        WorldGenerator.SetNoiseType(FastNoise::Simplex);
        BiomeGenerator.SetNoiseType(FastNoise::Simplex);

        float generated_x = 0;
        float generated_y = 0;
        float generated_z = 0;

        static TreeStructure WorldStructureTree;
        static CactusStructure WorldStructureCactus;
        WorldStructure* Structure;

        static float HeightMap[ChunkSizeX][ChunkSizeY]; // 2D heightmap to create terrain

        Biome chunk_biome;

        chunk_biome = GetBiome(BiomeGenerator.GetNoise(chunk->p_Position.x * ChunkSizeX, chunk->p_Position.y * ChunkSizeY, chunk->p_Position.z * ChunkSizeZ));

        for (int x = 0; x < ChunkSizeX; x++)
        {
            for (int y = 0; y < ChunkSizeY; y++)
            {
                HeightMap[x][y] = WorldGenerator.GetNoise(x + chunk->p_Position.x * ChunkSizeX, y + chunk->p_Position.z * ChunkSizeZ);
            }
        }

        for (int x = 0; x < ChunkSizeX; x++)
        {
            for (int z = 0; z < ChunkSizeZ; z++)
            {
                generated_x = x;
                generated_z = z;

                switch (chunk_biome)
                {
                    case Biome::Grassland :
                    {
                        generated_y = (HeightMap[x][z] / 2 + 1.0) * (ChunkSizeY - 32);
                        Structure = &WorldStructureTree;
                        break;
                    }

                    case Biome::Desert : 
                    {
                        generated_y = (HeightMap[x][z] / 2 + 1.0) * (ChunkSizeY - 32);
                        Structure = &WorldStructureCactus;
                        break;
                    }

                    default :
                    {
                        generated_y = (HeightMap[x][z] / 2 + 1.0) * (ChunkSizeY - 32);
                        Structure = nullptr;
                        break;
                    }
                }

                SetVerticalBlocks(chunk, generated_x, generated_z, generated_y, chunk_biome);
                
                if (WorldTreeGenerator.UnsignedInt(75) == 7 &&
                    generated_x + MaxStructureX < ChunkSizeX && 
                    generated_y + MaxStructureY < ChunkSizeY &&
                    generated_z + MaxStructureZ < ChunkSizeZ &&
                    Structure != nullptr)
                {
                     FillInWorldStructure(chunk, Structure, generated_x, generated_y - 1, generated_z);
                }
            }
        }
    }
}