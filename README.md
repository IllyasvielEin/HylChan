# Hy0 Chan Server

## 项目描述

详见个人博客__（坏了，在修，占个位）

### v0.1 

简单Server，个人学习用。包含简单的解析与GET Method响应，可能存在并发问题。

因为没经过严谨设计，类关系不甚美观，权当研究。

## BenchMark

### v0.1

```
Target: smallhyl (512K)

Runing info: 1000 clients, running 10 sec.

Speed=1514088 pages/min, 12926612 bytes/sec.
Requests: 252348 susceed, 0 failed.
```

```
Target: bighyl(1G)

Runing info: 100 clients, running 10 sec.

Speed=13769826 pages/min, -85439584 bytes/sec.
Requests: 2294971 susceed, 0 failed.
```

## Thanks

感谢陈硕的开源库[Muduo](https://github.com/chenshuo/muduo)，qinguoyi的[TinyWebServer](https://github.com/qinguoyi/TinyWebServer)，以及MARK的[WebServer](https://github.com/markparticle/WebServer)。

