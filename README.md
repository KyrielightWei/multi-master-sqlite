# multi-master-sqlite
A prototype of multi-master share-storage database

### Basis

+ sqlite 3.28
+ grpc

### Reference

+ sqlite 源码分析 <http://huili.github.io/>
+ Sqlite3.07.14中文注释源码 <https://github.com/HuiLi/Sqlite3.07.14>
+ grpc 文档 <https://grpc.io/docs/guides/>
+ oltpbench <https://github.com/oltpbenchmark/oltpbench>
+ sqlite JDBC <https://github.com/xerial/sqlite-jdbc>
+ Google开源项目风格指南-命名约定 <https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/naming/>

### Folder

+ sqlite-src-3280000 分文件版源码（参考用）
+ sqlite-autoconf-3280000 单文件编译版

### Comment Format
#### Sqlite-autoconf
```
//===============修改编号+起始
修改代码
//===============修改编号+结束
```
#### Sqlite-src
##### modify
```
/*
* 摘要：
* 修改人：
* 修改时间：
*/
```
##### note
```
/*
* 注释：
*/
```

### Autoconf Modify Index
| 修改编号 | 修改人 | 修改时间 | 修改简述 |
| ------ | ------ | ------ | ------ |
| id  | name | date | brief |

### Src Modify Index
| 修改文件 | 修改人 | 修改时间 | 修改简述 |
| ------ | ------ | ------ | ------ |
| file_path  | name | date | brief |
