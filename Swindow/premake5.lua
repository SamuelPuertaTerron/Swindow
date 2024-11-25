project "Swindow"
    kind "staticlib"
    language "C"
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    { 
        "src/**.h", 
        "src/**.c",
    }

    filter "system:windows"
        systemversion "latest"
        defines 
        {
            "SW_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines { "SWINDOW_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "SWINDOW_RELEASE" }
        optimize "On"