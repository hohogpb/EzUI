#pragma once

#include <functional>
#include <vector>
#include "signal_dispatcher.h"

template <typename... Args>
class Signal {
 public:
  using Callback = std::function<void(Args...)>;
  using ListenerId = std::size_t;

  // ��Ӽ������������� ID
  ListenerId connect(Callback cb) {
    ListenerId id = _nextId++;
    _listeners.push_back({id, std::move(cb)});
    return id;
  }

  // operator+= �﷨�ǣ���Ӽ�����
  ListenerId operator+=(Callback cb) { return connect(std::move(cb)); }

  // �Ƴ�������
  void disconnect(ListenerId id) {
    _listeners.erase(
      std::remove_if(_listeners.begin(), _listeners.end(), [=](const Listener& l) { return l.id == id; }),
      _listeners.end());
  }

  // �����¼�
  void emit(Args... args) {
    for(const auto& l : _listeners) {
      l.callback(args...);
    }
  }

  // ͬ����
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
