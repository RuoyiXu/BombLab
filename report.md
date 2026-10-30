BombLab 实验报告

1. 实验目的

本实验通过分析 x86-64 程序的汇编代码，理解程序控制流程、寄存器使用及整数运算，并尝试编写 defuser.c，根据输入的 challenge 计算对应的答案。

2. 实验环境

操作系统：macOS

编程语言：C

编译工具：GCC

代码管理：Git、GitHub

实验项目：ICS 2026 Fall BombLab

3. 实现过程

本次实验主要完成了 solve_phase_1 函数的初步实现。

程序首先将 64 位的 challenge 转换并拆分为多个 32 位整数：

uint32_t low = (uint32_t)challenge;
uint32_t high = (uint32_t)(challenge >> 32);
uint32_t middle = (uint32_t)(challenge >> 16);

随后，通过整数加法、乘法和异或运算计算候选答案：

uint32_t a = (5u * low + high) ^ 0x4ec2u;
uint32_t b = (middle + 0xfcd4u) ^ high;
uint32_t c = (5u * b) ^ (low + 0xd405u);
uint32_t answer = c ^ a;

最后将计算结果存入 values[0]，设置 count = 1，并调用 write_answer() 输出结果。

这里使用 uint32_t 保持 32 位无符号整数运算语义。

4. 编译与测试

在本地执行：

make
./defuser --seed 42 --phase 1

程序编译成功，测试输出为：

3990712229

该结果说明程序能够正常编译并生成候选答案，但尚未通过真实 bomb 程序验证答案的正确性。

由于本地环境为 macOS，而实验中的 bomb 为 Linux 可执行程序，本次未完成真实炸弹的运行验证。

5. 未完成部分

目前 Phase 1 已有初步实现，但正确性尚未得到完整验证。Phase 2 至 Phase 6 及隐藏关卡尚未完成。

6. 实验总结

通过本次实验，我初步练习了 C 语言中的无符号整数运算、位移和异或操作，并了解了将程序中的计算逻辑转换为 C 代码的方法。

实验过程中也认识到，程序编译成功并不等同于逆向分析正确，最终仍需结合目标程序进行验证。

7. AI 工具使用说明

本次实验使用了 ChatGPT 辅助理解实验流程、分析候选计算公式、编写和检查部分 C 代码，并排查 GitHub 提交及本地开发环境问题。报告中的实现状态与测试结果按照实际操作记录，未将未经验证的结果描述为成功拆弹。
