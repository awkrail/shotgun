#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class BlockingQueue {
    public:
        explicit BlockingQueue(size_t max_size) : max_size_{max_size} {}

        void push(const T& item) {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_producer_.wait(lock, [this]() { return queue_.size() < max_size_; });
            
            queue_.push(item);
            cond_consumer_.notify_one();
        }

        T get() {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_consumer_.wait(lock, [this]() { return !queue_.empty(); });

            T item = queue_.front();
            queue_.pop();
            cond_producer_.notify_one();
            return item;
        }

        size_t size() {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.size();
        }

        bool empty() {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }

    private:
        std::queue<T> queue_;
        const size_t max_size_;
        std::mutex mutex_;
        std::condition_variable cond_producer_;
        std::condition_variable cond_consumer_;
};

#endif
