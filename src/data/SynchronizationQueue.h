#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include <cstdlib>
#include <queue>
#include <SFML/System/Mutex.hpp>

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
		sf::Mutex mutex;
		std::queue<T> queue;
};

template<class T>
SynchronizationQueue<T>::SynchronizationQueue(void)
{

}

template<class T>
void SynchronizationQueue<T>::push(T t, bool release)
{
	mutex.lock();
		queue.push(t);
	if(release)mutex.unlock();
}

template<class T>
void SynchronizationQueue<T>::manualRelease(void)
{
	mutex.unlock();
}

template<class T>
T SynchronizationQueue<T>::pop(void)
{
	T t=NULL;
	mutex.lock();
		if(queue.size()>0)
		{
			t=queue.front();
			queue.pop();
		}
	mutex.unlock();
	return t;
}

template<class T>
void SynchronizationQueue<T>::popAll(std::queue<T>& ret)
{
	mutex.lock();
		queue.swap(ret);
	mutex.unlock();
}

#endif
