#pragma once

#include <mutex>

#if 0

class EzUiAnimationManager {
public:
  //////////////////////////
  // Animation Thread
  //////////////////////////
  void animationThread() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (mRunning) {
      std::unique_lock<std::mutex> lock(tweenMutex);
      // 等待有动画或线程退出
      tweenCV.wait(lock, [] { return !activeTweens.empty() || !mRunning; });
      if (!mRunning) break;
      lock.unlock();

      auto now = std::chrono::high_resolution_clock::now();
      float delta = std::chrono::duration<float, std::milli>(now - lastTime).count();
      lastTime = now;

      lock.lock();
      for (auto it = activeTweens.begin(); it != activeTweens.end(); ) {
        (*it)->step(delta);
        if ((*it)->finished) {
          delete* it;
          it = activeTweens.erase(it);
        } else {
          ++it;
        }
      }
      lock.unlock();
    }
  }

protected:
  std::vector<Tween*> activeTweens;
  std::mutex tweenMutex;
  std::condition_variable tweenCV;
  std::atomic<bool> mRunning(true);
};

#endif