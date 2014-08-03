# simplex

Time to get back into GL. 3D, procedural, that's the idea at least.

Runs in Linux, requires SDL2.

Run `./configure`, then `./compile`. The executable is compiled to `build/simplex`.

http://codeflow.org/entries/2010/dec/09/minecraft-like-rendering-experiments-in-opengl-4/

![](https://raw2.github.com/krig/simplex/master/data/screenshot.png)


## worldgen studio

goal: allow playing around with terrain generation in realtime without
needing to build tons of ui for it.

* define the whole terrain generation process in a script file. This
  includes what the available biomes are, the functions that generate
  heightmaps etc.

* hotload the whole script including base seed and regenerate the
  landscape

* display heightmaps as 2d views, fly around in map, scroll wheel to
  show different layers

* goal 2: generate trees and other world structures

* goal 3: generate water, rivers, waterfalls, erosion...


# some rough ideas

- civilizations: generate pockets of societies both over and
  underground

- dwarf cities
- goblin caves
- elven forest fortresses
- human castles

- have a much deeper underground with dangerous stuff like zombie
  crypts and demon pits deep low, and civilizations in the middle

- put dwarf cities in mountains


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



chunks are 16x16x256 blocks -
- generate meshes for every material in a chunk
- 



# from spoutcraft

http://spoutcraft.org/threads/vanilla-terrain-generation-algorithms-explained.5352/

Note: I'm temporarily placing this in developer ramblings. When this is complete, I'll move it to the wiki.

Vanilla terrain generation: in-depth explanation

So, as requested by many users on another thread on the Spout forums, here's a detailed breakdown of Vanilla terrain generation algorithms. This only covers the basic terrain, or more precisely the various implementations of the generate() and generateTerrain() methods of the BiomeGenerator class.

Common Generation Code
All three world generators (normal, Nether and The End) have a common superclass in Vanilla, called VanillaBiomeGenerator. This class' purpose is simple, fill chunks under 0 with bedrock, since Minecraft clients don't support vertically infinite worlds. All chunks at or above are passed on to the subclasses. For the Nether and The End generators, a second intermediate superclass is used, called VanillaSingleBiomeGenerator. Due to the Nether and The End having only one biome throughout the world, a custom implementation of generateBiomes() is used. This class doesn't affect generation in any other way.

The three generators also have similar noise generators. It's relatively simple noise, three perlin modules are used: elevation (large and smooth), roughness (smaller, and rougher) and detail (smallest, and a bit rough). They are combined using roughness * detail + elevation. The noise is scaled to make it larger, and in some cases (Nether), to stretch it horizontally. Turbulence is then added to help reduce feature redundancy, and finally, the noise values are clamped between -1 and 1, to make it easier to work with.

Now, onto the generators themselves.

Normal Generation
First, you have to understand that each normal biome is defined by a maximum and a minimum elevation value, in world coordinates. So a biome with min = 56 and max = 78 would only have air above 78, and only stone bellow 56. Let's break the generation down step by step:
Any chunk above the generator height is ignored (this will result in an air chunk). Generator height is calculated using the maximum of the maximum heights for all biomes. For a reason I'll explain soon, the actual height used is the next multiple of four.
Now we define the size of the terrain to generate. The x and z sizes used are the same as the CuboidShortBuffer to fill, but the y size used is limited to the height, to prevent unnecessary generation.
The noise is seeded. Each source module gets it's own seed, simply by multiplying the world seed with a prime number.
The noise is now generated. The fastNoise() method from the WorldGeneratorUtils class does something very important for performance here. The noise array that is returned is generated using reduced sampling. That means that not all values are obtained directly from the noise module passed to the method. The sampling rate defines the gap between each value. A four times sampling rate is used in all of the Vanilla generators. To fill in the gaps, trilinear interpolation is used, this is why we use multiples of 4 for the height, it simplifies the interpolation work.
Now we iterate, first on x and z. What we need to do in order to prevent jagged edges where biomes meet is to calculate an average of all adjacent biomes values in a certain radius. To do this, the box filtering algorithm is used. Basically, a second iteration is done on x and z in a square around the point, and the minimum and maximum biome values are averaged the usual way (sum divided by count). The length of the side of the square determines the smoothness. The longer the smoother.
Before we can use the noise we generated earlier, there's one more step that needs to be done. If you tried to skip this part and generate the terrain, you'd end up with something like swiss cheese, and with no minimum and maximum boundaries. So far, the distribution of the noise values along y in the array is even, and by that I mean that values at extremities could also be found in the middle, or anywhere else. We need to change that, so that values at the lower end are larger, and at the higher and, smaller. Why? because to decide if we either set a solid block or not at a x, y, z point, what we'll use is density generation. It's pretty simple, if the noise value is above a specific threshold, set a block (stone), else, set another (air or water). If we want solid stone bellow minimum elevation, we must make sure that all noise values bellow that point are above the threshold, and for pure air above the maximum, that all are bellow the threshold.This is done using a linear equation to calculate a bias for a y value. This bias increases from the middle point (halfway between maximum and minimum elevation) outwards, and is added (when nearing minimum) or subtracted (when nearing maximum) to the original noise value. Pass the maximum and minimum elevations, the bias is always larger than 1 so, if noise values are from -1 to 1, we have values from 0 and up when added, and when subtracted, we have values from negative to 0. We now iterate on y, and add or subtract the bias calculated from the y value.
Finally, we set the block depending on the noise value at x, y, z. If it's above or equal to 0, we set stone, else, if above sea level we set air, else we set water.
Nether Generation
Coming soon.

The End Generation
Coming soon.


# notch on minecraft

http://notch.tumblr.com/post/3746989361/terrain-generation-part-1

In the very earliest version of Minecraft, I used a 2D Perlin noise heightmap to set the shape of the world. Or, rather, I used quite a few of them. One for overall elevation, one for terrain roughness, and one for local detail. For each column of blocks, the height was (elevation + (roughness*detail))*64+64. Both elevation and roughness were smooth, large scale noises, and detail was a more intricate one. This method had the great advantage of being very fast as there’s just 16*16*(noiseNum) samples per chunk to generate, but the disadvantage of being rather dull. Specifically, there’s no way for this method to generate any overhangs.
So I switched the system over into a similar system based off 3D Perlin noise. Instead of sampling the “ground height”, I treated the noise value as the “density”, where anything lower than 0 would be air, and anything higher than or equal to 0 would be ground. To make sure the bottom layer is solid and the top isn’t, I just add the height (offset by the water level) to the sampled result.
Unfortunately, I immediately ran into both performance issues and playability issues. Performance issues because of the huge amount of sampling needed to be done, and playability issues because there were no flat areas or smooth hills. The solution to both problems turned out to be just sampling at a lower resolution (scaled 8x along the horizontals, 4x along the vertical) and doing a linear interpolation. Suddenly, the game had flat areas, smooth hills, and also most single floating blocks were gone.
The exact formula I use is a bit involved (and secret!), but it
evolved slowly over time as I worked on the game. It still uses the 2D
elevation and noisyness maps, though.

"11. Ahawks This may be "confidential "
information, but I have to ask: As a developer , I
am amazed at the amazing world that ' s created
dynamically. Towering mountains, twisting
caves opening up to caverns , islands , precious
ores sprinkled throughout, etc . Notch, could
you explain at a high level how the world
generation algorithm does this ?
I ' m not sure how to explain it without getting
technical. . The complicated high level technical
version is: First I generate a linearly interpolated
3 d perlin noise offset along the y axis. I fill that
in so that everything except the top x blocks is
stone, then I do a second pass to add features
like grass, trees , gravel, sand , caves and
flowers . The world is generated in chunks of
16x16x128 blocks , based of pseudorandom
seeds that are a mix of the level base seed and
the chunk location in the world . This ensures
that you always get the same terrain in an area
regardless of what direction you traveled there
from."

biome: pseudorandomly generate humidity and temperature for a region,
region influences parameters to the terrain generation, then region
also influences tree cover, grass cover, type of trees, likelihood of
structures of different kinds etc.

STOP! This might actually not be a good idea - minecraft stopped doing
this. First off, changing the humidity/temperature function reassigns
biomes for all chunks already generated - this might not be good
(although, why not? Hmm.)

What MC does now is randomly/fractally assign biome on chunk
generation, and then storing the biome choice in the chunk data.

Ah.. right. biomes are chosen per region, but chunks are generated by
chunk, not region. So either whole regions need to be generated all at
once, or.. the first chunk generated in a region also creates the
region info, and this is used to generate more chunks in that
region.


Desired state: Have many different biomes. Make it easy to create new
biomes - can have larger categories of biomes that share object types
/ general terrain type, but have varying generator inputs (so flat,
hilly, extreme, river etc. variants of different biomes). Generate
biomes fairly randomly to make things unpredictable, don't try to keep
biomes close to each other. BUT - make biome size variable within a
certain range.

Difficult part: how to do this infinitely and predictable. Need a
function that returns biome for a given chunk based on the seed, where
the biomes are at least N large but no larger than M, with something
like a standard deviation within that range.

Also: Need good visualization/editing mode. Best would be if biomes
could be tweaked in an editor, that allows for test-generation of
chunks of that biome for a given set of inputs...


Q2: how to smoothly connect biomes?


Things controlled by biome:

* Terrain shape - ground height, noise function inputs
* Sky color
* Sun color
* Horizon color
* Sun position - cold climates have shorter days and longer nights
* Cloud cover - height, density
* Weather - rain, snow, sandstorm, thunder, blistering heat, fog,
  cyclones, ice storm, firestorm/bush fire, hail
* Fog color, fog density
* Mob spawning - possible mobs, likelihood of mobs
* Cave generation
* Resource density
* Grass type / grass cover
* Tree type, tree cover
* Snow as a function of height
* Frequency of lakes
* Volcanoes
* Valleys, gorges
* Buildings, societies

every biome has a layer of <filler block> covered with a layer of <top
block>, where the thickness of the layers and type of block is
controlled by the biome.

Climate categories:

* Tropical - Wet, wet and dry
* Dry - semiarid, arid
* Moderate - mediterranean, humid subtropical, marine west coast
* Continental - humid continental, subarctic
* Polar - tundra, ice cap, highlands


* biomes with seasons?

https://en.wikipedia.org/wiki/K%C3%B6ppen_climate_classification

* Maybe some aspects of biomes should be controlled by **seasons**,
  where the possible seasons are controlled by the biome.


generating chunks and structures that are larger than a single chunk
-- how? biomes, structures, villages, cities, dungeons...


ok, I think this is how it works: generate chunks in blocks of 10x10
at a time. By doing that, decisions can be made across multiple
chunks, like creating larger structures and so on.

so chunk generation is influenced by nearby chunks... but how can that
be made so that chunks can generate in any order??

no, I think it has to be done like this: biome generation happens on a
lower frequency noise function with the same seed, so that for a given
chunk, the biome choice is always deterministic. However, what that
biome is is likely to be the same or similar as nearby
chunks. Something like that.

Other larger structures would have to be generated with similar
things.. so for example, is this chunk in a stronghold? well, evaulate
the stronghold function for x>>6,y>>6, if true, there is a stronghold
here. Now the question is, exactly how it fits into this particular
chunk. That's more complicated... but generally we need to avoid
iterative algorithms and stick to deterministic algorithms.

step 1: generate terrain -

use noise function to create terrain, if density > 0, stone, else air

step 2: replace blocks for biome

1. if level <= 0 + rand(5) => bedrock
2. if ??? use topblock / fillerblock
3. 


when generating chunk X, also generate the temp/rainfall/height for
surrounding chunks (or check directly if they are already loaded)

store loadlevel in chunk, check if loadlevel < whatever is necessary
for the information we're looking for, if so load to the required
level

multi-octave 2d noise for elevation
3d noise for density (to get overhangs)
perlin worms for caves



so that would be something like...

1. noise maps + anything that modifies terrain
2. structures
3. vegetation
4. entities


I think I know enough to start doing basic terrain generation. I need
to work on the core rendering library / structure though. Mesh /
material, etc.

I should do a terrain studio of some kind, sliders to play around with
noise values and so on... or maybe just reloading on the fly for noise
definition scripts, so I can edit the scripts and see the results
instantly.

So: Hotloading of assets, basic scripting (lua?), render tech
things that should definitely be hotload-scriptable are the terrain
functions and biome generation and so on, so I can resave the file and
the terrain regenerates (or optionally generates on new chunks, to
test that functionality)



idea: generate the 2d elevation / temperature / humidity maps for
large areas, all choices for terrain like top block / filler block and
so on are derived from the elevation / temperature / humidity

another thought: if I require that to tesselate/render chunk X, I need
to also generate all chunks around it, that would allow for not
generating extra faces on chunk borders (since we can check for air
blocks in neighbour chunks)



* asset management:

  - load
  - inotify reload

* builtin web server for realtime tweaking

  - expose internal variables
  - save state of variables with history

# game scripting / definition

- have a single big database file with all configurables, written in
  yaml - that file can include other files maybe?



# materials

- shader
- textures
- transparency
- mapping shader to a mesh -- if the mesh doesn't have normals,
  generate normals.. if no colors, generate colors. and so on.
  but also allow for completely custom materials / meshes
- flags like lighting, fog, emits light, reflective, animated...
- blend function...
- step sound, type of particle emitted, physical properties,
  collisions

# meshes

- generated vertex array objects
  - vertex
  - normal
  - binormal
  - tangent
  - color
  - ...etc
  - tesselator
  - spheres, boxes, etc.

- hierarchical animated objects
  - generate meshes...

# renderable

