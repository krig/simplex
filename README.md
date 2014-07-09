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

