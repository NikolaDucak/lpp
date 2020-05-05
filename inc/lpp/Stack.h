#ifndef __STACK_H__
#define __STACK_H__

#include <deque>

namespace lpp{

	/*
	 * Simple wrapper around std::deque to provide some 
	 * extra functionality used in VirtualMachine
	 */
	template <typename T>
	class Stack{
		std::deque<T> deque;
	public:

		typename std::deque<T>::iterator begin(){
			return deque.begin();
		}

		typename std::deque<T>::iterator end(){
			return deque.end();
		}

		void clearFrom(int i){
			deque.erase(deque.begin()+i, deque.end());
		}
		
		size_t size() const{
			return deque.size();
		}

		T& top(){
			return deque.back();
		}

		T pop(){
			// should pop on empty stack be error?
			if(deque.size() == 0) return T();
			T r = deque.back();
			deque.pop_back();
			return r;
		}

		void push(const T& val){
			deque.push_back(val);
		}

		T& fromTop(size_t i){
			return deque[ deque.size() - 1 - i ];
		}

		T& fromStart(size_t i){
			return deque[ i ];
		}

		bool empty(){
			return deque.empty();
		}
	};

}
#endif
