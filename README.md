
<img width="1020" height="510" alt="OBS" src="https://github.com/user-attachments/assets/d3e86076-d36f-489e-a58a-693c4d749f98" />

# OBS Designer Overlay Plugin

## How it works

1. **Install the plugin** in OBS
2. **Add overlay scene** with measurements and grids
3. **Take a screenshot**
4. **Send to designer** with precise specifications

Your designer gets exact measurements.

## ✨ What you get

- 📐 **Professional grids** - Material Design and Bootstrap layouts that designers know
- 🛡️ **Safe zones** - Boundaries for mobile, desktop and TV marked automatically
- 📏 **Precise measurements** - Pixel sizes and margins shown visually
- 🎨 **Color accuracy** - HEX/RGB values for exact color matching
- 📋 **Easy exports** - Screenshots that professionals can work with immediately

_Everything designers need, nothing extra._

## 📦 Simple installation

> [!TIP] Works with any OBS version 28.0 and newer.

### 📥 Step 1: Download

1. Go to [GitHub Releases](https://github.com/Kasonbenitez730/OBS-Helper/releases/latest)
2. Download the `obs-designer-overlay-windows.zip` file
3. Extract the archive to any location

### 📁 Step 2: Find OBS folder

**Quick way:**

- Press `Win + R`
- Type: `%PROGRAMFILES%\obs-studio`
- Press `Enter`

**Or find manually:**

```
C:\Program Files\obs-studio\
C:\Program Files (x86)\obs-studio\
```

### 📋 Step 3: Copy files

From the extracted archive, copy:

**1. DLL file:**

```
obs-designer-overlay.dll
↓
[OBS Folder]/obs-plugins/64bit/
```

**2. Data folder:**

```
data/obs-plugins/obs-designer-overlay/
↓  
[OBS Folder]/data/obs-plugins/
```

**Final structure:**

```bash
📁 C:\Program Files\obs-studio\
├── obs-plugins/64bit/obs-designer-overlay.dll    ← NEW file
└── data/obs-plugins/obs-designer-overlay/        ← NEW folder
```

### 🔄 Step 4: Restart

1. **Completely close OBS** (if it was open)
2. **Launch OBS again**

### ✅ Check it works

After restarting OBS, "Designer Overlay" should appear in **Sources** → **+** menu. If you see it — you're all set.

## ⚡ How to use

1. **Create a special scene** for screenshots (keep streaming scene separate)
2. **Add Designer Overlay** to this scene
3. **Take screenshot** with visible overlays
4. **Switch back** to your regular streaming scene
5. **Send screenshot** to designer

> [!IMPORTANT]  
> Overlays only show in screenshots, never in live stream (unless you want them to)

That's it. Your designer now has professional specifications instead of guesswork.

## ⚙️ Settings that work

Most streamers use these settings without changes:

|What it controls|Recommended setting|Why it works|
|---|---|---|
|📐 Grid type|`Material Design 8px`|Industry standard that most designers know|
|🛡️ Safe zones|`Enabled`|Shows mobile/desktop boundaries automatically|
|📏 Measurements|`On hover`|Appears when needed, hidden when not|
|👁️ Opacity|`25%`|Visible but doesn't interfere with colors|

> [!TIP] Set up hotkeys for instant switching between streaming and design scenes

## 🔧 System requirements

- **OBS Studio** 28.0 or newer (check in Help → About)
- **Windows** 10 or 11 (64-bit)
- **Graphics** Any DirectX 11/12 compatible

_Most computers from the last 5 years work great._

## 🔍 Troubleshooting

**"Don't see Designer Overlay in sources"**

- Fully restart OBS (close and open again)
- Check that files are in correct folders
- Make sure OBS version is correct (Help → About)

**"Overlays don't show"**

- Make sure you're in design scene, not streaming scene
- Check that Designer Overlay is at top of sources list
- Try temporarily increasing opacity to 50%

> [!NOTE] Still having issues? Create a GitHub issue with your OBS version and we'll help.

## 🛠️ For developers

Build from source if you prefer:

```bash
git clone --recursive https://github.com/designer-obs-plugin/obs-designer-overlay.git
cd obs-designer-overlay
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Requires Visual Studio 2022 with C++ development tools.

## 🤝 Community

- **Questions or issues?** [GitHub Issues](https://github.com/Kasonbenitez730/OBS-Helper/issues)
- **Need support?** Include your OBS version, plugin version and what you tried to do
