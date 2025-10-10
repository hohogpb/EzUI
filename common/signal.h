#pragma once

#include <functional>
#include <vector>
#include "signal_dispatcher.h"

template <typename... Args>
class Signal {
 public:
  using Callback = std::function<void(Args...)>;
  using ListenerId = std::size_t;

  // 添加监听器，返回其 ID
  ListenerId connect(Callback cb) {
    ListenerId id = _nextId++;
    _listeners.push_back({id, std::move(cb)});
    return id;
  }

  // operator+= 语法糖：添加监听器
  ListenerId operator+=(Callback cb) { return connect(std::move(cb)); }

  // 移除监听器
  void disconnect(ListenerId id) {
    _listeners.erase(
      std::remove_if(_listeners.begin(), _listeners.end(), [=](const Listener& l) { return l.id == id; }),
      _listeners.end());
  }

  // 触发事件
  void emit(Args... args) {
    for(const auto& l : _listeners) {
      l.callback(args...);
    }
  }

  // 同線程
  void enqueue(Args... args) {
    SignalDispatcher::enqueue([=, this]() { emit(args...); });
  }

 private:
  struct Listener {
    ListenerId id;
    Callback callback;
  };

  std::vector<Listener> _listeners;
  ListenerId _nextId = 1;
};
