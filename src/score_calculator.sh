#!/bin/sh
# Copyright (c) 2017 Lex Personal.
# Author: Enxu Li (wordlienxu2010@163.com)

if [ ! -d "./score_calculator_log" ]; then
  mkdir ./score_calculator_log
fi

nohup ./score_calculator \
-log_dir="./score_calculator_log" \
-logbuflevel=0 \
-v=100 \
-score_file_path="test_data/chengji.txt" \
-answer_file_path="test_data/answers.txt" \
-output_file_path="test_data/all_results.txt" > /dev/null 2>&1 &

