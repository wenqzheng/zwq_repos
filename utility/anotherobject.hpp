class Object
{
    using funcType = function_wrapper<>;
    
    struct ObjectConcept()
    {
        virtual ~ObjectConcept() {}
        virtual void call() = 0;
    };

    template<typename T>
    struct ObjectModel:ObjectConcept
    {
        virtual ~ObjectModel() {}
        virtual void call()
        {
            object.__call();
        }

    private:
        T object;
    };

    typename<typename T>
    struct decorator:typename T
    {
        decorator(const T& t)
            :__sp_W(std::make_shared<T>(t))
        {}
        auto __call(*callFunc)
        {
            std::bind(funcType(std::move(__func)), &__sp_W)();
        }
    private:
        typedef void (T::*callFunc)();
        
        shared_ptr_wrapper<T> __sp_W;
    };

    shared_ptr_wrapper<ObjectConcept> object;
public:
    template<typename T>
    Object(const T& obj):
}
