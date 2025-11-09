#pragma once

#include "EzUiLayoutBox.h"

class EzUiLayoutBuilder {
public:
  static EzUiLayoutBox* BuildTree(EzUiStyledNode* styleNode);

protected:
  EzUiLayoutBox* Build(EzUiStyledNode* styleNode);

  EzUiLayoutBox* BuildNode(EzUiStyledNode* styleNode);
};

