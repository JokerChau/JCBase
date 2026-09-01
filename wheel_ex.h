#pragma once

#ifdef JCBASE_EXPORTS
#define wheels __declspec(dllexport)
#else
#define wheels __declspec(dllimport)
#endif

/*
* 如何使用：
* 一、Visual Studio 2026（以下简称2026）
* 1.在2026中创建C++ Dll项目
* 2.删除项目模板中的所有头文件和C++文件
* 3.将从GitHub上下载的代码粘贴到项目代码所在文件夹
* 4.在2026中右键“头文件/源文件”点击添加已有项，选中粘贴过来的代码文件
* 5.右键该项目，在“C/C++”中选中“预编译头”，选择“否”
* 6.将生成解决方案模式调成“Release”
* 7.按下“shift+ctrl+B”生成解决方案
* 8.在你的项目中动态链接该库即可
*/

/*
* How to use:
* I. Visual Studio 2026 (hereinafter referred to as 2026)
* 1. Create a C++ DLL project in 2026.
* 2. Delete all header files and C++ files from the project template.
* 3. Paste the code downloaded from GitHub into the project's source folder.
* 4. In 2026, right-click "Header Files" / "Source Files", select "Add Existing Item", and choose the pasted code files.
* 5. Right-click the project, go to "C/C++" -> "Precompiled Headers", and select "Not Using Precompiled Headers".
* 6. Change the solution configuration to "Release".
* 7. Press "Shift+Ctrl+B" to build the solution.
* 8. Dynamically link this library in your project.
*/