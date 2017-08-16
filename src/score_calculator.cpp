// Copyright (c) 2017 Lex Personal.
// Author: Enxu Li (wordlienxu2010@163.com)

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"
#include "toft/storage/file/file.h"
#include "toft/base/string/algorithm.h"
#include "toft/base/string/number.h"

DEFINE_string(score_file_path, "empty", "score path");
DEFINE_string(answer_file_path, "empty", "answer file path");
DEFINE_string(output_file_path, "test_data/all_results.txt", "output file path");
/* 需要进行打分的全部数据的下标集合
 * 例如："1-3,4-10,15-34"
 * example file index:
 * 0  1  2  3  4
 * 1  *
 * 2     *
 * 3        *
 */
DEFINE_string(all_data_index, "null", "all data indexs");

bool ReadFileToVector(std::vector<std::string>* scores) {
  return toft::File::ReadLines(FLAGS_score_file_path, scores);
}

bool ReadAnswers(std::map<int, std::map<int, int> >* number_2_answers) {
  // 读文件
  std::vector<std::string> all_answers;
  if (!toft::File::ReadLines(FLAGS_answer_file_path, &all_answers)) {
    return false;
  }

  // 存储答案
  // 1.文件的第一行，是答案的选项，单独解析存储
  std::vector<int> options;
  std::vector<std::string> options_str;
  LOG(INFO) << "answers option: " << all_answers[0];

  toft::SplitString(all_answers[0], "\t", &options_str);
  for (size_t i = 0; i < options_str.size(); ++i) {
    int tmp_value = 0;
    toft::StringToNumber(options_str[i], &tmp_value);
    options.push_back(tmp_value);
  }

  // 2.文件的余下的行，是答案的得分，此处解析
  for (size_t i = 1; i < all_answers.size(); ++i) {
    // 因为是从1开始读的，所以对应number_2_answers题号下标应减1
    std::map<int, int>& single_answer = (*number_2_answers)[i - 1];
    std::vector<std::string> single_answer_vec_str;
    toft::SplitString(all_answers[i], "\t", &single_answer_vec_str);

    // 文件第1列是题号，因此从1开始读
    for (size_t j = 1; j < single_answer_vec_str.size(); ++j) {
      // 转换并存储分值
      int tmp_value = 0;
      toft::StringToNumber(single_answer_vec_str[j], &tmp_value);
      single_answer.insert(std::make_pair(options[j], tmp_value));
    }
  }

  return true;
}

bool OutputResult(const std::map<int, std::vector<int> >& final_scores) {
  std::ofstream outfile(FLAGS_output_file_path, std::ofstream::binary);

  for (auto map_it = final_scores.begin();
       map_it != final_scores.end(); ++map_it) {
    /*
    std::cout << "number: " << map_it->first
        << ", score: " << map_it->second << std::endl;
    */

    // std::cout << map_it->first << "\t" << map_it->second << std::endl;
    outfile << map_it->first;
    auto& final_score = map_it->second;
    for (size_t i = 0; i < final_score.size(); ++i) {
      outfile <<  "\t" << final_score[i];
    }

    outfile << std::endl;
  }

  return true;
}

int main(int argc, char* argv[]) {
  // 读取gflags配置
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  // 读文件
  std::vector<std::string> scores;
  if (!ReadFileToVector(&scores)) {
    std::cout << "score file read failed, please check." << std::endl;
    return -1;
  }

  // 解析内容
  // key：该生的编号；value：该生的解答(只存储需要计算分值的解答)
  std::map<int, std::vector<int> > number_2_score;
  for (size_t i = 0; i < scores.size(); ++i) {
    std::vector<int>& single_score_vec_int = number_2_score[i];
    std::vector<std::string> single_score_vec_str;
    toft::SplitString(scores[i], "\t", &single_score_vec_str);

    for (size_t j = 0; j < single_score_vec_str.size(); ++j) {
      // 只计算固定范围内的成绩
      // 例如：1.1题-1.37题、2.1题-2.57题
      // 1.1题-1.37题对应下标：9-45
      // 2.1题-2.57题对应下标：46-102
      // TODO:此处先写hard code，后续变成可配置
      if (j < 9 || j > 102) {
        continue;
      }

      // 转换并存储分值
      int value = 0;
      toft::StringToNumber(single_score_vec_str[j], &value);
      single_score_vec_int.push_back(value);
    }
  }

  // 读入正确答案及分值
  // key：需要计算分值的题的vector下标，该vector是number_2_score的value，即该生的解答的vector;
  // value：对应题号的答案及分值
  std::map<int, std::map<int, int> > number_2_answers;
  if (!ReadAnswers(&number_2_answers)) {
    std::cout << "answers file read failed, please check." << std::endl;
    return -1;
  }

  // 计算成绩
  std::map<int, std::vector<int> > final_scores;
  for (auto map_it = number_2_score.begin();
       map_it != number_2_score.end(); ++map_it) {
    // 获取该生的解答
    std::vector<int>& single_score_vec_int = map_it->second;
    std::vector<int>& final_score = final_scores[map_it->first];
    int sum_score = 0;

    // 依次该生计算每题的得分，并累加总得分
    for (size_t i = 0; i < single_score_vec_int.size(); ++i) {
      // 获取该题正确答案及分值
      std::map<int, int>& answers = number_2_answers[i];

      // 存储每一题的结果，并累加总分
      final_score.push_back(answers[single_score_vec_int[i]]);
      sum_score += answers[single_score_vec_int[i]];
    }

    // 存储总分
    final_score.push_back(sum_score);
  }

  // 输出计算结果
  if(!OutputResult(final_scores)) {
    std::cout << "results file write failed, please check." << std::endl;
    return -1;
  }

  return 0;
}

