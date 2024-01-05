cl -utf-8 -std:c++20 -EHsc CheckPathMatchSpec.cpp shlwapi.lib
cl -utf-8 -std:c++20 -EHsc check_fn_match.cpp shlwapi.lib User32.lib
cl -std:c++20 -EHsc smp_recursive_directory_iterator.cpp user32.lib
