#pragma once

#include <functional>
#include <queue>

class SignalDispatcher {
 public:
  static void enqueue(std::function<void()> task) { _queue.push(std::move(task)); }

  static void poll() {
    while(!_queue.empty()) {
      auto task = _queue.front();
      _queue.pop();
      task();
    }
  }

  static bool hasPending() { return !_queue.empty(); }

 private:
  static inline std::queue<std::function<void()>> _queue;
};