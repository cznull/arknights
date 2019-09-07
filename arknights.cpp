// arknights.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "arknights.h"
#include <gl/glew.h>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <ft2build.h>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>
#include "vec.h"
#include FT_FREETYPE_H

#ifdef _DEBUG
#pragma comment(lib,"opencv_world411d.lib")
#else
#pragma comment(lib,"opencv_world411.lib")

#endif 

#define _SHOW
#define _RECORD

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"freetype.lib")

#define MAX_LOADSTRING 100

struct tween_t {
	float x, y;
	float sx, sy;
	int frame;
};

struct image_t {
	float x, y;
	float sx, sy;
	GLuint tex;
	tween_t tween;
};

struct char_t {
	GLuint c1, c2;
	int focus;
};

struct charcutin_t {
	GLuint cc;
	float x, width;
};

struct blocker_t {
	float r, g, b, a;
};

struct dec_t {
	GLuint dectex;
	int count;
	float turn[8];
};

struct texdata_t {
	int w, h;
	std::vector<int> data;
};

struct framedes_t {
	GLuint dia, cname;
	float diapos, turnpos, cnamepos;
	float amount;
	blocker_t blocker;
	image_t image, bg;
	char_t c;
	charcutin_t cc;
	dec_t dec;
}; 

struct frametexdate_t {
	texdata_t dia, cname;
	texdata_t image, bg;
	texdata_t c1,c2;
	texdata_t cc;
};

struct fadedes_t {
	int imageall, imageleft;
	int bgall, bgleft;
	int blockerall, blockerleft;
	int charall, charleft;
	int camerashakeall, camerashakeleft, fadeout, xf;
	int charcutinall, charcutinleft;
	float moveleft, moveall, sx, sy, yf, xt, yt;
};

struct framecache_t {
	framedes_t frame;
	frametexdate_t frametexdata;
};

struct musicpic_t {
	size_t length;
	int rate;
	float2 *data;
};

struct musicevent_t {
	musicpic_t intro, loop;
	int starttime, fadetime;
	float volume;
};


struct imgname_t {
	std::string rgb, a;
};

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HDC hdc1, hdc2;
HGLRC m_hrc;
int mx, my, cx, cy;

GLuint texbuffer, framebuffer, depthbuffer;
GLuint texbufferim, framebufferim, depthbufferim;
GLuint bg, image, c1, c2, cc, dia, cname, dec,diabg;
GLuint bg_, image_, c1_, c2_, cc_;
cv::VideoWriter writer;
cv::Mat frame(1080, 1920, CV_8UC3, cv::Scalar(0, 0, 0));
unsigned char frim[2048 * 2048 * 3];
int totalframe = 0;
std::map<std::string, musicpic_t> musicpics;
std::map<std::string, std::string> musicmap;
std::map<std::string, std::vector<musicevent_t>> channel;
std::map<std::string, std::vector<imgname_t>> charnamemap;
std::string filename;

FT_Library ftlib;
FT_Face face;
FILE* filog;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int play(int cur);

bool eventsort(const musicevent_t &a,const musicevent_t &b) {
	return a.starttime < b.starttime;
}

int cmp(const char* s1, const char* s2, int nof2) {
	for (int i = 0; i < nof2; i++) {
		if (!s1[i] || s1[i] != s2[i]) {
			return 1;
		}
	}
	return 0;
}

int find(const char* text, int textlength, const char* tag, int taglength) {
	int i;
	for (i = 0; i <= textlength - taglength; i++) {
		if (!cmp(text + i, tag, taglength)) {
			return i;
		}
	}
	return -1;
}

int cmp(const wchar_t* s1, const wchar_t* s2, int nof2) {
	for (int i = 0; i < nof2; i++) {
		if (!s1[i] || s1[i] != s2[i]) {
			return 1;
		}
	}
	return 0;
}

int find(const wchar_t* text, int textlength, const wchar_t* tag, int taglength) {
	int i;
	for (i = 0; i <= textlength - taglength; i++) {
		if (!cmp(text + i, tag, taglength)) {
			return i;
		}
	}
	return -1;
}

int getline(char* s, int length, std::vector<std::string>& line) {
	int cur, next;
	cur = 0;
	next = find(s + cur, length - cur, "\n", 1);
	while (next >= 0) {
		if (next > 0) {
			//int i;
			//for (i = 0; i < next&&s[cur+i]=='/'; i++);
			//if (next > i) {
				line.push_back(std::string(s + cur, next));
			//}
		}
		else {
			//line.push_back(std::string());
		}
		cur += next + 1;
		next = find(s + cur, length - cur, "\n", 1);
	}
	if (length > cur) {
		line.push_back(std::string(s + cur, length - cur));
	}
	return 0;
}

int getline(const char* s, int length, std::vector<std::string>& line, const char* flag, int flaglength) {
	int cur, next;
	cur = 0;
	next = find(s + cur, length - cur, flag, flaglength);
	while (next >= 0) {
		if (next > 0) {
			//int i;
			//for (i = 0; i < next&&s[cur+i]=='/'; i++);
			//if (next > i) {
			line.push_back(std::string(s + cur, next));
			//}
		}
		cur += next + flaglength;
		next = find(s + cur, length - cur, flag, flaglength);
	}
	if (length > cur) {
		line.push_back(std::string(s + cur, length - cur));
	}
	return 0;
}

int loadmusicmap(void) {
	FILE* fi;
	char* ficon;
	int ficount;
	std::vector<std::string> line;
	if (!fopen_s(&fi, "D:/files/data/5/avg/story_variables.json", "rb")) {
		fseek(fi, 0, SEEK_END);
		ficount = ftell(fi);
		fseek(fi, 0, SEEK_SET);
		ficon = (char*)malloc(ficount * sizeof(char));
		ficount = fread(ficon, 1, ficount, fi);
		fclose(fi);
		getline(ficon, ficount, line, ",", 1);
		for (int i = 0; i < line.size(); i++) {
			std::vector<std::string> line1;
			getline(line[i].c_str(), line[i].length(), line1, "\"", 1);
			if (line1.size() == 4) {
				int s = -1;
				for (int j = 0; j < line1[3].length(); j++) {
					if (line1[3][j] == '/') {
						s = j;
					}
				}
				musicmap.insert(std::make_pair(line1[1], std::string(line1[3].c_str() + s + 1, line1[3].length() - s - 1)));
			}
		}
		free(ficon);
	}
	return 0;
}



int loadcharnamemap(void) {
	FILE* fi;
	char* ficon;
	int ficount;
	std::vector<std::string> line;
	if (!fopen_s(&fi, "D:/files/data/5/avg/charname.txt", "rb")) {
		fseek(fi, 0, SEEK_END);
		ficount = ftell(fi);
		fseek(fi, 0, SEEK_SET);
		ficon = (char*)malloc(ficount * sizeof(char));
		ficount = fread(ficon, 1, ficount, fi);
		fclose(fi);
		getline(ficon, ficount, line, "\n\n", 2);
		for (int i = 0; i < line.size(); i++) {
			std::vector<std::string> line1;
			getline(line[i].c_str(), line[i].length(), line1, "\n", 1);
			std::vector<imgname_t> imgs;
			for (int j = 1; j < line1.size(); j++) {
				std::vector<std::string> line2;
				getline(line1[j].c_str(), line1[j].length(), line2, "\t", 1);
				if (line2.size() == 2) {
					imgs.push_back({ line2[0],line2[1] });
				}
			}
			charnamemap.insert(std::make_pair(line1[0], imgs));
		}
	}
	return 0;
}

int savemusic(musicpic_t music) {
	FILE* fi;
	int x;
	if (!fopen_s(&fi, ("C:/files/avg/" + filename + ".wav").c_str(), "wb")) {
		fwrite("RIFF", 1, 4, fi);
		x = music.length * 8 + 36;
		fwrite(&x, 1, 4, fi);
		fwrite("WAVEfmt ", 1, 8, fi);
		x = 16;
		fwrite(&x, 1, 4, fi);
		x = 3;
		fwrite(&x, 1, 2, fi);
		x = 2;
		fwrite(&x, 1, 2, fi);
		x = 44100;
		fwrite(&x, 1, 4, fi);
		x = 44100 * 8;
		fwrite(&x, 1, 4, fi);
		x = 4;
		fwrite(&x, 1, 2, fi);
		x = 32;
		fwrite(&x, 1, 2, fi);
		fwrite("data", 1, 4, fi);
		x = music.length * 8;
		fwrite(&x, 1, 4, fi);
		fwrite(music.data, 1, music.length * 8, fi);
		fclose(fi);
	}
	return 0;
}

musicpic_t loadmusic(std::string s) {
	std::map<std::string, musicpic_t>::const_iterator it;
	std::map<std::string, std::string>::const_iterator it1;
	it = musicpics.find(s);
	if (it == musicpics.end()) {
		FILE* fi;
		char* ficon;
		int ficount;
		musicpic_t music = { 0 };
		std::string filename;
		if (s[0] == '$') {
			filename = musicmap[s.substr(1)];
		}
		else {
			int pos = -1;
			for (int j = 0; j < s.length(); j++) {
				if (s[j] == '/') {
					pos = j;
				}
			}
			filename = s.substr(pos + 1);
		}
		if (!fopen_s(&fi, ("D:/files/data/5/avg/AudioClip/" +filename + ".wav").c_str(), "rb")) {
			fseek(fi, 0, SEEK_END);
			ficount = ftell(fi);
			fseek(fi, 0, SEEK_SET);
			ficon = (char*)malloc(ficount * sizeof(char));
			ficount = fread(ficon, 1, ficount, fi);
			fclose(fi);
			int n, b;
			n = ficon[22];
			b = ficon[34];
			int pos = find(ficon, ficount, "data", 4);
			if (n == 2) {
				if (b == 16) {
					music.length = ((int*)(ficon + pos + 4))[0] / 4; 
					music.data = (float2*)malloc(music.length * 8);
					for (int i = 0; i < music.length; i++) {
						music.data[i].x = ((short*)(ficon + pos + 8))[i * 2] * (1.0 / 32768.0);
						music.data[i].y = ((short*)(ficon + pos + 8))[i * 2 + 1] * (1.0 / 32768.0);
					}
				}
			}
			else {
				if (b == 16) {
					music.length = ((int*)(ficon + pos + 4))[0] / 2;
					music.data = (float2*)malloc(music.length * 8);
					for (int i = 0; i < music.length; i++) {
						music.data[i].x = ((short*)(ficon + pos + 8))[i] * (1.0 / 32768.0);
						music.data[i].y = ((short*)(ficon + pos + 8))[i] * (1.0 / 32768.0);
					}
				}
			}
		}
		else if(s.length()){
			fprintf(filog, "%cannot open:%s\n", s.c_str());
		}
		musicpics.insert(std::make_pair(s, music));
		return music;
	}
	else {
		return it->second;
	}
}

