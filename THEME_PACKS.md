# Spectra ZIP 主题包格式

主题包是扩展名为 `.zip` 的压缩文件，放置在：

```text
.Spectra/themepacks/
```

ZIP 根目录必须包含 `theme.json`。示例：

```json
{
  "schema": 1,
  "id": "example-theme",
  "name": "Example Theme",
  "version": "1.0.0",
  "author": "Author",
  "accent": "#1677d2",
  "wallpaper": "images/wallpaper.png",
  "stylesheet": "theme.qss"
}
```

字段说明：

- `id`：唯一标识，只允许字母、数字、点、下划线和连字符。
- `name`：显示名称。
- `version`：主题包版本。
- `author`：可选作者信息。
- `accent`：可选主色。
- `wallpaper`：可选，ZIP 内的壁纸路径。
- `stylesheet`：必需，ZIP 内的 Qt Widgets QSS 样式表。

主题包接口由 `IThemePack` 定义，ZIP 实现为 `ZipThemePack`，注册表为 `ThemePackRegistry`。主题包不会替换默认 Windows 11 主题，只通过 `.Spectra/themepacks` 提供扩展入口。
