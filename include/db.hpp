// Copyright 2020 Your Name <your_email>

#ifndef INCLUDE_DB_HPP_
#define INCLUDE_DB_HPP_

#include "element.hpp"

#include <boost/log/trivial.hpp>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <rocksdb/table.h>
#include <boost/lexical_cast.hpp>
#include <PicoSHA2/picosha2.h>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/date_time/gregorian/greg_month.hpp>


#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <mutex>
#include <thread>



class Database{
 public:
  Database() = default;
  void create_db(std::string &path, std::vector<std::string> &family_names);
  void open_db();
  void close_db();
  void parse(std::string &way_to_db);
  static void put_value(Element &element);
  Element get_value(std::string &key, std::string &column_family_name);
  void print();
  void fill_vector(int &num);
  static void fill_db();
  static void read_db();
  static void logger(std::string &level);

  inline static std::vector<std::string> _column_families_names;
  inline static rocksdb::DB *_db;
  inline static std::vector<rocksdb::ColumnFamilyHandle *> _handles;
  std::string _way;
  inline static std::vector<rocksdb::ColumnFamilyDescriptor> _column_families;
  inline static std::mutex m1;
  inline static std::mutex m2;
  inline static  size_t _256_MiB = 1024 * 256 * 1024;
  inline static std::string loglevel;
};






#endif // INCLUDE_DB_HPP_
