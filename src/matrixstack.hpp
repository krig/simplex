#pragma once

template <uint32_t Size = 32>
struct MatrixStack {
	MatrixStack() {
		_top = 0;
	}

	void load_identity() {
		_stack[_top] = mat4();
	}

	void load(const mat4& m) {
		_stack[_top] = m;
	}

	MatrixStack& operator*=(const mat4& m) {
		_stack[_top] *= m;
		return *this;
	}

	mat4& get() {
		return _stack[_top];
	}

	const mat4& get() const {
		return _stack[_top];
	}

	void push() {
		if (++_top >= Size)
			throw error("MatrixStack overflow");
		_stack[_top] = _stack[_top - 1];
	}

	void pop() {
		if (--_top < 0)
			throw error("MatrixStack underflow");
	}

private:
	mat4 _stack[Size];
	int _top;
};
