#pragma once

template <class T>
class Singleton
{
private:
    static T* _instance; 

protected:
    Singleton() {}
    virtual ~Singleton() {}

public:
    static T* GetInstance()
    {
        if (_instance == nullptr)
        {
            _instance = new T();
        }
        return _instance;
    }
};

template <class T>
T* Singleton<T>::_instance = nullptr;
