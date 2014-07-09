#pragma once

/*

  describes the chunk data format in memory and on disk

  kinds of chunks:

  solid chunk - mostly a single material (like air or stone)
  pure chunks - chunks with only procedural contents
  regular chunks -

  data stored in IFF like file format: every section of a file
  begins with an ID and a length in bytes.

  chunk data as stored in a file is stored in a compressed format,
  so each file contains a map of all chunks stored in that file

  a chunk is 16x16x256 blocks,
  a region is 32x32 chunks
  a world has a metadata file called _meta,
  in a plain text format
  a world stores regions as region_XX_YY.dat

*/

namespace worldfile {

	struct tag {
		uint32_t id;
		uint32_t len;
	};

	struct chunk : public tag {
	};

	struct region : public tag {
	};

	region* load_region(const char* mapname, int x, int y) {
		char fname[512];
		snprintf(fname, 512, "%s/region_%02d_%02d.dat", mapname, x, y);
		FILE* f = fopen(fname, "rb");
		region head;
		size_t l = fread(&head, sizeof(region), 1, f);
		fclose(f);
		return r;
	}

	void save_region(region* r) {
	}
};
