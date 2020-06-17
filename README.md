Linux下C++轻量级Web服务器
* 使用线程池 + 非阻塞socket + epoll(ET和LT均实现) + 事件处理(Proactor) 的并发模型
* 使用状态机解析HTTP请求报文，支持解析GET和POST请求
* 访问服务器数据库实现web端用户注册、登录功能，可以请求服务器图片和视频文件
* 实现同步/异步日志系统，记录服务器运行状态
* 使用定时器清理非活跃连接

* build

    ```C++
    sh ./build.sh
    ```

* 启动server

    ```C++
    ./server
    ```

* 浏览器端

    ```C++
    ip:9006
    ```

致谢
------------
Linux高性能服务器编程，游双著.  
我模仿的项目TinyWebServer(https://github.com/qinguoyi/TinyWebServer.git)