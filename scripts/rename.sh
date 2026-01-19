#!/bin/bash
# 递归替换当前目录及子目录下文件名、文件内容中的指定字符串

set -e

# 参数检查
if [ $# -ne 2 ]; then
    echo "用法: $0 <旧字符串> <新字符串>"
    echo "示例: $0 aaa bbb"
    exit 1
fi

OLD="$1"
NEW="$2"

# 排除的文件夹列表
# EXCLUDE_DIRS="manifest .git patches"

echo "=== 开始替换 '$OLD' → '$NEW' ==="

# === 第1阶段：替换文件内容 ===
echo "--- 替换文件内容 ---"
find . -type f \( -path "*/.git" -prune -o -path "*/manifest/*" -prune -o -path "*/patches/*" -prune -o -print \) | while read -r file; do
    # 跳过二进制文件
    if file "$file" | grep -q "text"; then
        # 使用 sed 替换
        sed -i "s/${OLD}/${NEW}/g" "$file"
        echo "内容替换: $file"
    fi
done

# === 第2阶段：重命名文件（从深层路径开始）===
echo "--- 重命名文件 ---"
find . -depth -type f -name "*${OLD}*" \( -path "*/.git" -prune -o -path "*/manifest/*" -prune -o -path "*/patches/*" -prune -o -print \) | while read -r file; do
    newfile="$(dirname "$file")/$(basename "$file" | sed "s/${OLD}/${NEW}/g")"
    mv "$file" "$newfile"
    echo "文件重命名: $file → $newfile"
done

# === 第3阶段：重命名目录 ===
echo "--- 重命名目录 ---"
find . -depth -type d -name "*${OLD}*" \( -path "*/.git" -prune -o -path "*/manifest/*" -prune -o -path "*/patches/*" -prune -o -print \) | while read -r dir; do
    newdir="$(dirname "$dir")/$(basename "$dir" | sed "s/${OLD}/${NEW}/g")"
    mv "$dir" "$newdir"
    echo "目录重命名: $dir → $newdir"
done

echo "=== 全部完成 ==="
