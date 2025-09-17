# OBS Designer Overlay Plugin

---

## 🇷🇺 Русская версия

### ✨ Возможности

- 📐 **Сетки** - Material Design 8px, Bootstrap колонки, золотое сечение
- 🛡️ **Зоны безопасности** - Мобильные, десктопные и ТВ границы
- 📏 **Измерения** - Точные размеры и отступы в пикселях
- 🔤 **Анализ типографики** - Метрики шрифтов и WCAG контрастность
- 🎨 **Цветовые инструменты** - HEX/RGB значения и проверка доступности
- 📋 **Готово к экспорту** - Профессиональные спецификации для дизайнеров

### 📦 Установка

> [!IMPORTANT] Требует OBS Studio 28.0+ и Windows 10/11 x64

1. Скачайте [последний релиз](https://url)
2. Извлеките файлы в папку OBS Studio:
    
    ```
    📁 OBS Studio/├── obs-plugins/64bit/obs-designer-overlay.dll└── data/obs-plugins/obs-designer-overlay/
    ```
    
3. Перезапустите OBS Studio
4. Добавьте новую сцену "Designer Overlay" в список сцен

> [!TIP] Используйте Windows+R → `%PROGRAMFILES%\obs-studio` для быстрого поиска папки OBS

### ⚡ Быстрый старт

> [!NOTE] Overlay видны только в OBS для скриншотов, не в стриме

1. Создать новую сцену → **Designer Overlay**
2. Переключиться на эту сцену
3. Настроить нужные элементы overlay
4. Сделать скриншот для дизайнера

> [!IMPORTANT]  
> После передачи скриншота переключитесь обратно на основную сцену стрима

> [!CAUTION] Если забыть переключить сцену, overlay будут видны зрителям в эфире

### ⚙️ Настройки

> [!TIP] Настройте горячие клавиши для быстрого переключения между сценами: Настройки → Горячие клавиши

> [!TIP] Настройки по умолчанию подходят для 90% дизайн-задач

|Настройка|Описание|По умолчанию|Варианты|
|---|---|---|---|
|📐 Система сеток|Направляющие разметки|`8px Material`|4px/8px/12col|
|🛡️ Зоны безопасности|Границы устройств|`✅ Включено`|Mobile/Desktop/TV|
|📏 Измерения|Индикаторы размеров|`❌ Выключено`|Hover/Всегда|
|👁️ Прозрачность|Прозрачность overlay|`25%`|10-90%|

> [!CAUTION] Высокая прозрачность может влиять на точность цветов

### 🔧 Требования

> [!IMPORTANT] Проверьте версию OBS: Справка → О программе

- 🎥 **OBS Studio** 28.0+
- 💻 **Windows** 10/11 x64
- 🖥️ **Графика** DirectX 11/12 или OpenGL 4.1+

### 🛠️ Сборка из исходников

> [!WARNING] Версия для разработчиков - не для продакшена

```bash
git clone --recursive https://github.com/designer-obs-plugin/obs-designer-overlay.git
cd obs-designer-overlay
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

> [!NOTE] Требует Visual Studio 2022 с C++ workload

### 🤝 Участие в разработке

> [!TIP] Проверьте существующие issues перед созданием новых

Issues и pull request приветствуются. Смотрите [CONTRIBUTING.md](https://url).

### 📄 Лицензия

> [!NOTE] Бесплатно для коммерческого использования

MIT License - подробности в файле [LICENSE](https://url).

---

## 🇺🇸 English Version

### ✨ Features

- 📐 **Grid Systems** - Material Design 8px, Bootstrap columns, Golden ratio guides
- 🛡️ **Safe Areas** - Mobile, desktop, and TV safe zones
- 📏 **Measurements** - Pixel-perfect sizing and spacing indicators
- 🔤 **Typography Analysis** - Font metrics and WCAG contrast validation
- 🎨 **Color Tools** - HEX/RGB values and accessibility checking
- 📋 **Export Ready** - Professional specs for designers

### 📦 Installation

> [!IMPORTANT] Requires OBS Studio 28.0+ and Windows 10/11 x64

1. Download the [latest release](https://url)
2. Extract files to your OBS Studio directory:
    
    ```
    📁 OBS Studio/├── obs-plugins/64bit/obs-designer-overlay.dll└── data/obs-plugins/obs-designer-overlay/
    ```
    
3. Restart OBS Studio
4. Add new "Designer Overlay" scene to your scene list

> [!TIP] Use Windows+R → `%PROGRAMFILES%\obs-studio` to quickly find OBS directory

### ⚡ Quick Start

> [!NOTE] Overlays are visible only in OBS for screenshots, not on stream

1. Create new scene → **Designer Overlay**
2. Switch to this scene
3. Configure desired overlay elements
4. Take screenshot for design handoff

> [!IMPORTANT]  
> Switch back to your main streaming scene after taking screenshot

> [!CAUTION] If you forget to switch scenes, overlays will be visible to your viewers

### ⚙️ Configuration

> [!TIP] Set up hotkeys for quick scene switching: Settings → Hotkeys

> [!TIP] Default settings work for 90% of design workflows

|Setting|Description|Default|Options|
|---|---|---|---|
|📐 Grid System|Layout guides|`8px Material`|4px/8px/12col|
|🛡️ Safe Areas|Device margins|`✅ Enabled`|Mobile/Desktop/TV|
|📏 Measurements|Size indicators|`❌ Disabled`|Hover/Always|
|👁️ Opacity|Overlay transparency|`25%`|10-90%|

> [!CAUTION] High opacity may interfere with color accuracy

### 🔧 Requirements

> [!IMPORTANT] Check OBS version: Help → About

- 🎥 **OBS Studio** 28.0+
- 💻 **Windows** 10/11 x64
- 🖥️ **Graphics** DirectX 11/12 or OpenGL 4.1+

### 🛠️ Building

> [!WARNING] Development build - not for production use

```bash
git clone --recursive https://github.com/url
cd obs-designer-overlay
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

> [!NOTE] Requires Visual Studio 2022 with C++ workload

### 🤝 Contributing

> [!TIP] Check existing issues before creating new ones

Issues and pull requests are welcome. See [CONTRIBUTING.md](https://url).

### 📄 License

> [!NOTE] Free for commercial use

MIT License - see [LICENSE](https://url) file for details.
