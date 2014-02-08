#pragma once

namespace util {
	struct file {
		mutable FILE* _file;
		mutable size_t _size;

		file() : _file(0), _size(0) {
		}

		file(const char* name, const char* mode = "rb") : _file(0), _size(0) {
			open(name, mode);
		}

		~file() {
			close();
		}

		inline
		void close() {
			if (_file) {
				fclose(_file);
				_file = 0;
				_size = 0;
			}
		}

		inline
		bool open(const char* name, const char* mode = "rb") {
			close();
			_file = fopen(name, mode);
			return _file != 0;
		}

		inline
		long tell() const {
			return ftell(_file);
		}

		size_t size() const;

		inline
		bool open() const {
			return _file != 0;
		}

		inline
		bool eof() const {
			return feof(_file) != 0;
		}

		inline
		bool error() const {
			return ferror(_file) != 0;
		}

		// handles POD only
		// appends nitems to to
		template <typename T>
		size_t read(std::vector<T>& to, size_t nitems) {
			const long fpos = tell();
			if (fpos == -1)
				throw std::runtime_error("ftell() returned -1");
			const size_t nitemsleft = (size() - fpos)/sizeof(T);
			const size_t toread = (std::min)(nitems, nitemsleft);
			to.resize(to.size() + toread);
			void* head = &to[to.size()-toread];
			return read(head, sizeof(T), toread);
		}

		// handles POD only
		template <typename T>
		size_t write(const std::vector<T>& from) {
			if (from.size()) {
				for (size_t i = 0; i < from.size(); ++i) {
					if (fwrite(&from[i], sizeof(T), 1, _file) != 1) {
						return i;
					}
				}
				return from.size();
				//const void* head = &from.front();
				//return write(head, sizeof(T), from.size());
			}
			return from.size();
		}

		inline
		size_t read(void* ptr, size_t sz, size_t nitems) {
			return fread(ptr, sz, nitems, _file);
		}

		inline
		size_t write(const void* ptr, size_t sz, size_t nitems) {
			return fwrite(ptr, sz, nitems, _file);
		}
	};

	// read entire file into memory
	string read_file(const char* fname);
}

