# XNE Toolkit
**This project is for learning, practice and experiment.**

## Overview
Because handling multiple formats at runtime is not efficient and very hard to implements, XNE Engine comes with his own file extension : ```.xneo```. Thoses files are JSON based and can be compressed with [zlib](https://zlib.net/).

## Projects
XNEngine comes with development tools :
- [XNE Asset](xne-assets/)
<br>xne-asset is the development suite's main C# class library. 
It is used to transform and concatenate universal data in the ```.xneo``` format, the only format understood by the engine.
<br> wiht xne-asset, you can convert 3d model file formats, create sprites and scenes.

- [XNE Asset CLI](xne-cli/)
<br>xne-cli is a terminal based application that you can use to simply use the xne-asset class library.

- [XNE Asset Blender](xne-blender/)
<br>xne-blender is a simply yo use Blender extension that direcly convert your scene into ```.xneo``` models.

---
<br>XNEngine - Version 0.0.0
<br>Ernest - 07.08.2024
