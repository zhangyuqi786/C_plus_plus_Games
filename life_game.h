#pragma once

#ifndef LIFE_GAME_H
#define LIFE_GAME_H

#include <windows.h>
#include <mutex>
#include "plot.h"

const int CELL_SIZE = 4;     // ÿ����������
const int GRID_W = 200;     // ���������
const int GRID_H = 200;     // ���������
const int WIN_W = GRID_W * CELL_SIZE + 200;  // ���ڿ�ȣ��Ҳ�����ť����
const int WIN_H = GRID_H * CELL_SIZE;

/* �������飺����ʾ��һȦ 0 ����߽� */
bool world[GRID_H + 2][GRID_W + 2] = { 0 };
bool next[GRID_H + 2][GRID_W + 2] = { 0 };
int survivingGenerations[GRID_H + 2][GRID_W + 2] = { 0 };

/* ��ť�������꣨�Ҳࣩ */
const int BTN_X = GRID_W * CELL_SIZE + 20;
const int BTN_Y = 20;
const int BTN_W = 160;
const int BTN_H = 40;
const int BTN_GAP = 50;

/* ��ťö�� */
enum BTN { BTN_STEP, BTN_PLAY, BTN_CLEAR, BTN_RAND };

/* ��ť�ı� */
const wchar_t* btn_text[4] = { L"����", L"��ʼ", L"���", L"���" };
const wchar_t* info_text = L"evolve:";
/* ȫ��״̬ */
bool running = false;   // �Զ����б�־
int evolveNum = 0;

/* ���ߣ�����Ļ����ת�ɸ������� */
inline int sx2x(int sx) { return sx / CELL_SIZE; }
inline int sy2y(int sy) { return sy / CELL_SIZE; }

/* ������ */
void drawWorld()
{
    for (int y = 1; y <= GRID_H; ++y) {
        for (int x = 1; x <= GRID_W; ++x)
        {
            int sx = (x - 1) * CELL_SIZE;
            int sy = (y - 1) * CELL_SIZE;
            setfillcolor(world[y][x] ? GREEN : BLACK);
            solidrectangle(sx, sy, sx + CELL_SIZE, sy + CELL_SIZE);
        }
    }
    settextcolor(BLACK);
    setfont(16, 0, L"����");
    outtextxy(BTN_X, GRID_H*CELL_SIZE - textheight(info_text), info_text);
    std::ostringstream oss_evo;
    oss_evo << std::fixed << evolveNum;
    std::wstring w_evo = s_ws(oss_evo.str());
    //
    setfillcolor(getbkcolor());
    solidrectangle(BTN_X + textwidth(info_text), GRID_H * CELL_SIZE - textheight(info_text), WIN_W, WIN_H);
    setbkmode(TRANSPARENT);
    outtextxy(BTN_X+ textwidth(info_text), GRID_H * CELL_SIZE - textheight(info_text), w_evo.c_str());
}

/* ����ť */
void drawButtons()
{
    for (int i = 0; i < 4; ++i)
    {
        int y = BTN_Y + i * BTN_GAP;
        setfillcolor(LIGHTGRAY);
        solidrectangle(BTN_X, y, BTN_X + BTN_W, y + BTN_H);
        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        setfont(16, 0, L"����");
        outtextxy(BTN_X + BTN_W / 2 - textwidth(btn_text[i]) / 2,
            y + BTN_H / 2 - textheight(btn_text[i]) / 2,
            btn_text[i]);
    }
}

/* �ж�����Ƿ���ڰ�ť�� */
bool inBtn(int mx, int my, int idx)
{
    int y = BTN_Y + idx * BTN_GAP;
    return mx >= BTN_X && mx <= BTN_X + BTN_W &&
        my >= y && my <= y + BTN_H;
}

/* ������Χ��ϸ�� */
int countLive(int x, int y)
{
    int cnt = 0;
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx)
            if (!(dx == 0 && dy == 0)) {
                if (y + dy == GRID_H + 1) {
                    if (x + dx == GRID_W + 1) {
                        cnt += world[1][1];
                    }
                    else if (x + dx == 0) {
                        cnt += world[1][GRID_W];
                    }
                    else {
                        cnt += world[1][x + dx];
                    }
                }
                else if (y + dy == 0) {
                    if (x + dx == GRID_W + 1) {
                        cnt += world[GRID_H][1];
                    }
                    else if (x + dx == 0) {
                        cnt += world[GRID_H][GRID_W];
                    }
                    else {
                        cnt += world[GRID_H][x + dx];
                    }
                }
                else {
                    if (x + dx == GRID_W + 1) {
                        cnt += world[y + dy][1];
                    }
                    else if (x + dx == 0) {
                        cnt += world[y + dy][GRID_W];
                    }
                    else {
                        cnt += world[y + dy][x + dx];
                    }
                }
            }
                
    return cnt;
}

