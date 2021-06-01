// Copyright 2020 Your Name <your_email>

#include <db.hpp>

std::queue<Element> elements;

void Database::create_db(std::string &path,
                         std::vector<std::string> &family_names)
{
  //open db
  rocksdb::Options options;
  options.create_if_missing = true;
  rocksdb::DB *db;
  rocksdb::Status s = rocksdb::DB::Open(options, path, &db);
  assert(s.ok());

  //create column_family
  rocksdb::ColumnFamilyHandle *h1;
  for (auto &iter : family_names) {
    s = db->CreateColumnFamily
        (rocksdb::ColumnFamilyOptions(), iter, &h1);
    delete h1;
  }
  delete db;
  assert(s.ok());
}


void Database::open_db() {
  rocksdb::DB::Open(rocksdb::DBOptions(), _way, _column_families,
                    &_handles, &_db);
}

void Database::close_db() {
  delete _db;
}


void Database::fill_vector(int &num) {
  for (int i = 0; i != num; i++) {
    _column_families_names.push_back("fam"+ std::to_string(i));
  }
}


void Database::parse(std::string &way_to_db){
  _way = way_to_db;
  rocksdb::Status s;

  s = rocksdb::DB::ListColumnFamilies(rocksdb::DBOptions(),
                                      way_to_db,
                                      &_column_families_names);
  assert(s.ok());
  for (auto a : _column_families_names) {
    _column_families.emplace_back(a,
                                  rocksdb::ColumnFamilyOptions());
  }
}

void Database::put_value(Element &element) {
  m2.lock();
  std::vector<std::string>::iterator it =
      std::find(_column_families_names.begin(),
                _column_families_names.end(), element._family_name);
  if (it != _column_families_names.end()){
    rocksdb::Status s;
    s = _db ->Put(rocksdb::WriteOptions(),
                  _handles[std::distance(_column_families_names.begin(), it)],
                  element._key,
                  picosha2::hash256_hex_string(element._key + element._value));
    assert(s.ok());
  }
  m2.unlock();
}

Element Database::get_value(std::string &key, std::string &column_family_name) {
  std::vector<std::string>::iterator
      it = std::find(_column_families_names.begin(),
                     _column_families_names.end(), column_family_name);
  if (it != _column_families_names.end()){
    rocksdb::Status s;
    std::string value;
    s = _db->Get(rocksdb::ReadOptions(),
                 _handles[std::distance(_column_families_names.begin(), it)],
                 key, &value);
    assert(s.ok());
    Element tmp(key, value, column_family_name);
    return tmp;
  }  else {
    return Element();
  }
}

void Database::print() {
  for (const auto iter : _handles) {
    std::cout << "family " + iter->GetName() << std::endl;
    rocksdb::Iterator *it = _db->NewIterator
        (rocksdb::ReadOptions(), iter);
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
      std::cout << it->key().ToString() << ": "
                << it->value().ToString() << std::endl;
    }
    assert(it->status().ok());
    delete it;
  }
}

void Database::fill_db() {
  bool status = true;
  while (status) {
    m1.lock();
    if (!elements.empty())
    {
      Element tmp = elements.front();
      put_value(tmp);
      elements.pop();
      if (loglevel == "debug" || loglevel == "Debug"){
        BOOST_LOG_TRIVIAL(debug) << " :" << tmp._key << " "
                                 << tmp._value << std::endl;
      } else if (loglevel == "info" || loglevel == "Info"){
        BOOST_LOG_TRIVIAL(info) << " :" << tmp._key << " "
                                << tmp._value << std::endl;
      } else if (loglevel == "warning" || loglevel == "Warning"){
        BOOST_LOG_TRIVIAL(warning) << " :" << tmp._key << " "
                                   << tmp._value << std::endl;
      } else if (loglevel == "error" || loglevel == "Error"){
        BOOST_LOG_TRIVIAL(error) << " :" << tmp._key << " "
                                 << tmp._value << std::endl;
      } else if (loglevel == "fatal" || loglevel == "Fatal"){
        BOOST_LOG_TRIVIAL(fatal) << " :" << tmp._key << " "
                                 << tmp._value << std::endl;
      } else if (loglevel == "trace" || loglevel == "Trace"){
        BOOST_LOG_TRIVIAL(trace) << " :" << tmp._key << " "
                                 << tmp._value << std::endl;
      }
      m1.unlock();
    } else {
      status = false;
      m1.unlock();
    }
  }
}

void Database::read_db() {
  for (const auto iter : _handles) {
    rocksdb::Iterator *it = _db->NewIterator(rocksdb::ReadOptions(), iter);
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
      Element tmp(it->key().ToString(), it->value().ToString(),
                  iter->GetName());
      elements.push(tmp);
    }
    assert(it->status().ok());
    if (!elements.empty()) delete it;
  }
}

void Database::logger(std::string &level) {
  loglevel = level;
  boost::log::register_simple_formatter_factory<boost::log
  ::trivial::severity_level, char>(level);
  const std::string format =
      "%TimeStamp% <%Severity%> (%ThreadID%): %Message%";
  boost::log::add_file_log(
      boost::log::keywords::file_name = "logs/log_trace_%N.log",
      boost::log::keywords::rotation_size = _256_MiB,
      boost::log::keywords::format = format);
  boost::log::add_console_log(
      std::clog,
      boost::log::keywords::format =
          format);
  boost::log::add_common_attributes();
}