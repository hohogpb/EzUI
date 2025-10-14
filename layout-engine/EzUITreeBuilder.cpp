#include "pch.h"
#include "EzUITreeBuilder.h"
#include "EzUIElementBuilder.h"

using std::stack;


UIElement* EzUITreeBuilder::Build(EzUIDocNode* domRoot) {
  EzUIElementBuilder elBuilder;
  auto uiRoot = elBuilder.Build(domRoot);

  stack<UIElement*> uiStack;
  uiStack.push(uiRoot);

  stack<EzUIDocNode*> domStack;
  domStack.push(domRoot);

  while (!domStack.empty()) {
    auto domParent = domStack.top();
    domStack.pop();

    auto uiParent = uiStack.top();
    uiStack.pop();

    // 为子节点创建 UIElement
    for (int i = 0; i < (int)domParent->children.size(); i++) {
      auto& childDomNode = domParent->children[i];

      auto childUiNode = elBuilder.Build(childDomNode.get());

      uiParent->AddChild(childUiNode);

      // 子节点入栈
      domStack.push(childDomNode.get());
      uiStack.push(childUiNode);
    }
  }

  return uiRoot;
}
