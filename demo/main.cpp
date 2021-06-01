#include <db.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <thread>
#include <ThreadPool.h>

namespace po = boost::program_options;
std::string path_in("/home/boyyko/lab-10-kv-storage/cmake-build-debug/db1");
std::string path_out("/home/boyyko/lab-10-kv-storage/cmake-build-debug/db2");

int main(int argc, char* argv[]) {

  po::options_description descr("Usage");
  po::options_description desc("Options");
  desc.add_options()("log_level",po::value<std::string>(),
                     "= \"info\"|\"warning\"|\"error\"\n= default: \"Error\"\n")(
      "thread_count", po::value<size_t>(),
      "= default: count of logical core\n")(
      "input", po::value<std::string>(),
      "= <path/to/input/storage.db>\n= default: /home/.../cmake-build-debug/db1\n")(
      "output", po::value<std::string>(),
      "= <path/to/output/storage.db>\n= default: /home/.../cmake-build-debug/db2\n")(
      "help", "optional information\n");

  po::variables_map vm; // хранит ключи
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);


  if (vm.count("help")) {
    std::cout << descr;
    std::cout << "  dbcs [options] <path/to/input/storage.db>\n" << std::endl;
    std::cout << desc << std::endl;
    return 0;
  }

  std::string log_level{};
  size_t thread_count;
  std::string  input{};
  std::string output{};


  if (vm.count("log_level")) {
    log_level =  vm["log_level"].as<std::string>();
    std::cout << "log_level:" << log_level << std::endl;
  } else {
    log_level = "Trace";
    std::cout << "log_level:" << log_level << std::endl;
  }
  if (vm.count("thread_count")) {
    thread_count = vm["thread_count"].as<size_t>();
    std::cout << "threads:" << thread_count << std::endl;
  } else {
    thread_count = std::thread::hardware_concurrency();
    std::cout << "threads:" << thread_count << std::endl;
  }
  if (vm.count("output")) {
    output = vm["output"].as<std::string>();
    std::cout << "output:" << output << std::endl;
  } else {
    output = path_out;
    std::cout << "output:" << output << std::endl;
  }
  if (vm.count("input")) {
    input = vm["input"].as<std::string>();
    std::cout << "input:" << input << std::endl;
  } else {
    input = path_in;
    std::cout << "input:" << input << std::endl;
  }


  Database::logger(log_level);


  std::vector <std::thread> threads;
  for (size_t i = 0; i < thread_count; ++i) {
    std::thread th(Database::fill_db);
    threads.push_back(std::move(th));
  }
  for (auto &it : threads) {
    it.join();
  }

  /* ThreadPool pool_in(thread_count);
   ThreadPool pool_out(thread_count);
   pool_in.enqueue(Database::read_db);
   pool_out.enqueue(Database::fill_db);*/

  Database db1;
  db1.parse(path_in);
  db1.open_db();
  db1.read_db();


  Database db2;
  db2.create_db(path_out, db1._column_families_names);
  db2.parse(path_out);
  db2.open_db();
  db2.fill_db();
  db2.print();
}