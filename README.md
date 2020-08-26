## PathFinding

+ A Star
+ B Star (with bugs!)
+ BFS
+ DFS

## 设置障碍物

+ 手动设置: 

    + 鼠标左键点击设置障碍物，右键点击清除障碍物。按住左/右键不松，移动鼠标，可连续设置/清除障碍物。
    + 左键点击一个点，然后按住`shift`键，再点击一个点，即可生成水平或竖直直线。

    ![manual](assets/manual.png)

+ 随机生成: 点击`Random Obstacles`按钮，输入千分比，自动随机生成相应比例的障碍物

    ![random](assets/random.png)


## 修改颜色

双击颜色块，即可修改相应的颜色

![set-colors](assets/colors.png) 


## 添加新的寻路算法

如果要写其他寻路算法，只需要继承自`FindPathBase`类，

并重写`std::list<Coordinate> findPath(src, dest)`函数。


例如，假设有一个新的 算法 `C Star` :

<font color="red">注意: 标有 [Required] 的地方是必须要写的 !</font>

```
class CStar : public FindPathBase
{
public:
    std::list<Coordinate> findPath(const Coordinate& src, const Coordinate& dest) {
        stopFindPath = false;  // [Required]

        // ...

        //  开始寻路
        while (...) {

            // 是否强制终止寻路, [Required]
            if (stopFindPath) {
                return {};
            }

            // ...

            // 新的点 newCoord
            Coordinate newCoord = ...;

            // 该点是障碍物
            if (detectObstacle(newCoord)) {
                continue;
            }

            // 该点是目标，返回
            else if (newCoord == dest) {
                std::list<Coordinate> path;
                // add coord to path
                // ...
                return path;
            }

            else {
                // 通知GUI线程，绘制该点(已经访问的点，默认为橘色)  [Required]
                notifyVisitedCoordinate(newCoord);
                // ...
            }

            // ...
        }

        // ...
        
        return {};  // 返回空路径
    }
}
```


然后在`src/controldialog.cpp`中，将该算法添加到候选列表中

1、构造函数中，添加

`comboxFindPathList->addItem("C Star");  // 算法名称`

2、`onFindPathMethodChanged`函数中，添加

```
    else if (methodName == "C Star") {
        findPathBaseNew = new CStar();
        if (controlPanelExtend) {
            delete controlPanelExtend;
            controlPanelExtend = nullptr;
        }
        // 扩展控制面板（该算法的独有的设置项，如果没有，就忽略）
        //controlPanelExtend = new ControlPanelExtendCStar(world, nullptr);
    }
```

## Screenshots

### [1/4] A Star
![astar](assets/astar.png) 

![astar-2](assets/astar-2.png)

### [2/4] BFS
![bfs](assets/bfs.png) 

### [3/4] DFS
![dfs](assets/dfs.png) 

![dfs-2](assets/dfs-2.png) 

### [4/4] B Star (With bugs)

Only suitable for situations with few obstacles. (At least)

![bstar](assets/bstar.png) 




## END

Author: <a href="https://github.com/Leopard-C" target="_blank"> github@Leopard-C</a>

Email:  <leopard.c@outlook.com>