int addevent(std::string channelname, std::string intro, std::string loop, float delay,float fadetime,float volume) {
	musicevent_t event;
	event.starttime = (totalframe * (1.0 / 60.0) + delay)*44100.0;
	event.fadetime = fadetime * 44100.0;
	event.intro = loadmusic(intro);
	event.loop = loadmusic(loop);
	event.volume = volume;
	channel[channelname].push_back(event);
	return 0;
}

int getmusicval(musicevent_t e, int t,float2 &x) {
	if (t >= e.starttime) {
		if (t < e.starttime + e.intro.length) {
			x = e.intro.data[t - e.starttime];
			x = e.volume * x;
		}
		else {
			if (e.loop.length) {
				x = e.loop.data[(t - e.starttime - e.intro.length) % e.loop.length];
				x = e.volume * x;
			}
			else {
				x = { 0.0,0.0 };
			}
		}
	}
	else {
		x = { 0.0,0.0 };
	}
	return 0;
}

int genmusic(void) {
	musicpic_t m;
	m.length = totalframe * (44100.0 / 60.0);
	m.data = (float2*)malloc(m.length * 8);
	memset(m.data, 0, m.length * 8);
	std::map<std::string, std::vector<musicevent_t>>::const_iterator it;
	for (it = channel.begin(); it != channel.end(); it++) {
		std::vector<musicevent_t> eventvec(it->second);
		std::sort(eventvec.begin(), eventvec.end(), eventsort);
		musicevent_t last = { 0 };
		for (int i = 0; i < eventvec.size(); i++) {
			int end;
			float2 curmusic, lastmusic;
			end = m.length;
			if (eventvec[i].loop.length) {
				if (i < eventvec.size() - 1) {
					end = eventvec[i + 1].starttime < end ? eventvec[i + 1].starttime : end;
				}
			}
			else {
				end = eventvec[i].starttime + eventvec[i].intro.length < end ? eventvec[i].starttime + eventvec[i].intro.length : end;
			}
			for (int curpos = eventvec[i].starttime; curpos < end; curpos++) {
				getmusicval(eventvec[i], curpos, curmusic);
				getmusicval(last, curpos, lastmusic);
				if (curpos < eventvec[i].starttime + eventvec[i].fadetime) {
					m.data[curpos] = m.data[curpos] + ((float)(curpos - eventvec[i].starttime) / eventvec[i].fadetime) * curmusic;
					m.data[curpos] = m.data[curpos] + (1.0 - (float)(curpos - eventvec[i].starttime) / eventvec[i].fadetime) * curmusic;
				}
				else {
					m.data[curpos] = m.data[curpos] + curmusic;
				}
				last = eventvec[i];
			}
		}
	}
	savemusic(m);
	return 0;
}

int drawquad(float x, float y, float sx, float sy, float r, float g, float b, float a, GLuint tex, float width = 1.0) {

#ifdef _SHOW
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	glColor4f(r, g, b, a);
	glTexCoord2f(0.5f - 0.5f * width, 1.0f);
	glVertex3f((x - sx * width) * (9.0f / 16.0f), y - sy, 0.0f);

	glColor4f(r, g, b, a);
	glTexCoord2f(0.5f + 0.5f * width, 1.0f);
	glVertex3f((x + sx * width) * (9.0f / 16.0f), y - sy, 0.0f);

	glColor4f(r, g, b, a);
	glTexCoord2f(0.5f + 0.5f * width, 0.0f);
	glVertex3f((x + sx * width) * (9.0f / 16.0f), y + sy, 0.0f);

	glColor4f(r, g, b, a);
	glTexCoord2f(0.5f - 0.5f * width, 0.0f);
	glVertex3f((x - sx * width) * (9.0f / 16.0f), y + sy, 0.0f);
	glEnd();
#endif
	return 0;
}

int drawquad1(float x, float y, float sx, float sy, float r, float g, float b, float a, GLuint tex) {

#ifdef _SHOW
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	glColor4f(r, g, b, a);
	glTexCoord2f(-0.5f, 1.5f);
	glVertex3f((x - 2.0 * sx) * (9.0f / 16.0f), y - 2.0 * sy, 0.0f);

	glColor4f(r, g, b, a);
	glTexCoord2f(1.5f, 1.5f);
	glVertex3f((x + 2.0 * sx) * (9.0f / 16.0f), y - 2.0 * sy, 0.0f);

	glColor4f(r, g, b, a);
	glTexCoord2f(1.5f, -0.5f);
	glVertex3f((x + 2.0 * sx) * (9.0f / 16.0f), y + 2.0 * sy, 0.0f);

	glColor4f(r, g, b, a);
	glTexCoord2f(-0.5f, -0.5f);
	glVertex3f((x - 2.0 * sx) * (9.0f / 16.0f), y + 2.0 * sy, 0.0f);
	glEnd();
#endif
	return 0;
}

int exectween(image_t& image) {
	if (image.tween.frame > 0) {
		image.tween.frame--;
		image.x += image.tween.x;
		image.y += image.tween.y;
		image.sx += image.tween.sx;
		image.sy += image.tween.sy;
	}
	return 0;
}

int draw(framedes_t &f, framedes_t &f1,fadedes_t &fade) {
	totalframe++;
	glViewport(0, 0, 1920, 1080);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,framebuffer);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(0x00004100);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (fade.camerashakeleft) {
		fade.camerashakeleft--;
		fade.moveleft -= 1.0;
		while(fade.moveleft<0.0) {
			fade.moveleft += fade.moveall;
			fade.xf = fade.xt;
			fade.yf = fade.yt;
			fade.xt = fade.sx * (rand() * (1.0 / RAND_MAX) * 2.0 - 1.0);
			fade.yt = fade.sy * (rand() * (1.0 / RAND_MAX) * 2.0 - 1.0);
			if (fade.fadeout) {
				fade.xt *= 1.0 * fade.camerashakeleft / fade.camerashakeall;
				fade.yt *= 1.0 * fade.camerashakeleft / fade.camerashakeall;
			}
		}
		glTranslatef((fade.xf*(fade.moveleft)+fade.xt*(fade.moveall-fade.moveleft)) *(1.0/fade.moveall)* (9.0f / 16.0f), (fade.yf * (fade.moveleft) + fade.yt * (fade.moveall - fade.moveleft)) * (1.0 / fade.moveall), 0);
	}

	if (fade.bgleft) {
		if (f1.bg.tex) {
			exectween(f1.bg);
			drawquad1(f1.bg.x, f1.bg.y, (16.0 / 9.0) * f1.bg.sx, f1.bg.sy, 1.0, 1.0, 1.0, 1.0 * fade.bgleft / (fade.bgall+1.0), f1.bg.tex);
		}
	}

	if (f.bg.tex) {
		exectween(f.bg);
		drawquad1(f.bg.x, f.bg.y, (16.0 / 9.0) * f.bg.sx, f.bg.sy, 1.0, 1.0, 1.0, 1.0 - 1.0 * fade.bgleft / (fade.bgall + 1.0), f.bg.tex);
	}

	if (fade.bgleft) {
		fade.bgleft--;
	}


	/*glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferim);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(0x00004100);*/

	if (fade.imageleft) {
		if (f1.image.tex) {
			exectween(f1.image);
			drawquad1(f1.image.x, f1.image.y, (16.0 / 9.0) * f1.image.sx, f1.image.sy, 1.0, 1.0, 1.0, 1.0 * fade.imageleft / (fade.imageall + 1.0), f1.image.tex);
		}
	}

	if (f.image.tex) {
		exectween(f.image);
		drawquad1(f.image.x, f.image.y, (16.0 / 9.0) * f.image.sx, f.image.sy, 1.0, 1.0, 1.0, 1.0 - 1.0 * fade.imageleft / (fade.imageall + 1.0), f.image.tex);
	}

	if (fade.imageleft) {
		fade.imageleft--;
	}

	/*
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
	drawquad(0, 0, (16.0 / 9.0) * 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, texbufferim);
	*/

	if (fade.charleft) {
		if (f1.c.c1) {
			if (f1.c.c2) {
				if (f1.c.focus == 1) {
					drawquad(0.5555, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c2);
					drawquad(-0.5555, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c1);
				}
				else if (f1.c.focus == 2) {
					drawquad(-0.5555, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c1);
					drawquad(0.5555, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c2);
				}
				else if (f1.c.focus = -1) {
					drawquad(0.5555, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c2);
					drawquad(-0.5555, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c1);
				}
				else {
					drawquad(0.5555, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c2);
					drawquad(-0.5555, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c1);
				}
			}
			else {
				if (f1.c.focus == -1) {
					drawquad(0.0, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c1);
				}
				else {
					drawquad(0.0, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 * fade.charleft / (fade.charall + 1.0), f1.c.c1);
				}
			}
		}
	}

	if (f.c.c1) {
		if (f.c.c2) {
			if (f.c.focus == 1) {
				drawquad(0.5555, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c2);
				drawquad(-0.5555, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c1);
			}
			else if (f.c.focus == 2) {
				drawquad(-0.5555, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c1);
				drawquad(0.5555, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c2);
			}
			else if (f.c.focus == -1) {
				drawquad(0.5555, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c2);
				drawquad(-0.5555, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c1);
			}
			else {
				drawquad(0.5555, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c2);
				drawquad(-0.5555, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c1);
			}
		}
		else {
			if (f.c.focus == -1) {
				drawquad(0.0, -0.5, 1.4, 1.4, 0.5, 0.5, 0.5, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c1);
			}
			else {
				drawquad(0.0, -0.5, 1.4, 1.4, 1.0, 1.0, 1.0, 1.0 - 1.0 * fade.charleft / (fade.charall + 1.0), f.c.c1);
			}
		}
	}

	if (fade.charleft) {
		fade.charleft--;
	}

	if (fade.charcutinleft) {
		if (f1.cc.cc) {
			drawquad(f1.cc.x, -0.41, 1.48, 1.48, 0.5, 0.5, 0.5, 1.0, 0, f1.cc.width/ (2.0*1.48) * fade.charcutinleft / (fade.charcutinall + 1.0));
			drawquad(f1.cc.x, -0.41, 1.48, 1.48, 1.0, 1.0, 1.0, 1.0, f1.cc.cc, f1.cc.width/ (2.0 * 1.48) * fade.charcutinleft / (fade.charcutinall + 1.0));
		}
	}

	if (f.cc.cc) {
		drawquad(f.cc.x, -0.41, 1.48, 1.48, 0.5, 0.5, 0.5, 1.0, 0, f.cc.width/ (2.0 * 1.48) - f.cc.width/ (2.0 * 1.48) * fade.charcutinleft / (fade.charcutinall + 1.0));
		drawquad(f.cc.x, -0.41, 1.48, 1.48, 1.0, 1.0, 1.0, 1.0, f.cc.cc, f.cc.width/ (2.0 * 1.48) - f.cc.width/ (2.0 * 1.48) * fade.charcutinleft / (fade.charcutinall + 1.0));
	}

	if (fade.charcutinleft) {
		fade.charcutinleft--;
	}

	glLoadIdentity();

	if (fade.blockerleft) {
		fade.blockerleft--;
		drawquad(0.0, 0.0, (16.0 / 9.0), 1.0,
			f1.blocker.r * (1.0 * fade.blockerleft / fade.blockerall) + f.blocker.r * (1.0 - 1.0 * fade.blockerleft / fade.blockerall),
			f1.blocker.g * (1.0 * fade.blockerleft / fade.blockerall) + f.blocker.g * (1.0 - 1.0 * fade.blockerleft / fade.blockerall),
			f1.blocker.b * (1.0 * fade.blockerleft / fade.blockerall) + f.blocker.b * (1.0 - 1.0 * fade.blockerleft / fade.blockerall),
			f1.blocker.a * (1.0 * fade.blockerleft / fade.blockerall) + f.blocker.a * (1.0 - 1.0 * fade.blockerleft / fade.blockerall), 0);
	}
	else {
		drawquad(0.0, 0.0, (16.0 / 9.0), 1.0, f.blocker.r, f.blocker.g, f.blocker.b, f.blocker.a, 0);
	}

