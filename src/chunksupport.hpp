#pragma once

// support tools for chunk generation (like noise functions and so on)

/*
  for(int i = 0; i < CHUNKMAX_X; i++)
    for(int j = 0; j < CHUNKMAX_Y; j++)
        for(int k = 0; k < CHUNKMAX_Z; k++)
            if(isSolid(perlinNoise.get(chunkPosition.x + i,
                                         chunkPosition.y + j,
                                         chunkPosition.z + k))
                thisChunk[i,j,k] = new Voxel(solid);
            else
                thisChunk[i,j,k] = new Voxel(air);
*/

