# over view
- 用于计算问卷得分

# 使用方法
```sh
nohup ./score_calculator \
        -log_dir="./score_calculator_log" \
        -logbuflevel=0 \
        -v=100 \
        -score_file_path="chengji.txt" \
        -answer_file_path="answers.txt" \
        -output_file_path="all_results.txt" > /dev/null 2>&1 &
```

# 注意事项

