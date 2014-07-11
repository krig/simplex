# simplex

Time to get back into GL. 3D, procedural, that's the idea at least.

Runs in Linux, requires SDL2.

Run `./configure`, then `./make`. The executable is compiled to `build/simplex`.

http://codeflow.org/entries/2010/dec/09/minecraft-like-rendering-experiments-in-opengl-4/

![](https://raw2.github.com/krig/simplex/master/data/screenshot.png)


# world format

this is roughly the minecraft chunk format:

a chunk on disk is 16x16x256 blocks. This is visually split into
16^3-sized chunks for rendering. A radius of 10 chunks around the
player is loaded (in multiplayer). These are active chunks, so AI,
trees grow, water flows, items time out etc.

# a better format

regions are zip-compressed collections of 16^3 chunks, 32x32x16 in a
single file

metadata for a region is stored separately

store YZX for compression benefits



# major things to do

* fps controls, running, view bobbing, jumping, physics

* block texture generation
  - generate 16x16 texture based on color + intensity noise
  - normal maps
  - specular maps
  - generate combination textures like stone + gold, stone + diamond
    etc.

* world block generation
  - terrain generation
  - biomes
  - caves
  - tesselation
  - virtual addressing for chunks

* sky
  - day/night cycle
  - clouds



# terrain generation notes

http://pcg.wikidot.com/pcg-algorithm:whittaker-diagram

http://www.neilblevins.com/cg_education/procedural_noise/procedural_noise.html

- biome selected based on temperature + humidity
- generate base terrain using 3d simplex noise + biome selection
- carve caves
- ore distribution
- water, lava

Musgrave's Ridged Multifractal - good for mountain ranges


# render engine

- some things need to be deferred for later:
  - alpha blended things, defer and sort
  - ideally other things should be sorted by material
  - debug rendered lines and spheres and so on

- ui, text
- make a ui texture, render in 4x scale (nearest)

