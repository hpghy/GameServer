#ifndef LOCKFREE_QUEUE_HPP
#define LOCKFREE_QUEUE_HPP

#include <atomic>
#include <memory>

//运用C++11的atomic实现无锁队列，替代boost::lockfree::queue

template <class T>
class lockfree_queue_node
{
public:
	using T_ptr = std::shared_ptr<T>;
	lockfree_queue_node(T_ptr v);

	lockfree_queue_node<T>* next() { return next_; }
	T_ptr& value() { return value_; }

private:
	T_ptr value_;
	lockfree_queue_node<T> *next_;
};

template <class T>
class lockfree_queue
{
public:
	using node_type = lockfree_queue_node<T>;
	using T_ptr = std::shared_ptr<T>;

	lockfree_queue()
	{
	}

	~lockfree_queue()
	{
	}

	void push(T_ptr value)
	{
		//operator new/delete(内存分配)是thread safety,但是constructor不是
		push(new node_type(value));
	}

	void push(node_type *node)
	{
	}

	std::shared_ptr<T> pop()
	{
	}

private:
	std::atomic<node_type*> head_;
};



#endif
