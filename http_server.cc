#include "searcher.hpp"
#include "cpp-httplib/httplib.h"

// const std::string proc_path = "/home/ssj/Boost/boost-search/data/output/raw.txt";
const std::string root_path = "./wwwroot";

int main()
{
  ns_searcher::Searcher search;
  search.InitSearcher(proc_path);

  httplib::Server svr;
  svr.set_base_dir(root_path.c_str());
  svr.Get("/s", [&search](const httplib::Request &req, httplib::Response &rsp)
  {
    if (!req.has_param("word"))
    {
      rsp.set_content("need key words:", "text/plain; charset=utf-8");
      return;
    }
    std::string word = req.get_param_value("word");
    // std::cout << "用户在搜索：" << word << std::endl;
    LOG(NORMAL, "用户在搜索: " + word);
    std::string json_string;
    search.Search(word, &json_string);
    rsp.set_content(json_string, "application/json");
    // rsp.set_content("hello world!", "text/plain; charset=utf-8");
  });
  LOG(NORMAL, "服务器启动成功...");
  svr.listen("0.0.0.0", 8081);
  return 0;
}
