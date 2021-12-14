# 我的音圈电机驱动器 —— Cyber-VCM
因为项目需要，需要设计一个音圈电机的驱动器，先开个仓库，后面陆续整理资料
## 关于硬件
硬件设计已经设计验证完成，H桥电路采用的是市面上常用的模块，自带电流检测。由于我再实际测试过程中发现模块的电流检测效果并不理想，于是在底板设计中添加了电流采样电路。
<div align=center><img src="https://user-images.githubusercontent.com/48275625/146013407-ce12c6d6-48fa-4e45-872f-81eb7d943120.png" width="600"></div>
实物效果如下所示：
<div align=center><img src="https://user-images.githubusercontent.com/48275625/146015106-a8137603-6461-41f5-87d3-fc7512cdf0ea.jpg" width="500"></div>

<div align=center><img src="https://user-images.githubusercontent.com/48275625/146014853-2746ced2-a4e5-46dc-981b-0b72892a6261.jpg" width="500"></div>

## 关于软件
程序还在调试，理论上已经大致打通，剩下的就是写bug了吧~


