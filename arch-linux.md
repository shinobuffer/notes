## ArchLinux 装机必备包：
开发：google-chrome visual-studio-code-bin ttf-fira-code postman-bin gnu-netcat
音视频：netease-cloud-music vlc
解压缩：ark 及可选依赖 unzip-iconv
终端：https://www.cnblogs.com/orginly/archive/2021/04/11/14646078.html
网络、下载：qv2ray v2ray qbittorrent-enhanced-git
更新支持：packagekit-qt5 packagekit appstream-qt appstream
推荐：gwenview spectacle typora timeshift

## ArchLinux 疑难杂症解决方案
### 连接WiFi后无法打开应用
在/etc/hostname中添加一主机名（如oshino）
在/etc/hosts中添加如下
```
127.0.0.1 localhost
::1 localhost
1267.0.0.1 oshino.localdomain oshino
```
### 蓝牙启用
启用蓝牙前（`systemctl enable bluetooth`）安装`pulseaudio-bluetooth`

## ArchLinux 必备命令
### pacman 命令
pacman -Syu 同步库并更新系统
pacman -S 安装包
pacman -Sy 同步库再安装包
pacman -U 安装本地包pkg.tar.gz

pacman -R 只删除包（不包含该包的依赖）
pacman -Rs 删除包及其依赖
pacman -Rd 删除包时不检查依赖

pacman -Qi 查看包信息
pacman -Ql 列出包文件

pacman -Sc 清理/var/cache/pacman/pkg下未安装的包
pacman -Scc 清理缓存