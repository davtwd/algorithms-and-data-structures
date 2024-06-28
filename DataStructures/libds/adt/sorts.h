#pragma once

#include <libds/amt/implicit_sequence.h>
#include <libds/adt/queue.h>
#include <libds/adt/array.h>
#include <functional>
#include <cmath>
#include <algorithm>
#include <type_traits>

namespace ds::adt {
    template <typename T>
    struct Sort {
    public:
        virtual void sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) = 0;
        void sort(amt::ImplicitSequence<T>& is) {
            sort(is, [](const T& a, const T& b)->bool {return a < b; });
        }
    };

    //----------

    template <typename T>
    class SelectSort :
        public Sort<T> {
    public:
        void sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) override;
    };

    //----------

    template <typename T>
    class InsertSort :
        public Sort<T> {
    public:
        void sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) override;
    };

    //----------

    template <typename T>
    class BubbleSort :
        public Sort<T> {
    public:
        void sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) override;
    };

    //----------

    template <typename T>
    class QuickSort :
        public Sort<T> {
    public:
        void sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) override;

    private:
        void quick(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare, size_t min, size_t max);
    };

    //----------

    template <typename T>
    class HeapSort :
        public Sort<T> {
    public:
        void sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) override;
    };

    //----------

    template <typename T>
    class ShellSort :
        public Sort<T> {
    public:
        void sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) override;

    private:
        void shell(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare, size_t k);
    };

    //----------

    template <typename Key, typename T>
    class RadixSort :
        public Sort<T> {
        static_assert(std::is_integral_v<Key>, "Radix sort supports only integral types.");

    public:
        RadixSort();
        RadixSort(std::function<Key(const T&)> getKey);

        void sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) override;

    private:
        std::function<Key(const T&)> getKey_;
    };

    //----------

    template <typename T>
    class MergeSort :
        public Sort<T> {
    public:
        void sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) override;

    private:
        void split(size_t n);
        void merge(std::function<bool(const T&, const T&)> compare, size_t n);

    private:
        ImplicitQueue<T>* queue1_ {nullptr};
        ImplicitQueue<T>* queue2_ {nullptr};
        ImplicitQueue<T>* mergeQueue_ {nullptr};
    };

    //----------

    template<typename T>
    void SelectSort<T>::sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) {

        if (is.isEmpty()) {
            return;
        }

        for (size_t i = 0; i < is.size() - 1; ++i) {
            size_t min = i;
            for (size_t j = i + 1; j < is.size(); ++j) {
                if (compare(is.access(j)->data_, is.access(min)->data_)) {
                    min = j;
                }
            }
            std::swap(is.access(i)->data_, is.access(min)->data_);
        }
    }

    template<typename T>
    void InsertSort<T>::sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) {

        if (is.isEmpty()) {
            return;
        }

        for (size_t i = 1; i < is.size(); ++i) {
            T temp = is.access(i)->data_;
            size_t j = i;
            while (j > 0 && compare(temp, is.access(j - 1)->data_)) {
                is.access(j)->data_ = is.access(j - 1)->data_;
                --j;
            }
            is.access(j)->data_ = temp;
        }
    }

    template<typename T>
    void BubbleSort<T>::sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) {
        if (is.isEmpty()) {
            return;
        }

        bool swap;
        do {
            swap = false;
            for (size_t i = 0; i < is.size() - 1; ++i) {
                if (compare(is.access(i + 1)->data_, is.access(i)->data_)) {
                    std::swap(is.access(i + 1)->data_, is.access(i)->data_);
                    swap = true;
                }
            }
        } while (swap);
    }

    template<typename T>
    void QuickSort<T>::sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) {
        if (!is.isEmpty()) {
            quick(is, compare, 0, is.size() - 1);
        }
    }

    template<typename T>
    void QuickSort<T>::quick(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare, size_t min, size_t max) {
        T pivot = is.access(min + (max - min) / 2)->data_;
        size_t left = min;
        size_t right = max;

        do {
            while (compare(is.access(left)->data_, pivot)) {
                ++left;
            }

            while (right > 0 && compare(pivot, is.access(right)->data_)) {
                --right;
            }

            if (left <= right) {
                std::swap(is.access(left)->data_, is.access(right)->data_);
                ++left;
                if (right > 0) {
                    --right;
                }
            }
        } while (left <= right);

        if (min < right) {
            quick(is, compare, min, right);
        }

        if (left < max) {
            quick(is, compare, left, max);
        }
    }

    template<typename T>
    void HeapSort<T>::sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) {

        for (size_t i = 1; i < is.size(); ++i) {
            bool swap;
            size_t current = i;
            do {
                swap = false;
                size_t parent = (current - 1) / 2;
                if (current > 0 && compare(is.access(parent)->data_, is.access(current)->data_)) {
                    std::swap(is.access(current)->data_, is.access(parent)->data_);
                    current = parent;
                    swap = true;
                }
            } while (swap);
        }

        for (size_t i = is.size() - 1; i >= 1; --i) {
            std::swap(is.access(0)->data_, is.access(i)->data_);
            bool swap;
            size_t current = 0;
            do {
                swap = false;
                size_t left = 2 * current + 1;
                size_t right = 2 * current + 2;
                size_t max;
                if (left < i && right < i) {
                    max = compare(is.access(right)->data_, is.access(left)->data_) ? left : right;
                } else {
                    max = left < i ? left : right;
                }
                if (max < i && compare(is.access(current)->data_, is.access(max)->data_)) {
                    std::swap(is.access(current)->data_, is.access(max)->data_);
                    current = max;
                    swap = true;
                }
            } while (swap);
        }
    }

    template<typename T>
    void ShellSort<T>::sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) {
        shell(is, compare, static_cast<size_t>(std::ceil(std::log10(is.size()))));
    }

    template<typename T>
    void ShellSort<T>::shell(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare, size_t k) {
        for (size_t d = 0; d < k; ++d) {
            for (size_t i = d; i < is.size(); ++i) {
                size_t j = i;
                while (j >= k && j - k >= d && compare(is.access(j)->data_, is.access(j - k)->data_)) {
                    std::swap(is.access(j)->data_, is.access(j - k)->data_);
                    j = j - k;
                }
            }
        }
        if (k > 1) {
            shell(is, compare, k - 1);
        }
    }

    template<typename Key, typename T>
    RadixSort<Key, T>::RadixSort() :
        getKey_([](auto const& x) { return x; }) {}

    template<typename Key, typename T>
    RadixSort<Key, T>::RadixSort(std::function<Key(const T&)> getKey) :
        getKey_(getKey) {}

    template<typename Key, typename T>
    void RadixSort<Key, T>::sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) {
        Array<ExplicitQueue<T>*> buckets(10);
        for (size_t i = 0; i < 10; ++i) {
            buckets.set(new ExplicitQueue<T>(), i);
        }

        size_t component = 1;
        bool exists = true; // check if there is a component to sort
        while (exists) {
            exists = false;
            for (auto& block : is) {
                Key key = getKey_(block);
                ExplicitQueue<T>* bucket = buckets.access((key / component) % 10);
                bucket->push(block);
                if (key / (component * 10) > 0) {
                    exists = true;
                }
            }
            component *= 10;
            size_t index = 0;
            for (size_t i = 0; i < 10; ++i) {
                ExplicitQueue<T>* bucket = buckets.access(i);
                while (!bucket->isEmpty()) {
                    is.access(index)->data_ = bucket->pop();
                    ++index;
                }
            }
        }

        for (size_t i = 0; i < 10; ++i) {
            delete buckets.access(i);
        }
    }

    template<typename T>
    void MergeSort<T>::sort(amt::ImplicitSequence<T>& is, std::function<bool(const T&, const T&)> compare) {
        queue1_ = new ImplicitQueue<T>(is.size());
        queue2_ = new ImplicitQueue<T>(is.size());
        mergeQueue_ = new ImplicitQueue<T>(is.size());

        for (auto& block : is) {
            mergeQueue_->push(block);
        }

        size_t i = 1;
        while (i < is.size()) {
            split(i);
            merge(compare, i);
            i *= 2;
        }

        for (size_t j = 0; j < is.size(); ++j) {
            is.access(j)->data_ = mergeQueue_->pop();
        }

        delete queue1_;
        delete queue2_;
        delete mergeQueue_;
        queue1_ = nullptr;
        queue2_ = nullptr;
        mergeQueue_ = nullptr;
    }

    template<typename T>
    void MergeSort<T>::split(size_t n) {
        size_t i = 0;
        bool isFirst = true;
        while (!mergeQueue_->isEmpty()) {
            if (i % n == 0) {
                i = 0;
                isFirst = !isFirst;
            }

            if (isFirst) {
                queue1_->push(mergeQueue_->pop());
            } else {
                queue2_->push(mergeQueue_->pop());
            }

            ++i;
        }
    }

    template<typename T>
    void MergeSort<T>::merge(std::function<bool(const T&, const T&)> compare, size_t n) {
        size_t nFirsts = 0;
        size_t nSeconds = 0;

        do {
            if (nFirsts == 0 && nSeconds == 0) {
                nFirsts = std::min(n, queue1_->size());
                nSeconds = std::min(n, queue2_->size());
            }

            T* key1 = (nFirsts > 0) ? &queue1_->peek() : nullptr;
            T* key2 = (nSeconds > 0) ? &queue2_->peek() : nullptr;

            if (key1 != nullptr && key2 != nullptr) {
                if (compare(*key1, *key2)) {
                    mergeQueue_->push(queue1_->pop());
                    --nFirsts;
                } else {
                    mergeQueue_->push(queue2_->pop());
                    --nSeconds;
                }
            } else {
                if (key1 != nullptr) {
                    mergeQueue_->push(queue1_->pop());
                    --nFirsts;
                } else {
                    mergeQueue_->push(queue2_->pop());
                    --nSeconds;
                }
            }
        } while (!queue1_->isEmpty() || !queue2_->isEmpty());
    }
}