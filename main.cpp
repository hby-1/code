#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include"tools.hpp"
#include<easyx.h> //不是标准库
#include<time.h>
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")

#define ENEMY_NUM 10
#define BULLET_NUM 10

IMAGE img_bk;
IMAGE img_gamer;
IMAGE bullet;
IMAGE enemies[2];
IMAGE overimg;

//自定义定时器
bool Timer(int ms, int id)
{
	static int start[5];
	int end = clock();
	if (end - start[id] >= ms)
	{
		start[id] = end;
		return true;
	}
	return false;
}

struct Plane {
	int x;
	int y;
	int speed;
	bool isdie;
	IMAGE p;
};

struct Bullet {
	int x;
	int y;
	int speed;
	bool isdie;
}Bullet[BULLET_NUM] = {0};

Plane gamer;
Plane enemy[ENEMY_NUM];

void loadResource() {
	loadimage(&img_bk, L"Resource/images/background.png");//加载
	loadimage(&img_gamer, L"Resource/images/gamer1.png");//加载
	loadimage(&bullet, L"Resource/images/basketball1.png");//加载
	loadimage(enemies+0, L"Resource/images/enemy1.png");//加载
	loadimage(enemies+1, L"Resource/images/enemy2.png");//加载
	loadimage(&overimg, L"Resource/images/quit.png");
}

void init_plane(Plane* pthis,int x,int y,IMAGE p) {
	pthis->x = x;
	pthis->y = y;
	pthis->isdie = false;
	pthis->p = p;
	pthis->speed = 15;
}

void plane_draw(Plane* pthis) {
	drawImg(pthis->x, pthis->y, &(pthis->p));
}

void init() {
	loadResource();
	init_plane(&gamer, (getwidth() - img_gamer.getwidth()) / 2, getheight() - img_gamer.getheight(),img_gamer);
	//子弹
	for (int i = 0; i < BULLET_NUM; i++) {
		Bullet[i].isdie = true;
	}
	//敌机
	for (int i = 0; i < ENEMY_NUM; i++) {
		enemy[i].p = enemies[i % 2];
		enemy[i].isdie = true;

	}
}

void draw() {
	//背景
	putimage(0, 0, &img_bk);
	//gamer
	plane_draw(&gamer);
	for (int i = 0; i < BULLET_NUM; i++) {
		if (!Bullet[i].isdie) {
			drawImg(Bullet[i].x, Bullet[i].y, &bullet);
		}
		if (Bullet[i].y < 0) {
			Bullet[i].isdie = true;
		}
	}

	for (int i = 0; i < ENEMY_NUM; i++) {
		if (!enemy[i].isdie) {
			drawImg(enemy[i].x,enemy[i].y,&enemy[i].p);
		}
	}

}

void creatBullet() {
	for (int i = 0; i < BULLET_NUM; i++) {
		if (Bullet[i].isdie) {
			Bullet[i].x = gamer.x + gamer.p.getwidth() / 2;
			Bullet[i].y = gamer.y;
			Bullet[i].isdie = false;
			Bullet[i].speed = 15;
			break;
		}
	}
}

void createnemy() {
	for (int i = 0; i < ENEMY_NUM; i++) {
		if (enemy[i].isdie) {
			enemy[i].isdie = false;
			enemy[i].x = rand() % (getwidth()-60);
			enemy[i].y =-enemy[i].p.getheight();
			enemy[i].speed = 5;
			break;
		}
	}
}

void moveBullet() {
	for (int i = 0; i < BULLET_NUM; i++) {
		if (!Bullet[i].isdie) {
			Bullet[i].y -= Bullet[i].speed;
			for (int n = 0; n < ENEMY_NUM; n++) {
				if (Bullet[i].x >= (enemy[n].x - bullet.getwidth()) && Bullet[i].x <= (enemy[n].x + enemy[n].p.getwidth() + bullet.getwidth())&&(Bullet[i].y-enemy[n].y)<enemy[n].p.getheight()) {
					enemy[n].isdie = true;
					Bullet[i].isdie = true;
				}
			}
		}
	}
}

void moveenemy() {
	for (int i = 0; i < ENEMY_NUM; i++) {
		if (!enemy[i].isdie) {
			if (enemy[i].y > getheight()) {
				enemy[i].isdie = true;
				gamer.isdie = true;
			}
			else{
				enemy[i].y += enemy[i].speed;
			}
		}
	}
}

void control_plane(Plane* pthis) {
	if (GetAsyncKeyState(VK_UP)) {
		pthis->y -= pthis->speed;
	}

	if (GetAsyncKeyState(VK_DOWN)&&(pthis->y<(img_bk.getheight() - pthis->p.getheight())) )
	{
		pthis->y += pthis->speed;
	}

	if (GetAsyncKeyState(VK_LEFT)&&(pthis->x > -(pthis->p.getwidth()/2))) {
		pthis->x -= pthis->speed;
	}
	
	if (GetAsyncKeyState(VK_RIGHT)&&(pthis->x < (img_bk.getwidth()-pthis->p.getwidth()/2))) {
		pthis->x += pthis->speed;
	}

	if (GetAsyncKeyState(VK_SPACE)&&Timer(200,0)) {
		creatBullet();	
		mciSendString(L"open Resource/sound/ji.mp3", 0, 0, 0);
		mciSendString(L"play ji.mp3", 0, 0, 0);
	}
	
}
//碰撞检测
void crashcheck() {
	//gamer和enemy
	for (int i = 0; i < ENEMY_NUM; i++) {
		if (!enemy[i].isdie&& enemy[i].y > gamer.y && enemy[i].x > gamer.x) {
			if (enemy[i].y -gamer.y < gamer.p.getheight() && enemy[i].x - gamer.x<gamer.p.getwidth()) {
				gamer.isdie = true;
			}
		}
		else if (!enemy[i].isdie && enemy[i].y < gamer.y && enemy[i].x > gamer.x) {
			if (gamer.y- enemy[i].y < enemy[i].p.getheight() && enemy[i].x - gamer.x < gamer.p.getwidth()) {
				gamer.isdie = true;
			}
		}
		else if (!enemy[i].isdie && enemy[i].y > gamer.y && enemy[i].x < gamer.x) {
			if (enemy[i].y- gamer.y < gamer.p.getheight() &&  gamer.x- enemy[i].x < enemy[i].p.getwidth()) {
				gamer.isdie = true;
			}
		}
		else if (!enemy[i].isdie && enemy[i].y < gamer.y && enemy[i].x < gamer.x) {
			if ( gamer.y- enemy[i].y < enemy[i].p.getheight() && gamer.x - enemy[i].x < enemy[i].p.getwidth()) {
				gamer.isdie = true;
			}
		}
		
	}
}

int main() 
{
	// 创建窗口喽
	initgraph(480, 852);
	init();
	while (true) {
		int startTime = clock();
		crashcheck();
		draw();
		control_plane(&gamer);
		moveBullet();
		if (Timer(500, 0)) {
			createnemy();
		}
		moveenemy();
		if (gamer.isdie) {
			drawImg((getwidth() - overimg.getwidth()) / 2, (getheight() - overimg.getheight()) / 2, &overimg);
			break;
		}
		int frameTime = clock() - startTime;//获取一次循环执行的毫秒数fps 60->1s
		if(1000/60-frameTime>0){
			Sleep(1000 / 60 - frameTime);
		}
	}
	getchar();
	return 0;
}