#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cmath>

#pragma once

#include <vector>

class ATextItem {
  public:
    std::string text;
    int alpha;

    ATextItem(const char* text, int alpha) {
      this->text = text;
      this->alpha = alpha;
    }
};

class AText {
  private:
    std::string font_name;

    std::vector<ATextItem> after_items;
    std::string last_text;

  public:

    AText(const char *font_name) {
      this->font_name = font_name;
    }

    void update(const UIState *s, float x, float y, const char *string, int size, NVGcolor color) {
      if(last_text != string) {
        after_items.insert(after_items.begin(), ATextItem(string, 255));
        last_text = string;
      }

      for(auto it = after_items.begin() + 1; it != after_items.end();)  {
        it->alpha -= 1000 / UI_FREQ;
        if(it->alpha <= 0)
          it = after_items.erase(it);
        else
          it++;
      }

      nvgTextAlign(s->vg, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE);

      for(auto it = after_items.rbegin(); it != after_items.rend(); ++it)  {
        color.a = it->alpha/255.f;
        ui_draw_text(s, x, y, it->text.c_str(), size, color, this->font_name.c_str());
      }
    }

    void ui_draw_text(const UIState *s, float x, float y, const char *string, float size, NVGcolor color, const char *font_name) {
      nvgFontFace(s->vg, font_name);
      nvgFontSize(s->vg, size);
      nvgFillColor(s->vg, color);
      nvgText(s->vg, x, y, string, NULL);
    }
};

static void ui_draw_extras_limit_speed(UIState *s)
{
    const UIScene *scene = &s->scene;
    cereal::CarControl::SccSmoother::Reader scc_smoother = scene->car_control.getSccSmoother();
    int activeNDA = scc_smoother.getRoadLimitSpeedActive();
    int limit_speed = scc_smoother.getRoadLimitSpeed();
    int left_dist = scc_smoother.getRoadLimitSpeedLeftDist();

    if(activeNDA > 0)
    {
        int w = 185;
        int h = 60;
        int x = (s->fb_w + (bdr_s*2))/2 - w/2 - bdr_s - 844;
        int y = 238;

        const char* img = activeNDA == 1 ? "img_nda" : "img_hda";
        ui_draw_image(s, {x, y, w, h}, img, 1.f);
    }

    if(limit_speed > 10 && left_dist > 0)
    {
        int w = 160;
        int h = 160;
        int x = (bdr_s*2) + 1690;
        int y = 260;
        char str[32];

        nvgBeginPath(s->vg);
        nvgRoundedRect(s->vg, x, y, w, h, 210);
        nvgStrokeColor(s->vg, nvgRGBA(255, 0, 0, 200)); //red
        nvgStrokeWidth(s->vg, 33);
        nvgStroke(s->vg);

        NVGcolor fillColor = nvgRGBA(255, 255, 255, 200);//white
        nvgFillColor(s->vg, fillColor);
        nvgFill(s->vg);

        nvgFillColor(s->vg, nvgRGBA(0, 0, 0, 250));//black

        nvgFontSize(s->vg, 120);
        nvgFontFace(s->vg, "sans-bold");
        nvgTextAlign(s->vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

        snprintf(str, sizeof(str), "%d", limit_speed);
        nvgText(s->vg, x+w/2, y+h/2, str, NULL);

        nvgFillColor(s->vg, nvgRGBA(255, 255, 255, 250));//white
        
        nvgFontSize(s->vg, 90);

        if(left_dist >= 1000)
            snprintf(str, sizeof(str), "%.1fkm", left_dist / 1000.f);
        else
            snprintf(str, sizeof(str), "%dm", left_dist);

        nvgText(s->vg, x+w/2, y+h + 70, str, NULL);
    }
    else
    {
        auto controls_state = (*s->sm)["controlsState"].getControlsState();
        int sccStockCamAct = (int)controls_state.getSccStockCamAct();
        int sccStockCamStatus = (int)controls_state.getSccStockCamStatus();

        if(sccStockCamAct == 2 && sccStockCamStatus == 2)
        {
            int w = 200;
            int h = 200;
            int x = (bdr_s*2) + 300;
            int y = 80;
            char str[32];

            nvgBeginPath(s->vg);
            nvgRoundedRect(s->vg, x, y, w, h, 210);
            nvgStrokeColor(s->vg, nvgRGBA(255, 0, 0, 200));
            nvgStrokeWidth(s->vg, 30);
            nvgStroke(s->vg);

            NVGcolor fillColor = nvgRGBA(0, 0, 0, 50);
            nvgFillColor(s->vg, fillColor);
            nvgFill(s->vg);

            nvgFillColor(s->vg, nvgRGBA(255, 255, 255, 250));

            nvgFontSize(s->vg, 140);
            nvgFontFace(s->vg, "sans-bold");
            nvgTextAlign(s->vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

            nvgText(s->vg, x+w/2, y+h/2, "CAM", NULL);
        }
    }
}

static void ui_draw_extras(UIState *s)
{
    ui_draw_extras_limit_speed(s);
}
