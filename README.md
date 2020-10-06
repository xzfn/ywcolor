# 亚伟速录机软件颜色更改器
Change the background and foreground color of a win32 Rich Edit control in another application.

The source code encoding is UTF-8-BOM. This app is useful most likely only in Chinese environment.

更改另一个程序的win32 Rich Edit控件的前景色和背景色，主要是为了改亚伟速录机程序的背景色和前景色，保护视力。

## 快速开始
下载ywcolorpp.exe；打开亚伟速录机程序；双击ywcolorpp.exe；依次选择背景色和前景色。

或者：下载ywcolorpp.exe和ywcolorpp.bat；打开亚伟速录机程序；双击ywcolorpp.bat。具体十六进制颜色可在bat里更改。

## 使用
先打开亚伟速录机程序，再打开本程序ywcolorpp.exe，会自动搜索打开的速录机程序实例。找到后会一次弹出两个颜色选择器，先选择的是背景色，后选择的是前景色，也就是字体颜色。

也可以使用命令行来控制，这样可以直接把颜色放在命令行的参数里。颜色格式为#RRGGBB，十六进制。

```batch
ywcolorpp.exe /console /image seperate.exe /class RichEdit20A /foreground #101010 /background #e0f0e0
```

实际上可以用命令行选择其他的程序名称和RichEdit的ClassName，默认分别是seperate.exe和RichEdit20A。

可以把这个命令行放到一个bat文件里，把里面的颜色参数改成自己喜欢的颜色，之后打开速录机程序后双击一下这个bat即可，不用每次重新选颜色了。

注意，如果改成了白色的字，从速录机程序复制文字后，如果直接粘贴到Word里，字仍然是白色的，这样和Word的白色背景完全一样了，需要在Word里Ctrl+A全选后改一下字体颜色才可以看到。

## 开发
这个基本只可能在中文环境下使用，所以能用中文的地方都用了中文。

源代码编码是UTF-8-BOM。注意，并不是UTF-8。

使用了Visual Studio 2019来编译，其他版本应该也可以用。

附有两个Python脚本，安装pywin32后也可以使用，功能一致，原本用于Prototype，供参考。

实现上，主要是跨进程向另一个窗口发送消息，SendMessage。改背景色很简单，颜色就放在了SendMessage的param里。改前景色稍复杂，需要先在目标进程地址空间分配内存，填满具体参数后，再SendMessage把刚才分配的内存的指针地址传过去。不能在本进程分配内存，因为目标进程和本进程是两个地址空间，否则不但不能用，还会直接crash掉目标进程。

亚伟速录机程序，可执行文件是seperate.exe，编辑空间是RichEdit20A，这里也附上了一个最简单的实现用来测试，在seperate文件夹里。
