#pragma once

#include "EzUiLayoutBox.h"

enum class EzUiDisplayCommandType {
  SolidColor
};

struct EzUiDisplayCommand {
  enum class Type {
    SolidColor,
    PushOpacity,
    PopOpacity
  };

  Type type;
  EzUI::Color color{};
  EzUI::RectF rect{};
};

using EzUiDisplayList = std::vector<EzUiDisplayCommand>;

struct EzUiDrawContext {

};

class EzUiPainter {
public:
  static EzUiDisplayList BuildDisplayList(EzUiLayoutBox* layoutRoot);

  void RenderLayoutBox(EzUiDisplayList& list, EzUiLayoutBox* layoutBox);

  void RenderBackground(EzUiDisplayList& list, EzUiLayoutBox* layoutBox);

  void RenderBorders(EzUiDisplayList& list, EzUiLayoutBox* layoutBox);

  std::optional<EzUI::Color> GetColor(EzUiLayoutBox* layoutBox, const std::wstring& name);


};

