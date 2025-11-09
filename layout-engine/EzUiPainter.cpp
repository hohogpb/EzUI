#include "pch.h"
#include "EzUiPainter.h"

EzUiDisplayList EzUiPainter::BuildDisplayList(EzUiLayoutBox* layoutRoot) {
  EzUiDisplayList list;

  EzUiPainter painter;
  painter.RenderLayoutBox(list, layoutRoot);

  return list;
}

void EzUiPainter::RenderLayoutBox(EzUiDisplayList& list, EzUiLayoutBox* layoutBox) {
  RenderBackground(list, layoutBox);
  RenderBorders(list, layoutBox);

  for (auto& child : layoutBox->children)
    RenderLayoutBox(list, child);
}

void EzUiPainter::RenderBackground(EzUiDisplayList& list, EzUiLayoutBox* layoutBox) {
  //auto color = GetColor(layoutBox, L"background")
  auto bgColor = layoutBox->GetBackgroundColor();

  if (bgColor) {
    EzUiDisplayCommand command{ EzUiDisplayCommand::Type::SolidColor,*bgColor,layoutBox->rect };
    list.push_back(command);
  }
}

void EzUiPainter::RenderBorders(EzUiDisplayList& list, EzUiLayoutBox* layoutBox) {
  // auto colorOpt = layoutBox->GetColor(layoutBox, L"border-color");
  auto colorOpt = layoutBox->GetBorderColor();
  if (!colorOpt) return;
  EzUI::Color color = *colorOpt;

  auto borderBox = layoutBox->rect;
  auto border = layoutBox->GetBorder();

  // ×ó±ß
  list.push_back({ EzUiDisplayCommand::Type::SolidColor, color,
      { borderBox.x, borderBox.y, border.left, borderBox.height } });

  // ÓÒ±ß
  list.push_back({ EzUiDisplayCommand::Type::SolidColor, color,
    { borderBox.x + borderBox.width - border.right, borderBox.y, border.right, borderBox.height } });

  // ÉÏ±ß
  list.push_back({ EzUiDisplayCommand::Type::SolidColor, color,
    { borderBox.x, borderBox.y, borderBox.width, border.top } });

  // ÏÂ±ß
  list.push_back({ EzUiDisplayCommand::Type::SolidColor, color,
    { borderBox.x, borderBox.y + borderBox.height - border.bottom, borderBox.width, border.bottom } });
}

std::optional<EzUI::Color> EzUiPainter::GetColor(EzUiLayoutBox* layoutBox, const std::wstring& name) {
  return layoutBox->styleNode->color;

#if 0
  if (layout_box.boxType == BoxTypeTag::BlockNode || layout_box.boxType == BoxTypeTag::InlineNode) {
    if (layout_box.style) {
      auto v = layout_box.style->value(name);
      if (v && v->type == Value::Type::ColorValue)
        return v->color;
    }
  }
  return std::nullopt;
#endif
}
