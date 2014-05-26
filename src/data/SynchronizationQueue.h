#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include <thread>
#include <queue>

template<class T>
class SynchronizationQueue
{
	public:
		SynchronizationQueue(void);
		void push(T t);
		T pop(void);
	private:
		std::mutex mutex;
		std::queue<T> queue;
};

template<class T>
SynchronizationQueue<T>::SynchronizationQueue(void)
{

}

template<class T>
void SynchronizationQueue<T>::push(T t)
{
	mutex.lock();
		queue.push(t);
	mutex.unlock();
}

template<class T>
T SynchronizationQueue<T>::pop(void)
{
	mutex.lock();
		T t=queue.pop();
	mutex.unlock();
	return t;
}

#endif
