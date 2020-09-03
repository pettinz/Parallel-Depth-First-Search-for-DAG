#include <exception>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <optional>

namespace threadsafe
{
    class queue_error : public std::exception
    {
        std::string msg_;

    public:
        explicit queue_error(std::string msg) { msg_ = std::move(msg); }
        const char *what() const noexcept override { return msg_.c_str(); }
    };

    template <typename T>
    class queue
    {
        std::queue<T> queue_;
        mutable std::mutex mutex;

    public:
        queue() = default;
        queue(const queue<T> &) = delete;
        queue &operator=(const queue<T> &) = delete;

        queue(queue<T> &&other) noexcept(false)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (!empty())
                throw queue_error("moving into a non-empty queue");

            queue_ = std::move(other.queue);
        }

        virtual ~queue() noexcept(false)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (!empty())
                throw queue_error("destroying a non-empty queue");
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue_.size();
        }

        bool empty() const
        {
            return queue_.empty();
        }

        void push(const T &item)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue_.push(item);
        }

        std::optional<T> pop()
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue_.empty())
            {
                return {};
            }

            T tmp = queue_.front();
            queue_.pop();

            return tmp;
        }
    };
} // namespace threadsafe