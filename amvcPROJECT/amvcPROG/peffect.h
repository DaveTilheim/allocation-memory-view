#ifndef __PEFFECT__
#define __PEFFECT__

#define cprint1(str, color) printf("\033[%dm%s\033[0m", color, str)
#define cprint2(str, color1, color2) printf("\033[%d;%dm%s\033[0m", color1, color2, str)
#define cprint3(str, color1, color2, color3) printf("\033[%d;%d;%dm%s\033[0m", color1, color2,color3, str)
#define cprint4(str, color1, color2, color3, color4) printf("\033[%d;%d;%d;%dm%s\033[0m", color1, color2, color3, color4, str)
#define begcprint1(color1) printf("\033[%dm", color1)
#define endcprint() printf("\0330m")
#define helloUser() printf("Hello %s!\n", getenv("USER"))

typedef unsigned long Millsec;
void print_delay(const char *, Millsec);
void print_delay_max(const char *, Millsec);

#define printb(var) print_bin(sizeof(var), &var)
void print_bin(const size_t, const void*);
void print_bini(const size_t, const int);
void print_binf(const float);
void print_bind(const double);

typedef enum
{
	BOLD=1,
	DIM,
	ITALIC,
	UNDERLINE,
	BLINK,
	INVERTED=7,
	HIDDEN,
	DEFAULT=39,
	BLACK=30,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	LIGHT_GRAY,
	DARK_GRAY=90,
	LIGHT_RED,
	LIGHT_GREEN,
	LIGHT_YELLOW,
	LIGHT_BLUE,
	LIGHT_MAGENTA,
	LIGHT_CYAN,
	WHITE,
	DEFAULT_BG=49,
	BLACK_BG=40,
	RED_BG,
	GREEN_BG,
	YELLOW_BG,
	BLUE_BG,
	MAGENTA_BG,
	CYAN_BG,
	LIGHT_GRAY_BG,
	DARK_GRAY_BG=100,
	LIGHT_RED_BG,
	LIGHT_GREEN_BG,
	LIGHT_YELLOW_BG,
	LIGHT_BLUE_BG,
	LIGHT_MAGENTA_BG,
	LIGHT_CYAN_BG,
	WHITE_BG
}Style_t;

#endif


