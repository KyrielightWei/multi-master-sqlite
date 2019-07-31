# Sqlite3.28 src （参考用源码）

## Pager
pager负责读写数据库，管理内存缓存和页面（page），以及管理事务，锁和崩溃恢复。

页面大小 0.5K ~ 32K，编译时确认默认的页面大小

## Pcache1
内存分配有三种来源。

1. 一般的内存分配 - sqlite3Malloc()
2. sqlite3_config() 提供的全局页缓存内存(SQLITE_CONFIG_PAGECACHE)
3. PCache本地的批量分配                   

如果传入的参数N是正的，则分配N个页的内存，并使用前N个页的内存；
如果传入的参数N是负的，则分配-1024\*N字节的内存，并使用尽可能多个页的内存。
实验表明在一般的负载下，N=100的方法（3）性能会高出5%

## 页缓存的结构
```
** -------------------------------------------------------------
** | database page content | PgHdr1 | MemPage | PgHdr |
** -------------------------------------------------------------
```
+ PgHdr1
sqlite3_pcache_page的子类，包含通过页号搜索页的需要的信息
sqlite3_pcache_page.pBuf指向database page content的起始
sqlite3_pcache_page.pExtra指向PgHdr
+ MemPage
由bree.c组件添加，描述了数据库页的序号和数据库页如何被使用
+ PgHdr
由pcache.c层添加，包含了追踪脏页需要的信息          
> 注：（模块交流机制）
> 1. pcache1.c 追踪PgHdr1指针，与pcache1.c模块有交流的只有pcache.c，通过传入或传出PgHdr1指针传递信息
> 2. pcache.c与pager.c模块处理PgHdr指针
> 3. btree.c模块处理MemPage的指针




## PCache 部分注解
### Pcache
> 一个完整的页缓存是这个结构的一个实例
+ 见源码中文注释

### PgHdr
> 缓存中的每个页由这个结构控制                      

神奇的结构：   （互相指向）                    
同一个页的sqlite3_pcache_page和PgHdr两个结构体同时存在

 * sqlite3_pcache_page的pExtra指向PgHdr对象
 * PgHdr的pPage指向sqlite3_pcache_page对象 

具体初始化方式见pcacheFetchFinishWithInit

### sqlite3GlobalConfig.pcache2
> 自定义的页缓存处理机制

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

## 注
### 排序
````
/*
** Sort the list of pages in accending order by pgno.  Pages are
** connected by pDirty pointers.  The pDirtyPrev pointers are
** corrupted by this sort.
**
** Since there cannot be more than 2^31 distinct pages in a database,
** there cannot be more than 31 buckets required by the merge sorter.
** One extra bucket is added to catch overflow in case something
** ever changes to make the previous sentence incorrect.
*/
/**
 * 将页链表按页号升序排序，使用pDirty进行单链表排序，本身的LRU双向链表不受影响
 * 
 * * * 排序方法很有意思，形式上有些像归并排序和桶排序的结合，效率不好简单估计
 * 排序的工作方式有些像二进制加法，每次输入一个元素，将其放在最低位上（最低位+1），
 * 若+1后需要进位，则合并（传入的元素和当前位上的有序列表后）向更高位进位，
 * 更高位+1后继续判断是否需要进位，执行上述过程直到进位工作完成为止
 * 全部元素输入完成后，再执行一次从最低位到最高位的逐次合并，获得最终的排序结果
 * 最多能排序2^N_SORT_BUCKET-1个元素
 */
#define N_SORT_BUCKET  32
static PgHdr *pcacheSortDirtyList(PgHdr *pIn){
  PgHdr *a[N_SORT_BUCKET], *p;
  int i;
  memset(a, 0, sizeof(a));
  while( pIn ){
    p = pIn;
    pIn = p->pDirty;
    p->pDirty = 0;
    for(i=0; ALWAYS(i<N_SORT_BUCKET-1); i++){
      if( a[i]==0 ){
        a[i] = p;
        break;
      }else{
        p = pcacheMergeDirtyList(a[i], p);
        a[i] = 0;
      }
    }
    if( NEVER(i==N_SORT_BUCKET-1) ){
      /* To get here, there need to be 2^(N_SORT_BUCKET) elements in
      ** the input list.  But that is impossible.
      */
      a[i] = pcacheMergeDirtyList(a[i], p);
    }
  }
  p = a[0];
  for(i=1; i<N_SORT_BUCKET; i++){
    if( a[i]==0 ) continue;
    p = p ? pcacheMergeDirtyList(p, a[i]) : a[i];
  }
  return p;
}

````