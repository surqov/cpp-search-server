Ошибка компиляции:
/exposed/submission/main.cpp: In lambda function:
/exposed/submission/main.cpp:60:31: error: could not convert ‘(& key_words)->std::set<std::__cxx11::basic_string<char>, std::less<void> >::find((* & w))’ from ‘std::set<std::__cxx11::basic_string<char>, std::less<void> >::const_iterator’ {aka ‘std::_Rb_tree<std::__cxx11::basic_string<char>, std::__cxx11::basic_string<char>, std::_Identity<std::__cxx11::basic_string<char> >, std::less<void>, std::allocator<std::__cxx11::basic_string<char> > >::const_iterator’} to ‘bool’
   60 |             if (key_words.find(w)) {
      |                 ~~~~~~~~~~~~~~^~~
      |                               |
      |                               std::set<std::__cxx11::basic_string<char>, std::less<void> >::const_iterator {aka std::_Rb_tree<std::__cxx11::basic_string<char>, std::__cxx11::basic_string<char>, std::_Identity<std::__cxx11::basic_string<char> >, std::less<void>, std::allocator<std::__cxx11::basic_string<char> > >::const_iterator}
compilation terminated due to -Wfatal-errors.