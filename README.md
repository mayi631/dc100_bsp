# Sophcam project

- Chip: cv1842cp
- Toolchain: musl_arm32
- Board: dc100

## 配置文件

`repo_config` 中定义了bsp、app使用的xml配置文件，以及各个仓库对应的git版本文件。没有应用的版本可以选择移除 app 部分。

```json
{
    "bsp": {
        "xml": "manifest/sophcam_bsp_golden.xml",
        "txt": "manifest/git_version_cv184x_2025-12-19.txt"
    },
    "app": {
        "xml": "manifest/sophcam_app.xml",
        "txt": "manifest/git_version_sophcam_2025-12-30.txt"
    }
}
```

## 代码拉取

- ‼️不要尝试更改相对路径，按照步骤来！
- ‼️使用reproduce切换到特定版本的SDK，否则可能出现patch冲突！

**内部代码**

```bash
mkdir SDK_CV184X && cd SDK_CV184X

# 拉取项目代码
git clone "ssh://${whoami}$@172.25.4.9:29418/Projects/Sophcam/dc100_bsp"
```

**github 代码**

```bash
mkdir SDK_CV184X && cd SDK_CV184X

# 拉取项目代码
git clone git@github.com:mayi631/dc100_bsp.git -b v6.3.2-20251219
```

**常用命令**

```bash
# 拉取 SDK 代码，一定要使用reproduce切换到特定版本的SDK，否则可能出现patch冲突！
./dc100_bsp/scripts/repos --gitclone --reproduce

# 打上额外的patch到SDK代码（修复该版本已知的bug或者添加新的功能）
./dc100_bsp/scripts/repos --applypatch

# 同步板卡配置到 SDK （注意这个脚本的运行位置需要固定）
./dc100_bsp/scripts/sync.sh

# 检查SDK本地提交和远端的差异
./dc100_bsp/scripts/repos --quiet --run lp

# 在每个git仓库中执行命令
./dc100_bsp/scripts/repos --quiet --run git status

# 更新SDK
./dc100_bsp/scripts/repos --bsp --run git fetch
./dc100_bsp/scripts/repos --bsp --run git rebase

# release 代码时记录版本信息
./dc100_bsp/scripts/repos --run release > dc100_bsp/manifest/release/release_dc100_20260106.txt
```

## SDK 编译

```bash
# 需要设置TPU_REL=1，应用依赖TPU相关的库
export TPU_REL=1; source build/envsetup_soc.sh
defconfig cv1842cp_dc100_spinand

clean_all && build_all
```

## 代码更改与脚本使用

- 板卡特定的所有的代码更改都应该在 `dc100_bsp` 目录下进行，不要直接在 `SDK_CV184X` 目录下进行更改。
  ```bash
  # 在 dc100_bsp 目录下进行代码更改后，需要执行下面的命令来同步到SDK
  ./dc100_bsp/scripts/sync.sh
  # 使用 -c 参数可以检查是否有未同步的更改
  ./dc100_bsp/scripts/sync.sh -c
  ```
- 所有的脚本都应该在 `SDK_CV184X` 目录下运行，不支持在其他目录下运行。
- 此项目下的配置是基于算能的demo板子，若硬件有差异，建议更具实际情况修改配置。可以选择fork此项目，然后根据实际情况修改配置，可以使用`scripts/rename.sh`脚本将项目名称改为自己的项目名称，原理是遍历所有文件内容、文件名、目录名，做简单的替换，大小写敏感。
  ```bash
  # 重命名项目名称为自己的项目名称，大小写都需要更改
  cd dc100_bsp
  ./scripts/rename.sh dc100 projectname
  ./scripts/rename.sh dc100 PROJECTNAME
  ```
- 如果需要修改SDK本身，建议修改后，将patch保存到`dc100_bsp/patches`目录下，并以`xxx--0001-xxx.patch`格式命名，其中`xxx`为文件夹的名称，以`--`分隔。这样其他人拉代码之后，可以方便的应用patch。以linux_5.10为例：
  ```bash
  cd linux_5.10
  # 假设有两笔patch
  git format-patch -o ../dc100_bsp/patches/ -2
  # 重命名patch
  cd ../dc100_bsp/patches/
  mv 0001-xxx.patch linux_5.10--0001-xxx.patch
  mv 0002-xxx.patch linux_5.10--0002-xxx.patch

  # 批量重命名
  for f in 00*.patch; do mv "$f" "xxx--$f"; done
  ```
- 使用`applypatch`命令应用patch：
  ```bash
  cd SDK_CV184X
  ./dc100_bsp/scripts/repos --applypatch
  ```
  该命令会自动提取patches目录下文件的前缀，确定是哪个仓库的patch，并进行应用。如果仓库当前存在未提交的改动或者与指定的txt文件中的commit-id不匹配，会提示用户是否继续应用patch。如果用户选择继续，会先重置到指定的commit-id，然后再应用patch。

## patches_rls 与 patches 目录说明

- patches_rls 目录下存放的是 release 版本所需的补丁文件。因为部分源码未释放，内部的补丁无法应用到 github 版本中。
- patches 目录下存放的是开发版本所需的补丁文件。

通过在 github 版本的 sdk 下使用，应用部分带源码的修改，然后将内部打上补丁，编译得到的库文件替换到 github 版本的 sdk 下，即可得到与 release 版本一致的功能。

```bash
git apply --reject xxx.patch
```

## release 注意事项

- cvi_mpi 中有一个pq参数相关的patch。
- isp 未提供源码，只提供了lib，放在 cvi_alios/components/cvi_mmf_sdk/lib 目录下。
- audio 未提供源码，只提供了lib。
- cvi_mpi/lib 下有区分工具链的库文件，发布时需要注意替换正确的库文件。

```bash
# 发布 release 版本
./scripts/release.sh

rsync -av --delete --exclude .git dc100_bsp_rls/ ../github_dc100_bsp/
```
