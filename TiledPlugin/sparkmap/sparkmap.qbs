import qbs 1.0

TiledPlugin {
    cpp.defines: ["SPARKMAP_LIBRARY"]

    files: [
        "sparkmap_global.h",
        "sparkmapplugin.cpp",
        "sparkmapplugin.h",
        "plugin.json",
    ]
}