#ifdef _SHOW

	if (f.dia) {
		/*glBindTexture(GL_TEXTURE_2D, 0);
		glBegin(GL_QUADS);
		glColor4f(0.0, 0.0, 0.0, 1.0);
		glVertex3f(-1.0,-1.0, -0.0f);
		glColor4f(0.0, 0.0, 0.0, 1.0);
		glVertex3f(1.0, -1.0, -0.0f);
		glColor4f(0.0, 0.0, 0.0, 0.75);
		glVertex3f(1.0, -0.85, -0.0f);
		glColor4f(0.0, 0.0, 0.0, 0.75);
		glVertex3f(-1.0, -0.85, 0.0f);
		glColor4f(0.0, 0.0, 0.0, 0.75);
		glVertex3f(-1.0, -0.85, -0.0f);
		glColor4f(0.0, 0.0, 0.0, 0.75);
		glVertex3f(1.0, -0.85, -0.0f);
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glVertex3f(1.0, -0.7, -0.0f);
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glVertex3f(-1.0, -0.7, 0.0f);
		glEnd();*/
		drawquad(0, 0, 16.0 / 9.0, 1.0, 1.0, 1.0, 1.0, 1.0, diabg);

		float dx = 36.0 * 2.0 / 1920;
		float dy = 36.0 * 2.0 / 1080;
		glBindTexture(GL_TEXTURE_2D, dia);
		if (f.diapos <= 32.0) {
			glBegin(GL_QUADS);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-0.41f, -0.858f - dy, -0.0f);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.diapos * (1.0f / 64.0f), 1.0f);
			glVertex3f(-0.41f + dx * f.diapos, -0.858f - dy, -0.0f);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.diapos * (1.0f / 64.0f), 0.0f);
			glVertex3f(-0.41f + dx * f.diapos, -0.858f + dy, -0.0f);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.41f, -0.858f + dy, -0.0f);
			glEnd();
		}
		else {
			glBegin(GL_QUADS);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-0.41f, -0.858f - dy, -0.0f);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.turnpos * (1.0f / 64.0f), 1.0f);
			glVertex3f(-0.41f + dx * f.turnpos, -0.858f - dy, -0.0f);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.turnpos * (1.0f / 64.0f), 0.0f);
			glVertex3f(-0.41f + dx * f.turnpos, -0.858f + dy, -0.0f);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.41f, -0.858f + dy, -0.0f);


			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.turnpos * (1.0f / 64.0f), 1.0f);
			glVertex3f(-0.41f, -0.946f - dy, -0.0f);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.diapos * (1.0f / 64.0f), 1.0f);
			glVertex3f(-0.41f + dx * (f.diapos - f.turnpos), -0.946f - dy, -0.0f);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.diapos * (1.0f / 64.0f), 0.0f);
			glVertex3f(-0.41f + dx * (f.diapos - f.turnpos), -0.946f + dy, -0.0f);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.turnpos * (1.0f / 64.0f), 0.0f);
			glVertex3f(-0.41f, -0.946f + dy, -0.0f);
			glEnd();
		}
	}
	if (f.cname) {
		float dx = 45.0 * 2.0 / 1920;
		float dy = 45.0 * 2.0 / 1080;
		glBindTexture(GL_TEXTURE_2D, cname);
		glBegin(GL_QUADS);
		glColor4f(0.57, 0.57, 0.57, 1.0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-0.483f-dx*f.cnamepos, -0.868f - dy, -0.0f);
		glColor4f(0.57, 0.57, 0.57, 1.0);
		glTexCoord2f(f.cnamepos * (1.0f / 64.0f), 1.0f);
		glVertex3f(-0.483f , -0.868f - dy, -0.0f);
		glColor4f(0.57, 0.57, 0.57, 1.0);
		glTexCoord2f(f.cnamepos * (1.0f / 64.0f), 0.0f);
		glVertex3f(-0.483f , -0.868f + dy, -0.0f);
		glColor4f(0.57, 0.57, 0.57, 1.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.483f - dx * f.cnamepos, -0.868f + dy, -0.0f);
		glEnd();
	}
	if (f.dec.dectex) {
		int count = f.dec.count;
		for (int i = 0; i < count; i++) {
			drawquad(0.0, ((count - 1) * 0.5 - i) * 0.278 + 0.222, 0.506, 0.089, 1.0, 1.0, 1.0, 1.0, 0);
			drawquad(0.0, ((count - 1) * 0.5 - i) * 0.278 + 0.222, 0.5, 0.083, 0.2, 0.2, 0.2, 1.0, 0);
			float dx = 36.0 * 2.0 / 1920;
			float dy = 36.0 * 2.0 / 1080;
			float x0 = -0.5f * dx * (f.dec.turn[i + 1] - f.dec.turn[i]);
			float y0 = ((count - 1) * 0.5 - i) * 0.278 + 0.192;
			float yc = ((count - 1) * 0.5 - i) * 0.278 + 0.222;
			float xc = 0.0;
			if (x0 < -0.2667) {
				dx *= -0.2667 / x0;
				dy *= -0.2667 / x0;
				y0 = yc + (y0 - yc) * (-0.2667 / x0);
				x0 = -0.2667;
			}

			glBindTexture(GL_TEXTURE_2D, f.dec.dectex);
			glBegin(GL_QUADS);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.dec.turn[i] * (1.0f / 64.0f), 1.0f);
			glVertex3f(x0, y0 - dy, -0.0f);

			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.dec.turn[i + 1] * (1.0f / 64.0f), 1.0f);
			glVertex3f(x0 + dx * (f.dec.turn[i + 1] - f.dec.turn[i]), y0 - dy, -0.0f);

			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.dec.turn[i + 1] * (1.0f / 64.0f), 0.0f);
			glVertex3f(x0 + dx * (f.dec.turn[i + 1] - f.dec.turn[i]), y0 + dy, -0.0f);

			glColor4f(1.0, 1.0, 1.0, 1.0);
			glTexCoord2f(f.dec.turn[i] * (1.0f / 64.0f), 0.0f);
			glVertex3f(x0, y0 + dy, -0.0f);
			glEnd();
		}
	}
	glFinish(); 

