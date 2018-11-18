# GameServer
一个简单的手游服务器框架，自娱自乐

工作计划：
1. 重构service-channel结构，注意MobileServer的通用性 完成
2. gate <--> game 的连接建立 完成
3. 压测读写socket的正确性，发现会崩掉（数据线程不安全）完成；
4. 考虑底层socket的线程安全，封装一些Proxy供主线程使用，屏蔽IO线程的概念
5. 仔细检查各个server/connection/channel的线程安全，KcpConnection有问题！完成
6. 重新组织各个cpp，搭建简单的gate/game/dbproxy, 重点是服务注册和发现 TODO...
7. 做一些上层业务逻辑，完成一些简单的游戏业务；
8. gate上增加加密解密的功能
9. 使用glog替换boost::log
10. static的创建问题,参考boost! 完成
11. gate/game的proto文件全部放在GameServer/proto里面, 因为gate、game进程都需要读取, 完成
12. 使用protobuf3代替protobuf2 完成
13. review代码，完善各处不好的设计：上层应该使用Service/Channel,而不是Connection, MobileServer作为单实例；---不太可能
14. 使用c++11/14/17 完善各种写法, 比如使用lambda替换std::bind, shared_from使用static create方法
15. 开启AddressSanitizer, 查内存泄漏
16. 完善断线再尝试重连的逻辑
