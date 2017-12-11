#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include <cstdlib>
#include <queue>
#ifndef __EMSCRIPTEN__
  #include <SFML/System/Mutex.hpp>
#endif

template<class T>
class SynchronizationQueue
{
	public:
		SynchronizationQueue(void);
		void popAll(std::queue<T>& ret);
		void push(T t, bool release=true);
		void manualRelease(void);
		T pop(void);
	private:
#ifndef __EMSCRIPTEN__
    sf::Mutex mutex;
#endif
		std::queue<T> queue;
};

template<class T>
SynchronizationQueue<T>::SynchronizationQueue(void)
{

}

template<class T>
void SynchronizationQueue<T>::push(T t, bool release)
{
#ifndef __EMSCRIPTEN__
  mutex.lock();
#endif
  queue.push(t);
#ifndef __EMSCRIPTEN__
  if (release)
    mutex.unlock();
#endif
}

template<class T>
void SynchronizationQueue<T>::manualRelease(void)
{
#ifndef __EMSCRIPTEN__
  mutex.unlock();
#endif
}

template<class T>
T SynchronizationQueue<T>::pop(void)
{
	T t=NULL;
#ifndef __EMSCRIPTEN__
  mutex.lock();
#endif
  if (queue.size() > 0) {
    t = queue.front();
    queue.pop();
  }
#ifndef __EMSCRIPTEN__
  mutex.unlock();
#endif
  return t;
}

template<class T>
void SynchronizationQueue<T>::popAll(std::queue<T>& ret)
{
#ifndef __EMSCRIPTEN__
  mutex.lock();
#endif
  queue.swap(ret);
#ifndef __EMSCRIPTEN__
  mutex.unlock();
#endif
}

#endif
