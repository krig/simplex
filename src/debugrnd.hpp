#pragma once

// debug render helpers
// draw debug stuff on screen
// queued and then rendered as a renderpass

namespace debugrnd {
	void obb();
	void aabb();
	void line();
	void point();
	void text();
	void sphere();

	void render();
}
