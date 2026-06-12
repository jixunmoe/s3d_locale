# s3d_locale

用于构建 Simplify3D 5.1.2 本地化用的 `version.dll`。

## 配置环境

安装/解压 [MinGW](https://www.mingw-w64.org/downloads/) 到一个你熟悉的目录。安装完成后，你可以将 `bin` 目录添加到系统的 `PATH` 中（不推荐）。

需要准备以下工具，并确保它们都在 `PATH` 中：

- [CMake](https://cmake.org/)
- [Ninja](https://ninja-build.org/)

Windows 下建议用 [Scoop](https://scoop.sh/) 安装 CMake 和 Ninja：

```powershell
scoop install cmake ninja
```

## 构建发布

```powershell
# 如果 PATH 中没有 mingw64/bin，**临时**加入
$env:PATH = "C:\path\to\mingw64\bin;${env:PATH}"

# Configure
cmake -S . -B build/release -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw.cmake `
  -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build/release
```

之后将 `contrib` 目录的内容以及输出的 `version.dll` 一同打包即可发布。
