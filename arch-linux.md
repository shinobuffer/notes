## ArchLinux 安装指南：
https://arch.icekylin.online

https://archlinuxstudio.github.io/ArchLinuxTutorial

pandoc：pandoc pandoc-crossref texlive-core（texlive-langchinese texlive-fontsextra）

## 软件安装历史
```bash
sudo pacman -S sof-firmware alsa-firmware alsa-ucm-conf
sudo pacman -S ntfs-3g 
sudo pacman -S adobe-source-han-serif-cn-fonts wqy-zenhei 
sudo pacman -S noto-fonts-cjk noto-fonts-emoji noto-fonts-extra 
sudo pacman -S ark
sudo pacman -S p7zip unrar unarchiver lzop lrzip 
sudo pacman -S packagekit-qt5 packagekit appstream-qt appstream
sudo pacman -S gwenview git wget
sudo pacman -S yay
sudo pacman -S fcitx5-im
sudo pacman -S fcitx5-chinese-addons
sudo pacman -S fcitx5-anthy
sudo pacman -S fcitx5-pinyin-zhwiki
sudo pacman -S fcitx5-material-color 
sudo pacman -S bluez bluez-utils
sudo pacman -S pulseaudio-bluetooth 
sudo pacman -S qv2ray-dev-git v2ray
sudo pacman -S nvidia nvidia-settings lib32-nvidia-utils
sudo pacman -S proxychains-ng 
sudo pacman -S ttf-fira-code 
sudo pacman -S netease-cloud-music
sudo pacman -S typora 
sudo pacman -S neofetch 
sudo pacman -S zsh
sudo pacman -S zsh-autosuggestions zsh-syntax-highlighting zsh-completions
sudo pacman -S autojump

yay -S nerd-fonts-fira-code
yay -S fcitx5-pinyin-moegirl
yay -S google-chrome
yay -S visual-studio-code-bin
yay -S logiops-git

sudo pacman -S kvantum
sudo pacman -S qbittorrent-enhanced-git
sudo pacman -S openssh
sudo pacman -S ksysguard
sudo pacman -S flameshot
sudo pacman -S timeshift
sudo pacman -S blender
sudo pacman -S tela-icon-theme-git
sudo pacman -S latte-dock-git plasma5-applets-window-title plasma5-applets-window-appmenu plasma5-applets-netspeed plasma5-applets-eventcalendar plasma5-applets-weather-widget-2

```

## ArchLinux 疑难杂症解决方案
### 连接WiFi后无法打开应用
在/etc/hostname中添加一主机名（如oshino）
在/etc/hosts中添加如下
```
127.0.0.1 localhost
::1 localhost
1267.0.0.1 oshino.localdomain oshino
```

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
pacman -Qdtq 查看孤立包
pacman -Rs $(pacman -Qtdq) 删除孤立软件包
pacman -F xxx

pacman -Sc 清理/var/cache/pacman/pkg下未安装的包
pacman -Scc 清理缓存