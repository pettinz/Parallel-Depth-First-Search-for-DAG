#ifndef _THREADSAFE_QUEUE_H
#define _THREADSAFE_QUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <optional>

namespace threadsafe
{
    /**
     * @brief Implementation of a multi-threaded queue.
     */
    template <typename T>
    class queue
    {
        std::queue<T> queue_;      /*!< standard queue */
        mutable std::mutex mutex_; /*!< mutex to access the queue safely in a multi-threaded context */

    public:
        queue() = default;

        /**
         * @brief To avoid copy.
         */
        queue(const queue<T> &) = delete;

        /**
         * @brief To avoid copy.
         */
        queue &operator=(const queue<T> &) = delete;

        queue(queue<T> &&other) noexcept(false)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!empty())
                // TODO: Exception handler
                throw "Error";

            queue_ = std::move(other.queue_);
        }

        virtual ~queue() noexcept(true) {}

        /**
         * @brief Compute the size of the queue
         * 
         * @return size of the queue
         */
        size_t size() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.size();
        }

        /**
         * @brief Check if the queue is empty
         * 
         * @return true, if the queue is empty
         * @return false, otherwise
         */
        inline bool empty() const { return queue_.empty(); }

        /**
         * @brief Push a new item on top of the queue
         * 
         * @param item a const pointer to the item to push into the queue
         */
        void push(const T &item)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(item);
        }

        /**
         * @brief Pop the element from the queue and return it
         * 
         * @return an optional containing the popped element
         */
        std::optional<T> pop()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (queue_.empty())
                return {};

            T tmp = queue_.front();
            queue_.pop();

            return tmp;
        }

        /**
         * @brief Swap the queue with another one
         * 
         * @param q the queue to be swapped with the current one
         */
        void swap(std::queue<T> &q)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::queue<T> tmp = std::move(q);
            q = std::move(queue_);
            queue_ = std::move(tmp);
        }
    };
} // namespace threadsafe

#endif // _THREADSAFE_QUEUE_H