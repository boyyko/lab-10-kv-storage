
#ifndef LAB_10_KV_STORAGE_ELEMENT_HPP
#define LAB_10_KV_STORAGE_ELEMENT_HPP

#include <string>

struct Element{
  Element() = default;

  Element(std::string key, std::string value, std::string family_name){
    _key = key;
    _value = value;
    _family_name = family_name;
  }

  std::string _key;
  std::string _value;
  std::string _family_name;
};

#endif  // LAB_10_KV_STORAGE_ELEMENT_HPP
