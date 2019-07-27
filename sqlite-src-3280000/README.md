# Sqlite3.28 src （参考用源码）

## PCache 部分注解
### Pcache
+ 见源码中文注释
### sqlite3GlobalConfig.pcache2
 在sqlite3.h中定义的pcache2的类型是             
 `sqlite3_pcache_methods2`           
 包含了一系列方法              
 
 注释中称之为“应用定义的page cache”，在许多应用中，大多数sqlite分配的堆内存都用于分配给页缓存，使用这一API实现自定义的页缓存，应用可以更好的控制sqlite消耗的内存
 
 这一特性可选，内建的默认页缓存机制足以满足大部分场景

+ xIniT                      
这个函数指针为空时，调用默认的页缓存机制，而不是应用定义的；
xInit和xShutdown函数不需要线程安全，其它函数必须保证线程安全
+ xCreate      
创建一个cache实例（通常一个打开的数据库文件一个实例）
+ xCachesize           
设置最大的cache大小
+ xPageCount            
返回当前存储在cache中的页的数量，包含pinned和unpinned
+ xFetch            
定位一个页在缓存上的位置并返回一个与之关联的sqlite3_pcache_page对象的指针（个人理解是注册使用该页）
    + pinned                      
    使用xFetch取回的页                    
    如果请求的页已经在页缓存中，则返回指针，如果请求的页不存在，则通过createFlag来控制后续的执行内容：

    |  createFlag   | 动作  |             
    |  ----  | ----  |                    
    | 0  | 不分配新的页，返回NULL |                     
    | 1  | 在方便的情况下分配新的页，否则返回NULL |            
    | 2  | 尽力分配新的页，在不可能做到时返回NULL |

    一般使用的都是0或1,2仅在前一次使用1调用失败时使用

+ xUnpin      
第二个参数是一个pinned的page，第三个参数discard非0时页必须被驱逐出cache；如果discard是0，则是否驱逐页取决于实现                 
这一操作不考虑之前有多少次fetch，引用计数是多少

+ xRekey                
改变页的key值,cache中若已有页的key等于传入的newKey，则使冲突页失效           

+ xTruncate      
大于某个页号（key）或是等于iLimit的缓存项全部失效，如果这些页是pinned的，会隐式使其unpinned

+ xDestroy         
释放所有指定cache相关的资源，调用后sqlite会认为[sqlite3_pcache*] handle不可用，不会再任何其他的sqlite3_pcache_methods2函数中使用

+ xShrink            
尽可能释放堆内存