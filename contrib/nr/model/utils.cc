#include <iostream>
#include <queue>

template <typename T>
class LimitedSizeQueue {
  public:
    LimitedSizeQueue(int max_size) : m_max_size(max_size) {}

    void push(const T& value) {
        if (m_queue.size() >= m_max_size) {
            std::cout << "Queue is full, removing the oldest element." << std::endl;
            m_queue.pop();
        }
        m_queue.push(value);
    }

    T front() {
        return m_queue.front();
    }

    void pop() {
        m_queue.pop();
    }

    int size() {
        return m_queue.size();
    }

    bool empty() {
        return m_queue.empty();
    }

  private:
    int m_max_size;
    std::queue<T> m_queue;
};