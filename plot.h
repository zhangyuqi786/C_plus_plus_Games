#pragma once

#ifndef PLOT_H
#define PLOT_H
#include <graphics.h>
#include <vector>
#include <string>
#include <sstream>


// 将 const char* 转换为 const wchar_t*
std::wstring s_ws(const std::string& str) {
    int len;
    int slength = static_cast<int>(str.length()) + 1;
    len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
    std::wstring buf(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, &buf[0], len);
    return buf;
}
class Plot {
private:
    int width;
    int height;
    int padding;
    double xScale, yScale;
    double xOffset, yOffset;
    std::vector<std::pair<double, double>> data;
    std::string title;
    std::string xlabel, ylabel;
    double xmin, ymin;

public:
    Plot(int w = 800, int h = 600, int pad = 50) : width(w), height(h), padding(pad) {
        initgraph(width, height);
        setbkcolor(WHITE);
        cleardevice();
    }

    ~Plot() {
        closegraph();
    }

    // 设置图表标题
    void setTitle(const std::string& t) { title = t; }

    // 设置坐标轴标签
    void setLabels(const std::string& x, const std::string& y) {
        xlabel = x;
        ylabel = y;
    }

    // 添加数据点
    void addPoint(double x, double y) {
        data.push_back({ x, y });
    }

    // 绘制坐标轴
    void drawAxes() {
        // 计算数据范围
        xmin = data[0].first;
        double xmax = xmin;
        ymin = data[0].second;
        double ymax = ymin;
        for (auto& p : data) {
            xmin = min(xmin, p.first);
            xmax = max(xmax, p.first);
            ymin = min(ymin, p.second);
            ymax = max(ymax, p.second);
        }

        // 计算缩放比例
        xScale = (width - 2 * padding) / (xmax - xmin);
        yScale = (height - 2 * padding) / (ymax - ymin);
        xOffset = padding - xmin * xScale;
        yOffset = height - padding + ymin * yScale;
        setlinecolor(BLUE);
        setlinestyle(PS_SOLID, 2);
        // 绘制x轴
        line(padding, height - padding, width - padding, height - padding);
        // 绘制y轴
        line(padding, padding, padding, height - padding);

        // 绘制坐标轴标签
        settextcolor(BLACK);
        std::wstring wtitle = s_ws(title);
        std::wstring wxlabel = s_ws(xlabel);
        std::wstring wylabel = s_ws(ylabel);
        outtextxy(width / 2 - 20, height - padding + 20, wtitle.c_str());
        outtextxy(width - padding + 10, height - padding - 20, wxlabel.c_str());
        outtextxy(padding - 50, height / 2, wylabel.c_str());
        // 绘制x轴刻度和范围
        const int numXTicks = 10;
        double xTickStep = (xmax - xmin) / numXTicks;
        for (int i = 0; i <= numXTicks; ++i) {
            double xValue = xmin + i * xTickStep;
            int xPixel = padding + (xValue - xmin) * xScale;
            line(xPixel, height - padding, xPixel, height - padding + 5);
            std::ostringstream oss;
            oss.precision(2);
            oss << std::fixed << xValue;
            std::string label = oss.str();
            std::wstring wlabel = s_ws(label);
            outtextxy(xPixel - 10, height - padding + 10, wlabel.c_str());
        }
        std::ostringstream ossXMin, ossXMax;
        ossXMin.precision(2);
        ossXMin << std::fixed << xmin;
        std::wstring wXmin = s_ws(ossXMin.str());
        ossXMax.precision(2);
        ossXMax << std::fixed << xmax;
        std::wstring wXmax = s_ws(ossXMax.str());
        outtextxy(padding - 10, height - padding + 10, wXmin.c_str());
        outtextxy(width - padding - 10, height - padding + 10, wXmax.c_str());

        // 绘制y轴刻度和范围
        const int numYTicks = 10;
        double yTickStep = (ymax - ymin) / numYTicks;
        for (int i = 0; i <= numYTicks; ++i) {
            double yValue = ymin + i * yTickStep;
            int yPixel = height - padding - (yValue - ymin) * yScale;
            line(padding, yPixel, padding - 5, yPixel);
            std::ostringstream oss;
            oss.precision(2);
            oss << std::fixed << yValue;
            std::string label = oss.str();
            std::wstring wlabel = s_ws(label);
            outtextxy(padding - 30, yPixel - 5, wlabel.c_str());
        }
        std::ostringstream ossYMin, ossYMax;
        ossYMin.precision(2);
        ossYMin << std::fixed << ymin;
        std::wstring wYmin = s_ws(ossYMin.str());
        ossYMax.precision(2);
        ossYMax << std::fixed << ymax;
        std::wstring wYmax = s_ws(ossYMax.str());
        outtextxy(padding - 30, height - padding - 5, wYmin.c_str());
        outtextxy(padding - 30, padding - 5, wYmax.c_str());
    }

    // 绘制折线图
    void plotLine(int color = BLUE, int thickness = 2) {
        setlinecolor(color);
        setlinestyle(PS_SOLID, thickness);

        for (size_t i = 1; i < data.size(); i++) {
            int x1 = padding + (data[i - 1].first - xmin) * xScale;
            int y1 = height - padding - (data[i - 1].second - ymin) * yScale;
            int x2 = padding + (data[i].first - xmin) * xScale;
            int y2 = height - padding - (data[i].second - ymin) * yScale;
            line(x1, y1, x2, y2);
        }
    }

    // 绘制散点图
    void plotScatter(int color = RED, int size = 5) {
        drawAxes();
        setfillcolor(color);
        for (auto& p : data) {
            int x = padding + (p.first - xmin) * xScale;
            int y = height - padding - (p.second - ymin) * yScale;
            solidcircle(x, y, size);
        }
        data.clear();
    }

    // 绘制函数曲线
    void plotFunction(double (*func)(double), double start, double end, int samples = 1000) {
        data.clear();
        double step = (end - start) / samples;
        for (int i = 0; i <= samples; i++) {
            double x = start + i * step;
            double y = func(x);
            addPoint(x, y);
        }
        drawAxes();
        plotLine();
        data.clear();
    }

    // 保存图像
    void saveImage(const std::string& filename) {
        saveimage(s_ws(filename).c_str());
    }
};

#endif//PLOT_H
