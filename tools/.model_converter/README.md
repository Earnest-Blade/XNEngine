# XNE Model Converter

## Overview
XNE Model Converter is a tool that allows you to transform various formats (such as ```.fbx```, ```.dae```or ```.obj```) into a single format that can be loaded into XNEngine.
Even though this format may not be very efficient at the moment, it's easier than using [Assimp](https://github.com/assimp/assimp). It is possible to compress the files so that they take up less space through [zlib](https://www.zlib.net/).

## How to Run ?
Simply use the ```xneconverter.exe``` app, put the file you want to convert into the same folder. Then, enter your file's name and output path. It will ask you for some more options, such as whether the file needs to be compressed or if you want the file to be optimized by ASSIMP.

## File Structure
```json
{
  "asset": {
    "Name": "cube.dae",
    "Version": "0.0.0",
    "Scene": {
      "Name": "root",
      "Root": {
        "Name": "DefaultScene",
        "ChildCount": 1,
        "MeshCount": 0,
        "Childs": [
          {
            "Name": "ChildName",
            "ChildCount": 0,
            "MeshCount": 1,
            "Childs": [],
            "Mesh": [
              0
            ]
          }
        ],
        "Mesh": []
      },
      "MeshCount": 1,
      "Meshes": [
        {
          "Name": "Mesh",
          "VerticesCount": 192,
          "IndicesCount": 36,
          "Vertices": {VerticesHere},
          "Indices": {IndicesHere},
          "Material": 0
        }
      ]
    }
  }
}
```
## Third Party 
- [Assimp.Net](https://www.nuget.org/packages/AssimpNet)
- [Newtonsoft.Json](https://www.newtonsoft.com/json)
- [Zlib.Net](https://www.zlib.net/)

---
<br> XNEngine Model Converter - Version 0.0.0
<br> Ernest - 25.01.2024