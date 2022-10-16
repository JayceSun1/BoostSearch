#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "util.hpp"
#include <mutex>
#include "log.hpp"
// #include <unistd.h>

const std::string proc_path = "/home/ssj/Boost/boost-search/data/output/raw.txt";

namespace ns_index
{
    struct DocInfo
    {
        std::string title;
        std::string content;
        std::string url;
        uint64_t doc_id; // 文档的id
    };

    struct InvertedElem
    {
        uint64_t doc_id;
        std::string word;
        int weight;
        InvertedElem():weight(0){}
    };

    // 倒排拉链
    typedef std::vector<InvertedElem> InvertedList;

    class Index
    {
    private:
        // 正排索引的数据结构用数组，数组的下标天然是文档的id
        std::vector<DocInfo> forward_index; // 正排索引
        // 倒排索引一定是一个关键字和一组InvetedElem对应[关键字和倒排拉链的映射关系]
        std::unordered_map<std::string, InvertedList> inverted_index;

    private:
        Index(){} // 一定要有函数体，不能delete
        Index(const Index&) = delete;
        Index& operator=(const Index&) = delete;

        static Index* instance;
        static std::mutex mtx;
    public:
        ~Index(){}
    public:
        static Index* GetInstance()
        {
            if (nullptr == instance)
            {
                mtx.lock();
                if (nullptr == instance)
                {
                    instance = new Index();
                }
            mtx.unlock();
            }
            return instance;
        }
        // 根据doc_id找到文档内容
        DocInfo *GetForwardIndex(uint64_t doc_id)
        {
            if (doc_id >= forward_index.size())
            {
                std::cerr << "doc_id out range" << std::endl;
                return nullptr;
            }
            return &forward_index[doc_id];
        }
        // 根据关键字string,获得倒排拉链
        InvertedList *GetInvertedList(const std::string &word)
        {
            auto iter = inverted_index.find(word);
            if (iter == inverted_index.end())
            {
                std::cerr << word << " has no InvertedList" << std::endl;
                return nullptr;
            }
            return &(iter->second);
        }

        // 根据文档去标签，格式化之后的文档，构建正派和倒排索引
        bool BuildIndex(const std::string &input) // parse处理完毕的数据
        {
            std::ifstream in(input, std::ios::in | std::ios::binary);
            if (!in.is_open())
            {
                std::cerr << "sorry, " << input << " open error" << std::endl;
                return false;
            }

            std::string line;
            int count = 0;
            while (std::getline(in, line))
            {
                DocInfo *doc = BuildForwardIndex(line);
                // std::cout << "BuildForwardIndex over once--------------\n" << std::endl;
                // sleep(5);
                if (nullptr == doc)
                {
                    // debug
                    std::cerr << "build " << line << "error" << std::endl;
                    continue;
                }
                BuildInvertedIndex(*doc);
                // std::cout << "BuildIntervertedIndex over once--------------\n" << std::endl;
                // sleep(5);
                count++;
                if (count % 50 == 0)
                {
                    // std::cout << "当前已经建立的索引文档: " << count << std::endl;
                    LOG(NORMAL, "当前已经建立的索引文档" + std::to_string(count));
                }
                // 待修改 1
            }
            return true;
        }
    private:
        DocInfo *BuildForwardIndex(const std::string &line)
        {
            //1. 解析line，字符串切分
            std::vector<std::string> results;
            const std::string sep = "\3";
            // 待修改 2
            ns_util::StringUtil::Split(line, &results, sep);
            if (results.size() != 3)
            {
                // std::cout << "CutString failed-----------" << std::endl;
                // sleep(5);
                return nullptr;
            }
            //2. 字符串填充到DocInfo
            DocInfo doc;
            doc.title = results[0];
            doc.content = results[1];
            doc.url = results[2];
            doc.doc_id = forward_index.size();//doc在vector中的下标
            //3. 插入到正排索引的vector
            forward_index.push_back(std::move(doc));
            return &forward_index.back();
        }

        bool BuildInvertedIndex(const DocInfo &doc)
        {
            // DocInfo{title, content, url, doc_id}
            //word->倒排拉链
            struct word_cnt
            {
                int title_cnt;
                int content_cnt;
                word_cnt():title_cnt(0), content_cnt(0){}
            };
            std::unordered_map<std::string, word_cnt> word_map; // 用来暂存词频的映射表
            // 拆分标题
            std::vector<std::string> title_words;
            ns_util::JiebaUtil::CutString(doc.title, &title_words);
            // 对标题进行词频统计
            for (std::string s : title_words)
            {
                boost::to_lower(s); // 将分词统一转化为小写
                word_map[s].title_cnt++;
            }
            // 拆分内容
            std::vector<std::string> content_words;
            ns_util::JiebaUtil::CutString(doc.content, &content_words);
            // 统计内容
            for (std::string s : content_words)
            {
                boost::to_lower(s); // 将分词统一转化为小写
                word_map[s].content_cnt++;
            }
#define X 10
#define Y 1
            for (auto &word_pair : word_map)
            {
                InvertedElem item;
                item.doc_id = doc.doc_id;
                item.word = word_pair.first;
                item.weight =X * word_pair.second.title_cnt + Y * word_pair.second.content_cnt; // 相关性
                InvertedList &inverted_list = inverted_index[word_pair.first];
                inverted_list.push_back(std::move(item));
            }
            return true;
        }
    };
    Index* Index::instance = nullptr;
    std::mutex Index::mtx;
}