/* ����һ�� */
void evolve_(int ystart, int row_to_process)
{
    for (int y = ystart; y < ystart + row_to_process; ++y) {
        for (int x = 1; x <= GRID_W; ++x)
        {
            int n = countLive(x, y);
            if (survivingGenerations[y][x] >= 11 || n == 0 || n == 1 || n > 6) {
                next[y][x] = 0;
                survivingGenerations[y][x] = 0;
                continue;
            }

            next[y][x] = (n == 3) || (world[y][x] && n == 2);
            if (next[y][x]) {
                if (survivingGenerations[y][x] >= 9) {
                    next[y][x] = (rand() % 100 == 0);
                    if (next[y][x]) survivingGenerations[y][x] += 1;
                    else survivingGenerations[y][x] = 0;
                }
                else if (survivingGenerations[y][x] >= 7) {
                    next[y][x] = (rand() % 5 == 0);
                    if (next[y][x]) survivingGenerations[y][x] += 1;
                    else survivingGenerations[y][x] = 0;
                }
                else {
                    survivingGenerations[y][x] += 1;
                }
            }
            else {
                if (survivingGenerations[y][x] <= 3) {
                    next[y][x] = (rand() % 60 == 0);
                    if (next[y][x]) survivingGenerations[y][x] += 1;
                    else survivingGenerations[y][x] = 0;
                }
                else {
                    survivingGenerations[y][x] = 0;
                }
            }
        }
    }
}
void evolve() {
    std::mutex mtx; // �����̰߳�ȫ���
    // ��ȡ CPU �߼�������������
    const int numThreads = std::thread::hardware_concurrency();
    // ����ÿ���̴߳��������
    int rowsPerThread = GRID_H / numThreads;
    int remainingRows = GRID_H % numThreads;
    // �����߳�����
    std::vector<std::thread> threads;

    // ����������߳�
    int startRow = 0;
    for (int i = 0; i < numThreads; ++i) {
        int rowsToProcess = (i < remainingRows) ? rowsPerThread + 1 : rowsPerThread;
        threads.emplace_back(evolve_, startRow, rowsToProcess);
        startRow += rowsToProcess;
    }
    // �ȴ������߳����
    for (auto& thread : threads) {
        thread.join();
    }
    memcpy(world, next, sizeof(world));
    evolveNum++;
}
/* �����ʼ�� */
void randomWorld()
{
    for (int y = 1; y <= GRID_H; ++y) {
        for (int x = 1; x <= GRID_W; ++x) {
            world[y][x] = (rand() % 6 == 0); 
            survivingGenerations[y][x] = world[y][x] ? 1 : 0;
        }
    }
}

/* ������� */
void clearWorld()
{
    memset(world, 0, sizeof(world));
    memset(survivingGenerations, 0, sizeof(survivingGenerations));
}

/* ��ʼ������ */
void init()
{
    initgraph(WIN_W, WIN_H);
    FlushMouseMsgBuffer();
    srand((unsigned)time(nullptr));
    setbkcolor(WHITE);
    cleardevice();
    drawButtons();
}

void LifeGame()
{
    init();
    randomWorld();   // Ĭ���������
    drawWorld();

    MOUSEMSG msg;
    bool isPressing = false;
    while (true)
    {
        /* ��������¼� */
        if (MouseHit())
        {
            msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN)
            {
                if (msg.x < GRID_W * CELL_SIZE && msg.x > 0
                    && msg.y < GRID_H * CELL_SIZE && msg.y > 0) {
                    isPressing = true;
                }
                if (inBtn(msg.x, msg.y, BTN_STEP))
                {
                    evolve();
                    drawWorld();
                }
                if (inBtn(msg.x, msg.y, BTN_PLAY))
                {
                    running = !running;
                }
                if (inBtn(msg.x, msg.y, BTN_CLEAR))
                {
                    clearWorld();
                    evolveNum = 0;
                    drawWorld();
                        
                }
                if (inBtn(msg.x, msg.y, BTN_RAND))
                {
                    randomWorld();
                    evolveNum = 0;
                    drawWorld();

                }
            }
            else if (msg.uMsg == WM_LBUTTONUP) {
                isPressing = false;
            }
        }
        if (isPressing) {
            int x = sx2x(msg.x) + 1;
            int y = sy2y(msg.y) + 1;
            if (x >= 1 && x <= GRID_W && y >= 1 && y <= GRID_H) {
                world[y][x] = true;
                survivingGenerations[y][x] = 1;
                drawWorld();
            }
        }
        /*else {
            flushmessage();
        }*/
    
        /* �Զ����� */
        if (running)
        {
            evolve();
            drawWorld();
            //Sleep(100);   // �Զ��ٶ�
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;//ESC
    }

    closegraph();
    return;
}

#endif//LIFE_GAME_H