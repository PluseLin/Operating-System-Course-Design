使用说明：
Ljyux，即Lin Jiayi's Unix，是Lin Jiayi同学的操作系统课设，实现参考了Unix/Linux的设计方式，但又有所区别。
打开可执行文件，会提示输出用户名和密码。root用户的用户名是root，密码是root123。
用户名和密码匹配后，会询问是否进行默认初始化，输入y+回车表示进行默认初始化，其余则不进行
（这个是课设验收要求，会格式化，谨慎使用~）
输入命令的方式类似于Linux，即输入一行+回车，鉴于实现方式问题，命令的参数用空格分割，因此所有的参数（包括路径名）不应含有空格回车。
退出程序采用exit命令。

可用指令：
可用指令如下: 
mkfs : 格式化文件卷
mkdir : 创建目录
rmdir : 删除目录
ls : 展示目录信息
cd : 更改当前目录
mkfile : 创建文件
rmfile : 删除文件
fopen : 打开文件
fclose : 关闭文件
fread : 读取文件
fwrite : 写入文件
fseek : 移动文件读写指针
cat : 输出文件
load : 从宿主机读取文件到Ljyux中
dump : 从Ljyux写出文件到宿主机中
whoami : 查看当前用户
adduser : 添加用户
deluser : 删除用户
su : 更换用户
setgroup : 为用户设置分组
lsinode : 展示inode信息
lsblk : 展示盘块
help : 展示帮助信息
lsuser : 展示用户
chmod : 更改文件权限
setmod : 设置文件权限
exit：退出Ljyux

命令说明:
命令名称: adduser
命令格式: adduser [-u name] [-p password] (name为用户名,password为密码)
命令作用: 添加用户
命令说明: 用户名和密码最长长度为13。若不添加-u -p项，则会在后续要求输入

命令名称: cat
命令格式: cat fp(fp为文件路径)
命令作用: 输出文件
命令说明: 必须保证文件已打开，此操作不改变读写指针

命令名称: cd
命令格式: cd fp(fp为路径名)
命令作用: 更改当前目录
命令说明: 无

命令名称: chmod
命令格式: chmod fp (u|g|v)(+|-)(r|w|x) (fp为文件路径)
命令作用: 更改文件权限
命令说明: 类似Linux的chmod命令

命令名称: deluser
命令格式: deluser -u uname (uname为用户名)
命令作用: 删除用户
命令说明: 必须保证用户存在。删除时保留此用户的文件

命令名称: dump
命令格式: dump src dst (src为Ljyux文件路径，dst为宿主机文件路径)
命令作用: 从Ljyux写出文件到宿主机中
命令说明: 必须保证文件已打开

命令名称: fclose
命令格式: fclose fp(fp为文件路径)
命令作用: 关闭文件
命令说明: 必须保证文件已打开

命令名称: fopen
命令格式: fopen fp [-rw|-ro](fp为文件路径)
命令作用: 打开文件
命令说明: 打开文件时需要保证有权限

命令名称: fread
命令格式: fread fp [-n length](fp为文件路径，length为读取长度)
命令作用: 读取文件
命令说明: 必须保证文件打开从读写指针开始读取length个字节到屏幕中，会修改读写指针

命令名称: fseek
命令格式: 
fseek fp (fp为文件路径, 默认回到开头)
fseek fp -b size (fp为文件路径，从文件开始向后移动size字节)
fseek fp -e size (fp为文件路径，从文件结尾向前移动size字节)
fseek fp -a size (fp为文件路径，从当前位置移动size个字节)
命令作用: 移动文件读写指针
命令说明: 必须保证文件已打开

命令名称: fwrite
命令格式: 
fwrite fp -s str [-n size] (fp为文件路径，str为写入内容，size为写入大小)
fwrite fp -i [-n size](输入模式，fp为文件路径，size为写入大小)
命令作用: 写入文件
命令说明: 
-s输入模式不支持字符串含有空格和回车等间隔符
-i模式下，以单行#加回车表示输入结束
必须保证文件已打开

命令名称: help
命令格式: help [command] (command为命令)
命令作用: 展示帮助信息
命令说明: 若不指明命令，或命令有误，则会展示所有可用命令

命令名称: load
命令格式: load src dst (src为宿主机文件路径，dst为Ljyux文件路径)
命令作用: 从宿主机读取文件到Ljyux中
命令说明: 必须保证文件已打开，此操作不改变读写指针，会覆盖

命令名称: ls
命令格式: ls [-l] fp (fp是目录路径)
命令作用: 展示目录信息
命令说明: fp不允许是数据文件

命令名称: lsblk
命令格式: lsblk blk_id (blk_id是盘块号)
命令作用: 展示盘块
命令说明: 无

命令名称: lsinode
命令格式: lsinode inode_id (inode_id是inode号)
命令作用: 展示inode信息
命令说明: 无

命令名称: lsuser
命令格式: lsuser
命令作用: 展示用户
命令说明: 无

命令名称: mkdir
命令格式: mkdir fp (fp为路径名)
命令作用: 创建目录
命令说明: Ljyux不允许目录和文件重名

命令名称: mkfile
命令格式: mkfile fp [-m auth](fp为文件路径, auth为文件权限，16进制表示) 
命令作用: 创建文件
命令说明: 不允许与其他文件重名，不支持重名覆盖

命令名称: mkfs
命令格式: mkfs
命令作用: 格式化文件卷
命令说明: 格式化后，当前目录为/，用户为root

命令名称: rmdir
命令格式: rmdir fp(fp为路径名)
命令作用: 删除目录
命令说明: 删除目录时必须保证目录为空

命令名称: rmfile
命令格式: rmdir fp(fp为文件路径)
命令作用: 删除文件
命令说明: 删除文件时必须保证文件不被打开

命令名称: setgroup
命令格式: setgroup -u uname -g gid (uname为用户名，gid为id号，要求在0-15，若错误则默认为0)
命令作用: 为用户设置分组
命令说明: 必须指明用户和gid

命令名称: setmod
命令格式: setmod fp mode (fp为文件路径，mode为文件权限，16进制)
命令作用: 设置文件权限
命令说明: 权限设置参考Unix/Linux文件权限，若对文件权限不熟悉不建议使用此命令

命令名称: su
命令格式: su [- uname] (uname为用户名)
命令作用: 更换用户
命令说明: 若不指明用户，则切换到root用户

命令名称: whoami
命令格式: whoami
命令作用: 查看当前用户
命令说明: 无

