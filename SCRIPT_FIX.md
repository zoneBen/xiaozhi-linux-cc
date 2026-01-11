# 脚本修复说明

## 问题描述

在Linux环境下运行构建脚本时遇到以下错误：
```
./scripts/build.sh: line 1: #!/bin/bash: No such file or directory
```

这是由于脚本文件在Windows和Linux之间传输时存在换行符差异导致的。

## 解决方案

在Linux环境中运行以下命令来修复脚本文件：

```bash
# 方法1：使用sed命令修复换行符
sed -i 's/\r$//' scripts/*.sh

# 方法2：使用dos2unix命令（如果已安装）
dos2unix scripts/*.sh
```

## 验证修复

修复后，再次尝试运行构建脚本：

```bash
./scripts/build.sh
```

如果仍然遇到opusenc库找不到的问题，请参考[TROUBLESHOOTING.md](TROUBLESHOOTING.md)文档中的相关章节。

## 预防措施

为避免将来出现类似问题，在Windows上创建的脚本文件应使用Unix换行符（LF）而不是Windows换行符（CRLF）。