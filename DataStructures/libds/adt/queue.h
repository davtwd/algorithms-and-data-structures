#pragma once

#include <libds/adt/abstract_data_type.h>
#include <libds/amt/implicit_sequence.h>
#include <libds/amt/explicit_sequence.h>

namespace ds::adt {

    template <typename T>
    class Queue :
        virtual public ADT
    {
    public:
        virtual void push(T element) = 0;
        virtual T& peek() = 0;
        virtual T pop() = 0;
    };

    //----------

    template<typename T>
    class ImplicitQueue :
        public Queue<T>,
        public ADS<T>
    {
    public:
        ImplicitQueue();
        ImplicitQueue(const ImplicitQueue& other);
        ImplicitQueue(size_t capacity);

        size_t getCapacity() const;

        ADT& assign(const ADT& other) override;
        void clear() override;
        size_t size() const override;
        bool equals(const ADT& other) override;

        void push(T element) override;
        T& peek() override;
        T pop() override;

        static const int INIT_CAPACITY = 100;

    private:
        amt::CIS<T>* getSequence() const;

    private:
        size_t insertionIndex_;
        size_t removalIndex_;
        size_t size_;
    };

    //----------

    template<typename T>
    class ExplicitQueue :
        public Queue<T>,
        public ADS<T>
    {
    public:
        ExplicitQueue();
        ExplicitQueue(const ExplicitQueue& other);
        void push(T element) override;
        T& peek() override;
        T pop() override;

    private:
        amt::SinglyLS<T>* getSequence() const;
    };

    //----------

    template<typename T>
    ImplicitQueue<T>::ImplicitQueue():
        ImplicitQueue(INIT_CAPACITY)
    {
    }

    template<typename T>
    ImplicitQueue<T>::ImplicitQueue(size_t capacity) :
        ADS<T>(new amt::CIS<T>(capacity, true)),
        insertionIndex_(0),
        removalIndex_(0),
        size_(0)
    {
    }

    template <typename T>
    size_t ImplicitQueue<T>::getCapacity() const
    {
        return this->getSequence()->size();
    }

    template<typename T>
    ImplicitQueue<T>::ImplicitQueue(const ImplicitQueue& other) :
        ADS<T>(new amt::CIS<T>(), other),
        insertionIndex_(other.insertionIndex_),
        removalIndex_(other.removalIndex_),
        size_(other.size_)
    {
    }

    template<typename T>
    ADT& ImplicitQueue<T>::assign(const ADT& other)
    {
        const ImplicitQueue<T>* otherQueue = dynamic_cast<const ImplicitQueue<T>*>(&other);
        if (otherQueue == nullptr)             {
			throw std::invalid_argument("Invalid argument!");
		}

        if (this != &other) {

            if (this->getSequence()->size() < otherQueue->size()) {
                throw std::invalid_argument("Invalid argument!");
            }

            this->clear();
            insertionIndex_ = this->getSequence()->indexOfNext(otherQueue->size() - 1);
            removalIndex_ = 0;
            size_ = otherQueue->size_;
            size_t otherRemovalIndex = otherQueue->removalIndex_;

            for (size_t i = 0; i < size_; ++i) {
                this->getSequence()->access(i)->data_ = otherQueue->getSequence()->access(otherRemovalIndex)->data_;
                otherRemovalIndex = otherQueue->getSequence()->indexOfNext(otherRemovalIndex);
            }
        }

        return *this;
    }

    template<typename T>
    void ImplicitQueue<T>::clear()
    {
        insertionIndex_ = removalIndex_;
        size_ = 0;
    }

    template<typename T>
    size_t ImplicitQueue<T>::size() const
    {
        return size_;
    }

    template<typename T>
    bool ImplicitQueue<T>::equals(const ADT& other)
    {
        if (this == &other) {
            return true;
        }

        if (this->size() != other.size()) {
            return false;
        }

        const ImplicitQueue<T>* otherQ = dynamic_cast<const ImplicitQueue<T>*>(&other);

        if (otherQ == nullptr) {
            return false;
        }

        size_t myIndex = removalIndex_;
        size_t otherIndex = otherQ->removalIndex_;

        for (size_t i = 0; i <= this->size() - 1; ++i) {
            if (this->getSequence()->access(myIndex)->data_ !=
                otherQ->getSequence()->access(otherIndex)->data_
                ) {
                return false;
            }

            myIndex = this->getSequence()->indexOfNext(myIndex);
            otherIndex = otherQ->getSequence()->indexOfNext(otherIndex);
        }

        return true;
    }

    template<typename T>
    void ImplicitQueue<T>::push(T element)
    {
        auto structure = this->getSequence();

        if (this->size() == structure->getCapacity()) {
            throw std::out_of_range("Queue is full!");
        }

        structure->access(insertionIndex_)->data_ = element;
        insertionIndex_ = structure->indexOfNext(insertionIndex_);
        ++size_;
    }

    template<typename T>
    T& ImplicitQueue<T>::peek()
    {
        if (this->isEmpty()) {
            throw std::out_of_range("Queue is empty!");
        }
        return this->getSequence()->access(removalIndex_)->data_;
    }

    template<typename T>
    T ImplicitQueue<T>::pop()
    {
        if (this->isEmpty()) {
            throw std::out_of_range("Queue is empty!");
        }

        auto structure = this->getSequence();

        T result = structure->access(removalIndex_)->data_;
        removalIndex_ = structure->indexOfNext(removalIndex_);
        --size_;

        return result;
    }

    template<typename T>
    amt::CIS<T>* ImplicitQueue<T>::getSequence() const
    {
        return dynamic_cast<amt::CIS<T>*>(this->memoryStructure_);
    }

    template<typename T>
    ExplicitQueue<T>::ExplicitQueue() :
        ADS<T>(new amt::SinglyLS<T>())
    {
    }

    template<typename T>
    ExplicitQueue<T>::ExplicitQueue(const ExplicitQueue& other) :
        ADS<T>(new amt::SinglyLS<T>(), other)
    {
    }

    template<typename T>
    void ExplicitQueue<T>::push(T element)
    {
        this->getSequence()->insertLast().data_ = element;
    }

    template<typename T>
    T& ExplicitQueue<T>::peek()
    {
        if (this->isEmpty())
        {
            throw std::out_of_range("Queue is empty!");
        }

        return this->getSequence()->accessFirst()->data_;
    }

    template<typename T>
    T ExplicitQueue<T>::pop()
    {
        if (this->isEmpty())
        {
            throw std::out_of_range("Queue is empty!");
        }

        T result = this->getSequence()->accessFirst()->data_;
        this->getSequence()->removeFirst();
        return result;
    }

    template<typename T>
    amt::SinglyLS<T>* ExplicitQueue<T>::getSequence() const
    {
        return dynamic_cast<amt::SinglyLS<T>*>(this->memoryStructure_);
    }
}