#ifdef _RECORD
	int i, j;
	glBindTexture(GL_TEXTURE_2D, texbuffer);
	glReadPixels(0, 0, 1920, 1080, GL_RGB, GL_UNSIGNED_BYTE, frim);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, frim);
	if (f.amount) {
		for (i = 0; i < 1080; i++) {
			for (j = 0; j < 1920; j++) {
				float g = frim[((1079 - i) * 1920 + j) * 3 + 2] * 0.299 + frim[((1079 - i) * 1920 + j) * 3 + 1] * 0.587 + frim[((1079 - i) * 1920 + j) * 3 + 0] * 0.114;
				if (g > 255) {
					g = 255;
				}
				frame.data[(i * 1920 + j) * 3 + 0] = g;
				frame.data[(i * 1920 + j) * 3 + 1] = g;
				frame.data[(i * 1920 + j) * 3 + 2] = g;
			}
		}
	}
	else {
		for (i = 0; i < 1080; i++) {
			for (j = 0; j < 1920; j++) {
				frame.data[(i * 1920 + j) * 3 + 0] = frim[((1079 - i) * 1920 + j) * 3 + 2];
				frame.data[(i * 1920 + j) * 3 + 1] = frim[((1079 - i) * 1920 + j) * 3 + 1];
				frame.data[(i * 1920 + j) * 3 + 2] = frim[((1079 - i) * 1920 + j) * 3 + 0];
			}
		}
	}
	writer << frame;
#endif

	glViewport(0, 0, cx, cy);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(0x00004100);

	glBindTexture(GL_TEXTURE_2D, texbuffer);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -0.0f);
	glEnd();
	
	SwapBuffers(hdc1);


#endif
	return 0;
}

/*int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{*/
int main(int argc,char **argv){
    //UNREFERENCED_PARAMETER(hPrevInstance);
    //UNREFERENCED_PARAMETER(lpCmdLine);
	HINSTANCE hInstance = ::GetModuleHandle(NULL);
    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ARKNIGHTS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	//std::cout << "yweiuryiwuyruw\n\n" << argc << argv[1];
	if (argc > 1) {
		filename = argv[1];
	}
	else {
		filename = "test";
	}
	srand(GetTickCount());
	loadmusicmap();
	loadcharnamemap();
	if (fopen_s(&filog, "D:/files/data/5/avg/log.txt", "ab")) {
		return 0;
	}


    // Perform application initialization:
    if (!InitInstance (hInstance, SW_SHOW))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ARKNIGHTS));

    MSG msg;
	int cur = 0;
    while (1)
    {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			cur = play(cur);
			if (cur == -1) {
				writer.release();
				genmusic();
				break;
			}
		}
    }
	fclose(filog);
    return (int) msg.wParam;
}

int getparaf(const char* text, int textlength, const char* tag, int taglength, float& x) {
	int next = find(text, textlength, tag, taglength);
	int pos = 0;
	while (next > 0) {
		pos += next;
		if (pos == 0 || !('a' <= text[pos - 1] && text[pos - 1] <= 'z' || 'A' <= text[pos - 1] && text[pos - 1] <= 'Z')) {
			sscanf_s(text + pos + taglength, "%f", &x);
			return 0;
		}
		pos += taglength;
		next = find(text + pos, textlength - pos, tag, taglength);
	}
	return -1;
}

int getparai(const char* text, int textlength, const char* tag, int taglength, int& x) {
	int pos = find(text, textlength, tag, taglength);
	if (pos > 0) {
		sscanf_s(text + pos + taglength, "%d", &x);
		return 0;
	}
	return -1;
}

int getparas(const char* text, int textlength, const char* tag, int taglength, std::string& s) {
	int pos = find(text, textlength, tag, taglength);
	if (pos > 0) {
		if (text[pos + taglength] == '"') {
			pos++;
		}
		int next;
		next = find(text + pos + taglength, textlength - pos - taglength, "\"", 1);
		if (next > 0) {
			s = std::string(text + pos + taglength, next);
			return 0;
		}
		next = find(text + pos + taglength, textlength - pos - taglength, ",", 1);
		if (next > 0) {
			s = std::string(text + pos + taglength, next);
			return 0;
		}
		return -1;
	}
	return -1;
}

int tr1610(char c) {
	if ('0' <= c && c <= '9') {
		return c - '0';
	}
	else if ('a' <= c && c <= 'z') {
		return c - 'a' + 10;
	}
	else if ('A' <= c && c <= 'Z') {
		return c - 'A' + 10;
	}
	return 0;
}

int gendia(const char* text, int textlength,float *charpos,int &charcount, GLuint tex) {
	wchar_t s[256]; 
	unsigned char* texdata,r=255,g=255,b=255;
	int i, j, x, y;
	int px = 0;
	texdata = (unsigned char*)malloc(256 * 8192 * 4);
	int length=MultiByteToWideChar(CP_UTF8, 0, text, textlength, s, 256);
	if (length > 128) {
		length = 128;
	}
	charcount = 0;
	for (int c = 0; c < length; c++) {
		if (s[c] == L"æ«"[0] && c > 1 && s[c - 1] == L"Ã×"[0]) {
			s[c] = L"Â¿"[0];
		}
		if (s[c] == L"ÍÃ"[0]) {
			s[c] = L"Â¿"[0];
		}
		if (s[c] == L"<"[0]) {
			int pos = find(s + c, length - c, L">", 1);
			if (pos > 0) {
				if (s[c + 1] == L"c"[0]) {
					int sharp = find(s + c, length - c, L"#", 1);
					if (sharp >= 0) {
						r = tr1610(s[c + sharp + 1]) * 16 + tr1610(s[c + sharp + 2]);
						g = tr1610(s[c + sharp + 3]) * 16 + tr1610(s[c + sharp + 4]);
						b = tr1610(s[c + sharp + 5]) * 16 + tr1610(s[c + sharp + 6]);
					}
				}
				else if (s[c + 1] == L"/"[0]) {
					r = 255;
					g = 255;
					b = 255;
				}
				c += pos;
				continue;
			}
		}
		if (s[c] == L"{"[0]) {
			int pos = find(s + c, length - c, L"}", 1);
			if (pos == 10) {
				memcpy(s + c, L"Faisal#6675", 22);
			}
		}
		FT_UInt charindex = FT_Get_Char_Index(face,s[c]);
		i = FT_Load_Glyph(face, charindex, FT_LOAD_DEFAULT);
		i = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		for (i = 0; i < 256; i++) {
			for (j = 0; j < 128; j++) {
				texdata[i * 8192 * 4 + (j + px) * 4] = r;
				texdata[i * 8192 * 4 + (j + px) * 4 + 1] = g;
				texdata[i * 8192 * 4 + (j + px) * 4 + 2] = b;
				texdata[i * 8192 * 4 + (j + px) * 4 + 3] = 0;
			}
		}

		for (i = 0; i < face->glyph->bitmap.rows; i++) {
			for (j = 0; j < face->glyph->bitmap.width; j++) {
				x = j + face->glyph->bitmap_left;
				y = i+127 - face->glyph->bitmap_top;
				if (0 <= x && x <= 127 && 0 <= y && y <= 255) {
					texdata[y * 8192 * 4 + (x + px) * 4 + 3] = face->glyph->bitmap.buffer[i * face->glyph->bitmap.pitch + j];
				}
			}
		}
		px += face->glyph->advance.x / 64;
		charpos[charcount] = px * (1.0/128.0);
		charcount++;
		if (px > 8192 - 128) {
			break;
		}
	}
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 128 && j < 8192 - px; j++) {
			texdata[i * 8192 * 4 + (j + px) * 4] = 255;
			texdata[i * 8192 * 4 + (j + px) * 4 + 1] = 255;
			texdata[i * 8192 * 4 + (j + px) * 4 + 2] = 255;
			texdata[i * 8192 * 4 + (j + px) * 4 + 3] = 0;
		}
	}
#ifdef _SHOW
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8192, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);
#endif 
	free(texdata);
	return 0;
}


int gendec(std::vector<std::string> decs, float* charpos, GLuint tex) {
	wchar_t s[256];
	unsigned char* texdata;
	int i, j, x, y;
	int px = 0;
	texdata = (unsigned char*)malloc(256 * 8192 * 4);
	charpos[0] = 0.0f;
	for (int sc = 0; sc < decs.size()&& sc < 7; sc++) {
		int length = MultiByteToWideChar(CP_UTF8, 0, decs[sc].c_str(), decs[sc].length(), s, 256);
		if (length > 128) {
			length = 128;
		}
		for (int c = 0; c < length; c++) {
			if (s[c] == L"æ«"[0] && c > 1 && s[c - 1] == L"Ã×"[0]) {
				s[c] = L"Â¿"[0];
			}
			if (s[c] == L"ÍÃ"[0]) {
				s[c] = L"Â¿"[0];
			}
			FT_UInt charindex = FT_Get_Char_Index(face, s[c]);
			i = FT_Load_Glyph(face, charindex, FT_LOAD_DEFAULT);
			i = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			for (i = 0; i < 256; i++) {
				for (j = 0; j < 128; j++) {
					texdata[i * 8192 * 4 + (j + px) * 4] = 255;
					texdata[i * 8192 * 4 + (j + px) * 4 + 1] = 255;
					texdata[i * 8192 * 4 + (j + px) * 4 + 2] = 255;
					texdata[i * 8192 * 4 + (j + px) * 4 + 3] = 0;
				}
			}

			for (i = 0; i < face->glyph->bitmap.rows; i++) {
				for (j = 0; j < face->glyph->bitmap.width; j++) {
					x = j + face->glyph->bitmap_left;
					y = i + 127 - face->glyph->bitmap_top;
					if (0 <= x && x <= 127 && 0 <= y && y <= 255) {
						texdata[y * 8192 * 4 + (x + px) * 4 + 3] = face->glyph->bitmap.buffer[i * face->glyph->bitmap.pitch + j];
					}
				}
			}
			px += face->glyph->advance.x / 64;
			if (px > 8192 - 128) {
				goto full;
			}
		}
		charpos[sc + 1] = px * (1.0 / 128.0);
	}
full:
	;
	for (i = 0; i < 7; i++) {
		if (charpos[i + 1] < charpos[i]) {
			charpos[i + 1] = charpos[i];
		}
	}
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 128 && j < 8192 - px; j++) {
			texdata[i * 8192 * 4 + (j + px) * 4] = 255;
			texdata[i * 8192 * 4 + (j + px) * 4 + 1] = 255;
			texdata[i * 8192 * 4 + (j + px) * 4 + 2] = 255;
			texdata[i * 8192 * 4 + (j + px) * 4 + 3] = 0;
		}
	}
