# UserRegEnum_0x727

域内普通域用户权限查找域内所有计算机上登录的用户

## 使用方法

```
Usage example:
  program.exe -d missyou.com -dc dc.missyou.com -o result.csv
Options:
  -d,  --domain=Domain               domain name to search
  -dc, --dc=domainControllers        domain controller to query
  -t,  --threads=Theads              number of threads, default: 1000
  -o,  --output=outputResult         output result file
  -h,  --help                        display this help and exit
```

其中 `-t` 是非必须参数, 默认是 1000, 在本地调试的时候觉得 1000 线程挺合适的, 如果没有特殊需求可以不设置这个参数.

`-d`, `-dc`, `-o` 这三个参数时必须的.

![image](https://github.com/0x727/UserRegEnum_0x727/blob/master/Pictures/Xnip2023-03-31_12-50-08.png)

结果会保存在当前 `.exe` 同级目录下.

![image](https://github.com/0x727/UserRegEnum_0x727/blob/master/Pictures/Pasted%20image%2020230408204134.png)

如果信息量大的话, 可以通过文本筛选, 条件设置为: 不包含 "NT AUTHORITY" 字段.

![image](https://github.com/0x727/UserRegEnum_0x727/blob/master/Pictures/Pasted%20image%2020230408202607.png)

![image](https://github.com/0x727/UserRegEnum_0x727/blob/master/Pictures/Pasted%20image%2020230408203110.png)

>**域内计算机越多, 程序执行时间越长, 目前已知 2w 余台机器的域查询耗时 7 分钟左右.**

### 没有域用户帐户权限的用法

如果当前计算机在域内，但是我们又没有域用户帐户的权限，那么可以提权致 SYSTEM，也是能够正常使用此工具的。

为什么 SYSTEM 权限可以查询？

个人的理解是：

在 Windows 系统中，SYSTEM 帐户是系统帐户，对于系统帐户来说，它拥有计算机帐户的身份，而在域环境中，每个计算机帐户都有一个对应的域计算机对象，所以在域环境下，SYSTEM 帐户具有域内计算机对象的身份，因此，它可以执行一些需要域用户帐户权限的命令。

## 原理

尝试与域内所有计算机建立远程注册表连接 ( 远程计算机需要启用 "Remote Registry" 服务 ), 一旦成功连接到指定计算机的注册表, 枚举其 `HKEY_USERS` 根键下的子键名称 (SID), 再通过 SID 转换得到用户帐户名称, 具体原理请看文章[《域内普通域用户权限查找域内所有计算机上登录的用户》](https://trya9ain.github.io/posts/%E6%9E%9A%E4%B8%BE%E5%9F%9F%E5%86%85%E8%AE%A1%E7%AE%97%E6%9C%BA%E4%B8%8A%E7%99%BB%E5%BD%95%E7%9A%84%E7%94%A8%E6%88%B7/).
