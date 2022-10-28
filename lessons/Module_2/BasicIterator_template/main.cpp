#include <cstddef>
#include <iterator> // Содержит объявления категорий итераторов

template <typename Type>
class SingleLinkedList {
    // Шаблон класса «Базовый Итератор»
    // определяет поведение итератора на элементы односвязного списка
    // ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;
        ...
    };
    ...
}; 