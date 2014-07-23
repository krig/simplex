#pragma once

struct Color : public vec4 {
	explicit Color(uint32_t argb) : vec4(0, 0, 0, 1.f) {
		uint8_t r, g, b, a;
		r = (argb >> 16) & 0xff;
		g = (argb >> 8) & 0xff;
		b = (argb) & 0xff;
		a = (argb >> 24) & 0xff;
		x = (float)r / 255.f;
		y = (float)g / 255.f;
		z = (float)b / 255.f;
		w = (float)a / 255.f;
	}

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : vec4(0, 0, 0, 1.f) {
		x = (float)r / 255.f;
		y = (float)g / 255.f;
		z = (float)b / 255.f;
		w = (float)a / 255.f;
	}

	Color(float R, float G, float B, float A = 1.f) : vec4(R, G, B, A) {
	}

};

namespace colors {
	namespace flatui {
		static const Color turquoise(0xff1abc9c);
		static const Color emerald(0xff2ecc71);
		static const Color peter_river(0xff3498db);
		static const Color amethyst(0xff9b59b6);
		static const Color wet_asphalt(0xff34495e);
		static const Color green_sea(0xff16a085);
		static const Color nephritis(0xff27ae60);
		static const Color belize_hole(0xff2980b9);
		static const Color wisteria(0xff8e44ad);
		static const Color midnight_blue(0xff2c3e50);
		static const Color sun_flower(0xfff1c40f);
		static const Color carrot(0xffe67e22);
		static const Color alizarin(0xffe74c3c);
		static const Color clouds(0xffecf0f1);
		static const Color concrete(0xff95a5a6);
		static const Color orange(0xfff39c12);
		static const Color pumpkin(0xffd35400);
		static const Color pomegranate(0xffc0392b);
		static const Color silver(0xffbdc3c7);
		static const Color asbestos(0xff7f8c8d);
	}

	namespace dawnbringer {
		static const Color black(0xff140c1c);
		static const Color dark_red(0xff442434);
		static const Color dark_blue(0xff30346d);
		static const Color dark_gray(0xff4e4a4e);
		static const Color dark_brown(0xff854c30);
		static const Color dark_green(0xff346524);
		static const Color red(0xffd04648);
		static const Color gray(0xff757161);
		static const Color blue(0xff597dce);
		static const Color orange(0xffd27d2c);
		static const Color light_gray(0xff8595a1);
		static const Color green(0xff6daa2c);
		static const Color beige(0xffd2aa99);
		static const Color light_blue(0xff6dc2ca);
		static const Color yellow(0xffdad45e);
		static const Color white(0xffdeeed6);
	}

	namespace tango {
		static const Color aluminium_1(0xff2e3436);
		static const Color aluminium_2(0xff555753);
		static const Color aluminium_3(0xff888a85);
		static const Color aluminium_4(0xffbabdb6);
		static const Color aluminium_5(0xffd3d7cf);
		static const Color aluminium_6(0xffecf0eb);
		static const Color aluminium_7(0xfff7f8f5);
		static const Color butter_1(0xff291e00);
		static const Color butter_2(0xff725000);
		static const Color butter_3(0xffc4a000);
		static const Color butter_4(0xffedd400);
		static const Color butter_5(0xfffce94f);
		static const Color butter_6(0xfffffc9c);
		static const Color butter_7(0xfffeffd0);
		static const Color orange_1(0xff301700);
		static const Color orange_2(0xff8c3700);
		static const Color orange_3(0xffce5c00);
		static const Color orange_4(0xfff57900);
		static const Color orange_5(0xfffcaf3e);
		static const Color orange_6(0xffffd797);
		static const Color orange_7(0xfffff0d7);
		static const Color chocolate_1(0xff271700);
		static const Color chocolate_2(0xff503000);
		static const Color chocolate_3(0xff8f5902);
		static const Color chocolate_4(0xffc17d11);
		static const Color chocolate_5(0xffe9b96e);
		static const Color chocolate_6(0xffefd0a7);
		static const Color chocolate_7(0xfffaf0d7);
		static const Color chameleon_1(0xff173000);
		static const Color chameleon_2(0xff2a5703);
		static const Color chameleon_3(0xff4e9a06);
		static const Color chameleon_4(0xff73d216);
		static const Color chameleon_5(0xff8ae234);
		static const Color chameleon_6(0xffb7f774);
		static const Color chameleon_7(0xffe4ffc7);
		static const Color skyblue_1(0xff00202a);
		static const Color skyblue_2(0xff0a3050);
		static const Color skyblue_3(0xff204a87);
		static const Color skyblue_4(0xff3465a4);
		static const Color skyblue_5(0xff729fcf);
		static const Color skyblue_6(0xff97c4f0);
		static const Color skyblue_7(0xffdaeeff);
		static const Color plum_1(0xff170720);
		static const Color plum_2(0xff371740);
		static const Color plum_3(0xff5c3566);
		static const Color plum_4(0xff75507b);
		static const Color plum_5(0xffad7fa8);
		static const Color plum_6(0xffe0c0e4);
		static const Color plum_7(0xfffce0ff);
		static const Color scarletred_1(0xff270000);
		static const Color scarletred_2(0xff600000);
		static const Color scarletred_3(0xffa40000);
		static const Color scarletred_4(0xffcc0000);
		static const Color scarletred_5(0xffef2929);
		static const Color scarletred_6(0xfff78787);
		static const Color scarletred_7(0xffffcccc);
	}
}
