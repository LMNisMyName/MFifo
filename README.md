# MFifo
mfifo是一个线程安全且支持读写并发的高性能缓冲队列数据结构。
设计时的初衷如下：
1. MFifo的所有关键状态都会自动在 mfifostatusui中展示，这一切都是自动发生的，且mfifostatusui不会影响mfifo的读写性能。
2. Mfifo既可以看成fifo，也可以看成一个array，可以用下标进行索引，这个下标是动态的，也就是说下标0永远指向第一个。下标-1同理。
3. 在硬件上，FIFO有两种：Normal和First-word-fall-through。成员方法last()与popLast()即是体现了这两种硬件观点。
4. 在较大的数据输入的情况下，MFifo可以直接保存从PC硬件（如网卡）获得的智能数据指针，来尽量减少不必要的数据挪动拷贝次数，真正实现“零冗余”拷贝。

用户指南和示例工程演示可查阅以下链接：
https://kpcbf4ul2l.feishu.cn/docx/U6nJdBsZHoHjhSxIxTwcWxwwnAb

# 应用背景
2021年8月，该数据结构已被应用于“南海中微子望远镜：海铃计划”的先导实验“海铃探路者”实验DAQ系统上位机GUI软件当中，成功帮助实验完成采数目标。

# 开发人员
中国科学技术大学 缪鹏博士

上海交通大学 孙正阳硕士

# 致谢
中国科学技术大学

上海交通大学

李政道研究所




