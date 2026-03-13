#ifndef IENUMERATOR_H
#define IENUMERATOR_H

template <class T>
class IEnumerator {
public:
    virtual bool move_next() = 0; // Перейти к следующему элементу
    virtual const T& get_current() const = 0; // Получить текущий элемент
    virtual void reset() = 0; // Вернуться в начало
    virtual ~IEnumerator() {}
};

#endif