#ifdef _SHOW
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8192, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);
#endif 
	free(texdata);
	return 0;
}


int loadtex(std::string& s0, GLuint tex) {
	cv::Mat rgb, a;
	int c1, c2;
	int pos;
	std::string s;
	std::string srgb, sa;
	int charn=0;
	std::map<std::string, std::vector<imgname_t>>::const_iterator it;
	pos = find(s0.c_str(), s0.length(),"#", 1);
	for (int i = 0; i < s0.length(); i++) {
		if ('A' <= s0[i] && s0[i] <= 'Z') {
			s0[i] = s0[i] - 'A' + 'a';
		}
	}
	if (pos > 1) {
		int i;
		for (i = 1; i < s0.length() - pos && '0' <= s0[pos + i] && s0[pos + i] <= '9'; i++) {
			charn = charn * 10 + s0[pos + i]-'0';
		}
		s = std::string(s0.c_str(), pos);
	}
	else {
		s = s0;
	}
	if (charn == 0) {
		charn = 1;
	}
	it = charnamemap.find(s);
	if (it != charnamemap.end()) {
		srgb = it->second[charn - 1].rgb;
		sa = it->second[charn - 1].a;
	}
	else {
		srgb = s0;
		sa = s0 + "[alpha]";
	}

	rgb = cv::imread("D:/files/data/5/avg/Texture2D/" + srgb + ".png",-1);
	a = cv::imread("D:/files/data/5/avg/Texture2D/" + sa + ".png",-1);

	if (rgb.data) {

#ifdef _SHOW
		uchar* texdata;
		texdata = (uchar*)malloc(rgb.rows * rgb.cols * 4);
		if (a.data) {
			if (rgb.step.buf[1] == 4) {
				for (int i = 0; i < rgb.rows * rgb.cols; i++) {
					texdata[i * 4] = rgb.data[i * 4 + 2];
					texdata[i * 4 + 1] = rgb.data[i * 4 + 1];
					texdata[i * 4 + 2] = rgb.data[i * 4 + 0];
					texdata[i * 4 + 3] = a.data[i * a.step.buf[1]];
				}
			}
			else {
				for (int i = 0; i < rgb.rows * rgb.cols; i++) {
					texdata[i * 4] = rgb.data[i * 3 + 2];
					texdata[i * 4 + 1] = rgb.data[i * 3 + 1];
					texdata[i * 4 + 2] = rgb.data[i * 3 + 0];
					texdata[i * 4 + 3] = a.data[i * a.step.buf[1]];
				}
			}
		}
		else {
			if (rgb.step.buf[1] == 4) {
				for (int i = 0; i < rgb.rows * rgb.cols; i++) {
					texdata[i * 4] = rgb.data[i * 4 + 2];
					texdata[i * 4 + 1] = rgb.data[i * 4 + 1];
					texdata[i * 4 + 2] = rgb.data[i * 4 + 0];
					texdata[i * 4 + 3] = rgb.data[i * 4 + 3];
				}
			}
			else {
				for (int i = 0; i < rgb.rows * rgb.cols; i++) {
					texdata[i * 4] = rgb.data[i * 3 + 2];
					texdata[i * 4 + 1] = rgb.data[i * 3 + 1];
					texdata[i * 4 + 2] = rgb.data[i * 3 + 0];
					texdata[i * 4 + 3] = 255;
				}
			}
		}
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgb.cols, rgb.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);
		free(texdata);
#endif
		return 0;
	}
	else {
		fprintf(filog, "cannot open:%s\n", s0.c_str());
	}
	return -1;
}

int cachetex(GLuint tex, texdata_t& texdata) {
	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &(texdata.w));
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &(texdata.h));
	texdata.data.resize(texdata.w * texdata.h);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata.data.data());
	return 0;
}

int loadtex(GLuint tex, texdata_t& texdata) {
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texdata.w, texdata.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata.data.data());
	return 0;
}

int cacheframetex(framedes_t frame, frametexdate_t &texdata) {
	if (frame.bg.tex) {
		cachetex(frame.bg.tex, texdata.bg);
	}
	if (frame.image.tex) {
		cachetex(frame.image.tex, texdata.image);
	}
	if (frame.c.c1) {
		cachetex(frame.c.c1, texdata.c1);
	}
	if (frame.c.c2) {
		cachetex(frame.c.c2, texdata.c2);
	}
	if (frame.cc.cc) {
		cachetex(frame.cc.cc, texdata.cc);
	}
	if (frame.cname) {
		cachetex(frame.cname, texdata.cname);
	}
	if (frame.dia) {
		cachetex(frame.dia, texdata.dia);
	}
	return 0;
}


int loadframetex(framedes_t &frame, framecache_t& framecache) {
	frame = framecache.frame;
	if (framecache.frame.bg.tex) {
		loadtex(bg, framecache.frametexdata.bg);
		frame.bg.tex = bg;
	}
	if (framecache.frame.image.tex) {
		loadtex(image, framecache.frametexdata.image);
		frame.image.tex = image;
	}
	if (framecache.frame.c.c1) {
		loadtex(c1, framecache.frametexdata.c1);
		frame.c.c1 = c1;
	}
	if (framecache.frame.c.c2) {
		loadtex(c2, framecache.frametexdata.c2);
		frame.c.c2 = c2;
	}
	if (framecache.frame.cc.cc) {
		loadtex(cc, framecache.frametexdata.cc);
		frame.cc.cc = cc;
	}
	if (framecache.frame.cname) {
		loadtex(cname, framecache.frametexdata.cname);
		frame.cname = cname;
	}
	if (framecache.frame.dia) {
		loadtex(dia, framecache.frametexdata.dia);
		frame.dia = dia;
	}
	return 0;
}



struct tn_t {
	int dec;
	int line;
	std::vector<int> chil, pare;
};

int gentree(std::vector<tn_t>& tree, std::vector<std::string>& line) {
	tn_t tn;
	tn.line = -1;
	tn.dec = 0;
	std::vector<int> dec;
	dec.push_back(tree.size());
	tree.push_back(tn);
	int i, j;
	std::set<int> curdec;
	curdec.insert(0);
	int hed;
	std::string cmd;
	int linec = line.size();
	for (i = 0; i < linec; i++) {
		cmd.clear();
		if (line[i][0] == '[') {
			hed = find(line[i].c_str(), line[i].length(), "]", 1);
			if (hed > 0) {
				for (j = 1; j < hed && (line[i][j] >= 'a' && line[i][j] <= 'z' || line[i][j] >= 'A' && line[i][j] <= 'Z'); j++) {
				}
				cmd = std::string(line[i].c_str() + 1, j - 1);
			}
		}
		if (cmd == "Decision") {
			tn.line = i;
			tn.dec = 0;
			tree.push_back(tn);
			for (j = 0; j < dec.size(); j++) {
				if (curdec.find(tree[dec[j]].dec) != curdec.end()) {
					tree[dec[j]].chil.push_back(tree.size() - 1);
					tree[tree.size() - 1].pare.push_back(dec[j]);
					dec.erase(dec.begin() + j);
					j--;
				}
			}

			std::string  valstr, optstr;
			std::vector<std::string> val, opt;
			getparas(line[i].c_str(), hed, "options=\"", strlen("options=\""), optstr);
			getparas(line[i].c_str(), hed, "values=\"", strlen("values=\""), valstr);
			getline(valstr.c_str(), valstr.length(), val, ";", 1);
			getline(optstr.c_str(), optstr.length(), opt, ";", 1);
			for (j = 0; j < val.size(); j++) {
				int x;
				sscanf_s(val[j].c_str(), "%d", &x);
				line.push_back("[name=\"Doctor\"]" + opt[j]);
				tn.line = line.size() - 1;
				tn.dec = x;
				tree.push_back(tn);

				tree[tree.size() - 2 - j].chil.push_back(tree.size() - 1);
				tree[tree.size() - 1].pare.push_back(tree.size() - 2 - j);
				dec.push_back(tree.size() - 1);
			}
		}
		else if (cmd == "Predicate") {
			curdec.clear();
			std::string refstr;
			std::vector<std::string> ref;
			getparas(line[i].c_str(), hed, "references=\"", strlen("references=\""), refstr);
			getline(refstr.c_str(), refstr.length(), ref, ";", 1);
			for (j = 0; j < ref.size(); j++) {
				int x;
				sscanf_s(ref[j].c_str(), "%d", &x);
				curdec.insert(x);
			}
		}
		else {
			for (j = 0; j < dec.size(); j++) {
				if (curdec.find(tree[dec[j]].dec) != curdec.end()) {
					tn.line = i;
					tn.dec = tree[dec[j]].dec;
					tree.push_back(tn);
					tree[dec[j]].chil.push_back(tree.size() - 1);
					tree[tree.size() - 1].pare.push_back(dec[j]);
					dec[j] = tree.size() - 1;
				}
			}
		}
	}

	tn.line = -1;
	tn.dec = 0;
	tree.push_back(tn);
	for (j = 0; j < dec.size(); j++) {
		tree[dec[j]].chil.push_back(tree.size() - 1);
		tree[tree.size() - 1].pare.push_back(dec[j]);
	}
	//return 0;
	for (;;) {
		int haschange = 0;
		for (i = 0; i < tree.size(); i++) {
			if (tree[i].pare.size() > 1) {
				for (j = 1; j < tree[i].pare.size(); j++) {
					int k;
					for (k = 0; k < j;k++) {
						if (tree[tree[i].pare[j]].line == tree[tree[i].pare[k]].line&& tree[i].pare[j]!= tree[i].pare[k]) {
							int l;
							for (l = 0; l < tree[tree[i].pare[k]].pare.size();l++) {
								for (int m = 0; m < tree[tree[tree[i].pare[k]].pare[l]].chil.size(); m++) {
									if (tree[tree[tree[i].pare[k]].pare[l]].chil[m] == tree[i].pare[k]) {
										int hchild = 0;
										for (int n = 0; n < tree[tree[tree[i].pare[k]].pare[l]].chil.size(); n++) {
											if (tree[tree[tree[i].pare[k]].pare[l]].chil[n] == tree[i].pare[j]) {
												hchild == 1;
											}
										}
										if (hchild) {
											tree[tree[tree[i].pare[k]].pare[l]].chil.erase(tree[tree[tree[i].pare[k]].pare[l]].chil.begin() + m);
											m--;
										}
										else {
											tree[tree[tree[i].pare[k]].pare[l]].chil[m] = tree[i].pare[j];
											tree[tree[i].pare[j]].pare.push_back(tree[tree[i].pare[k]].pare[l]);
										}
									}
								}
							}
							tree[i].pare.erase(tree[i].pare.begin() + k);
							goto next;
						}
					}
				}
			}
		}
		break;
	next:
		;
	}
	return 0;
}

std::vector<std::string> line;
std::vector<tn_t> tree;
std::vector<framecache_t> framecache;
framedes_t  fcur = { 0 }, flast = { 0 };
fadedes_t fade = { 0 };

int play(char* con, int length) {
	getline(con, length, line);
	gentree(tree, line);
	std::cout << tree.size();
	fcur.bg.sx = 1.0;
	fcur.bg.sy = 1.0;
	fcur.image.sx = 1.0;
	fcur.image.sy = 1.0;
	fcur.blocker.a = 0.0;
	fcur.blocker.r = 0.0;
	fcur.blocker.g = 0.0;
	fcur.blocker.b = 0.0;
	return 0;
}

int play(int cur) {
	int frame;
	int hed, pos, next, i, j, pc1, pc2;
	std::string cmd;
	i = tree[cur].line;
	if (i >= 0) {
		if (line[i][0] == '[') {
			hed = find(line[i].c_str(), line[i].length(), "]", 1);
			if (hed > 0) {
				for (j = 1; j < hed && (line[i][j] >= 'a' && line[i][j] <= 'z' || line[i][j] >= 'A' && line[i][j] <= 'Z'); j++) {
				}
				cmd = std::string(line[i].c_str() + 1, j - 1);
				if (cmd == "Dialog" || cmd == "dialog") {
					fcur.dia = 0;
					fcur.cname = 0;
				}
				else if (cmd == "Character" || cmd == "character") {
					float t = 0.0;
					flast.c = fcur.c;
					std::swap(c1, c1_);
					std::swap(c2, c2_);
					if ((pc1 = find(line[i].c_str(), hed, "name=\"", strlen("name=\""))) > 0) {
						if ((pc1 = find(line[i].c_str(), hed, "name2=\"", strlen("name2=\""))) > 0) {
							std::string s;
							fcur.c.c1 = c1;
							getparas(line[i].c_str(), hed, "name=\"", strlen("name=\""), s);
							loadtex(s, c1);
							fcur.c.c2 = c2;
							getparas(line[i].c_str(), hed, "name2=\"", strlen("name2=\""), s);
							loadtex(s, c2);
							fcur.c.focus = 0;
							getparai(line[i].c_str(), hed, "focus=", strlen("focus="), fcur.c.focus);
						}
						else {
							fcur.c.c2 = 0;
							fcur.c.c1 = c1;
							std::string s;
							getparas(line[i].c_str(), hed, "name=\"", strlen("name=\""), s);
							loadtex(s, c1);
							fcur.c.focus = 0;
							getparai(line[i].c_str(), hed, "focus=", strlen("focus="), fcur.c.focus);
						}
					}
					else {
						fcur.c.c1 = 0;
						fcur.c.c2 = 0;
					}
					getparaf(line[i].c_str(), hed, "fadetime=", strlen("fadetime="), t);
					fade.charall = t * 60 + 0.5;
					fade.charleft = fade.charall;
					if (find(line[i].c_str(), hed, "block=true", strlen("block=true")) > 0) {
						while (fade.charleft) {
							draw(fcur, flast, fade);
						}
					}
					//fprintf(filog, "cmd:%s\n", line[i].c_str());
				}
				else if (cmd == "Image" || cmd == "image") {
					float t = 0.0;
					flast.image = fcur.image;
					std::swap(image, image_);
					if ((pc1 = find(line[i].c_str(), hed, "image=\"", strlen("image=\""))) > 0) {
						fcur.image.tex = image;
						std::string s;
						getparas(line[i].c_str(), hed, "image=\"", strlen("image=\""), s);
						loadtex(s, image);
						fcur.image.x = 0.0;
						getparaf(line[i].c_str(), hed, "x=", strlen("x="), fcur.image.x);
						fcur.image.x *= (3.0 / 1080.0);
						fcur.image.y = 0.0;
						getparaf(line[i].c_str(), hed, "y=", strlen("y="), fcur.image.y);
						fcur.image.y *= (2.0 / 1080.0);
						fcur.image.sx = 1.0;
						getparaf(line[i].c_str(), hed, "xScale=", strlen("xScale="), fcur.image.sx);
						fcur.image.sy = 1.0;
						getparaf(line[i].c_str(), hed, "yScale=", strlen("yScale="), fcur.image.sy);
					}
					else {
						fcur.image.tex = 0;
						fcur.image.sx = 1.0;
						fcur.image.sy = 1.0;
						fcur.image.x = 0.0;
						fcur.image.y = 0.0;
					}
					getparaf(line[i].c_str(), hed, "fadetime=", strlen("fadetime="), t);
					fade.imageall = t * 60 + 0.5;
					fade.imageleft = fade.imageall;
					if (find(line[i].c_str(), hed, "block=true", strlen("block=true")) > 0) {
						while (fade.imageleft) {
							draw(fcur, flast, fade);
						}
					}
					//fprintf(filog, "cmd:%s\n", line[i].c_str());
				}
				else if (cmd == "Blocker" || cmd == "blocker") {
					float t = 0.5;
					flast.blocker = fcur.blocker;
					fcur.blocker.a = 1.0;
					fcur.blocker.r = 0.0;
					fcur.blocker.g = 0.0;
					fcur.blocker.b = 0.0;
					getparaf(line[i].c_str(), hed, "a=", strlen("a="), fcur.blocker.a);
					getparaf(line[i].c_str(), hed, "r=", strlen("r="), fcur.blocker.r);
					getparaf(line[i].c_str(), hed, "g=", strlen("g="), fcur.blocker.g);
					getparaf(line[i].c_str(), hed, "b=", strlen("b="), fcur.blocker.b);
					getparaf(line[i].c_str(), hed, "fadetime=", strlen("fadetime="), t);
					fade.blockerall = t * 60 + 0.5;
					fade.blockerleft = fade.blockerall;
					if (find(line[i].c_str(), hed, "block=true", strlen("block=true")) > 0) {
						while (fade.blockerleft) {
							draw(fcur, flast, fade);
						}
					}
				}
				else if (cmd == "Background") {
					float t = 0.0;
					flast.bg = fcur.bg;
					std::swap(bg, bg_);
					if ((pc1 = find(line[i].c_str(), hed, "image=\"", strlen("image=\""))) > 0) {
						fcur.bg.tex = bg;
						std::string s;
						getparas(line[i].c_str(), hed, "image=\"", strlen("image=\""), s);
						loadtex(s, bg);
						fcur.bg.x = 0.0;
						getparaf(line[i].c_str(), hed, "x=", strlen("x="), fcur.bg.x);
						fcur.bg.x *= (2.0 / 1080.0);
						fcur.bg.y = 0.0;
						getparaf(line[i].c_str(), hed, "y=", strlen("y="), fcur.bg.y);
						fcur.bg.y *= (2.0 / 1080.0);
						fcur.bg.sx = 1.0;
						getparaf(line[i].c_str(), hed, "xScale=", strlen("xScale="), fcur.bg.sx);
						fcur.bg.sy = 1.0;
						getparaf(line[i].c_str(), hed, "yScale=", strlen("yScale="), fcur.bg.sy);
					}
					else {
						fcur.bg.tex = 0;
						fcur.bg.sx = 1.0;
						fcur.bg.sy = 1.0;
						fcur.bg.x = 0.0;
						fcur.bg.y = 0.0;
					}
					getparaf(line[i].c_str(), hed, "fadetime=", strlen("fadetime="), t);
					fade.bgall = t * 60 + 0.5;
					fade.bgleft = fade.bgall;
					if (find(line[i].c_str(), hed, "block=true", strlen("block=true")) > 0) {
						while (fade.bgleft) {
							draw(fcur, flast, fade);
						}
					}
				}
				else if (cmd == "name") {
					std::string s;
					float diapos[128];
					int length;

					fcur.cname = cname;
					getparas(line[i].c_str(), hed, "name=\"", strlen("name=\""), s);
					gendia(s.c_str(), s.length(), diapos, length, cname);
					fcur.cnamepos = diapos[length - 1];

					fcur.dia = dia;
					int sp, esp;
					for (sp = 1; sp < line[i].length() - hed && line[i][hed + sp] == ' '; sp++);
					for (esp = line[i].length() - hed - sp; esp > 0 && line[i][hed + sp + esp - 1] == ' '; esp--);
					gendia(line[i].c_str() + hed + sp, esp, diapos, length, dia);
					for (frame = 1; frame < length; frame++) {
						if (diapos[frame] > 32.0f) {
							fcur.turnpos = diapos[frame - 1];
							break;
						}
					}
					for (frame = 0; frame < length; frame++) {
						fcur.diapos = diapos[frame];
						draw(fcur, flast, fade);
						draw(fcur, flast, fade);
						draw(fcur, flast, fade);
						//draw(fcur, flast, fade);
					}
					for (frame = 0; frame < 10 - length; frame++) {
						draw(fcur, flast, fade);
						//draw(fcur, flast, fade);
					}
					for (frame = 0; frame < 36; frame++) {
						draw(fcur, flast, fade);
					}
				}
				else if (cmd == "Delay" || cmd == "delay") {
					float t = 0;
					getparaf(line[i].c_str(), hed, "time=", strlen("time="), t);
					for (frame = 0; frame < t * 60.0f + 0.5f; frame++) {
						draw(fcur, flast, fade);
					}
				}
				else if (cmd == "HEADER") {
				}
				else if (cmd == "ImageTween") {
					float t = 0.0;
					float xto, yto, sxto, syto;
					if ((pc1 = find(line[i].c_str(), hed, "image=\"", strlen("image=\""))) > 0) {
						fcur.image.tex = image;
						std::string s;
						getparas(line[i].c_str(), hed, "image=\"", strlen("image=\""), s);
						loadtex(s, image);
					}
					getparaf(line[i].c_str(), hed, "duration=", strlen("duration="), t);
					if (!getparaf(line[i].c_str(), hed, "xFrom=", strlen("xFrom="), fcur.image.x)) {
						fcur.image.x *= (3.0 / 1080.0);
					}
					if (!getparaf(line[i].c_str(), hed, "yFrom=", strlen("yFrom="), fcur.image.y)) {
						fcur.image.y *= (3.0 / 1080.0);
					}
					getparaf(line[i].c_str(), hed, "xScaleFrom=", strlen("xScaleFrom="), fcur.image.sx);
					getparaf(line[i].c_str(), hed, "yScaleFrom=", strlen("yScaleFrom="), fcur.image.sy);
					xto = fcur.image.x;
					yto = fcur.image.y;
					sxto = fcur.image.sx;
					syto = fcur.image.sy;
					if (!getparaf(line[i].c_str(), hed, "xTo=", strlen("xTo="), xto)) {
						xto *= (3.0 / 1080.0);
					}
					if (!getparaf(line[i].c_str(), hed, "yTo=", strlen("yTo="), yto)) {
						yto *= (2.0 / 1080.0);
					}
					getparaf(line[i].c_str(), hed, "xScaleTo=", strlen("xScaleTo="), sxto);
					getparaf(line[i].c_str(), hed, "yScaleTo=", strlen("yScaleTo="), syto);
					fcur.image.tween.frame = t * 60 + 0.5;
					if (fcur.image.tween.frame) {
						fcur.image.tween.x = (xto - fcur.image.x) * (1.0 / fcur.image.tween.frame);
						fcur.image.tween.y = (yto - fcur.image.y) * (1.0 / fcur.image.tween.frame);
						fcur.image.tween.sx = (sxto - fcur.image.sx) * (1.0 / fcur.image.tween.frame);
						fcur.image.tween.sy = (syto - fcur.image.sy) * (1.0 / fcur.image.tween.frame);
					}
					if (find(line[i].c_str(), hed, "block=true", strlen("block=true")) > 0) {
						while (fcur.image.tween.frame) {
							draw(fcur, flast, fade);
						}
					}
				}
				else if (cmd == "BackgroundTween" || cmd == "backgroundTween") {
					float t = 0.0;
					float xto, yto, sxto, syto;
					if ((pc1 = find(line[i].c_str(), hed, "image=\"", strlen("image=\""))) > 0) {
						fcur.bg.tex = bg;
						std::string s;
						getparas(line[i].c_str(), hed, "image=\"", strlen("image=\""), s);
						loadtex(s, bg);
					}
					getparaf(line[i].c_str(), hed, "duration=", strlen("duration="), t);
					if (!getparaf(line[i].c_str(), hed, "xFrom=", strlen("xFrom="), fcur.bg.x)) {
						fcur.bg.x *= (3.0 / 1080.0);
					}
					if (!getparaf(line[i].c_str(), hed, "yFrom=", strlen("yFrom="), fcur.bg.y)) {
						fcur.bg.y *= (3.0 / 1080.0);
					}
					getparaf(line[i].c_str(), hed, "xScaleFrom=", strlen("xScaleFrom="), fcur.bg.sx);
					getparaf(line[i].c_str(), hed, "yScaleFrom=", strlen("yScaleFrom="), fcur.bg.sy);
					xto = fcur.bg.x;
					yto = fcur.bg.y;
					sxto = fcur.bg.sx;
					syto = fcur.bg.sy;
					if (!getparaf(line[i].c_str(), hed, "xTo=", strlen("xTo="), xto)) {
						xto *= (3.0 / 1080.0);
					}
					if (!getparaf(line[i].c_str(), hed, "yTo=", strlen("yTo="), yto)) {
						yto *= (2.0 / 1080.0);
					}
					getparaf(line[i].c_str(), hed, "xScaleTo=", strlen("xScaleTo="), sxto);
					getparaf(line[i].c_str(), hed, "yScaleTo=", strlen("yScaleTo="), syto);
					fcur.bg.tween.frame = t * 60 + 0.5;
					if (fcur.bg.tween.frame) {
						fcur.bg.tween.x = (xto - fcur.bg.x) * (1.0 / fcur.bg.tween.frame);
						fcur.bg.tween.y = (yto - fcur.bg.y) * (1.0 / fcur.bg.tween.frame);
						fcur.bg.tween.sx = (sxto - fcur.bg.sx) * (1.0 / fcur.bg.tween.frame);
						fcur.bg.tween.sy = (syto - fcur.bg.sy) * (1.0 / fcur.bg.tween.frame);
					}
					if (find(line[i].c_str(), hed, "block=true", strlen("block=true")) > 0) {
						while (fcur.bg.tween.frame) {
							draw(fcur, flast, fade);
						}
					}
				}
				else if (cmd == "PlaySound" || cmd == "playsound") {
					std::string intro, loop;
					std::string channel("sound");
					float delay = 0.0, fadetime = 0.0, volume = 1.0;
					getparas(line[i].c_str(), hed, "key=\"", strlen("key=\""), intro);
					if (find(line[i].c_str(), hed, "block=true", strlen("block=true")) > 0) {
						loop = intro;
					}
					getparas(line[i].c_str(), hed, "channel=", strlen("channel="), channel);
					getparaf(line[i].c_str(), hed, "volume=", strlen("volume="), volume);
					getparaf(line[i].c_str(), hed, "delay=", strlen("delay="), delay);
					getparaf(line[i].c_str(), hed, "Delay=", strlen("Delay="), delay);
					addevent(channel, intro, loop, delay, fadetime, volume);
					//fprintf(filog, "cmd:%s\n", line[i].c_str());
				}
				else if (cmd == "PlayMusic") {
					std::string intro, loop;
					std::string channel("music");
					float delay = 0.0, fadetime = 0.0, volume = 1.0;
					getparas(line[i].c_str(), hed, "intro=\"", strlen("intro=\""), intro);
					getparas(line[i].c_str(), hed, "key=\"", strlen("key=\""), loop);
					getparaf(line[i].c_str(), hed, "volume=", strlen("volume="), volume);
					getparaf(line[i].c_str(), hed, "delay=", strlen("delay="), delay);
					getparaf(line[i].c_str(), hed, "crossfade=", strlen("crossfade="), fadetime);
					addevent(channel, intro, loop, delay, fadetime, volume);
					//fprintf(filog, "cmd:%s\n", line[i].c_str());
				}
				else if (cmd == "StopMusic" || cmd == "stopmusic" || cmd == "Stopmusic") {
					std::string intro, loop;
					std::string channel("music");
					float delay = 0.0, fadetime = 0.0, volume = 1.0;
					getparaf(line[i].c_str(), hed, "fadetime=", strlen("fadetime="), fadetime);
					addevent(channel, intro, loop, delay, fadetime, volume);
					//fprintf(filog, "cmd:%s\n", line[i].c_str());
				}
				else if (cmd == "stopsound" || cmd == "stopSound") {
					std::string intro, loop;
					std::string channel("sound");
					float delay = 0.0, fadetime = 0.0, volume = 1.0;
					getparas(line[i].c_str(), hed, "channel=", strlen("channel="), channel);
					getparaf(line[i].c_str(), hed, "fadetime=", strlen("fadetime="), fadetime);
					addevent(channel, intro, loop, delay, fadetime, volume);
					//fprintf(filog, "cmd:%s\n", line[i].c_str());
				}
				else if (cmd == "CameraShake") {
					float t = 0.0;
					fade.xf = 0.0;
					fade.yf = 0.0;
					fade.xt = 0.0;
					fade.yt = 0.0;
					fade.moveleft = 0.0;
					fade.moveall = 2.0;
					fade.fadeout = 0;
					fade.sx = 0.0;
					getparaf(line[i].c_str(), hed, "xstrength=", strlen("xstrength="), fade.sx);
					fade.sx *= (3.0 / 1080.0);
					fade.sy = 0.0;
					getparaf(line[i].c_str(), hed, "ystrength=", strlen("ystrength="), fade.sy);
					fade.sy *= (2.0 / 1080.0);
					if (find(line[i].c_str(), hed, "fadeout=true", strlen("fadeout=true")) > 0) {
						fade.fadeout = 1;
					}
					getparaf(line[i].c_str(), hed, "duration=", strlen("duration="), t);
					fade.camerashakeall = t * 60 + 0.5;
					fade.camerashakeleft = fade.camerashakeall;
					if (find(line[i].c_str(), hed, "block=true", strlen("block=true")) > 0) {
						while (fade.imageleft) {
							draw(fcur, flast, fade);
						}
					}
				}
				else if (cmd == "Predicate") {
				}
				else if (cmd == "Decision") {
					std::string  optstr;
					std::vector<std::string>  opt;
					getparas(line[i].c_str(), hed, "options=\"", strlen("options=\""), optstr);
					getline(optstr.c_str(), optstr.length(), opt, ";", 1);
					memset(fcur.dec.turn, 0, sizeof(fcur.dec.turn));
					gendec(opt, fcur.dec.turn, dec);
					fcur.dec.count = opt.size();
					fcur.dec.dectex = dec;
					if (fcur.dec.count > 7) {
						fcur.dec.count = 7;
					}
					int sum = 0;
					for (int oc = 0; oc < fcur.dec.count; oc++) {
						sum += opt[oc].length() * 2;
						if (opt[oc].length() < 5) {
							sum += 5 - opt[oc].length();
						}
						sum += 12;
					}
					sum += 18;
					for (frame = 0; frame < sum; frame++) {
						draw(fcur, flast, fade);
					}
					fcur.dec.dectex = 0;
				}
				else if (cmd == "cameraEffect" || cmd == "CameraEffect") {
					flast.cc = fcur.cc;
					fcur.amount = 0;
					getparaf(line[i].c_str(), hed, "amount=", strlen("amount="), fcur.amount);
				}
				else if (cmd == "CharacterCutin") {
					float t = 0.0;
					std::swap(cc, cc_);
					flast.cc = fcur.cc;
					if ((pc1 = find(line[i].c_str(), hed, "name=\"", strlen("name=\""))) > 0) {
						std::string s;
						getparas(line[i].c_str(), hed, "name=\"", strlen("name=\""), s);
						loadtex(s, cc);
						fcur.cc.cc = cc;
						fcur.cc.width = 200.0;
						getparaf(line[i].c_str(), hed, "width=", strlen("width="), fcur.cc.width);
						fcur.cc.width *= (3.0 / 1080.0);
						fcur.cc.x = 0.0;
						getparaf(line[i].c_str(), hed, "offsetx=", strlen("offsetx="), fcur.cc.x);
						fcur.cc.x *= (3.0 / 1080.0);
					}
					else {
						fcur.cc.cc = 0;
					}
					getparaf(line[i].c_str(), hed, "fadetime=", strlen("fadetime="), t);
					fade.charcutinall = t * 60 + 0.5;
					fade.charcutinleft = fade.charcutinall;
					if (find(line[i].c_str(), hed, "block=true", strlen("block=true")) > 0) {
						while (fade.charcutinleft) {
							draw(fcur, flast, fade);
						}
					}
				}
				else {
					fprintf(filog, "cmd:%s\n", cmd.c_str());
				}
			}
		}
		else {
			if (line[i] == "// Comments") {
				goto it;
			}
			if (line[i][1] == '/') {
				goto it;
			}
			float diapos[128];
			int length;
			fcur.dia = dia;
			fcur.cname = 0;
			gendia(line[i].c_str(), line[i].length(), diapos, length, dia);
			for (frame = 1; frame < length; frame++) {
				if (diapos[frame] > 32.0f) {
					fcur.turnpos = diapos[frame - 1];
					break;
				}
			}
			for (frame = 0; frame < length; frame++) {
				fcur.diapos = diapos[frame];
				draw(fcur, flast, fade);
				draw(fcur, flast, fade);
				draw(fcur, flast, fade);
				//draw(fcur, flast, fade);
			}
			for (frame = 0; frame < 10 - length; frame++) {
				draw(fcur, flast, fade);
				//draw(fcur, flast, fade);
			}
			for (frame = 0; frame < 36; frame++) {
				draw(fcur, flast, fade);
			}
		}
	}
it:
	int tempcur;
	if (tree[cur].chil.size() == 0) {
		return -1;
	}
	else {
		tempcur = tree[cur].chil[0];
		if (tree[cur].chil.size() > 1) {
			framecache_t framec;
			cacheframetex(fcur, framec.frametexdata);
			framec.frame = fcur;
			framecache.push_back(framec);
		}
	}
	if (tree[tempcur].pare.size() > 1) {
		for (j = 0; j < tree[tempcur].pare.size(); j++) {
			if (tree[tempcur].pare[j] == cur) {
				tree[tempcur].pare.erase(tree[tempcur].pare.begin() + j);
			}
		}
		while (tree[cur].chil.size() == 1) {
			cur = tree[cur].pare[0];
		}
		tree[cur].chil.erase(tree[cur].chil.begin());
		fade = { 0 };
		loadframetex(fcur, framecache[framecache.size() - 1]);
		framecache.pop_back();
		for (int frame = 0; frame < 60; frame++) {
			draw(fcur, flast, fade);
		}
	}
	else {
		cur = tempcur;
	}
	return cur;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ARKNIGHTS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ARKNIGHTS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			//draw({ 0 });
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_CREATE: {
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO,
			PFD_TYPE_RGBA,
			24,
			0,0,0,0,0,0,0,0,
			0,
			0,0,0,0,
			32,
			0,0,
			PFD_MAIN_PLANE,
			0,0,0,0
		};
		hdc1 = GetDC(hWnd);
		hdc2 = GetDC(NULL);
		int uds = ::ChoosePixelFormat(hdc1, &pfd);
		::SetPixelFormat(hdc1, uds, &pfd);
		m_hrc = ::wglCreateContext(hdc1);
		::wglMakeCurrent(hdc1, m_hrc);
		glewInit();
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		((bool(_stdcall*)(int))wglGetProcAddress("wglSwapIntervalEXT"))(0);

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_RENDERBUFFER);
		glEnable(GL_FRAMEBUFFER);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glGenFramebuffersEXT(1, &framebuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

		glGenRenderbuffersEXT(1, &depthbuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 1920, 1080);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);

		glGenTextures(1, &texbuffer);
		glBindTexture(GL_TEXTURE_2D, texbuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texbuffer, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); 
		
		glGenFramebuffersEXT(1, &framebufferim);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferim);

		glGenRenderbuffersEXT(1, &depthbufferim);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbufferim);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 1920, 1080);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbufferim);

		glGenTextures(1, &texbufferim);
		glBindTexture(GL_TEXTURE_2D, texbufferim);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texbufferim, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		//GLuint bg, image, c1, c2, dia, cname;
		glGenTextures(1, &bg);
		glBindTexture(GL_TEXTURE_2D, bg);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &image);
		glBindTexture(GL_TEXTURE_2D, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &bg_);
		glBindTexture(GL_TEXTURE_2D, bg_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &image_);
		glBindTexture(GL_TEXTURE_2D, image_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &c1);
		glBindTexture(GL_TEXTURE_2D, c1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &c2);
		glBindTexture(GL_TEXTURE_2D, c2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &cc);
		glBindTexture(GL_TEXTURE_2D, cc);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &c1_);
		glBindTexture(GL_TEXTURE_2D, c1_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &c2_);
		glBindTexture(GL_TEXTURE_2D, c2_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &cc_);
		glBindTexture(GL_TEXTURE_2D, cc_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &dia);
		glBindTexture(GL_TEXTURE_2D, dia);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &dec);
		glBindTexture(GL_TEXTURE_2D, dec);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &cname);
		glBindTexture(GL_TEXTURE_2D, cname);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenTextures(1, &diabg);
		glBindTexture(GL_TEXTURE_2D, diabg);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		std::string  s("3");
		loadtex(s, diabg);
		//std::string st("char_002_amiya_2");
		//loadtex(st, texbuffer);

		writer.open("C:/files/avg/" + filename + ".mp4", cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 60.0, cv::Size(1920, 1080));// cv::VideoWriter::fourcc('X', '2', '6', '4')

		int i, j;
		i = FT_Init_FreeType(&ftlib);
		i = FT_New_Face(ftlib, "D:/files/data/5/avg/Font/NotoSansHans-Medium.otf", 0, &face);
		i = FT_Set_Pixel_Sizes(face, 128, 128);
		i = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		break;
	}
	case WM_SIZE: {
		cx = lParam & 0xffff;
		cy = (lParam & 0xffff0000) >> 16;
		//draw({ 0 });
		//break;
	}
	case WM_KEYDOWN: {
		//switch (wParam) {
		//case(32): {
		FILE* fi;
		char* ficon;
		int ficount;
		/*for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 20; j++) {
				char s[256];
				sprintf_s(s, "D:/files/data/5/avg/TextAsset/level_act3d0_%02d_beg.txt",  j);
				if (!fopen_s(&fi, s, "rb")) {
					fseek(fi, 0, SEEK_END);
					ficount = ftell(fi);
					fseek(fi, 0, SEEK_SET);
					ficon = (char*)malloc(ficount * sizeof(char));
					ficount = fread(ficon, 1, ficount, fi);
					fclose(fi);
					fprintf(filog, "play:%s\n", s);
					play(ficon, ficount);
					free(ficon);
					//genmusic();
					//PostQuitMessage(0);
					//return 0;
				}
				sprintf_s(s, "D:/files/data/5/avg/TextAsset/level_act3d0_%02d_end.txt", j);
				if (!fopen_s(&fi, s, "rb")) {
					fseek(fi, 0, SEEK_END);
					ficount = ftell(fi);
					fseek(fi, 0, SEEK_SET);
					ficon = (char*)malloc(ficount * sizeof(char));
					ficount = fread(ficon, 1, ficount, fi);
					fprintf(filog, "play:%s\n", s);
					play(ficon, ficount);
					fclose(fi);
					free(ficon);
					//PostQuitMessage(0);
				}
				//PostQuitMessage(0);
				//return 0;
			}
		}*/
		if (!fopen_s(&fi, ("D:/files/data/5/avg/TextAsset/" + filename + ".txt").c_str(), "rb")) {
			fseek(fi, 0, SEEK_END);
			ficount = ftell(fi);
			fseek(fi, 0, SEEK_SET);
			ficon = (char*)malloc(ficount * sizeof(char));
			ficount = fread(ficon, 1, ficount, fi);
			//fprintf(filog, "play:%s\n", s);
			play(ficon, ficount);
			fclose(fi);
			free(ficon);
			//PostQuitMessage(0);
		}
		//writer.release(); 
		//genmusic();
		//PostQuitMessage(0);
		//break;
	//}
	//}
		break;
	}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
extern "C" {
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}